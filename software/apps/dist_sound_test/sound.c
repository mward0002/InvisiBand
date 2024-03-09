#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "nrf.h"
#include "nrf_delay.h"
#include "nrfx_pwm.h"
#include "app_timer.h"
#include "nrfx_saadc.h"
#include "nrfx_timer.h"

#include "microbit_v2.h"

#define SOUND_PIN EDGE_P2

static const nrfx_pwm_t speaker_pwm = NRFX_PWM_INSTANCE(0);

nrf_pwm_values_common_t sequence_data[1] = {0};

nrf_pwm_sequence_t pwm_sequence = {
    .values.p_common = sequence_data,
    .length = 1,
    .repeats = 0,
    .end_delay = 0,
};

void play_tone(uint32_t freq[], uint32_t max_dist, uint32_t dist, uint8_t volume) {
    size_t num_frequencies = sizeof(freq) / sizeof(freq[0]);

    nrfx_pwm_stop(&speaker_pwm, true);

    if (dist > max_dist) {
        return;
    }

    uint32_t freq_index = (num_frequencies * dist) / max_dist;

    uint32_t countertop = 1000000 / freq[freq_index];
    NRF_PWM0->COUNTERTOP = countertop;

    uint32_t duty_cycle = ((countertop / 2) * volume) / 100;

    sequence_data[0] = duty_cycle;
    nrfx_pwm_simple_playback(&speaker_pwm, &pwm_sequence, 1, NRFX_PWM_FLAG_LOOP);
}

void stop_tone(void) {
    nrfx_pwm_stop(&speaker_pwm, true);
}