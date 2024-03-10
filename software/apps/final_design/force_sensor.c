#include "force_sensor.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "app_timer.h"
#include "nrf_delay.h"
#include "nrfx_saadc.h"
#include "nrfx_timer.h"
#include "microbit_v2.h"

// Configuration definitions
#define RES NRF_SAADC_INPUT_AIN1 // Assuming the FSR is connected to pin P0.03
#define ADC_PAD_CHANNEL 0
const float VCC = 3.60;
const float R_DIV = 10000.0;
static nrfx_timer_t TIMER3 = NRFX_TIMER_INSTANCE(1);

// Forward declarations of static functions
static void timer_event_handler_one(nrf_timer_event_t event_type, void* p_context);
static void timer_init(void);
static void adc_init(void);
static float adc_sample_blocking(uint8_t channel);

void force_sensor_init(void) {
    timer_init();
    adc_init();
}

static void timer_event_handler_one(nrf_timer_event_t event_type, void* p_context) {
    // Not used, but required for timer initialization
}
static void saadc_event_callback(nrfx_saadc_evt_t const* _unused) {
 // don't care about saadc events
 // ignore this function
}

static void timer_init(void) {
    nrfx_timer_config_t timer_config = {
        .frequency = NRF_TIMER_FREQ_1MHz,
        .mode = NRF_TIMER_MODE_TIMER,
        .bit_width = NRF_TIMER_BIT_WIDTH_32,
        .interrupt_priority = 0,
        .p_context = NULL
    };
    nrfx_timer_init(&TIMER3, &timer_config, timer_event_handler_one);
    nrfx_timer_enable(&TIMER3);
}

static void adc_init(void) {
    nrfx_saadc_config_t saadc_config = {
        .resolution = NRF_SAADC_RESOLUTION_12BIT,
        .oversample = NRF_SAADC_OVERSAMPLE_DISABLED,
        .interrupt_priority = 4,
        .low_power_mode = false,
    };
    ret_code_t error_code = nrfx_saadc_init(&saadc_config, saadc_event_callback); // Callback not used
    APP_ERROR_CHECK(error_code);

    nrf_saadc_channel_config_t pad_channel_config = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(RES);
    error_code = nrfx_saadc_channel_init(ADC_PAD_CHANNEL, &pad_channel_config);
    APP_ERROR_CHECK(error_code);
}

static float adc_sample_blocking(uint8_t channel) {
    int16_t adc_counts = 0;
    ret_code_t error_code = nrfx_saadc_sample_convert(channel, &adc_counts);
    APP_ERROR_CHECK(error_code);
    return adc_counts;
}

uint32_t measure_force(void) {
    printf("Measuring force...\n");
    int fsrADC = adc_sample_blocking(0); // Assuming FSR is connected to ADC channel 0
    printf("FSR ADC value: %d\n", fsrADC);
    float force;
    if (fsrADC != 0) {
        float fsrV = fsrADC * VCC / 4095.0;
        float fsrR = R_DIV * (VCC / fsrV - 1.0);
        printf("Resistance : %.2f\n", fsrR);
        float fsrG = 1.0 / fsrR;

        if (fsrR <= 600) {
            force = (fsrG - 0.00075) / 0.00000032639;
        } else {
            force = fsrG / 0.000000642857;
        }
    }
    if (force < 25) {
        force = 0;
    }
    force = force / 32;
    printf("scalar : %.2f g\n", force);
    return (uint32_t)force;
}
