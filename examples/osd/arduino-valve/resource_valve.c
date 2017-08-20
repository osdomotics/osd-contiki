/**
 * \file
 *      Resource for Arduino PWM
 * \author
 *      Ralf Schlatterbeck <rsc@runtux.com>
 *
 * \brief get/put pwm and period for LED pin
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "jsonparse.h"
#include "er-coap.h"
#include "generic_resource.h"
#include "Arduino.h"
#include "valve.h"

uint8_t valve;

int valve_from_string
    (const char *name, const char *uri, const char *query, const char *s)
{
    uint32_t tmp = strtoul (s, NULL, 10);
    if (tmp) {
      tmp = 1;
    }
    if (tmp != valve) {
        digitalWrite (ENABLE_PIN, HIGH);
        if (tmp) {
            digitalWrite (BRIDGE1_PIN, HIGH);
            digitalWrite (BRIDGE2_PIN, LOW);
            clock_delay_msec (250);
            //clock_delay_msec (2500);
            digitalWrite (BRIDGE1_PIN, LOW);
            digitalWrite (ENABLE_PIN,  LOW);
        } else {
            digitalWrite (BRIDGE1_PIN, LOW);
            digitalWrite (BRIDGE2_PIN, HIGH);
            clock_delay_msec (62);
            //clock_delay_msec (2500);
            digitalWrite (BRIDGE2_PIN, LOW);
            digitalWrite (ENABLE_PIN,  LOW);
        }
    }
    valve = tmp;
    return 0;
}

size_t
valve_to_string
    ( const char *name
    , const char *uri
    , const char *query
    , char *buf
    , size_t bufsize
    )
{
  return snprintf (buf, bufsize, "%d", valve);
}

GENERIC_RESOURCE \
    ( valve
    , VALVE
    , valve-status
    , 0
    , valve_from_string
    , valve_to_string
    );

/*
 * VI settings, see coding style
 * ex:ts=8:et:sw=4
 */
