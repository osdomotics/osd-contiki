/**
 * \file
 *      Resources to get charger status
 * \author
 *      Marcus Priesch <marcus@priesch.co.at>
 *
 * \brief get various states of the min delta peak charger
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

extern uint8_t state;

extern uint16_t v_bat;
extern uint16_t v_sol;
extern uint16_t v_bat_max;

#define S_WAIT 0
#define S_LOAD 1
#define S_MEAS 2
#define S_WAIT_BAT_LOW 3

size_t
vbat_to_string
    ( const char *name
    , const char *uri
    , const char *query
    , char *buf
    , size_t bufsize
    )
{
  size_t res;
  res = snprintf (buf, bufsize, "%d", v_bat);
  return res;
}

GENERIC_RESOURCE \
    ( charger_v_bat
    , CHARGER_V_BAT
    , charger_v_bat
    , 0
    , 0
    , vbat_to_string
    );

size_t
vbatmax_to_string
    ( const char *name
    , const char *uri
    , const char *query
    , char *buf
    , size_t bufsize
    )
{
  size_t res;
  res = snprintf (buf, bufsize, "%d", v_bat_max);
  return res;
}

GENERIC_RESOURCE \
    ( charger_v_bat_max
    , CHARGER_V_BAT_MAX
    , charger_v_bat_max
    , 0
    , 0
    , vbatmax_to_string
    );

size_t
vsol_to_string
    ( const char *name
    , const char *uri
    , const char *query
    , char *buf
    , size_t bufsize
    )
{
  size_t res;
  res = snprintf (buf, bufsize, "%d", v_sol);
  return res;
}

GENERIC_RESOURCE \
    ( charger_v_sol
    , CHARGER_V_SOL
    , charger_v_sol
    , 0
    , 0
    , vsol_to_string
    );

size_t
cstate_to_string
    ( const char *name
    , const char *uri
    , const char *query
    , char *buf
    , size_t bufsize
    )
{
  size_t res;
  switch (state) {
    case S_WAIT:
      res = snprintf (buf, bufsize, "S_WAIT");
      break;
    case S_LOAD:
      res = snprintf (buf, bufsize, "S_LOAD");
      break;
    case S_MEAS:
      res = snprintf (buf, bufsize, "S_MEAS");
      break;
    case S_WAIT_BAT_LOW:
      res = snprintf (buf, bufsize, "S_WAIT_BAT_LOW");
      break;
    default:
      res = snprintf (buf, bufsize, "S_UNKNOWN (%d)", state);
      break;
  }
  return res;
}

GENERIC_RESOURCE \
    ( charger_state
    , CHARGER_STATE
    , charger_state
    , 0
    , 0
    , cstate_to_string
    );

/*
 * VI settings, see coding style
 * ex:ts=8:et:sw=4
 */
