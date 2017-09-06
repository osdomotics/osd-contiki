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
 *         Merkurboard-specific Contiki shell commands
 * \author
 *         Harald Pichler <harald@the-develop.net>
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
#include "lib/settings.h"
#include "extended-rf-api.h"
#include <stdio.h>
#include <string.h>

/*---------------------------------------------------------------------------*/
PROCESS(shell_txpower_process, "txpower");
SHELL_COMMAND(txpower_command,
	      "txpower",
	      "txpower <power>: change transmission power 0 (3dbm, default) to 15 (-17.2dbm)",
	      &shell_txpower_process);
PROCESS(shell_rfchannel_process, "rfchannel");
SHELL_COMMAND(rfchannel_command,
	      "rfchannel",
	      "rfchannel <channel>: change radio channel (11 - 26)",
	      &shell_rfchannel_process);
PROCESS(shell_ccathresholds_process, "ccathresholds");
SHELL_COMMAND(ccathresholds_command,
	      "ccathresholds",
	      "ccathresholds <threshold: change cca thresholds -91 to -61 dBm (default -77)",
	      &shell_ccathresholds_process);
PROCESS(shell_saveparam_process, "saveparam");
SHELL_COMMAND(saveparam_command,
	      "saveparam",
	      "saveparam <> save parameters to eeprom settingsmanager",
	      &shell_saveparam_process);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_txpower_process, ev, data)
{
  radio_value_t value;
  char buf[20];
  const char *newptr;
  PROCESS_BEGIN();

  value = shell_strtolong(data, &newptr);
  
  /* If no transmission power was given on the command line, we print
     out the current txpower. */
    if(newptr == data) {
	if(get_param(RADIO_PARAM_TXPOWER, &value) == RADIO_RESULT_OK) {

    }
  } else {
    set_param(RADIO_PARAM_TXPOWER, value);
  }

  snprintf(buf, sizeof(buf), "%3d dBm", value);
  shell_output_str(&txpower_command, "TX Power: ", buf);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_rfchannel_process, ev, data)
{
  radio_value_t value;
  char buf[20];
  const char *newptr;
  PROCESS_BEGIN();

  value = shell_strtolong(data, &newptr);
  
  /* If no channel was given on the command line, we print out the
     current channel. */
  if(newptr == data) {
	if(get_param(RADIO_PARAM_CHANNEL, &value) == RADIO_RESULT_OK) {
		
    }
  } else {
    set_param(RADIO_PARAM_CHANNEL, value);
  }

  snprintf(buf, sizeof(buf), "%d", value);
  shell_output_str(&rfchannel_command, "Channel: ", buf);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_ccathresholds_process, ev, data)
{
  radio_value_t value;
  char buf[20];
  const char *newptr;
  PROCESS_BEGIN();

  value = shell_strtolong(data, &newptr);
  
  /* If no channel was given on the command line, we print out the
     current channel. */
  if(newptr == data) {
	if(get_param(RADIO_PARAM_CCA_THRESHOLD, &value) == RADIO_RESULT_OK) {
		
    }
  } else {
    set_param(RADIO_PARAM_CCA_THRESHOLD, value);
  }

  snprintf(buf, sizeof(buf), "%d  dBm", value);
  shell_output_str(&rfchannel_command, "CCA Threshold: ", buf);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_saveparam_process, ev, data)
{
  radio_value_t value;
  char buf[20];
  const char *newptr;
  PROCESS_BEGIN();
  /* Save txpower */
  params_save_channel();
  /* Save rfchannel */
  
  /* Save ccathresholds */

  /* Save panid */

  shell_output_str(&rfchannel_command, "saveparam done ", 0);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
void
shell_merkur_init(void)
{
  shell_register_command(&txpower_command);
  shell_register_command(&rfchannel_command);
  shell_register_command(&ccathresholds_command);  
//  shell_register_command(&panid_command);
  shell_register_command(&saveparam_command);

}
/*---------------------------------------------------------------------------*/
