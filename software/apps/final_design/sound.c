#include "sound.h"

static const nrfx_pwm_t speaker_pwm = NRFX_PWM_INSTANCE(0);

nrf_pwm_values_common_t sequence_data[1] = {0};

nrf_pwm_sequence_t pwm_sequence = {
    .values.p_common = sequence_data,
    .length = 1,
    .repeats = 0,
    .end_delay = 0,
};

void pwm_init(uint32_t output_pin) {
    nrfx_pwm_config_t config0 = {
        .output_pins[0] = output_pin,
        .output_pins[1] = NRFX_PWM_PIN_NOT_USED,
        .output_pins[2] = NRFX_PWM_PIN_NOT_USED,
        .output_pins[3] = NRFX_PWM_PIN_NOT_USED,
        .base_clock = NRF_PWM_CLK_1MHz,
        .count_mode = NRF_PWM_MODE_UP,
        .top_value = 1000,
        .load_mode = NRF_PWM_LOAD_COMMON,
        .step_mode = NRF_PWM_STEP_AUTO
    };

    nrfx_pwm_init(&speaker_pwm, &config0, NULL);
    printf("pwm initialized\n");
}

void play_tone(uint32_t freq[], uint8_t num_frequencies, uint32_t max_dist, uint32_t dist, uint8_t volume) {

    if (dist > max_dist) {
        return;
    }

    uint32_t freq_index = (num_frequencies * dist) / max_dist;

    uint32_t countertop = 1000000 / freq[freq_index];
    NRF_PWM0->COUNTERTOP = countertop;

    uint32_t duty_cycle = ((countertop / 2) * volume) / 100;

    sequence_data[0] = duty_cycle;
    nrfx_pwm_simple_playback(&speaker_pwm, &pwm_sequence, 1, NRFX_PWM_FLAG_LOOP);
    printf("tone played: %d\n", freq[freq_index]);
}

void stop_tone(void) {
    nrfx_pwm_stop(&speaker_pwm, true);
}