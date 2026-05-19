#include "motor_control.h"
#include "tim.h"
#include "main.h"

// ─── Hardware Map ─────────────────────────────────────────────────────────────
typedef struct {
    TIM_HandleTypeDef *htim;
    uint32_t           channel;
    GPIO_TypeDef      *r_en_port;
    uint16_t           r_en_pin;
    GPIO_TypeDef      *l_en_port;
    uint16_t           l_en_pin;
} MotorHW_t;

static const MotorHW_t hw[MOTOR_COUNT] = {
    // Motor 0: PA8(PWM), PB12(R_EN), PB13(L_EN)
    { &htim1, TIM_CHANNEL_1, GPIOB, GPIO_PIN_12, GPIOB, GPIO_PIN_13 },
    // Motor 1: PA9(PWM), PB14(R_EN), PB15(L_EN)
    { &htim1, TIM_CHANNEL_2, GPIOB, GPIO_PIN_14, GPIOB, GPIO_PIN_15 },
    // Motor 2: PA10(PWM), PB4(R_EN), PB5(L_EN)
    { &htim1, TIM_CHANNEL_3, GPIOB, GPIO_PIN_4,  GPIOB, GPIO_PIN_5  },
    // Motor 3: PA11(PWM), PB0(R_EN), PB1(L_EN)
    { &htim1, TIM_CHANNEL_4, GPIOB, GPIO_PIN_0,  GPIOB, GPIO_PIN_1  },
};

static MotorState_t state[MOTOR_COUNT];

// ─── Private Helpers ─────────────────────────────────────────────────────────
static void set_en(uint8_t id, bool r, bool l)
{
    HAL_GPIO_WritePin(hw[id].r_en_port, hw[id].r_en_pin,
                      r ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(hw[id].l_en_port, hw[id].l_en_pin,
                      l ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void set_pwm(uint8_t id, uint16_t duty)
{
    __HAL_TIM_SET_COMPARE(hw[id].htim, hw[id].channel, duty);
}

// ─── Init ────────────────────────────────────────────────────────────────────
void Motor_Init(void)
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);

    for (uint8_t i = 0; i < MOTOR_COUNT; i++) {
        set_pwm(i, 0);
        set_en(i, false, false);
        state[i].direction = MOTOR_DIR_COAST;
        state[i].pwm_duty  = 0;
        state[i].enabled   = false;
    }
}

// ─── Set Motor ───────────────────────────────────────────────────────────────
void Motor_Set(uint8_t id, int16_t pwm_signed)
{
    if (id >= MOTOR_COUNT) return;

    // Clamp
    if (pwm_signed >  (int16_t)PWM_MAX) pwm_signed =  (int16_t)PWM_MAX;
    if (pwm_signed < -(int16_t)PWM_MAX) pwm_signed = -(int16_t)PWM_MAX;

    if (pwm_signed == 0) {
        Motor_Coast(id);
        return;
    }

    MotorDir_t new_dir = (pwm_signed > 0) ? MOTOR_DIR_FORWARD
                                           : MOTOR_DIR_REVERSE;
    uint16_t   new_pwm = (uint16_t)(pwm_signed > 0 ? pwm_signed : -pwm_signed);

    // Direction change guard — coast for one call cycle
    // In 100Hz loop this = 10ms dead time, safe for BTS7960
    if (state[id].direction != new_dir &&
        state[id].direction != MOTOR_DIR_COAST &&
        state[id].direction != MOTOR_DIR_BRAKE) {
        Motor_Coast(id);
        return;   // Apply new direction next cycle
    }

    // Apply
    if (new_dir == MOTOR_DIR_FORWARD) {
        set_en(id, true, false);
    } else {
        set_en(id, false, true);
    }
    set_pwm(id, new_pwm);

    state[id].direction = new_dir;
    state[id].pwm_duty  = new_pwm;
    state[id].enabled   = true;
}

// ─── Set All ─────────────────────────────────────────────────────────────────
void Motor_SetAll(int16_t pwm[MOTOR_COUNT])
{
    for (uint8_t i = 0; i < MOTOR_COUNT; i++) {
        Motor_Set(i, pwm[i]);
    }
}

// ─── Brake ───────────────────────────────────────────────────────────────────
void Motor_Brake(uint8_t id)
{
    if (id >= MOTOR_COUNT) return;
    set_pwm(id, 0);
    set_en(id, true, true);
    state[id].pwm_duty  = 0;
    state[id].direction = MOTOR_DIR_BRAKE;
    state[id].enabled   = false;
}

void Motor_BrakeAll(void)
{
    for (uint8_t i = 0; i < MOTOR_COUNT; i++) Motor_Brake(i);
}

// ─── Coast ───────────────────────────────────────────────────────────────────
void Motor_Coast(uint8_t id)
{
    if (id >= MOTOR_COUNT) return;
    set_pwm(id, 0);
    set_en(id, false, false);
    state[id].pwm_duty  = 0;
    state[id].direction = MOTOR_DIR_COAST;
    state[id].enabled   = false;
}

void Motor_CoastAll(void)
{
    for (uint8_t i = 0; i < MOTOR_COUNT; i++) Motor_Coast(i);
}

// ─── Emergency Stop — Direct Register Writes, Fastest Possible ───────────────
void Motor_EmergencyStop(void)
{
    // Zero all PWM immediately
    TIM1->CCR1 = 0;
    TIM1->CCR2 = 0;
    TIM1->CCR3 = 0;
    TIM1->CCR4 = 0;

    // Disable all EN pins — GPIOB ODR clear
    GPIOB->ODR &= ~(GPIO_PIN_0  | GPIO_PIN_1  |
                    GPIO_PIN_4  | GPIO_PIN_5  |
                    GPIO_PIN_12 | GPIO_PIN_13 |
                    GPIO_PIN_14 | GPIO_PIN_15);

    for (uint8_t i = 0; i < MOTOR_COUNT; i++) {
        state[i].pwm_duty  = 0;
        state[i].direction = MOTOR_DIR_COAST;
        state[i].enabled   = false;
    }
}

// ─── Get State ───────────────────────────────────────────────────────────────
MotorState_t Motor_GetState(uint8_t id)
{
    return state[id];
}
