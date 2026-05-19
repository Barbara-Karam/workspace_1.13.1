#ifndef ENCODER_H
#define ENCODER_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

// ─── Configuration ────────────────────────────────────────────────────────────
// JGB37-520: 11 PPR × 4 (x4 quadrature) × gear ratio
// Check your motor label for gear ratio — common variants:
//   30:1  → CPR = 11 × 4 × 30 = 1320
//   45:1  → CPR = 11 × 4 × 45 = 1980
//   60:1  → CPR = 11 × 4 × 60 = 2640
#define ENCODER_CPR         1320.0f    // ← Change to match your motor
#define ENCODER_COUNT       4
#define CONTROL_DT          0.01f      // 10ms = 100Hz

// ─── Per-Encoder State ────────────────────────────────────────────────────────
typedef struct {
    int32_t  count;           // Absolute count (accumulates forever)
    int32_t  count_prev;      // Count at previous tick
    int32_t  delta;           // Counts in last dt period
    float    velocity_rpm;    // Computed RPM (signed)
    uint32_t last_raw;        // Last raw timer counter value
} EncoderState_t;

// ─── API ─────────────────────────────────────────────────────────────────────
void    Encoder_Init(void);
void    Encoder_Update(void);        // Call at 100Hz inside TIM10 ISR
int32_t Encoder_GetCount(uint8_t id);
float   Encoder_GetRPM(uint8_t id);
void    Encoder_Reset(uint8_t id);
void    Encoder_ResetAll(void);

#endif
