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
 *      Potentiometer for regulating LED-strip brightness per color
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
#include "Arduino.h"

/*
 * Resources to be activated need to be imported through the extern keyword.
 * The build system automatically compiles the resources in the
 * corresponding sub-directory.
 */
 
#if PLATFORM_HAS_BATTERY
#include "dev/battery-sensor.h"
extern resource_t res_battery;
#endif

#if PLATFORM_HAS_RADIO
#include "dev/radio-sensor.h"
extern resource_t res_radio;
#endif

#define REMOTE_PORT UIP_HTONS(COAP_DEFAULT_PORT)
// should be the same :-)
#define UIP_NTOHS(x) UIP_HTONS(x)
#define SERVER_NODE(ip) \
    uip_ip6addr(ip,0xfe80,0,0,0,0x22e,0xffff,0x34,0xa600)
    /*uip_ip6addr(ip,0x2001,0xdb8,0xc001,0xf00d,0x22e,0xffff,0x34,0xa600)*/
#define LOOP_INTERVAL (1 * CLOCK_SECOND)

uip_ipaddr_t server_ipaddr, tmp_addr;
char         server_resource [20] = "led/G";
int          interval = 10; /* Retransmit interval after no change in value */

static size_t
ip_to_string
    ( const char *name
    , const char *uri
    , const char *query
    , char *buf
    , size_t bsize
    )
{
    #define IP(x) UIP_NTOHS(server_ipaddr.u16[x])
    return snprintf
        ( buf, bsize, "%x:%x:%x:%x:%x:%x:%x:%x"
        , IP(0), IP(1), IP(2), IP(3), IP(4), IP(5), IP(6), IP(7)
        );
}

int ip_from_string
    (const char *name, const char *uri, const char *query, const char *s)
{
    /* Returns 1 if successful, only copy valid address */
    if (uiplib_ip6addrconv (s, &tmp_addr)) {
        uip_ip6addr_copy (&server_ipaddr, &tmp_addr);
        return 0;
    }
    return -1;
}

GENERIC_RESOURCE
  ( server_ip
  , ip
  , ipv6_address
  , 1
  , ip_from_string
  , ip_to_string
  );

static size_t
resource_to_string
    ( const char *name
    , const char *uri
    , const char *query
    , char *buf
    , size_t bsize
    )
{
    return snprintf (buf, bsize, "%s", server_resource);
}

int resource_from_string
    (const char *name, const char *uri, const char *query, const char *s)
{
    strncpy (server_resource, s, sizeof (server_resource));
    server_resource [sizeof (server_resource) - 1] = 0;
    return 0;
}

GENERIC_RESOURCE
  ( server_resource
  , led-resource
  , resource-name
  , 1
  , resource_from_string
  , resource_to_string
  );

static size_t
interval_to_string
    ( const char *name
    , const char *uri
    , const char *query
    , char *buf
    , size_t bsize
    )
{
    return snprintf (buf, bsize, "%d", interval);
}

int interval_from_string
    (const char *name, const char *uri, const char *query, const char *s)
{
    interval = atoi (s);
    return 0;
}

GENERIC_RESOURCE
  ( interval
  , interval
  , s
  , 0
  , interval_from_string
  , interval_to_string
  );

/* Passed to COAP_BLOCKING_REQUEST to handle responses */
void chunk_handler (void *response)
{
    const uint8_t *chunk;
    int len = coap_get_payload (response, &chunk);
    printf ("|%.*s", len, (char *)chunk);
}

PROCESS(poti, "Potentiometer");
AUTOSTART_PROCESSES(&poti);

PROCESS_THREAD(poti, ev, data)
{

  static struct etimer loop_timer;
  PROCESS_BEGIN();

  /* Initialize the REST engine. */
  rest_init_engine ();
  SERVER_NODE (&server_ipaddr);
  adc_init ();

  /* Activate the application-specific resources. */
#if PLATFORM_HAS_BATTERY
  SENSORS_ACTIVATE(battery_sensor);
  rest_activate_resource (&res_battery,         "s/battery");
#endif
  rest_activate_resource (&res_server_ip,       "poti/ip");
  rest_activate_resource (&res_server_resource, "poti/resource");
  rest_activate_resource (&res_interval,        "poti/interval");

  etimer_set (&loop_timer, LOOP_INTERVAL);
  /* Define application-specific events here. */
  while(1) {
    static int count = 0;
    static int lastval = -1;
    static coap_packet_t request [1]; /* Array: treat as pointer */
    uint8_t val = 127;

    PROCESS_WAIT_EVENT();
    if (etimer_expired (&loop_timer)) {
        uint16_t sum = 0;
        int i;
        count++;
        adc_setup (ADC_DEFAULT, A5);
        for (i=0; i<5; i++) {
            sum += adc_read ();
            clock_delay_usec (50);
        }
        adc_fin ();
        val = (sum / 5) >> 2;
        if ((interval > 0 && count > interval) || (val != lastval)) {
            char buf [4];
            coap_transaction_t *transaction;

            sprintf (buf, "%d", val);
            lastval = val;
            printf ("Sending Value: %s\n", buf);
            coap_init_message (request, COAP_TYPE_NON, COAP_PUT, 0);
            coap_set_header_uri_path (request, server_resource);
            coap_set_header_content_format (request, REST.type.TEXT_PLAIN);
            coap_set_payload (request, buf, strlen (buf));
            request->mid = coap_get_mid ();
            transaction = coap_new_transaction
                (request->mid, &server_ipaddr, REMOTE_PORT);
            transaction->packet_len = coap_serialize_message
                (request, transaction->packet);
            coap_send_transaction (transaction);
            count = 0;
        }
        etimer_reset (&loop_timer);
    }
  } /* while (1) */

  PROCESS_END();
}
