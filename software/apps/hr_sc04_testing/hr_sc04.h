#pragma once

#include <stdbool.h>

//Measures Distance given a trig edge, echo edge, and a timer channel
static uint32_t measure_dist(uint32_t trig, uint32_t echo, nrf_timer_cc_channel_t timer_channel);

//inits a trig and echo edge
static void gpio_edge_init(uint32_t trig, uint32_t echo);

//init
void hr_sc04_init(void);
