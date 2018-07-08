/*
 * Copyright (C) 2017-2018, Marcus Priesch - open source consulting
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

extern enum states state;

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
    //int32_t tmp = strtol (s, NULL, 10);
    if (*s == '0') {
      valve (STOP);
    }
    else if (*s == 'o' || *s == 'O') {
      valve (OPEN);
    }
    else if (*s == 'c' || *s == 'C') {
      valve (CLOSE);
    }
    return 0;
}

size_t
direction_help
    ( const char *name
    , const char *uri
    , const char *query
    , char *buf
    , size_t bufsize
    )
{
  return snprintf (buf, bufsize, "o: open, c: close, s: stop");
}

GENERIC_RESOURCE \
    ( direction
    , DIRECTION
    , direction
    , 0
    , direction_from_string
    , direction_help
    );

int command_from_string
    (const char *name, const char *uri, const char *query, const char *s)
{
    //int32_t tmp = strtol (s, NULL, 10);
    if (*s == 'o' || *s == 'O') {
      state = FULLY_OPENING;
    }
    else if (*s == 'c' || *s == 'C') {
      state = FULLY_CLOSING;
    }
    return 0;
}

size_t
command_help
    ( const char *name
    , const char *uri
    , const char *query
    , char *buf
    , size_t bufsize
    )
{
  return snprintf (buf, bufsize, "o: open, c: close");
}

GENERIC_RESOURCE \
    ( command
    , COMMAND
    , command
    , 0
    , command_from_string
    , command_help
    );

/*
 * VI settings, see coding style
 * ex:ts=8:et:sw=4
 */
