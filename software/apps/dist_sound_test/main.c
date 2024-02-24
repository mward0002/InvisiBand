
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

#define TRIG1 EDGE_P0
#define ECHO1 EDGE_P1
#define TRIG2 EDGE_P5
#define ECHO2 EDGE_P6
#define TRIG3 EDGE_P11
#define ECHO3 EDGE_P12

#define SOUND_PIN EDGE_P2

// 
// sound util 
//

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


// 
// distance util
// 

// Global variables
// APP_TIMER_DEF(sample_timer);
static nrfx_timer_t TIMER4 = NRFX_TIMER_INSTANCE(0);
//static nrfx_timer_t TIMER3 = NRFX_TIMER_INSTANCE(1);
//static nrfx_timer_t TIMER2 = NRFX_TIMER_INSTANCE(2);

// Function prototypes
static void gpio_init(void);
static void timer_init(void); 

static uint32_t start_time;

void timer_event_handler(nrf_timer_event_t event_type, void* p_context) {
    // Not used, but required for timer initialization
}


static uint32_t measure_dist1(void){
    uint32_t distance;
    uint32_t duration = 0;

    nrf_gpio_pin_write(TRIG1, 1);
    nrf_delay_us(10);
    nrf_gpio_pin_write(TRIG1, 0);

    while(nrf_gpio_pin_read(ECHO1) == 0);
    start_time = nrfx_timer_capture(&TIMER4, NRF_TIMER_CC_CHANNEL0);
    
    while(nrf_gpio_pin_read(ECHO1) == 1);
    duration = nrfx_timer_capture(&TIMER4, NRF_TIMER_CC_CHANNEL0) - start_time;
    //printf("duration: %ld \n", duration);


    distance = (duration / 58);

    return distance;
}

static uint32_t measure_dist2(void){
    uint32_t distance;
    uint32_t duration = 0;

    nrf_gpio_pin_write(TRIG2, 1);
    nrf_delay_us(10);
    nrf_gpio_pin_write(TRIG2, 0);

    while(nrf_gpio_pin_read(ECHO2) == 0);
    start_time = nrfx_timer_capture(&TIMER4, NRF_TIMER_CC_CHANNEL1);
    
    while(nrf_gpio_pin_read(ECHO2) == 1);
    duration = nrfx_timer_capture(&TIMER4, NRF_TIMER_CC_CHANNEL1) - start_time;
    //printf("duration: %ld \n", duration);

    distance = (duration / 58);

    return distance;
}

static uint32_t measure_dist3(void){
    uint32_t distance;
    uint32_t duration = 0;

    nrf_gpio_pin_write(TRIG3, 1);
    nrf_delay_us(10);
    nrf_gpio_pin_write(TRIG3, 0);

    while(nrf_gpio_pin_read(ECHO3) == 0);
    start_time = nrfx_timer_capture(&TIMER4, NRF_TIMER_CC_CHANNEL2);
    
    while(nrf_gpio_pin_read(ECHO3) == 1);
    duration = nrfx_timer_capture(&TIMER4, NRF_TIMER_CC_CHANNEL2) - start_time;
    //printf("duration: %ld \n", duration);

    distance = (duration / 58);

    return distance;
}

static void gpio_init(void) {
  // Initialize pins
  nrf_gpio_cfg_output(TRIG1);
  nrf_gpio_cfg_input(ECHO1, NRF_GPIO_PIN_NOPULL);
  nrf_gpio_pin_clear(TRIG1);
  nrf_gpio_cfg_output(TRIG2);
  nrf_gpio_cfg_input(ECHO2, NRF_GPIO_PIN_NOPULL);
  nrf_gpio_pin_clear(TRIG2);
  nrf_gpio_cfg_output(TRIG3);
  nrf_gpio_cfg_input(ECHO3, NRF_GPIO_PIN_NOPULL);
  nrf_gpio_pin_clear(TRIG3);
}

static void timer_init(void) {
  nrfx_timer_config_t timer_config = {
      .frequency = NRF_TIMER_FREQ_1MHz,
      .mode = NRF_TIMER_MODE_TIMER,
      .bit_width = NRF_TIMER_BIT_WIDTH_32,
      .interrupt_priority = 0,
      .p_context = NULL
  };
  nrfx_timer_init(&TIMER4, &timer_config, timer_event_handler);
  //nrfx_timer_init(&TIMER3, &timer_config, timer_event_handler);
  //nrfx_timer_init(&TIMER2, &timer_config, timer_event_handler);

  // Start Timer4
  nrfx_timer_enable(&TIMER4);
  //nrfx_timer_enable(&TIMER3);
  //nrfx_timer_enable(&TIMER2);
  printf("timer init \n");
}

int main(void) {
    pwm_init(SOUND_PIN);
    gpio_init();
    timer_init();

    printf("Board started!\n");

    while (1) {
        printf("Distance 1: %ld cm\n", measure_dist1());
        uint32_t distance = measure_dist2();

        printf("Distance 2: %ld cm\n", measure_dist2());
        printf("Distance 3: %ld cm\n", measure_dist3());
        nrf_delay_ms(10);

        if (distance < 10) {
            play_tone(440, 10);
        } 
        else if (distance < 20) {
            play_tone(493, 10);
        }
        else if (distance < 30) {
            play_tone(523, 10);
        }
        else if (distance < 40) {
            play_tone(587, 10);
        }
        else if (distance < 50) {
            play_tone(659, 10);
        }
        else{
            stop_tone();
        }
    }
}