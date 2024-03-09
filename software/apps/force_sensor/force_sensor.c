#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>


#include "app_timer.h"
#include "nrf_delay.h"
#include "nrfx_saadc.h"
#include "nrfx_timer.h"
#

#include "microbit_v2.h"


#define RES NRF_SAADC_INPUT_AIN1 // Assuming the FSR is connected to pin P0.03
#define ADC_PAD_CHANNEL 0


// Global variables
// APP_TIMER_DEF(sample_timer);
const float VCC = 3.60;
const float R_DIV = 10000.0;
static nrfx_timer_t TIMER4 = NRFX_TIMER_INSTANCE(0);


// Function prototypes
static void gpio_init(void);
static void timer_init(void);
static void adc_init(void);
static float adc_sample_blocking(uint8_t channel);


static uint32_t start_time;


void timer_event_handler(nrf_timer_event_t event_type, void* p_context) {
   // Not used, but required for timer initialization
}
  

 uint32_t measure_force(void){
   printf("Measuring force...\n");
   int fsrADC = adc_sample_blocking(0); // Assuming FSR is connected to ADC channel 0
   printf("FSR ADC value: %d\n", fsrADC);
   float force;
   if (fsrADC != 0) {
       float fsrV = fsrADC * VCC / 4095.0;
       float fsrR = R_DIV * (VCC / fsrV - 1.0);
       printf("Resistance : %.2f\n", fsrR);
       float fsrG = 1.0 / fsrR;


       if (fsrR <= 600){
           force = (fsrG - 0.00075) / 0.00000032639;
       }
       else{
           force = fsrG / 0.000000642857;
       }
       
   }
   if (force < 25){
      force = 0;
   }
   force = force / 32;
   printf("scalar : %.2f g\n", force);
   return force;
}


static void gpio_init(void) {
   // Initialize pins
   //nrf_gpio_cfg_input(RES, NRF_GPIO_PIN_NOPULL);
   //nrf_gpio_pin_clear(RES);
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


   // Start Timer4
   nrfx_timer_enable(&TIMER4);
}


static void saadc_event_callback(nrfx_saadc_evt_t const* _unused) {
 // don't care about saadc events
 // ignore this function
}




static void adc_init(void) {
   // Initialize the SAADC
   nrfx_saadc_config_t saadc_config = {
       .resolution = NRF_SAADC_RESOLUTION_12BIT,
       .oversample = NRF_SAADC_OVERSAMPLE_DISABLED,
       .interrupt_priority = 4,
       .low_power_mode = false,
   };
   ret_code_t error_code = nrfx_saadc_init(&saadc_config, saadc_event_callback); // No callback function needed for now
   APP_ERROR_CHECK(error_code);


   // Initialize FSR channel
   nrf_saadc_channel_config_t pad_channel_config = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(RES);
   error_code = nrfx_saadc_channel_init(ADC_PAD_CHANNEL, &pad_channel_config);
   APP_ERROR_CHECK(error_code);
}


static float adc_sample_blocking(uint8_t channel) {
   // read ADC counts (0-4095)
   // this function blocks until the sample is ready
   int16_t adc_counts = 0;
   ret_code_t error_code = nrfx_saadc_sample_convert(channel, &adc_counts);
   APP_ERROR_CHECK(error_code);


   return adc_counts;
}



int main(void) {
   printf("Board started!\n");
    gpio_init();
   timer_init();


   // initialize ADC
   adc_init();


   // loop forever
   while (1) {
       // Don't put any code in here. Instead put periodic code in `sample_timer_callback()`
       uint32_t force = measure_force();
       nrf_delay_ms(1000);
   }
}

// static volatile uint32_t start_time;
// static volatile uint32_t duration;


// // Timer Interrupt Handler
// void TIMER4_IRQHandler(void) {
//     if (NRF_TIMER4->EVENTS_COMPARE[0]) {
//         // Clear the event to prevent re-entering this interrupt
//         NRF_TIMER4->EVENTS_COMPARE[0] = 0;
        
//         // Optionally, you can stop the timer if you only need one measurement per trigger
//         // NRF_TIMER4->TASKS_STOP = 1;

//         // Calculate duration here if needed, or signal main loop/other function that measurement is complete
//     }
// }

// static void timer_init(void) {
//     // Set to 32 bit timer
//     NRF_TIMER4->BITMODE = TIMER_BITMODE_BITMODE_32Bit << TIMER_BITMODE_BITMODE_Pos;

//     // Set to 16 MHz clock
//     NRF_TIMER4->PRESCALER = 0; // 0 prescaler for 16 MHz clock

//     // Enable interrupts on COMPARE[0] event
//     NRF_TIMER4->CC[0] = 0; // Set compare register, but may be set dynamically
//     NRF_TIMER4->INTENSET = TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos;

//     // Enable TIMER4 interrupt in the NVIC
//     NVIC_ClearPendingIRQ(TIMER4_IRQn);
//     NVIC_SetPriority(TIMER4_IRQn, 7); // Set lowest priority
//     NVIC_EnableIRQ(TIMER4_IRQn);

//     // Start the timer, if continuous measurement is needed
//     //NRF_TIMER4->TASKS_START = 1;
// }

// static uint32_t measure_dist(void) {

//     // Trigger the ultrasonic sensor
//     nrf_gpio_pin_write(TRIG, 1);
//     nrf_delay_us(10); // 10us pulse to trigger
//     nrf_gpio_pin_write(TRIG, 0);

//     // Wait for echo to start
//     while(nrf_gpio_pin_read(ECHO) == 0);

//     // Start the timer to measure the echo duration
//     NRF_TIMER4->TASKS_CLEAR = 1; // Clear the timer
//     NRF_TIMER4->TASKS_START = 1; // Start the timer

//     // Wait for echo to end
//     while(nrf_gpio_pin_read(ECHO) == 1);

//     // Stop the timer first before capturing to ensure the capture is of the event's end time
//     NRF_TIMER4->TASKS_STOP = 1; // Stop the timer

//       // Capture the end time
//     NRF_TIMER4->TASKS_CAPTURE[1] = 1; // Trigger capture to CC[1]
//     duration = NRF_TIMER4->CC[1];
//     printf("duration %ld \n", duration);

//     NRF_TIMER4->TASKS_STOP = 1; // Stop the timer

//     // Calculate distance
//     uint32_t distance = (duration) / 58; // Convert duration to distance

//     return distance;
// }

// static void gpio_init(void) {
//     // Initialize GPIOs for TRIG and ECHO
//     nrf_gpio_cfg_output(TRIG);
//     nrf_gpio_cfg_input(ECHO, NRF_GPIO_PIN_NOPULL);
//     nrf_gpio_pin_clear(TRIG); // Ensure TRIG is low
// }

// int main(void) {
//     printf("Board started!\n");

//     // Initialize GPIO and Timer
//     gpio_init();
//     timer_init();

//     while (1) {
//         uint32_t distance = measure_dist();
//         printf("Distance: %ld cm\n", distance);
//         nrf_delay_ms(1000); // Delay between measurements
//     }
// }
