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


#define TRIG1 EDGE_P0
#define ECHO1 EDGE_P1
#define TRIG2 EDGE_P5
#define ECHO2 EDGE_P6
#define TRIG3 EDGE_P11
#define ECHO3 EDGE_P12


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
    printf("trig sent \n");

    while(nrf_gpio_pin_read(ECHO1) == 0);
    start_time = nrfx_timer_capture(&TIMER4, NRF_TIMER_CC_CHANNEL0);
    printf("inital start for echo \n");
    
    while(nrf_gpio_pin_read(ECHO1) == 1);
    duration = nrfx_timer_capture(&TIMER4, NRF_TIMER_CC_CHANNEL1) - start_time;
    //printf("duration: %ld \n", duration);
    printf("echo done \n");

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
    start_time = nrfx_timer_capture(&TIMER4, NRF_TIMER_CC_CHANNEL2);
    
    while(nrf_gpio_pin_read(ECHO2) == 1);
    duration = nrfx_timer_capture(&TIMER4, NRF_TIMER_CC_CHANNEL3) - start_time;
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
    start_time = nrfx_timer_capture(&TIMER4, NRF_TIMER_CC_CHANNEL4);
    
    while(nrf_gpio_pin_read(ECHO3) == 1);
    duration = nrfx_timer_capture(&TIMER4, NRF_TIMER_CC_CHANNEL5) - start_time;
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
  printf("timier inti \n");
}



int main(void) {
  printf("Board started!\n");
  
  gpio_init();
  timer_init();


  // loop forever
  while (1) {
    // Don't put any code in here. Instead put periodic code in `sample_timer_callback()`
    printf("in while loop");
    uint32_t distance1 = measure_dist1();
    printf("distance1 gotten");
    uint32_t distance2 = measure_dist2();
    printf("distance2 gotten");
    uint32_t distance3 = measure_dist3();
    printf("distance3 gotten");
    if ((distance1 < 10) && (distance2 >= 10) && (distance3 >= 10)){
        printf("Distance Sense 1: %ld cm \n)", distance1);
        nrf_delay_ms(1000);
    }
    else if (distance2 < 10 && distance1 >= 10 && distance3 >= 10)
    {
        printf("Distance Sense 2: %ld cm \n", distance2);
        nrf_delay_ms(1000);
    }
    else if (distance1 < 10 && distance2 < 10 && distance3 >= 10){ 
        printf("Distance Sense 1 and 2 together \n");
        nrf_delay_ms(1000);
    }
    else if (distance3 < 10 && distance1 >= 10 && distance2 >= 10){ 
        printf("Distance Sense 3: %ld cm \n", distance3);
        nrf_delay_ms(1000);
    }
    else if ((distance1 < 10) && (distance3 < 10) && (distance2 >= 10)){ 
        printf("Distance Sense 1 and 3 together \n");
        nrf_delay_ms(1000);
    }
    else if ((distance2 < 10) && (distance3 < 10) && (distance1 >= 10)){ 
        printf("Distance Sense 2 and 3 together \n");
        nrf_delay_ms(1000);
    }
    else if ((distance2 < 10) && (distance3 < 10) && (distance1 < 10)){ 
        printf("Distance Sense 1 2 and 3 together \n");
        nrf_delay_ms(1000);
    }
    else {
        printf("Polling... \n");
        nrf_delay_ms(500);
    }
    nrf_delay_ms(500);
    

  }
}


