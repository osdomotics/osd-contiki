/**
 * \defgroup Arduino LED PWM example
 *
 * Resource definition for Arduino LED PWM module
 *
 * @{
 */

/**
 * \file
 *         Resource definitions for the Arduino LED PWM module
 *      
 * \author
 *         Ralf Schlatterbeck <rsc@tux.runtux.com>
 */

#ifndef led_pwm_h
#define led_pwm_h
#include "contiki.h"
#include "contiki-net.h"
#include "er-coap.h"

extern uint8_t  pwm;
extern uint8_t  period_100ms;
extern uint16_t analog2_voltage;
extern uint16_t analog5_voltage;

extern resource_t res_led_pwm;
extern resource_t res_led_period;
extern resource_t res_analog2_voltage;
extern resource_t res_analog5_voltage;

#endif // led_pwm_h
/** @} */
