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
#include "led_pwm.h"

int pwm_from_string
  (const char *name, const char *uri, const char *query, const char *s)
{
    uint32_t tmp = strtoul (s, NULL, 10);
    if (tmp > 255) {
      tmp = 255;
    }
    pwm = tmp;
    return 0;
}

size_t
pwm_to_string
  ( const char *name
  , const char *uri
  , const char *query
  , char *buf
  , size_t bufsize
  )
{
  return snprintf (buf, bufsize, "%d", pwm);
}

GENERIC_RESOURCE \
    ( led_pwm
    , LED PWM
    , duty-cycle
    , 0
    , pwm_from_string
    , pwm_to_string
    );

int period_from_string
  (const char *name, const char *uri, const char *query, const char *s)
{
    uint32_t tmp = (strtoul (s, NULL, 10) + 50) / 100;
    if (tmp > 10) {
        tmp = 10;
    }
    if (tmp == 0) {
        tmp = 1;
    }
    period_100ms = tmp;
    return 0;
}

size_t
period_to_string
  ( const char *name
  , const char *uri
  , const char *query
  , char *buf
  , size_t bufsize
  )
{
  return snprintf (buf, bufsize, "%d", period_100ms * 100);
}

GENERIC_RESOURCE \
    ( led_period
    , LED Period
    , ms
    , 0
    , period_from_string
    , period_to_string
    );

size_t
analog2_v
  ( const char *name
  , const char *uri
  , const char *query
  , char *buf
  , size_t bufsize
  )
{
  return snprintf
    (buf, bufsize, "%d.%03d", analog2_voltage / 1000, analog2_voltage % 1000);
}

GENERIC_RESOURCE \
    ( analog2_voltage
    , Analog 2 voltage
    , V
    , 0
    , NULL
    , analog2_v
    );

size_t
analog5_v
  ( const char *name
  , const char *uri
  , const char *query
  , char *buf
  , size_t bufsize
  )
{
  return snprintf
    (buf, bufsize, "%d.%03d", analog5_voltage / 1000, analog5_voltage % 1000);
}

GENERIC_RESOURCE \
    ( analog5_voltage
    , Analog 5 voltage
    , V
    , 0
    , NULL
    , analog5_v
    );

/*
 * VI settings, see coding style
 * ex:ts=8:et:sw=2
 */
