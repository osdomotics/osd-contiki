/*
 * Copyright (c) 2014, George Oikonomou (george@contiki-os.org)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 *   Example project demonstrating the extended RF API functionality
 */
#include "contiki.h"
#include <stdio.h>
#include <stdint.h>
#include "net/netstack.h"
#include "dev/radio.h"
#include "extended-rf-api.h"
/*---------------------------------------------------------------------------*/
struct rf_consts {
  radio_value_t channel_min;
  radio_value_t channel_max;
  radio_value_t txpower_min;
  radio_value_t txpower_max;
};

static struct rf_consts consts;

void
print_64bit_addr(const uint8_t *addr)
{
  unsigned int i;
  for(i = 0; i < 7; i++) {
    printf("%02x:", addr[i]);
  }
  printf("%02x (network order)\n", addr[7]);
}
/*---------------------------------------------------------------------------*/
radio_result_t
get_object(radio_param_t param, void *dest, size_t size)
{
  radio_result_t rv;

  rv = NETSTACK_RADIO.get_object(param, dest, size);

  switch(rv) {
  case RADIO_RESULT_ERROR:
    printf("Radio returned an error\n");
    break;
  case RADIO_RESULT_INVALID_VALUE:
    printf("Value is invalid\n");
    break;
  case RADIO_RESULT_NOT_SUPPORTED:
    printf("Param %u not supported\n", param);
    break;
  case RADIO_RESULT_OK:
    break;
  default:
    printf("Unknown return value\n");
    break;
  }

  return rv;
}
/*---------------------------------------------------------------------------*/
radio_result_t
set_object(radio_param_t param, void *src, size_t size)
{
  radio_result_t rv;

  rv = NETSTACK_RADIO.set_object(param, src, size);

  switch(rv) {
  case RADIO_RESULT_ERROR:
    printf("Radio returned an error\n");
    break;
  case RADIO_RESULT_INVALID_VALUE:
    printf("Value is invalid\n");
    break;
  case RADIO_RESULT_NOT_SUPPORTED:
    printf("Param %u not supported\n", param);
    break;
  case RADIO_RESULT_OK:
    break;
  default:
    printf("Unknown return value\n");
    break;
  }

  return rv;
}
/*---------------------------------------------------------------------------*/
radio_result_t
get_param(radio_param_t param, radio_value_t *value)
{
  radio_result_t rv;

  rv = NETSTACK_RADIO.get_value(param, value);
  
  switch(rv) {
  case RADIO_RESULT_ERROR:
    printf("Radio returned an error\n");
    break;
  case RADIO_RESULT_INVALID_VALUE:
    printf("Value %d is invalid\n", *value);
    break;
  case RADIO_RESULT_NOT_SUPPORTED:
    printf("Param %u not supported\n", param);
    break;
  case RADIO_RESULT_OK:
    break;
  default:
    printf("Unknown return value\n");
    break;
  }

  return rv;
}
/*---------------------------------------------------------------------------*/
radio_result_t
set_param(radio_param_t param, radio_value_t value)
{
  radio_result_t rv;

  rv = NETSTACK_RADIO.set_value(param, value);

  switch(rv) {
  case RADIO_RESULT_ERROR:
    printf("Radio returned an error\n");
    break;
  case RADIO_RESULT_INVALID_VALUE:
    printf("Value %d is invalid\n", value);
    break;
  case RADIO_RESULT_NOT_SUPPORTED:
    printf("Param %u not supported\n", param);
    break;
  case RADIO_RESULT_OK:
    break;
  default:
    printf("Unknown return value\n");
    break;
  }

  return rv;
}
/*---------------------------------------------------------------------------*/
void
get_rf_consts(void)
{
  printf("====================================\n");
  printf("RF Constants\n");
  printf("Min Channel : ");
  if(get_param(RADIO_CONST_CHANNEL_MIN, &consts.channel_min) == RADIO_RESULT_OK) {
    printf("%3d\n", consts.channel_min);
  }

  printf("Max Channel : ");
  if(get_param(RADIO_CONST_CHANNEL_MAX, &consts.channel_max) == RADIO_RESULT_OK) {
    printf("%3d\n", consts.channel_max);
  }

  printf("Min TX Power: ");
  if(get_param(RADIO_CONST_TXPOWER_MIN, &consts.txpower_min) == RADIO_RESULT_OK) {
    printf("%3d dBm\n", consts.txpower_min);
  }

  printf("Max TX Power: ");
  if(get_param(RADIO_CONST_TXPOWER_MAX, &consts.txpower_max) == RADIO_RESULT_OK) {
    printf("%3d dBm\n", consts.txpower_max);
  }
}
