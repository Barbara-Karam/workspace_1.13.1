/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    main.c
  * @brief   RPOD Arm Controller — STM32F401RCT6
  *          IMU + 2× TQ42-772 docking arm motors + limit switches → Jetson
  *
  *  Wiring:
  *    Motor A (arm 1): PA8=RPWM  LPWM=GND  PB12=R_EN  PB13=L_EN
  *    Motor B (arm 2): PA9=RPWM  LPWM=GND  PB14=R_EN  PB15=L_EN
  *    Limit switches:  PB0=SW1  PB1=SW2  PB2=SW3  PB10=SW4
  *                     (active LOW — wire between pin and GND)
  *    IMU MPU6050:     PB8=SCL  PB9=SDA  AD0=GND
  *    UART to Jetson:  PA2=TX   PA3=RX   shared GND
  *
  *  UART protocol (921600 baud):
  *    STM32 → Jetson every 50ms:
  *      $ARM,ax,ay,az,gx,gy,gz,sw1,sw2,sw3,sw4\r\n
  *      example: $ARM,+0.023,-0.011,+9.810,+0.001,-0.002,+0.000,0,0,0,0
  *      sw=0 → open (not triggered), sw=1 → contact detected
  *
  *    Jetson → STM32:
  *      $CMD,<A>,<B>\r\n
  *      A,B = -100 to +100 (percentage of full speed)
  *      example: $CMD,+80,+80   → both arms extend at 80%
  *      example: $CMD,0,0       → stop both
  *      example: $CMD,-50,-50   → both arms retract at 50%
  *
  *  Live Expressions:
  *    imu_ok, found_addr
  *    imu_data.accel_x, imu_data.accel_z
  *    sw_state          (4-bit: bit0=SW1 ... bit3=SW4)
  *    docking_detected  (1 = any switch triggered)
  *    motor_a_pwm, motor_b_pwm
  ******************************************************************************
  */
/* USER CODE END Header */

#include <imu.h>
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ── Live Expression globals ─────────────────────────────────────────── */
IMUData_t        imu_data;
uint8_t          imu_ok          = 0;
uint8_t          found_addr      = 0;
volatile uint8_t sw_state        = 0;  /* bit0=SW1 bit1=SW2 bit2=SW3 bit3=SW4 */
volatile uint8_t docking_detected = 0; /* 1 = at least one switch triggered   */
volatile int16_t motor_a_pwm     = 0;  /* -4199 to +4199                      */
volatile int16_t motor_b_pwm     = 0;

/* ── UART receive buffer ─────────────────────────────────────────────── */
#define RX_BUF_LEN  64
static char     rx_buf[RX_BUF_LEN];
static uint8_t  rx_byte;              /* single byte DMA target              */
static uint8_t  rx_idx   = 0;
static uint8_t  cmd_ready = 0;

void SystemClock_Config(void);
static void UART_Print(const char *str);
static void parse_command(const char *line);

/* ── Motor helpers ───────────────────────────────────────────────────── */

/* Set motor A, speed -100 to +100 percent */
static void motorA_set(int8_t pct)
{
    uint16_t duty = (uint16_t)(abs(pct) * 4199 / 100);
    if (duty > 4199) duty = 4199;

    if (pct > 0) {
        HAL_GPIO_WritePin(M_A_R_EN_GPIO_Port, M_A_R_EN_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(M_A_L_EN_GPIO_Port, M_A_L_EN_Pin, GPIO_PIN_RESET);
    } else if (pct < 0) {
        HAL_GPIO_WritePin(M_A_R_EN_GPIO_Port, M_A_R_EN_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(M_A_L_EN_GPIO_Port, M_A_L_EN_Pin, GPIO_PIN_SET);
    } else {
        /* Coast */
        HAL_GPIO_WritePin(M_A_R_EN_GPIO_Port, M_A_R_EN_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(M_A_L_EN_GPIO_Port, M_A_L_EN_Pin, GPIO_PIN_RESET);
        duty = 0;
    }
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, duty);
    motor_a_pwm = (pct >= 0) ? (int16_t)duty : -(int16_t)duty;
}

/* Set motor B, speed -100 to +100 percent */
static void motorB_set(int8_t pct)
{
    uint16_t duty = (uint16_t)(abs(pct) * 4199 / 100);
    if (duty > 4199) duty = 4199;

    if (pct > 0) {
        HAL_GPIO_WritePin(M_B_R_EN_GPIO_Port, M_B_R_EN_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(M_B_L_EN_GPIO_Port, M_B_L_EN_Pin, GPIO_PIN_RESET);
    } else if (pct < 0) {
        HAL_GPIO_WritePin(M_B_R_EN_GPIO_Port, M_B_R_EN_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(M_B_L_EN_GPIO_Port, M_B_L_EN_Pin, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(M_B_R_EN_GPIO_Port, M_B_R_EN_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(M_B_L_EN_GPIO_Port, M_B_L_EN_Pin, GPIO_PIN_RESET);
        duty = 0;
    }
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, duty);
    motor_b_pwm = (pct >= 0) ? (int16_t)duty : -(int16_t)duty;
}

static void motors_stop(void)
{
    motorA_set(0);
    motorB_set(0);
}

/* ── UART helpers ────────────────────────────────────────────────────── */
static void UART_Print(const char *str)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)str, (uint16_t)strlen(str), 200);
}

/* Parse "$CMD,+80,+80" from Jetson */
static void parse_command(const char *line)
{
    if (strncmp(line, "$CMD,", 5) != 0) return;

    int a = 0, b = 0;
    if (sscanf(line + 5, "%d,%d", &a, &b) == 2) {
        /* Clamp to -100..+100 */
        if (a >  100) a =  100;
        if (a < -100) a = -100;
        if (b >  100) b =  100;
        if (b < -100) b = -100;

        /* Safety: ignore commands if docking already detected */
        if (!docking_detected) {
            motorA_set((int8_t)a);
            motorB_set((int8_t)b);
        }
    }
}

/* ── UART RX callback (byte by byte) ────────────────────────────────── */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance != USART2) return;

    char c = (char)rx_byte;

    if (c == '\n') {
        rx_buf[rx_idx] = '\0';
        cmd_ready = 1;
        rx_idx = 0;
    } else if (c != '\r') {
        if (rx_idx < RX_BUF_LEN - 1)
            rx_buf[rx_idx++] = c;
    }

    /* Re-arm receive */
    HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
}

