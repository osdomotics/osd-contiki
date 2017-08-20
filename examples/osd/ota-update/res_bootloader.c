/*
 * Copyright (c) 2015, Ralf Schlatterbeck Open Source Consulting
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 */

/**
 * \file
 *      Bootloader ressources
 * \author
 *      Ralf Schlatterbeck <rsc@runtux.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "contiki.h"
#include "contiki-net.h"
#include "rest-engine.h"
#include "er-coap-engine.h"
#include "uiplib.h"
#include "generic_resource.h"
#include "bootloader_if.h"
#include "Arduino.h"

/*
 * Resources to be activated need to be imported through the extern keyword.
 * The build system automatically compiles the resources in the
 * corresponding sub-directory.
 */

static size_t
part_count
    ( const char *name
    , const char *uri
    , const char *query
    , char *buf
    , size_t bsize
    )
{
    return snprintf (buf, bsize, "%ld", bootloader_get_part_count ());
}

GENERIC_RESOURCE
  ( part_count
  , Partition Count
  , count
  , 0
  , NULL
  , part_count
  );

static size_t
part_size
    ( const char *name
    , const char *uri
    , const char *query
    , char *buf
    , size_t bsize
    )
{
    return snprintf (buf, bsize, "%ld", bootloader_get_part_size ());
}

GENERIC_RESOURCE
  ( part_size
  , Partition Size
  , count
  , 0
  , NULL
  , part_size
  );

static size_t
get_boot_default
    ( const char *name
    , const char *uri
    , const char *query
    , char *buf
    , size_t bsize
    )
{
    return snprintf (buf, bsize, "%ld", bootloader_get_boot_default ());
}

static int
set_boot_default
    (const char *name, const char *uri, const char *query, const char *s)
{
    uint32_t tmp = strtoul (s, NULL, 10);
    bootloader_set_boot_default (tmp);
    return 0;
}

GENERIC_RESOURCE
  ( boot_default
  , Default boot partition
  , count
  , 0
  , set_boot_default
  , get_boot_default
  );

static size_t
get_boot_next
    ( const char *name
    , const char *uri
    , const char *query
    , char *buf
    , size_t bsize
    )
{
    return snprintf (buf, bsize, "%ld", bootloader_get_boot_next ());
}

static int
set_boot_next
    (const char *name, const char *uri, const char *query, const char *s)
{
    uint32_t tmp = strtoul (s, NULL, 10);
    bootloader_set_boot_next (tmp);
    return 0;
}

GENERIC_RESOURCE
  ( boot_next
  , Next boot partition
  , count
  , 0
  , set_boot_next
  , get_boot_next
  );

static size_t
get_callers_part
    ( const char *name
    , const char *uri
    , const char *query
    , char *buf
    , size_t bsize
    )
{
    return snprintf (buf, bsize, "%ld", bootloader_get_callers_part ());
}

GENERIC_RESOURCE
  ( callers_part
  , Currently booted partition
  , count
  , 0
  , NULL
  , get_callers_part
  );

/*
 * Parse query info. We insist that the query starts with 'part='
 * Then we parse the integer following the part= string and return the
 * number. The number is always positive, if something goes wrong we
 * return a negative number.
 */
static int get_query_partition (const char *query)
{
    if (strncmp (query, "part=", 5)) {
        return -1;
    }
    return strtoul (query + 5, NULL, 10);
}

static size_t
get_part_start
    ( const char *name
    , const char *uri
    , const char *query
    , char *buf
    , size_t bsize
    )
{
    int idx = get_query_partition (query);
    printf ("part: %d", idx);
    if (idx < 0) {
        return snprintf (buf, bsize, "Invalid: %s", query);
    }
    return snprintf (buf, bsize, "%ld", bootloader_get_part_start (idx));
}

GENERIC_RESOURCE
  ( part_start
  , Start of partition
  , count
  , 0
  , NULL
  , get_part_start
  );

static int
set_part_ok
    (const char *name, const char *uri, const char *query, const char *s)
{
    uint32_t tmp = strtoul (s, NULL, 10);
    int idx = get_query_partition (query);
    printf ("part: %d", idx);
    if (idx < 0) {
        return -1;
    }
    if (tmp) {
        bootloader_set_part_ok (idx);
    } else {
        bootloader_clr_part_ok (idx);
    }
    return 0;
}

GENERIC_RESOURCE
  ( part_ok
  , Set/Clear Partition OK flag
  , count
  , 0
  , set_part_ok
  , NULL
  );

// FIXME: Find out how to pass two parameters, for set/clr_part_ok and
// for get_part_start
