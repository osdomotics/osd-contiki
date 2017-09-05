/*
 * Copyright (c) 2008, Swedish Institute of Computer Science.
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
 *
 */

/**
 * \file
 *         Tmote Sky-specific Contiki shell commands
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "contiki.h"
#include "sys/cc.h"
#include "shell-sky.h"

#include "dev/watchdog.h"

#include "net/rime/rime.h"
#include "net/netstack.h"
#include "net/rime/timesynch.h"
#include "dev/radio.h"
#include "sys/node-id.h"

#include <stdio.h>
#include <string.h>

/*---------------------------------------------------------------------------*/
PROCESS(shell_txpower_process, "txpower");
SHELL_COMMAND(txpower_command,
	      "txpower",
	      "txpower <power>: change CC2420 transmission power (0 - 31)",
	      &shell_txpower_process);
PROCESS(shell_rfchannel_process, "rfchannel");
SHELL_COMMAND(rfchannel_command,
	      "rfchannel",
	      "rfchannel <channel>: change CC2420 radio channel (11 - 26)",
	      &shell_rfchannel_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_txpower_process, ev, data)
{
  struct {
    uint16_t len;
    uint16_t txpower;
  } msg;
  const char *newptr;
  PROCESS_BEGIN();

  msg.txpower = shell_strtolong(data, &newptr);
  
  /* If no transmission power was given on the command line, we print
     out the current txpower. */
  
  if(newptr == data) {
    msg.txpower = rf230_get_txpower();
  } else {
    rf230_set_txpower(msg.txpower);
  }

  msg.len = 1;

  shell_output(&txpower_command, &msg, sizeof(msg), "", 0);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_rfchannel_process, ev, data)
{
  struct {
    uint16_t len;
    uint16_t channel;
  } msg;
  const char *newptr;
  PROCESS_BEGIN();

  msg.channel = shell_strtolong(data, &newptr);
  
  /* If no channel was given on the command line, we print out the
     current channel. */
  if(newptr == data) {
    msg.channel = rf230_get_channel();
  } else {
    rf230__set_channel(msg.channel);
  }

  msg.len = 1;

  shell_output(&rfchannel_command, &msg, sizeof(msg), "", 0);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
void
shell_sky_init(void)
{
  shell_register_command(&txpower_command);
  shell_register_command(&rfchannel_command);
}
/*---------------------------------------------------------------------------*/