int main(void)
{
    /* ── Init ────────────────────────────────────────────────────────── */
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_TIM1_Init();
    MX_USART2_UART_Init();

    HAL_Delay(300);

    /* ── Motors safe state ───────────────────────────────────────────── */
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    __HAL_TIM_MOE_ENABLE(&htim1);   /* TIM1 advanced timer — must enable */
    motors_stop();

    /* ── Start UART receive interrupt ───────────────────────────────── */
    HAL_UART_Receive_IT(&huart2, &rx_byte, 1);

    /* ── I2C scan ────────────────────────────────────────────────────── */
    for (uint8_t addr = 1; addr < 128; addr++) {
        if (HAL_I2C_IsDeviceReady(&hi2c1,
                (uint16_t)(addr << 1), 2, 10) == HAL_OK) {
            found_addr = addr;
        }
    }

    /* ── IMU Init ────────────────────────────────────────────────────── */
    if (found_addr == 0x68) {
        if (IMU_Init() == HAL_OK) {
            imu_ok = 1;
            for (int i = 0; i < 6; i++) {
                HAL_GPIO_TogglePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
                HAL_Delay(100);
            }
        }
    } else {
        for (int i = 0; i < 10; i++) {
            HAL_GPIO_TogglePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
            HAL_Delay(80);
        }
    }

    UART_Print("#BOOT arm_controller ready\r\n");

    /* ── State ───────────────────────────────────────────────────────── */
    uint32_t last_imu_ms  = 0;
    uint32_t last_tx_ms   = 0;
    uint32_t last_rein_ms = 0;
    uint8_t  imu_fails    = 0;
    char     tx_buf[128];

    /* ── Main Loop ───────────────────────────────────────────────────── */
    while (1)
    {
        uint32_t now = HAL_GetTick();

        /* ── Read limit switches (active LOW) ────────────────────── */
        uint8_t s1 = !HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin);
        uint8_t s2 = !HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin);
        uint8_t s3 = !HAL_GPIO_ReadPin(SW3_GPIO_Port, SW3_Pin);
        uint8_t s4 = !HAL_GPIO_ReadPin(SW4_GPIO_Port, SW4_Pin);

        sw_state = (s4 << 3) | (s3 << 2) | (s2 << 1) | s1;
        docking_detected = (sw_state != 0) ? 1 : 0;

        /* ── Auto-stop motors on docking contact ─────────────────── */
        if (docking_detected) {
            motors_stop();
        }

        /* ── Read IMU @ 50 Hz ────────────────────────────────────── */
        if ((now - last_imu_ms) >= 20)
        {
            last_imu_ms = now;
            if (imu_ok) {
                IMUData_t local;
                if (IMU_Read(&local) == HAL_OK) {
                    imu_fails = 0;
                    __disable_irq();
                    imu_data = local;
                    __enable_irq();
                } else {
                    if (++imu_fails >= 10) imu_ok = 0;
                }
            } else if ((now - last_rein_ms) >= 3000) {
                last_rein_ms = now;
                imu_fails = 0;
                imu_ok = (IMU_Init() == HAL_OK) ? 1 : 0;
            }
        }

        /* ── Transmit to Jetson @ 20 Hz ──────────────────────────── */
        if ((now - last_tx_ms) >= 50)
        {
            last_tx_ms = now;
            snprintf(tx_buf, sizeof(tx_buf),
                "$ARM,%+.3f,%+.3f,%+.3f,%+.3f,%+.3f,%+.3f,%d,%d,%d,%d\r\n",
                imu_data.accel_x,
                imu_data.accel_y,
                imu_data.accel_z,
                imu_data.gyro_x,
                imu_data.gyro_y,
                imu_data.gyro_z,
                s1, s2, s3, s4);
            UART_Print(tx_buf);
        }

        /* ── Process received command from Jetson ────────────────── */
        if (cmd_ready) {
            cmd_ready = 0;
            parse_command(rx_buf);
        }

        /* ── Heartbeat LED ───────────────────────────────────────── */
        HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin,
            (now % 1000) < 500 ? GPIO_PIN_RESET : GPIO_PIN_SET);

        HAL_Delay(5);
    }
}

/* ── Clock: HSI 16MHz → PLL → 84MHz ─────────────────────────────────── */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM            = 8;   /* 16MHz/8 = 2MHz      */
    RCC_OscInitStruct.PLL.PLLN            = 168; /* 2×168 = 336MHz VCO  */
    RCC_OscInitStruct.PLL.PLLP            = RCC_PLLP_DIV4; /* 336/4=84MHz */
    RCC_OscInitStruct.PLL.PLLQ            = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) Error_Handler();

    RCC_ClkInitStruct.ClockType           = RCC_CLOCKTYPE_HCLK  | RCC_CLOCKTYPE_SYSCLK
                                          | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource        = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider       = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider      = RCC_HCLK_DIV2;  /* 42MHz max */
    RCC_ClkInitStruct.APB2CLKDivider      = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) Error_Handler();
}

void Error_Handler(void)
{
    __disable_irq();
    while (1) {}
}
