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

extern enum states state, next_state;

extern int16_t current_position, pulseCount;

int pulses_reset
    (const char *name, const char *uri, const char *query, const char *s)
{
  printf ("pulses_reset\n");
  state = INIT_OPENING;
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
  return snprintf (buf, bufsize, "%d", pulseCount);
}

GENERIC_RESOURCE \
    ( pulses
    , PULSES
    , pulses
    , 0
    , pulses_reset
    , pulses_to_string
    );

size_t
state_to_string
    ( const char *name
    , const char *uri
    , const char *query
    , char *buf
    , size_t bufsize
    )
{
  if (state == IDLE) 
    return snprintf (buf, bufsize, "%s", "IDLE");
  
  if (state == WAIT_INIT_FULLY_CLOSED) 
    return snprintf (buf, bufsize, "%s", "WAIT_INIT_FULLY_CLOSED");
  
  if (state == INIT_OPENING) 
    return snprintf (buf, bufsize, "%s", "INIT_OPENING");
  
  if (state == FULLY_CLOSING) 
    return snprintf (buf, bufsize, "%s", "FULLY_CLOSING");
  
  if (state == FULLY_OPENING) 
    return snprintf (buf, bufsize, "%s", "FULLY_OPENING");
  
  if (state == WAIT_END) 
    return snprintf (buf, bufsize, "%s", "WAIT_END");
  
  if (state == WAIT_TO_CLOSE) 
    return snprintf (buf, bufsize, "%s", "WAIT_TO_CLOSE");
  
  return snprintf (buf, bufsize, "%s", "unknown");
}

GENERIC_RESOURCE \
    ( status
    , STATUS
    , status
    , 0
    , NULL
    , state_to_string
    );

size_t
valve_to_string
    ( const char *name
    , const char *uri
    , const char *query
    , char *buf
    , size_t bufsize
    )
{
  return snprintf (buf, bufsize, "%d", current_position);
}

GENERIC_RESOURCE \
    ( valve
    , VALVE
    , valve
    , 0
    , NULL
    , valve_to_string
    );

int command_from_string
    (const char *name, const char *uri, const char *query, const char *s)
{
    //int32_t tmp = strtol (s, NULL, 10);
    if (*s == 'o' || *s == 'O') {
      printf ("o\n");
      if (state == INIT_OPENING || state == WAIT_INIT_FULLY_CLOSED) {
        printf ("not ready, ignoring command\n");
	return 0;
      }
      state = FULLY_OPENING;
      next_state = WAIT_END;
    }
    else if (*s == 'c' || *s == 'C') {
      printf ("c\n");
      if (state == INIT_OPENING || state == WAIT_INIT_FULLY_CLOSED) {
        printf ("not ready, ignoring command\n");
	return 0;
      }
      state = FULLY_CLOSING;
      next_state = WAIT_END;
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
