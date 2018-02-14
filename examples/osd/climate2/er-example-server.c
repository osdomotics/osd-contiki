/*
 * Copyright (c) 2011, Matthias Kovatsch and other contributors.
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
 *      Erbium (Er) REST Engine example (with CoAP-specific code)
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "contiki.h"
#include "contiki-net.h"
#include "rest-engine.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]",(lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3],(lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

/*
 * Resources to be activated need to be imported through the extern keyword.
 * The build system automatically compiles the resources in the corresponding sub-directory.
 */
 
#if defined (PLATFORM_HAS_INFO)
extern resource_t res_info;
#endif

#if PLATFORM_HAS_DHT11HUM
#include "dev/dht11.h"
extern resource_t res_dht11hum;
extern uint16_t dht11_hum;
#endif

#if PLATFORM_HAS_DHT11TEMP
#include "dev/dht11.h"
extern resource_t res_dht11temp;
extern uint16_t dht11_temp;
#endif

#if defined (PLATFORM_HAS_DS1820)
#include "dev/ds1820.h"
extern resource_t res_ds1820;
#endif

#if defined (PLATFORM_HAS_LEDS)
#include "dev/led.h"
extern resource_t res_leds;
#endif

#if PLATFORM_HAS_BATTERY
#include "dev/battery-sensor.h"
extern resource_t res_battery;
#endif

#if PLATFORM_HAS_RADIO
#include "dev/radio-sensor.h"
extern resource_t res_radio;
#endif

void 
hw_init()
{
#if defined (PLATFORM_HAS_LEDS)
// leds_off(LEDS_RED);
led1_off();
#endif
#if PLATFORM_HAS_DS1820
  ds1820_temp();
#endif
#if PLATFORM_HAS_DHT11HUM
  DHT_Read_Data(&dht11_temp, &dht11_hum);
#endif
}
#define MESURE_INTERVAL		(20 * CLOCK_SECOND)
#define READ_TIME		( 2 * CLOCK_SECOND)

PROCESS(rest_server_example, "Erbium Example Server");
AUTOSTART_PROCESSES(&rest_server_example);

PROCESS_THREAD(rest_server_example, ev, data)
{
  static struct etimer ds_periodic_timer;
#if PLATFORM_HAS_DS1820
  static struct etimer ds_read_timer;
#endif

  PROCESS_BEGIN();

  PRINTF("Starting Erbium Example Server\n");
  PRINTF("uIP buffer: %u\n", UIP_BUFSIZE);
  PRINTF("LL header: %u\n", UIP_LLH_LEN);
  PRINTF("IP+UDP header: %u\n", UIP_IPUDPH_LEN);
  PRINTF("REST max chunk: %u\n", REST_MAX_CHUNK_SIZE);

/* if static routes are used rather than RPL */
//#if !UIP_CONF_IPV6_RPL && !defined (CONTIKI_TARGET_MINIMAL_NET) && !defined (CONTIKI_TARGET_NATIVE)
//  set_global_address();
//  configure_routing();
//#endif

  /* Initialize the OSD Hardware. */
  hw_init();

  /* Initialize the REST engine. */
  rest_init_engine();

  /* Activate the application-specific resources. */
#if PLATFORM_HAS_DS1820
  rest_activate_resource(&res_ds1820,"s/temp");
#endif
#if PLATFORM_HAS_DHT11HUM
  rest_activate_resource(&res_dht11hum,"s/hum");
#endif
#if PLATFORM_HAS_DHT11TEMP
  rest_activate_resource(&res_dht11temp,"s/temp");
#endif
#if PLATFORM_HAS_INFO
  rest_activate_resource(&res_info,"info");
#endif
#if PLATFORM_HAS_LEDS
  rest_activate_resource(&res_leds,"a/leds");
#endif /* PLATFORM_HAS_LEDS */
#if PLATFORM_HAS_TEMPERATURE
  SENSORS_ACTIVATE(temperature_sensor);
  rest_activate_resource(&resource_temperature);
#endif
#if PLATFORM_HAS_BATTERY
  SENSORS_ACTIVATE(battery_sensor);
  rest_activate_resource(&res_battery,"s/battery");
#endif

  /* Define application-specific events here. */
  etimer_set(&ds_periodic_timer, MESURE_INTERVAL);
  while(1) {
    PROCESS_WAIT_EVENT();
#if defined (PLATFORM_HAS_BUTTON)
    if (ev == sensors_event && data == &button_sensor) {
      PRINTF("BUTTON\n");
    }
#endif /* PLATFORM_HAS_BUTTON */
    if(etimer_expired(&ds_periodic_timer)) {
        PRINTF("Periodic\n");
        etimer_reset(&ds_periodic_timer);
#if PLATFORM_HAS_DHT11HUM
        DHT_Read_Data(&dht11_temp, &dht11_hum);
#endif
#if PLATFORM_HAS_DS1820
        if(ds1820_convert()){
          etimer_set(&ds_read_timer, READ_TIME);
        }
#endif
    }
#if PLATFORM_HAS_DS1820
    if(etimer_expired(&ds_read_timer)) {
        PRINTF("DS1820_Read\n");
        ds1820_read();
    }
#endif
  } /* while (1) */

  PROCESS_END();
}
