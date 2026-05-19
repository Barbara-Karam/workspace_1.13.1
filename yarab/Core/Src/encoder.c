#include "encoder.h"
#include "tim.h"

// ─── Timer Map ────────────────────────────────────────────────────────────────
// Matches physical wiring:
//   Encoder 0 (Motor 1): PA5/PB3  → TIM2 (32-bit)
//   Encoder 1 (Motor 2): PA6/PA7  → TIM3 (16-bit)
//   Encoder 2 (Motor 3): PB6/PB7  → TIM4 (16-bit)
//   Encoder 3 (Motor 4): PA0/PA1  → TIM5 (32-bit)

static TIM_HandleTypeDef *enc_tim[ENCODER_COUNT] = {
    &htim2,
    &htim3,
    &htim4,
    &htim5
};

// TIM2 and TIM5 are 32-bit, TIM3 and TIM4 are 16-bit
// We need to handle 16-bit overflow for TIM3/TIM4
static const uint8_t is_32bit[ENCODER_COUNT] = { 1, 0, 0, 1 };

static EncoderState_t enc[ENCODER_COUNT];

// ─── Init ────────────────────────────────────────────────────────────────────
void Encoder_Init(void)
{
    HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_ALL);

    for (uint8_t i = 0; i < ENCODER_COUNT; i++) {
        enc[i].count        = 0;
        enc[i].count_prev   = 0;
        enc[i].delta        = 0;
        enc[i].velocity_rpm = 0.0f;
        enc[i].last_raw     = __HAL_TIM_GET_COUNTER(enc_tim[i]);
    }
}

// ─── Update — Called Every 10ms From TIM10 ISR ───────────────────────────────
void Encoder_Update(void)
{
    for (uint8_t i = 0; i < ENCODER_COUNT; i++) {

        uint32_t raw = __HAL_TIM_GET_COUNTER(enc_tim[i]);

        if (is_32bit[i]) {
            // 32-bit timer: cast to signed handles wrap automatically
            int32_t delta_raw = (int32_t)(raw - enc[i].last_raw);
            enc[i].count += delta_raw;
        } else {
            // 16-bit timer: use int16_t cast to handle signed overflow
            // This correctly handles wrap at 0 and 65535
            int16_t delta_raw = (int16_t)((uint16_t)raw -
                                           (uint16_t)enc[i].last_raw);
            enc[i].count += (int32_t)delta_raw;
        }

        enc[i].last_raw = raw;

        // Delta since last control tick
        enc[i].delta      = enc[i].count - enc[i].count_prev;
        enc[i].count_prev = enc[i].count;

        // RPM = (delta_counts / CPR) / dt * 60
        enc[i].velocity_rpm = ((float)enc[i].delta / ENCODER_CPR)
                               / CONTROL_DT * 60.0f;
    }
}

// ─── Getters ─────────────────────────────────────────────────────────────────
int32_t Encoder_GetCount(uint8_t id)
{
    if (id >= ENCODER_COUNT) return 0;
    return enc[id].count;
}

float Encoder_GetRPM(uint8_t id)
{
    if (id >= ENCODER_COUNT) return 0.0f;
    return enc[id].velocity_rpm;
}

// ─── Reset Single ────────────────────────────────────────────────────────────
void Encoder_Reset(uint8_t id)
{
    if (id >= ENCODER_COUNT) return;
    __HAL_TIM_SET_COUNTER(enc_tim[id], 0);
    enc[id].count        = 0;
    enc[id].count_prev   = 0;
    enc[id].delta        = 0;
    enc[id].velocity_rpm = 0.0f;
    enc[id].last_raw     = 0;
}

// ─── Reset All ───────────────────────────────────────────────────────────────
void Encoder_ResetAll(void)
{
    for (uint8_t i = 0; i < ENCODER_COUNT; i++) {
        Encoder_Reset(i);
    }
}
