/**
 * \file
 *      Resource for timezone handling
 * \author
 *      Ralf Schlatterbeck <rsc@runtux.com>
 *
 * \brief get/put timezone string
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

int timezone_from_string
  (const char *name, const char *uri, const char *query, const char *s)
{
  set_tz (s);
  return 0;
}

size_t
timezone_to_string
  ( const char *name
  , const char *uri
  , const char *query
  , char *buf
  , size_t bsize
  )
{
  if (get_tz (buf, bsize) == NULL) {
    *buf = '\0';
  }
  return strlen (buf);
}

GENERIC_RESOURCE
  ( timezone
  , TZ
  , s
  , 1
  , timezone_from_string
  , timezone_to_string
  );

/*
 * VI settings, see coding style
 * ex:ts=8:et:sw=2
 */

