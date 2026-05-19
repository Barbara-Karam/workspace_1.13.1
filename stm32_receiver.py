import os
import time
import struct

# ── Config ────────────────────────────────────────────────────────────────────
PORT = '/dev/ttyTHS1'
BAUD = 921600

# ── Packet Types ──────────────────────────────────────────────────────────────
SYNC1   = 0xAA
SYNC2   = 0x55
SYNC3   = 0xA5   # Third sync byte — matches STM32 COMM_SYNC3
PKT_IMU = 0x05
PKT_FB  = 0x02
PKT_HB  = 0x03

# Valid TYPE → expected LEN.
# Any other combination after sync = false sync or unknown packet.
VALID_PACKETS = {
    PKT_IMU: 32,
    PKT_FB:  37,
    PKT_HB:  0,
}

# ── CRC8 (poly 0x07 — must match STM32 exactly) ───────────────────────────────
def crc8(data: bytes) -> int:
    crc = 0x00
    for byte in data:
        crc ^= byte
        for _ in range(8):
            crc = ((crc << 1) ^ 0x07) if (crc & 0x80) else (crc << 1)
            crc &= 0xFF
    return crc

# ── Safe Read ─────────────────────────────────────────────────────────────────
def read_exact(fd, n: int) -> bytes | None:
    if n == 0:
        return b''
    buf = b''
    while len(buf) < n:
        chunk = fd.read(n - len(buf))
        if not chunk:
            return None
        buf += chunk
    return buf

# ── Sync State Machine ────────────────────────────────────────────────────────
# Scans byte-by-byte for the 3-byte sequence [AA][55][A5].
# State machine handles overlapping patterns cleanly.
def wait_for_sync(fd) -> bool:
    state = 0
    while True:
        b = fd.read(1)
        if not b:
            return False
        byte = b[0]

        if state == 0:
            state = 1 if byte == SYNC1 else 0
        elif state == 1:
            if   byte == SYNC2: state = 2
            elif byte == SYNC1: state = 1   # AA AA → stay, still possible
            else:               state = 0
        elif state == 2:
            if   byte == SYNC3: return True  # AA 55 A5 ✅
            elif byte == SYNC1: state = 1   # Start over with this byte
            else:               state = 0

# ── Packet Reader ─────────────────────────────────────────────────────────────
def read_packet(fd) -> dict | None:
    while True:
        if not wait_for_sync(fd):
            return None  # Port closed or timeout

        # Read [TYPE][LEN]
        header = read_exact(fd, 2)
        if header is None:
            return None

        pkt_type = header[0]
        length   = header[1]

        # Validate TYPE+LEN before reading payload
        # This rejects the vast majority of false syncs
        if pkt_type not in VALID_PACKETS:
            continue   # False sync — keep scanning
        if VALID_PACKETS[pkt_type] != length:
            continue   # Length mismatch — false sync

        # Read payload + CRC
        payload = read_exact(fd, length)
        if payload is None:
            return None

        crc_byte = read_exact(fd, 1)
        if crc_byte is None:
            return None

        # Verify CRC
        crc_calc = crc8(header + payload)
        if crc_calc != crc_byte[0]:
            # Genuine corruption or rare false sync that passed TYPE+LEN check
            continue   # Keep scanning — don't give up

        # ── Decode ────────────────────────────────────────────────────────────
        if pkt_type == PKT_IMU:
            ax, ay, az, gx, gy, gz, temp, ts = struct.unpack('<7fI', payload)
            return {
                'type': 'imu',
                'ax': ax, 'ay': ay, 'az': az,
                'gx': gx, 'gy': gy, 'gz': gz,
                'temp': temp, 'ts': ts
            }

        elif pkt_type == PKT_FB:
            v0,v1,v2,v3, e0,e1,e2,e3, p0,p1,p2,p3, ts, status = \
                struct.unpack('<4h4i4hIB', payload)
            return {
                'type':     'motor',
                'rpm':      (v0/10, v1/10, v2/10, v3/10),
                'enc':      (e0, e1, e2, e3),
                'pwm':      (p0, p1, p2, p3),
                'ts':       ts,
                'status':   status,
                'imu_ok':   bool(status & 0x01),  # Bit 0 = IMU OK
                'fault':    bool(status & 0x04),  # Bit 2 = FAULT
            }

        elif pkt_type == PKT_HB:
            return {'type': 'heartbeat'}

# ── Main ──────────────────────────────────────────────────────────────────────
def main():
    ret = os.system(f'stty -F {PORT} {BAUD} raw -echo -echoe -echok -hupcl')
    if ret != 0:
        print(f"[ERROR] stty failed — check port permissions")
        return

    time.sleep(0.1)
    print(f"Opening {PORT} @ {BAUD} baud...")

    try:
        fd = open(PORT, 'rb', buffering=0)
    except OSError as e:
        print(f"[ERROR] Cannot open {PORT}: {e}")
        return

    print("Listening — Ctrl+C to stop\n")

    pkt_count  = 0
    imu_count  = 0
    motor_count = 0
    t_start    = time.time()
    t_last_stat = t_start

    try:
        while True:
            pkt = read_packet(fd)
            if pkt is None:
                print("[ERROR] Port read failed")
                break

            pkt_count += 1

            if pkt['type'] == 'imu':
                imu_count += 1
                mag = (pkt['ax']**2 + pkt['ay']**2 + pkt['az']**2) ** 0.5
                print(
                    f"[IMU   #{imu_count:05d}] "
                    f"A:({pkt['ax']:+6.3f},{pkt['ay']:+6.3f},{pkt['az']:+6.3f}) m/s² "
                    f"|g|={mag:.3f} "
                    f"G:({pkt['gx']:+6.2f},{pkt['gy']:+6.2f},{pkt['gz']:+6.2f}) °/s "
                    f"T:{pkt['temp']:.1f}°C "
                    f"ts={pkt['ts']}ms"
                )

            elif pkt['type'] == 'motor':
                motor_count += 1
                imu_status = "IMU:OK" if pkt['imu_ok'] else "IMU:FAIL ⚠️"
                print(
                    f"[MOTOR #{motor_count:05d}] "
                    f"RPM:({pkt['rpm'][0]:+6.1f},{pkt['rpm'][1]:+6.1f},"
                    f"{pkt['rpm'][2]:+6.1f},{pkt['rpm'][3]:+6.1f}) "
                    f"ENC:{pkt['enc']} "
                    f"PWM:{pkt['pwm']} "
                    f"{imu_status}"
                )

            elif pkt['type'] == 'heartbeat':
                print(f"[HB    #{pkt_count:05d}] alive")

            # Print rate stats every 5 seconds
            now = time.time()
            if now - t_last_stat >= 5.0:
                elapsed = now - t_start
                print(
                    f"\n--- Stats: "
                    f"IMU {imu_count/elapsed:.1f}Hz  "
                    f"Motor {motor_count/elapsed:.1f}Hz  "
                    f"Total {pkt_count} pkts ---\n"
                )
                t_last_stat = now

    except KeyboardInterrupt:
        elapsed = time.time() - t_start
        print(f"\nStopped after {elapsed:.1f}s")
        print(f"  IMU packets   : {imu_count}  ({imu_count/elapsed:.1f} Hz)")
        print(f"  Motor packets : {motor_count}  ({motor_count/elapsed:.1f} Hz)")
        print(f"  Total         : {pkt_count}")

    finally:
        fd.close()

if __name__ == '__main__':
    main()