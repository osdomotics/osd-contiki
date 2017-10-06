/*
 * Copyright (c) 2017, Marcus Priesch Open Source Consulting
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
 *      Reboot ressource
 * \author
 *      Marcus Priesch <marcus@priesch.co.at>
 */

#include <stdio.h>
#include <string.h>
#include "contiki.h"
#include "er-coap-engine.h"
#include "generic_resource.h"
#include "dev/watchdog.h"

PROCESS(reboot_process, "reboot");
PROCESS_THREAD(reboot_process, ev, data)
{
  static struct etimer etimer;

  //PROCESS_EXITHANDLER(leds_off(LEDS_ALL);)

  PROCESS_BEGIN();

  //shell_output_str(&reboot_command,
  //                 "Rebooting the node in four seconds...", "");

  etimer_set(&etimer, CLOCK_SECOND * 4);
  PROCESS_WAIT_UNTIL(etimer_expired(&etimer));
  //leds_on(LEDS_RED);
  //etimer_reset(&etimer);
  //PROCESS_WAIT_UNTIL(etimer_expired(&etimer));
  //leds_on(LEDS_GREEN);
  //etimer_reset(&etimer);
  //PROCESS_WAIT_UNTIL(etimer_expired(&etimer));
  //leds_on(LEDS_BLUE);
  //etimer_reset(&etimer);
  //PROCESS_WAIT_UNTIL(etimer_expired(&etimer));

  watchdog_reboot();

  PROCESS_END();
}



static size_t
get_reboot
    ( const char *name
    , const char *uri
    , const char *query
    , char *buf
    , size_t bsize
    )
{
    return snprintf (buf, bsize, "put 'OK' to reboot.");
}

static int
do_reboot
    (const char *name, const char *uri, const char *query, const char *s)
{
    if (strncmp (s, "OK", 2) == 0) {
        process_start (&reboot_process, NULL);
        return 0;
    }
    return 0;
}

GENERIC_RESOURCE
  ( reboot
  , Reboot node
  , count
  , 0
  , do_reboot
  , get_reboot
  );

