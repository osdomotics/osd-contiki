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
part_count (const char *name, const char *uri, char *buf, size_t bsize)
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
part_size (const char *name, const char *uri, char *buf, size_t bsize)
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

