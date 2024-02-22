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


#define TRIG EDGE_P5
#define ECHO EDGE_P6

// Global variables
// APP_TIMER_DEF(sample_timer);
static nrfx_timer_t TIMER4 = NRFX_TIMER_INSTANCE(0);

// Function prototypes
static void gpio_init(void);

static uint32_t start_time;

static uint32_t measure_dist(void){
    uint32_t distance;
    uint32_t duration = 0;


    nrf_gpio_pin_write(TRIG, 1);
    //printf("State of TRIG: %d\n", nrf_gpio_pin_out_read(TRIG));
    nrf_delay_us(10);
    nrf_gpio_pin_write(TRIG, 0);
    //printf("State of TRIG: %d\n", nrf_gpio_pin_out_read(TRIG));

  

    while(nrf_gpio_pin_read(ECHO) == 0);

    //uint32_t start_time = app_timer_cnt_get();
    start_time = nrfx_timer_capture(&TIMER4, NRF_TIMER_CC_CHANNEL0);
    while(nrf_gpio_pin_read(ECHO) == 1);

    //uint32_t end_time = app_timer_cnt_get();
    //uint32_t end_time = 
    //printf("time test: %ld \n", end_time);
    duration = nrfx_timer_capture(&TIMER4, NRF_TIMER_CC_CHANNEL0) - start_time;
    //duration = end_time - start_time;
    printf("duration: %ld \n", duration);

    distance = (duration / 58);

    return distance;
}

// static void sample_timer_callback(void* _unused) {
//   uint32_t distance = measure_dist();
//   printf("Distance: %ld cm \n", distance);
// }

static void gpio_init(void) {
  // Initialize pins
  nrf_gpio_cfg_output(TRIG);
  nrf_gpio_cfg_input(ECHO, NRF_GPIO_PIN_NOPULL);
  nrf_gpio_pin_write(TRIG, 1);

}




int main(void) {
  printf("Board started!\n");
  
  // initialize GPIO
  gpio_init();

  // app_timer_init();
  // app_timer_create(&sample_timer, APP_TIMER_MODE_REPEATED, sample_timer_callback);

  // // start timer
  // app_timer_start(sample_timer, APP_TIMER_TICKS(5000), NULL);
  // Initialize Timer4
  nrfx_timer_config_t timer_config = {
      .frequency = NRF_TIMER_FREQ_1MHz,
      .mode = NRF_TIMER_MODE_TIMER,
      .bit_width = NRF_TIMER_BIT_WIDTH_32,
      .interrupt_priority = 4,
      .p_context = NULL
  };
  nrfx_timer_init(&TIMER4, &timer_config, NULL);

  nrfx_timer_capture(&TIMER4, NRF_TIMER_CC_CHANNEL0);

    // Start Timer4
  nrfx_timer_enable(&TIMER4);

  // loop forever
  while (1) {
    // Don't put any code in here. Instead put periodic code in `sample_timer_callback()`
    uint32_t distance = measure_dist();
    printf("Distance: %ld cm \n", distance);
    nrf_delay_ms(1000);

  }
}