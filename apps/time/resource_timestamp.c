/**
 * \file
 *      Resource for timestamp handling
 * \author
 *      Ralf Schlatterbeck <rsc@runtux.com>
 *
 * \brief get/put time in seconds since 1970 (UNIX time)
 * Note: the internal format of the time in seconds is a 64bit number
 * unfortunately javascript (json) will only support double for which
 * the mantissa isn't long enough for representing that number. So we're
 * back to 32 bit and have a year 2038 problem.
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

int timestamp_from_string
  (const char *name, const char *uri, const char *query, const char *s)
{
  struct xtimeval tv;
  // FIXME: Platform has no strtoll (long long)?
  tv.tv_sec = strtol (s, NULL, 10);
  xsettimeofday (&tv, NULL);
  return 0;
}

size_t
timestamp_to_string
  ( const char *name
  , const char *uri
  , const char *query
  , char *buf
  , size_t bsize
  )
{
  struct xtimeval tv;
  xgettimeofday (&tv, NULL);
  // FIXME: Platform doesn't seem to support long long printing
  // We get empty string
  return snprintf (buf, bsize, "%ld", (long)tv.tv_sec);
}

GENERIC_RESOURCE
  ( timestamp
  , Time
  , s
  , 1
  , timestamp_from_string
  , timestamp_to_string
  );

/*
 * VI settings, see coding style
 * ex:ts=8:et:sw=2
 */

