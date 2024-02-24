// test code for getting speaker amp to work


#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf.h"
#include "nrf_delay.h"
#include "nrfx_pwm.h"

#include "microbit_v2.h"

static const nrfx_pwm_t pwm0 = NRFX_PWM_INSTANCE(0);

nrf_pwm_values_common_t sequence_data[1] = {0};

nrf_pwm_sequence_t pwm_sequence = {
  .values.p_common = sequence_data,
  .length = 1,
  .repeats = 0,
  .end_delay = 0,
};

void pwm_init(uint32_t output_pin) {
    nrfx_pwm_config_t config0 = {
        .output_pins[0] = output_pin | NRFX_PWM_PIN_INVERTED, 
        .output_pins[1] = NRFX_PWM_PIN_NOT_USED,
        .output_pins[2] = NRFX_PWM_PIN_NOT_USED,
        .output_pins[3] = NRFX_PWM_PIN_NOT_USED,
        .base_clock = NRF_PWM_CLK_1MHz,
        .count_mode = NRF_PWM_MODE_UP,
        .top_value = 1000,
        .load_mode = NRF_PWM_LOAD_COMMON,
        .step_mode = NRF_PWM_STEP_AUTO
    };
    
    nrfx_pwm_init(&pwm0, &config0, NULL);
}

void play_tone(uint16_t frequency, uint8_t volume) {
    nrfx_pwm_stop(&pwm0, true);

    NRF_PWM0->COUNTERTOP = 1000000 / frequency;

    uint32_t duty_cycle = ((NRF_PWM0->COUNTERTOP / 2) * volume) / 100;
    sequence_data[0] = duty_cycle;
    nrfx_pwm_simple_playback(&pwm0, &pwm_sequence, 1, NRFX_PWM_FLAG_LOOP);
}

void stop_tone() {
    nrfx_pwm_stop(&pwm0, true);
}

int main(void) {
    printf("Board started!\n");

    pwm_init(EDGE_P2);

    printf("Playing an A\n");

    play_tone(440, 10);

    nrf_delay_ms(1000);

    stop_tone();
    
    printf("Done\n");
    return 0;
}