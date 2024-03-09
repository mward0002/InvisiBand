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

// play_tone() -- plays a tone from the freq[] array at a given volume depending on the distance compared to max_dist
void play_tone(uint32_t freq[], uint8_t num_frequencies, uint32_t max_dist, uint32_t dist, uint8_t volume);

// stop_tone() -- stops the tone being played
void stop_tone(void);