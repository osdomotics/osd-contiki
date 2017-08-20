/**
 * \file
 *      Resource for gmtime (utc) / localtime handling
 * \author
 *      Ralf Schlatterbeck <rsc@runtux.com>
 *
 * \brief get time as a string in utc or localtime
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "xtime.h"
#include "time_resource.h"
#include "jsonparse.h"
#include "er-coap.h"
#include "generic_resource.h"

size_t time_to_string
  (const char *name, const char *uri, const char *query, char *buf, size_t bs)
{
    struct xtimeval tv;
    struct xtm tm;
    struct xtm *(*method)(const xtime_t *, struct xtm *) = xgmtime_r;
    if (0 == strcmp (name, "localtime")) {
        method = xlocaltime_r;
    }
    xgettimeofday (&tv, NULL);
    method (&tv.tv_sec, &tm);
    return snprintf
      ( buf
      , bs
      , "%lu-%02u-%02u %02u:%02u:%02u %s"
      , 1900 + tm.tm_year
      , tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec
      , tm.tm_zone
      );
}

GENERIC_RESOURCE \
    ( localtime
    , Local time
    , formatted time
    , 1
    , NULL
    , time_to_string
    );

GENERIC_RESOURCE \
    ( utc
    , UTC
    , formatted time
    , 1
    , NULL
    , time_to_string
    );

/*
 * VI settings, see coding style
 * ex:ts=8:et:sw=2
 */

