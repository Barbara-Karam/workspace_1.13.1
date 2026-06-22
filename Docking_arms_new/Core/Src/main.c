/**
  ******************************************************************************
  * @file    main.c
  * @brief   RPOD Lifter - Low Level Controller
  *          Hardware: STM32F401RCT6 (Black Pill)
  *          Clock:    HSE 25MHz -> PLL -> 84MHz
  *
  *  Responsibilities:
  *    - 2x TQ42-772 DC motors via BTS7960 (TIM1, 1kHz PWM)
  *    - 1x IMU via I2C1 (MPU6050 or ICM-42688, selectable in imu.h)
  *    - 4x Limit switches via EXTI (PC0-PC3, active-low)
  *    - Binary-framed UART to Jetson Orin Nano (921600 baud, DMA)
  *    - 50Hz main control loop (20ms tick via HAL_GetTick)
  *
  * ── STANDALONE TEST MODE ──────────────────────────────────────────────────
  *  Define STANDALONE_TEST below to run without a Jetson.
  *  Both motors will cycle: forward -> stop -> reverse -> stop, repeat.
  *  Limit switches still override and coast motors immediately.
  *  UART TX runs unconditionally every tick in all modes — IMU telemetry
  *  is always live regardless of motor state or test mode.
  *  Comment out #define STANDALONE_TEST to restore Jetson-controlled mode.
  * ─────────────────────────────────────────────────────────────────────────
  ******************************************************************************
  */
#include "main.h"
#include "dma.h"
#include "gpio.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "motor_control.h"
#include "limit_switch.h"
#include "imu.h"
#include "comm.h"

/* ── Standalone test toggle ──────────────────────────────────────────────── */
#define STANDALONE_TEST

#ifdef STANDALONE_TEST
  #define TEST_RUN_MS    2000u   /* How long motors run in each direction  */
  #define TEST_STOP_MS   1000u   /* How long motors pause between phases   */
  #define TEST_PWM_DUTY  2000    /* PWM duty for test (0-4199)             */

  typedef enum {
      TEST_PHASE_FORWARD = 0,
      TEST_PHASE_STOP_A,
      TEST_PHASE_REVERSE,
      TEST_PHASE_STOP_B,
  } TestPhase_t;
#endif

void SystemClock_Config(void);

/* Shared with comm.c */
extern volatile int16_t  g_target_pwm[2];
extern volatile uint8_t  g_estop_active;
extern IMU_Data_t         g_imu_data;

int main(void)
{
    /* ── HAL + Clock ──────────────────────────────────────────────────── */
    HAL_Init();
    SystemClock_Config();

    /* ── Peripheral Init ─────────────────────────────────────────────── */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_USART2_UART_Init();
    MX_I2C1_Init();
    MX_TIM1_Init();

    /* ── Driver Init ─────────────────────────────────────────────────── */
    HAL_Delay(200);             /* Let power rails settle               */

    Motor_Init();               /* Starts TIM1 PWM, coasts all motors   */
    LimitSwitch_Init();         /* No-op currently; EXTI set in gpio.c  */

    /* IMU init - non-fatal: imu_valid=0 is reported in telemetry if it fails */
    if (IMU_Init() != HAL_OK) {
        g_imu_data.valid = 0;
    }

    Comm_Init();                /* Arm DMA idle-line UART RX             */

    /* ── Main Loop ───────────────────────────────────────────────────── */
    uint32_t last_loop_ms = HAL_GetTick();

#ifdef STANDALONE_TEST
    TestPhase_t test_phase     = TEST_PHASE_FORWARD;
    uint32_t    phase_start_ms = HAL_GetTick();
#endif

    while (1)
    {
        uint32_t now = HAL_GetTick();

        /* ── 50Hz Control Loop (every 20ms) ─────────────────────────── */
        if (now - last_loop_ms >= 20)
        {
            last_loop_ms = now;

            /* 1. Read IMU — always, independent of motor state */
            IMU_Read(&g_imu_data);

            /* 2. Limit switches — highest priority, always override */
            uint8_t sw_mask = LimitSwitch_ReadAll();

            if (sw_mask != 0)
            {
                Motor_CoastAll();
            }

#ifdef STANDALONE_TEST
            /* ── Standalone test sequencer ──────────────────────────── */
            else
            {
                uint32_t elapsed = now - phase_start_ms;

                switch (test_phase)
                {
                    case TEST_PHASE_FORWARD:
                        Motor_Set(0,  TEST_PWM_DUTY);
                        Motor_Set(1,  TEST_PWM_DUTY);
                        if (elapsed >= TEST_RUN_MS) {
                            Motor_CoastAll();
                            test_phase     = TEST_PHASE_STOP_A;
                            phase_start_ms = now;
                        }
                        break;

                    case TEST_PHASE_STOP_A:
                        if (elapsed >= TEST_STOP_MS) {
                            test_phase     = TEST_PHASE_REVERSE;
                            phase_start_ms = now;
                        }
                        break;

                    case TEST_PHASE_REVERSE:
                        Motor_Set(0, -TEST_PWM_DUTY);
                        Motor_Set(1, -TEST_PWM_DUTY);
                        if (elapsed >= TEST_RUN_MS) {
                            Motor_CoastAll();
                            test_phase     = TEST_PHASE_STOP_B;
                            phase_start_ms = now;
                        }
                        break;

                    case TEST_PHASE_STOP_B:
                        if (elapsed >= TEST_STOP_MS) {
                            test_phase     = TEST_PHASE_FORWARD;
                            phase_start_ms = now;
                        }
                        break;
                }
            }

#else
            /* ── Jetson-controlled mode ──────────────────────────────── */
            else if (!g_estop_active)
            {
                Motor_Set(0, g_target_pwm[0]);
                Motor_Set(1, g_target_pwm[1]);
            }
            else
            {
                Motor_CoastAll();
            }
#endif /* STANDALONE_TEST */

            /* 3. Transmit telemetry — unconditional, every tick.
             *    IMU data and motor state are already updated above.
             *    This does NOT depend on whether motors moved or coasted. */
            Comm_TransmitFeedback();
        }

        /* ── Handle any pending RX processing (heartbeat reply) ─────── */
        Comm_ProcessRx();

        /* ── Status LED ──────────────────────────────────────────────── */
        /* Standalone: 2Hz. Normal: 1Hz. E-stop: 10Hz.                   */
#ifdef STANDALONE_TEST
        uint32_t blink_period = 500u;
#else
        uint32_t blink_period = g_estop_active ? 100u : 1000u;
#endif
        HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin,
                          (now % blink_period) < (blink_period / 2)
                          ? GPIO_PIN_RESET
                          : GPIO_PIN_SET);
    }
}

/* ── System Clock: HSE 25MHz -> PLL -> 84MHz ────────────────────────────── */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM       = 25;
    RCC_OscInitStruct.PLL.PLLN       = 336;
    RCC_OscInitStruct.PLL.PLLP       = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ       = 4;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) { Error_Handler(); }

    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK  | RCC_CLOCKTYPE_SYSCLK
                                     | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) { Error_Handler(); }
}

void Error_Handler(void)
{
    __disable_irq();
    while (1) {
        HAL_GPIO_TogglePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
        for (volatile uint32_t i = 0; i < 1000000; i++);
    }
}
