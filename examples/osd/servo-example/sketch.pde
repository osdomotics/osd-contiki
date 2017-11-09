/*
 */

extern "C" {
#include <hw_timer.h>
/* This needs to be the timer that can generate PWM */
#define TIMER 3
#define PIN_PWM_A 2 // D2 in Arduino notation, corresponds to HWT_CHANNEL_A
#define PIN_PWM_B 3 // D3 in Arduino notation, corresponds to HWT_CHANNEL_B
#define PIN_PWM_C 4 // D4 in Arduino notation, corresponds to HWT_CHANNEL_C
/* 2.5ms may be too much for your servo, this should be set from the
 * datasheet of the servo if available. If the servo hums when fully
 * engaged, this is almost certainly set too high.
 * The value below is in microseconds.
 */
#define SERVO_MAX_US 2500

static uint16_t pwm_max       = 0;
static uint16_t pwm_max_servo = 0;
static uint16_t pwm           = 1;

}

void setup (void)
{

    // 10.000 µS = 10ms
    hwtimer_pwm_ini (TIMER, 10000, HWT_PWM_FAST, 0);
    hwtimer_pwm_enable (TIMER, HWT_CHANNEL_A);
    hwtimer_set_pwm (TIMER, HWT_CHANNEL_A, pwm);
    pinMode (PIN_PWM_A, OUTPUT);
    pwm_max = hwtimer_pwm_max_ticks (TIMER) - 1;
    pwm_max_servo = (uint16_t)((uint32_t)pwm_max * SERVO_MAX_US / 10000);
    printf ("pwm_max: %u, pwm_max_servo: %u\n", pwm_max, pwm_max_servo);
    //rest_init_engine ();
    //rest_activate_resource (&res_led_pwm,         (char *)"led/pwm");
}

void loop (void)
{
    printf ("pwm: %d\n", pwm);
    hwtimer_set_pwm (TIMER, HWT_CHANNEL_A, pwm);
    pwm += pwm_max_servo / 9;
    if (pwm > pwm_max_servo) {
        pwm = 1;
    }
}
