/*
 * Copyright (C) 2017, Marcus Priesch, Ralf Schlatterbeck
 * with code from the res-plugtest-large-update.c by
 * Copyright (c) 2013, Institute for Pervasive Computing, ETH Zurich
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
 *      Over-the-air update using blockwise transfer
 * \author
 *      Marcus Priesch <marcus@priesch.co.at>
 *      Ralf Schlatterbeck <rsc@runtux.com>
 */

#include <string.h>
#include "sys/cc.h"
#include "rest-engine.h"
#include "er-coap.h"
#include "contiki.h"
#include "contiki-net.h"
#include "er-coap.h"
#include "Arduino.h"
#include <avr/interrupt.h>
#include "bootloader_if.h"

#if 1
#include <stdio.h>
#define PRINTF(x) printf x
#else
#define PRINTF(x)
#endif


// We allocate this statically, otherwise we cannot flash a new image
// when ram is exhausted!
static uint8_t current_page [256];
static uint32_t current_offset = 0;
#define PAGESIZE (sizeof (current_page))

static void
res_put_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  coap_packet_t *const packet = (coap_packet_t *)request;
  uint8_t *in_data = NULL;
  size_t len = 0;
  uint32_t partition_start = 0;
  const uint32_t partition_size  = bootloader_get_part_size  ();

  unsigned int ct = -1;

  if(bootloader_get_boot_default()){
	  partition_start = bootloader_get_part_start (0);
  }else{
	  partition_start = bootloader_get_part_start (1);	  
  }

  REST.get_header_content_type(request, &ct);

  /* Require content_type APPLICATION_OCTET_STREAM */
  if (ct != REST.type.APPLICATION_OCTET_STREAM) {
    REST.set_response_status(response, REST.status.BAD_REQUEST);
    const char *error_msg = "ContentType";
    REST.set_response_payload(response, error_msg, strlen(error_msg));
    return;
  }

  len = REST.get_request_payload (request, (const uint8_t **)&in_data);
  PRINTF (("cur: %lu len: %lu, offset: %lu\n",
    (uint32_t)current_offset, (uint32_t)len, (uint32_t)*offset));
  PRINTF (("b1-offs: %lu, b1-size: %u, b1-num: %lu b1-more: %d b1-size1: %lu\n",
      packet->block1_offset, packet->block1_size, packet->block1_num,
      packet->block1_more, packet->size1));
  if (len == 0 || NULL == in_data) {
    REST.set_response_status(response, REST.status.BAD_REQUEST);
    const char *error_msg = "NoPayload";
    REST.set_response_payload(response, error_msg, strlen(error_msg));
    return;
  }

  /* if the block1_offset is 0 a new transmission has started */
  if (!packet->block1_offset) {
    current_offset = 0;
  }

  if (packet->block1_offset > current_offset) {
    REST.set_response_status(response, REST.status.REQUEST_ENTITY_INCOMPLETE);
    const char *error_msg = "OutOfSequence";
    REST.set_response_payload(response, error_msg, strlen(error_msg));
    return;
  }

  /* Old packet or retransmission, immediately confirm */
  if (packet->block1_offset && packet->block1_offset + len <= current_offset) {
    REST.set_response_status(response, REST.status.CHANGED);
    coap_set_header_block1
      (response, packet->block1_num, 0, packet->block1_size);
    return;
  }

  // FIXME: blocksize may be larger than our flash page size
  if (len > PAGESIZE) {
    REST.set_response_status(response, REST.status.INTERNAL_SERVER_ERROR);
    const char *error_msg = "GRMPF: PageSize";
    REST.set_response_payload(response, error_msg, strlen(error_msg));
    return;
  }
  // FIXME: blocksize may be larger than our flash page size
  // So we should handle this case and repeatedly flash a block until the
  // received data is written.
  if (current_offset % PAGESIZE + len > PAGESIZE) {
    REST.set_response_status(response, REST.status.INTERNAL_SERVER_ERROR);
    const char *error_msg = "GRMPF: blocksize";
    REST.set_response_payload(response, error_msg, strlen(error_msg));
    return;
  }
  // Should never happen, we test for < and > earlier.
  if (packet->block1_offset != current_offset) {
    REST.set_response_status(response, REST.status.INTERNAL_SERVER_ERROR);
    const char *error_msg = "GRMPF: Offset";
    REST.set_response_payload(response, error_msg, strlen(error_msg));
    return;
  }

  if(packet->block1_offset + len > partition_size) {
    REST.set_response_status(response,
                             REST.status.REQUEST_ENTITY_TOO_LARGE);
    REST.set_response_payload(
      response,
      buffer,
      sprintf((char *)buffer, "%luB max.", partition_size));
    return;
  }

  memcpy (current_page + current_offset % PAGESIZE, in_data, len);
  current_offset += len;

  if (current_offset % PAGESIZE == 0) {
    uint32_t dst_address = partition_start + current_offset - PAGESIZE;
    PRINTF (("Flashing: %lu to %lu\n", (uint32_t)PAGESIZE, dst_address));
    bootloader_write_page_to_flash (dst_address, PAGESIZE, current_page);
  } else if (!packet->block1_more) {
    uint32_t dst_address =
        partition_start + (current_offset / PAGESIZE) * PAGESIZE;
    PRINTF (("Flashing: last page %lu to %lu\n", (uint32_t)PAGESIZE, dst_address));
    bootloader_write_page_to_flash (dst_address, PAGESIZE, current_page);
  }


  if (!packet->block1_more) {
    // we are finished
    if(bootloader_get_boot_default()){
		bootloader_backup_irq_table (0);
		bootloader_set_boot_next (0);
	}else{
		bootloader_backup_irq_table (1);
		bootloader_set_boot_next (1);		
	}
    current_offset = 0;
  }

  REST.set_response_status(response, REST.status.CHANGED);
  coap_set_header_block1(response, packet->block1_num, 0, packet->block1_size);
}

RESOURCE(
    res_upload_image
  , "title=\"Flash memory upgrade\";rt=\"block\""
  , NULL
  , NULL
  , res_put_handler
  , NULL
  );

