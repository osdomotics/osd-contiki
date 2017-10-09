/*
 * Copyright (C) 2017, Marcus Priesch - open source consulting
 * All rights reserved.
 *
 */

/**
 * \file
 *      Resources for the sygonix valve
 * \author
 *      Marcus Priesch <marcus@priesch.co.at>
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

int pulses_reset
    (const char *name, const char *uri, const char *query, const char *s)
{
    button_sensor.configure (4711, 0);
    return 0;
}

size_t
pulses_to_string
    ( const char *name
    , const char *uri
    , const char *query
    , char *buf
    , size_t bufsize
    )
{
  return snprintf (buf, bufsize, "%d", button_sensor.value (0));
}

GENERIC_RESOURCE \
    ( pulses
    , PULSES
    , pulses
    , 0
    , pulses_reset
    , pulses_to_string
    );

int direction_from_string
    (const char *name, const char *uri, const char *query, const char *s)
{
    int32_t tmp = strtol (s, NULL, 10);
    if (tmp == 0) {
      digitalWrite (DIR_UP_PIN, LOW);
      digitalWrite (DIR_DOWN_PIN, LOW);
    }
    else if (tmp == -1) {
      digitalWrite (DIR_UP_PIN, LOW);
      digitalWrite (DIR_DOWN_PIN, HIGH);
    }
    else if (tmp == 1) {
      digitalWrite (DIR_UP_PIN, HIGH);
      digitalWrite (DIR_DOWN_PIN, LOW);
    }
    return 0;
}

GENERIC_RESOURCE \
    ( direction
    , DIRECTION
    , direction
    , 0
    , direction_from_string
    , NULL
    );

/*
 * VI settings, see coding style
 * ex:ts=8:et:sw=4
 */


