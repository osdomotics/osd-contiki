/**
 * \defgroup Motorized Valve
 *
 * Resource definition for Motorized Valve
 *
 * @{
 */

/**
 * \file
 *         Resource definitions for the Motorized Valve
 *
 * \author
 *         Marcus Priesch <marcus@priesch.co.at>
 */

#ifndef valve_h
#define valve_h
#include "contiki.h"
#include "contiki-net.h"
#include "er-coap.h"
#include "dev/button-sensor.h"

#define BTN_COM_PIN     A0
#define PULSE_ON_PIN    14

#define DIR_UP_PIN      11
#define DIR_DOWN_PIN    10
  // rot0               13 (pcint3)
  // rot1               12 (pcint1)

#define BTN_1_PIN        7
#define BTN_2_PIN        5
#define BTN_3_PIN        6

#define OLED_ON_PIN      2
#define USER_LED_PIN    A1

#define LED_PIN          4

extern resource_t res_pulses, res_direction, res_command;

enum states {
   IDLE,
   FULLY_CLOSING,
   FULLY_OPENING,
   WAIT_END,
   WAIT_TO_CLOSE,
   MANUAL,
   MANUAL_FADE_OUT
};

#define OPEN  1
#define CLOSE 2
#define STOP  3

void valve (uint8_t direction);

#endif // valve_h
/** @} */
