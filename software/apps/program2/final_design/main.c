// HR_SC04 Testing

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "app_timer.h"
#include "nrf_delay.h"
#include "nrfx_saadc.h"
#include "nrfx_timer.h"

#include "hr_sc04.h"
#include "sound.h"
#include "force_sensor.h"

#include "microbit_v2.h"


#define TRIG2 EDGE_P7
#define ECHO2 EDGE_P8
#define TRIG1 EDGE_P5
#define ECHO1 EDGE_P6
#define TRIG3 EDGE_P11
#define ECHO3 EDGE_P12
#define TRIG4 EDGE_P15
#define ECHO4 EDGE_P16


// Global variables
// APP_TIMER_DEF(sample_timer);
static nrfx_timer_t TIMER4 = NRFX_TIMER_INSTANCE(0);
//static nrfx_timer_t TIMER3 = NRFX_TIMER_INSTANCE(1);
//static nrfx_timer_t TIMER2 = NRFX_TIMER_INSTANCE(2);

// Function prototypes
static void gpio_init_main(void);

static void gpio_init_main(void) {
  // Initialize pins
  gpio_edge_init(TRIG1, ECHO1);
  gpio_edge_init(TRIG2, ECHO2);
  gpio_edge_init(TRIG3, ECHO3);
  gpio_edge_init(TRIG4, ECHO4);
}

int main(void) {
  printf("Board started!\n");
  
  gpio_init_main();
  printf("GPIO initialized\n");
  hr_sc04_init();
  printf("HR_SC04 initialized\n");
  force_sensor_init();
  printf("Force sensor initialized\n");
  pwm_init(SOUND_PIN);

  uint32_t tones1[] = {622};
  uint32_t tones2[] = {523};
  uint32_t tones3[] = {261};
  uint32_t tones4[] = {130};

  uint32_t bias = 0;
  uint8_t volume = 50;
  // loop forever
  while (1) {
    // Don't put any code in here. Instead put periodic code in `sample_timer_callback()`
    //printf("in while loop \n");
    float distance1 = hr_sc04_measure_dist(TRIG1, ECHO1, NRF_TIMER_CC_CHANNEL0);
    printf("1\n");
    uint32_t distance2 = hr_sc04_measure_dist(TRIG2, ECHO2, NRF_TIMER_CC_CHANNEL1);
    printf("2\n");
    uint32_t distance3 = hr_sc04_measure_dist(TRIG3, ECHO3, NRF_TIMER_CC_CHANNEL2);
    printf("3\n");
    uint32_t distance4 = hr_sc04_measure_dist(TRIG4, ECHO4, NRF_TIMER_CC_CHANNEL3);
    printf("4\n");
    if ((distance1 < 10) && (distance2 >= 10) && (distance3 >= 10) && (distance4 >= 10)){
            printf("Distance Sense 1: %ld cm \n", distance1);
        //nrf_delay_ms(50);
    }
    else if (distance2 < 10 && distance1 >= 10 && distance3 >= 10 && distance4 >= 10)
    {
        printf("Distance Sense 2: %ld cm \n", distance2);
        //nrf_delay_ms(50);
    }
    // else if (distance1 < 10 && distance2 < 10 && distance3 >= 10){ 
    //     printf("Distance Sense 1 and 2 together \n");
    //     nrf_delay_ms(50);
    // }
    else if (distance3 < 10 && distance1 >= 10 && distance2 >= 10 && distance4 >= 10){ 
        printf("Distance Sense 3: %ld cm \n", distance3);
        //nrf_delay_ms(50);
    }
    else if (distance4 < 10 && distance1 >= 10 && distance2 >= 10 && distance3 >= 10){ 
        printf("Distance Sense 4: %ld cm \n", distance4);
        //nrf_delay_ms(50);
    }
    // else if ((distance1 < 10) && (distance3 < 10) && (distance2 >= 10)){ 
    //     printf("Distance Sense 1 and 3 together \n");
    //     nrf_delay_ms(50);
    // }
    // else if ((distance2 < 10) && (distance3 < 10) && (distance1 >= 10)){ 
    //     printf("Distance Sense 2 and 3 together \n");
    //     nrf_delay_ms(50);
    // }
    // else if ((distance2 < 10) && (distance3 < 10) && (distance1 < 10)){ 
    //     printf("Distance Sense 1 2 and 3 together \n");
    //     nrf_delay_ms(50);
    // }
    //bias = measure_force();
    bias = ((distance2 <= 100) ? distance2*2 : 0);
    volume = 2 + (measure_force() * 4);
    printf("Volume: %d\n", volume);
    stop_tone();
    //play_tone(tones1, 1, bias, 15, distance1, 50);
    play_tone(tones1, 1, distance1*3, 50, distance1, volume);
    play_tone(tones2, 1, distance2*3, 50, distance2, volume);
    play_tone(tones3, 1, distance3*2, 50, distance3, volume);
    play_tone(tones4, 1, distance4, 50, distance4, volume);
    nrf_delay_ms(10);
  }
}