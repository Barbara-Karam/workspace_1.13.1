/**
 * @file    main.c
 * @brief   RPOD Lifter - Low Level Controller
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

//#define STANDALONE_TEST
#ifdef STANDALONE_TEST
#define TEST_RUN_MS    2000u
#define TEST_STOP_MS   1000u
#define TEST_PWM_DUTY  2000
typedef enum {
    TEST_PHASE_FORWARD = 0,
    TEST_PHASE_STOP_A,
    TEST_PHASE_REVERSE,
    TEST_PHASE_STOP_B,
} TestPhase_t;
#endif

/* How long to wait between IMU reinit attempts after a persistent failure.
 * Without this, a dead sensor causes IMU_Init() to be called every loop
 * iteration (~50 Hz), burning I2C bus time and delaying motor updates. */
#define IMU_RESET_COOLDOWN_MS  500u

void SystemClock_Config(void);

extern volatile int16_t  g_target_pwm[2];
extern volatile uint8_t  g_estop_active;
extern IMU_Data_t        g_imu_data;
extern volatile uint8_t  g_imu_needs_reset;

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_DMA_Init();
    MX_USART2_UART_Init();
    MX_I2C1_Init();
    MX_TIM1_Init();

    HAL_Delay(200);
    Motor_Init();
    LimitSwitch_Init();

    if (IMU_Init() != HAL_OK) {
        g_imu_data.valid = 0;
    }
    Comm_Init();

    uint32_t last_loop_ms      = HAL_GetTick();
    uint32_t last_imu_reset_ms = 0u;   /* timestamp of last reinit attempt */

    #ifdef STANDALONE_TEST
    TestPhase_t test_phase     = TEST_PHASE_FORWARD;
    uint32_t    phase_start_ms = HAL_GetTick();
    #endif

    while (1)
    {
        uint32_t now = HAL_GetTick();

        /* ── IMU recovery ────────────────────────────────────────────────────
         * Fix #6: gate reinit attempts behind a cooldown period so that a
         * persistently-dead sensor does not monopolise the I2C bus and starve
         * the 20 ms control loop.  Without this the flag is re-set inside
         * IMU_Init() on every iteration (~50 calls/s) which also means
         * IMU_Read() is never reached and g_imu_data stays stale forever. */
        if (g_imu_needs_reset &&
            (now - last_imu_reset_ms) >= IMU_RESET_COOLDOWN_MS)
        {
            g_imu_needs_reset  = 0;
            last_imu_reset_ms  = now;

            HAL_I2C_DeInit(&hi2c1);
            MX_I2C1_Init();
            if (IMU_Init() != HAL_OK)
            {
                /* Sensor still not responding — mark invalid and wait for
                 * the next cooldown window before trying again. */
                g_imu_data.valid  = 0;
                g_imu_needs_reset = 1;
            }
        }

        if (now - last_loop_ms >= 20)
        {
            last_loop_ms = now;

            IMU_Read(&g_imu_data);

            uint8_t sw_mask = LimitSwitch_ReadAll();
            if (sw_mask != 0)
            {
                Motor_CoastAll();
            }
#ifdef STANDALONE_TEST
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
            else if (!g_estop_active)
            {
                __disable_irq();
                int16_t pwm0 = g_target_pwm[0];
                int16_t pwm1 = g_target_pwm[1];
                __enable_irq();

                Motor_Set(0, pwm0);
                Motor_Set(1, pwm1);
            }
            else
            {
                Motor_CoastAll();
            }
#endif /* STANDALONE_TEST */

            Comm_TransmitFeedback();
        }

        Comm_ProcessRx();

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
