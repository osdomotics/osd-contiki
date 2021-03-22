/**
 * \file
 *      Resource to get rf230_smallest_rssi
 * \author
 *      Marcus Priesch <marcus@priesch.co.at>
 *
 * \brief get min rssi since last call (min rssi gets reset after the call)
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

extern int8_t rf230_smallest_rssi;

size_t
linkstats_to_string
    ( const char *name
    , const char *uri
    , const char *query
    , char *buf
    , size_t bufsize
    )
{
  size_t res;
  res = snprintf (buf, bufsize, "%d", rf230_smallest_rssi);
  rf230_smallest_rssi = 0;
  return res;
}

GENERIC_RESOURCE \
    ( linkstats
    , LINKSTATS
    , linkstats
    , 0
    , 0
    , linkstats_to_string
    );

/*
 * VI settings, see coding style
 * ex:ts=8:et:sw=4
 */
