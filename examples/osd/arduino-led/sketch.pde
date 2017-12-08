/*
 * Sample arduino sketch using contiki features.
 * We turn the LED on and off and allow setting the interval and the
 * brightness of the LED via coap.
 * Unfortunately sleeping for long times in loop() isn't currently
 * possible, something turns off the CPU (including PWM outputs) if a
 * Proto-Thread is taking too long. We need to find out how to sleep in
 * a Contiki-compatible way.
 * Note that for a normal arduino sketch you won't have to include any
 * of the contiki-specific files here, the sketch should just work.
 */

extern "C" {
#include <stdio.h>
#include <hw_timer.h>
#include "led_setting.h"
#define TIMER 3
#define PIN_PWM_C 4
#define PIN_PWM_B 3

static uint16_t pwm_max = 0;
}

void setup (void)
{
    hwtimer_ini (3, HWT_WGM_PWM_PHASE_8_BIT, HWT_CLOCK_PRESCALER_8, 0);
    //hwtimer_pwm_ini (TIMER, 1000, HWT_PWM_FAST, 0);
    hwtimer_pwm_enable (TIMER, HWT_CHANNEL_C);
    hwtimer_pwm_enable (TIMER, HWT_CHANNEL_B);
    hwtimer_set_pwm (TIMER, HWT_CHANNEL_C, 0);
    hwtimer_set_pwm (TIMER, HWT_CHANNEL_B, 255);
    pinMode (PIN_PWM_C, OUTPUT);
    pinMode (PIN_PWM_B, OUTPUT);
    pwm_max = hwtimer_pwm_max_ticks (TIMER);
    printf ("pwm_max: %u\n", pwm_max);
}

void loop (void)
{
    static int direction = 1;
    static unsigned const char *led_ptr = led_setting;
    hwtimer_set_pwm (TIMER, HWT_CHANNEL_C, *led_ptr);
    hwtimer_set_pwm (TIMER, HWT_CHANNEL_B, 255 - *led_ptr);
    //printf ("p: %u\n", *led_ptr);
    if (led_ptr >= led_setting + sizeof (led_setting) - 1 && direction > 0) {
        direction = -1;
    }
    if (led_ptr <= led_setting && direction < 0) {
        direction = 1;
    }
    led_ptr += direction;
}
