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
#include "led_pwm.h"
#define LED_PIN 4
#define LED_STRING 3

uint8_t  pwm             = 128;
uint8_t  period_100ms    = 1; /* 1/10 second (period_100ms * 100ms) */
}

void setup (void)
{
    arduino_pwm_timer_init ();
    rest_init_engine ();
    rest_activate_resource (&res_led_pwm,         (char *)"led/pwm");
    rest_activate_resource (&res_led_period,      (char *)"led/period");
}

void loop (void)
{
    /* Use 255 - pwm, LED on merkur-board is wired to +3.3V */
    analogWrite (LED_PIN, 255 - pwm); // Merkurboard LED
    analogWrite (LED_STRING, pwm);    // Grove Modul

    printf ("clock : %lu\nmillis: %lu\n", clock_time (), millis ());
    delay (period_100ms * 100);
    analogWrite (LED_PIN, 255); /* OFF: LED on merkur-board is wired to +3.3V */
    analogWrite (LED_STRING, 0); 
    delay (period_100ms * 100);
}
