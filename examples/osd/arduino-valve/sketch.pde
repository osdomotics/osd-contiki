/*
 * Gardena 9V Magnet-Valve
 * We have a CoAP Resource for the Valve, it can be in state 1 (on) and
 * 0 (off).
 * Transition on-off outputs a negative pulse
 * Transition off-on outputs a positive pulse
 */

extern "C" {
#include <stdio.h>
#include "valve.h"
}

void setup (void)
{
    arduino_pwm_timer_init ();
    digitalWrite (ENABLE_PIN,  LOW);
    digitalWrite (BRIDGE1_PIN, LOW);
    digitalWrite (BRIDGE2_PIN, LOW);
    pinMode (ENABLE_PIN,  OUTPUT);
    pinMode (BRIDGE1_PIN, OUTPUT);
    pinMode (BRIDGE2_PIN, OUTPUT);
    digitalWrite (ENABLE_PIN,  LOW);
    digitalWrite (BRIDGE1_PIN, LOW);
    digitalWrite (BRIDGE2_PIN, LOW);

    rest_init_engine ();
    rest_activate_resource (&res_valve, (char *)"valve");
}

void loop (void)
{
    printf ("valve : %u\n", valve);
}
