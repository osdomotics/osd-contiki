/*
 * Copyright (C) 2016, Ralf Schlatterbeck and other contributors.
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
 *      Erbium (Er) Wallclock REST Engine example (with CoAP-specific code)
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 *      Ralf Schlatterbeck <rsc@runtux.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "er-coap-engine.h"
#include "xtime.h"
#include "cron.h"
#include "time_resource.h"
#include "jsonparse.h"
#include "Arduino.h"

#if PLATFORM_HAS_BUTTON
#include "dev/button-sensor.h"
#endif

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]", (lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3], (lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

#if PLATFORM_HAS_LEDS
#include "dev/leds.h"
extern resource_t res_leds, res_toggle;
#endif

#if PLATFORM_HAS_BATTERY
#include "dev/battery-sensor.h"
extern resource_t res_battery;
#endif
/*
#if PLATFORM_HAS_RADIO
#include "dev/radio-sensor.h"
extern resource_t res_radio;
#endif
*/

//******************************************************************************/

void 
hw_init ()
{
#if defined (PLATFORM_HAS_LEDS)
 leds_off(LEDS_RED);
#endif
}

PROCESS(rest_server_example, "Erbium Example Server");

AUTOSTART_PROCESSES(&rest_server_example, &sensors_process);

#define LED_PIN 4
#define LOOP_INTERVAL (30 * CLOCK_SECOND)

/*
 * Set led to on or off, we abuse the given pointer to simply carry the
 * on/off flag.
 */
void led_set (void *onoff)
{
    int status = (int)onoff;
    digitalWrite (LED_PIN, (status ? 0 : 1));
}

PROCESS_THREAD(rest_server_example, ev, data)
{
  static struct etimer loop_periodic_timer;
  PROCESS_BEGIN();
  PRINTF("Starting Erbium Example Server\n");
  PRINTF("uIP buffer: %u\n", UIP_BUFSIZE);
  PRINTF("LL header: %u\n", UIP_LLH_LEN);
  PRINTF("IP+UDP header: %u\n", UIP_IPUDPH_LEN);
  PRINTF("REST max chunk: %u\n", REST_MAX_CHUNK_SIZE);

/* if static routes are used rather than RPL */
#if !UIP_CONF_IPV6_RPL && !defined (CONTIKI_TARGET_MINIMAL_NET) && !defined (CONTIKI_TARGET_NATIVE)
  set_global_address ();
  configure_routing ();
#endif

  /* Initialize the OSD Hardware. */
  hw_init ();
  led_set (0);
  /* Initialize the REST engine. */
  rest_init_engine ();

  /* Activate the application-specific resources. */
  rest_activate_resource (&res_leds, "s/leds");


#if defined (PLATFORM_HAS_BATTERY) && REST_RES_BATTERY
  SENSORS_ACTIVATE(battery_sensor);
  rest_activate_resource (&res_battery, "s/battery");
#endif

  rest_activate_resource (&res_timestamp, "clock/timestamp");
  rest_activate_resource (&res_timezone, "clock/timezone");
  rest_activate_resource (&res_localtime, "clock/localtime");
  rest_activate_resource (&res_utc, "clock/utc");

  /* Register callback function(s) */
  cron_register_command ("led_on",  led_set, (void *)1);
  cron_register_command ("led_off", led_set, (void *)0);

  /* Allocate all cron entries and the necessary resources */
  activate_cron_resources ();

  /* Define application-specific events here.
   * We need to call cron every 30 seconds or so (at least once a
   * minute)
   */
  etimer_set (&loop_periodic_timer, LOOP_INTERVAL);
  while (1) {
    PROCESS_WAIT_EVENT();
    if (etimer_expired (&loop_periodic_timer)) {
        cron ();
        etimer_reset (&loop_periodic_timer);
    }
  } /* while (1) */

  PROCESS_END();
}
