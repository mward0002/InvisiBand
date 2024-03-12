// HR_SC04 Testing

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "app_timer.h"
#include "nrf_delay.h"
#include "nrfx_saadc.h"
#include "nrfx_timer.h"


#include "microbit_v2.h"

#include "hr_sc04.h"

// Global variables
// APP_TIMER_DEF(sample_timer);
static nrfx_timer_t TIMER4 = NRFX_TIMER_INSTANCE(0);

// Function prototypes
static void gpio_init(void);
static void timer_init(void); 

static uint32_t start_time;

void timer_event_handler(nrf_timer_event_t event_type, void* p_context) {
    // Not used, but required for timer initialization
}

//measure dist with trig, echo, and timer_channel passed in
 float hr_sc04_measure_dist(uint32_t trig, uint32_t echo, nrf_timer_cc_channel_t timer_channel){
    float distance;
    uint32_t duration = 0;

    nrf_gpio_pin_write(trig, 1);
    nrf_delay_us(10);
    nrf_gpio_pin_write(trig, 0);

    while(nrf_gpio_pin_read(echo) == 0);
    start_time = nrfx_timer_capture(&TIMER4, timer_channel);
    
    while(nrf_gpio_pin_read(echo) == 1);
    duration = nrfx_timer_capture(&TIMER4, timer_channel) - start_time;

    distance = (duration / 58);

    return distance;
}

//Initialize a trig and echo
 void gpio_edge_init(uint32_t trig, uint32_t echo) {
  // Initialize pins
  nrf_gpio_cfg_output(trig);
  nrf_gpio_cfg_input(echo, NRF_GPIO_PIN_NOPULL);
  nrf_gpio_pin_clear(trig);

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
  printf("timer is initialized");

  // Start Timer4
  nrfx_timer_enable(&TIMER4);
  printf("timer has started");
}

void hr_sc04_init(void){
    timer_init();
}



