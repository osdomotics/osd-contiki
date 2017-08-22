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
 *      Erbium (Er) PIR REST Engine example (with CoAP-specific code)
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 *      Harald Pichler <harald@the-develop.net>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"


/* Define which resources to include to meet memory constraints. */
#define REST_RES_INFO 1
#define REST_RES_TEMPERATURE 0
#define REST_RES_EVENT 1
#define REST_RES_LEDS 0
#define REST_RES_TOGGLE 0
#define REST_RES_BATTERY 1

#include "dev/led.h"
#if defined (PLATFORM_HAS_BUTTON)
#include "dev/button-sensor.h"
#endif
#if defined (PLATFORM_HAS_LEDS)
#include "dev/leds.h"
#endif
#if defined (PLATFORM_HAS_PIR)
#include "dev/pir-sensor.h"
#endif
#if defined (PLATFORM_HAS_TEMPERATURE)
#include "dev/temperature-sensor.h"
#endif
#if defined (PLATFORM_HAS_BATTERY)
#include "dev/battery-sensor.h"
#endif

#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]",(lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3],(lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

/******************************************************************************/

#if REST_RES_INFO
/*
 * Resources are defined by the RESOURCE macro.
 * Signature: resource name, the RESTful methods it handles, and its URI path (omitting the leading slash).
 */
RESOURCE(info, METHOD_GET, "info", "title=\"Info\";rt=\"text\"");

/*
 * A handler function named [resource name]_handler must be implemented for each RESOURCE.
 * A buffer for the response payload is provided through the buffer pointer. Simple resources can ignore
 * preferred_size and offset, but must respect the REST_MAX_CHUNK_SIZE limit for the buffer.
 * If a smaller block size is requested for CoAP, the REST framework automatically splits the data.
 */
void
info_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  char message[100];
  int index = 0;
  int length = 0; /*           |<-------->| */

  /* Some data that has the length up to REST_MAX_CHUNK_SIZE. For more, see the chunk resource. */
       // jSON Format
     index += sprintf(message + index,"{\n \"Version\" : \"V1.0pre1\",\n");
     index += sprintf(message + index," \"name\" : \"6lowpan-PIR\"\n");
     index += sprintf(message + index,"}\n");

    length = strlen(message);
    memcpy(buffer, message,length );

  REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
  REST.set_response_payload(response, buffer, length);
}
#endif


/*A simple actuator example, post variable mode, relay is activated or deactivated*/
RESOURCE(led1, METHOD_GET | METHOD_PUT , "aktors/led1",  "title=\"Led1\";rt=\"led\"");
void
led1_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  char mode[10];
  static uint8_t led1 = 0;
  static char name[17]="led1";
  int success = 1;

  char temp[100];
  int index = 0;
  size_t len = 0;

  const char *pmode = NULL;
  const char *pname = NULL;

  switch(REST.get_method_type(request)){
   case METHOD_GET:
     // jSON Format
     index += sprintf(temp + index,"{\n \"name\" : \"%s\",\n",name);
     if(led1 == 0)
         index += sprintf(temp + index," \"mode\" : \"off\"\n");
     if(led1 == 1)
         index += sprintf(temp + index," \"mode\" : \"on\"\n");
     index += sprintf(temp + index,"}\n");

     len = strlen(temp);
     memcpy(buffer, temp,len );

     REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
     REST.set_response_payload(response, buffer, len);
     break;
   case METHOD_POST:
     success = 0;
     break;
   case METHOD_PUT:
     if (success &&  (len=REST.get_post_variable(request, "mode", &pmode))) {
       PRINTF("name %s\n", mode);
       memcpy(mode, pmode,len);
       mode[len]=0;
       if (!strcmp(mode, "on")) {
         led1_on();
         led1 = 1;
       } else if (!strcmp(mode, "off")) {
         led1_off();
         led1 = 0;
       } else {
         success = 0;
       }
    } else if (success &&  (len=REST.get_post_variable(request, "name", &pname))) {
       PRINTF("name %s\n", name);
       memcpy(name, pname,len);
       name[len]=0;
    } else {
      success = 0;
    }
    break;
  default:
    success = 0;
  }

  if (!success) {
    REST.set_response_status(response, REST.status.BAD_REQUEST);
  }
}

/******************************************************************************/
#if REST_RES_EVENT && defined (PLATFORM_HAS_PIR)
/*
 * Example for an event resource.
 * Additionally takes a period parameter that defines the interval to call [name]_periodic_handler().
 * A default post_handler takes care of subscriptions and manages a list of subscribers to notify.
 */
EVENT_RESOURCE(pir, METHOD_GET, "sensors/pir", "title=\"Event demo\";obs");

void
pir_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
  /* Usually, a CoAP server would response with the current resource representation. */
  const char *msg = "It's eventful!";
  REST.set_response_payload(response, (uint8_t *)msg, strlen(msg));

  /* A post_handler that handles subscriptions/observing will be called for periodic resources by the framework. */
}

/* Additionally, a handler function named [resource name]_event_handler must be implemented for each PERIODIC_RESOURCE defined.
 * It will be called by the REST manager process with the defined period. */
void
pir_event_handler(resource_t *r)
{
  static uint16_t event_counter = 0;
  static char content[12];

  ++event_counter;

  PRINTF("PIR TICK %u for /%s\n", event_counter, r->url);
  
    /* Build notification. */
  coap_packet_t notification[1]; /* This way the packet can be treated as pointer as usual. */
  coap_init_message(notification, COAP_TYPE_CON, REST.status.OK, 0 );
  
  coap_set_payload(notification, content, snprintf(content, sizeof(content), "EVENT %u", event_counter));

  /* Notify the registered observers with the given message type, observe option, and payload. */
  REST.notify_subscribers(r, event_counter, notification);
}
#endif /* PLATFORM_HAS_PIR */

/******************************************************************************/
#if defined (PLATFORM_HAS_LEDS)
/******************************************************************************/
#if REST_RES_LEDS
/*A simple actuator example, depending on the color query parameter and post variable mode, corresponding led is activated or deactivated*/
RESOURCE(leds, METHOD_POST | METHOD_PUT , "actuators/leds", "title=\"LEDs: ?color=r|g|b, POST/PUT mode=on|off\";rt=\"Control\"");

void
leds_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  size_t len = 0;
  const char *color = NULL;
  const char *mode = NULL;
  uint8_t led = 0;
  int success = 1;

  if ((len=REST.get_query_variable(request, "color", &color))) {
    PRINTF("color %.*s\n", len, color);

    if (strncmp(color, "r", len)==0) {
      led = LEDS_RED;
    } else if(strncmp(color,"g", len)==0) {
      led = LEDS_GREEN;
    } else if (strncmp(color,"b", len)==0) {
      led = LEDS_BLUE;
    } else {
      success = 0;
    }
  } else {
    success = 0;
  }

  if (success && (len=REST.get_post_variable(request, "mode", &mode))) {
    PRINTF("mode %s\n", mode);

    if (strncmp(mode, "on", len)==0) {
      leds_on(led);
    } else if (strncmp(mode, "off", len)==0) {
      leds_off(led);
    } else {
      success = 0;
    }
  } else {
    success = 0;
  }

  if (!success) {
    REST.set_response_status(response, REST.status.BAD_REQUEST);
  }
}
#endif

/******************************************************************************/
#if REST_RES_TOGGLE
/* A simple actuator example. Toggles the red led */
RESOURCE(toggle, METHOD_GET | METHOD_PUT | METHOD_POST, "actuators/toggle", "title=\"Red LED\";rt=\"Control\"");
void
toggle_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  leds_toggle(LEDS_RED);
}
#endif
#endif /* PLATFORM_HAS_LEDS */

/******************************************************************************/

/******************************************************************************/
#if REST_RES_TEMPERATURE && defined (PLATFORM_HAS_TEMPERATURE)
/* A simple getter example. Returns the reading from light sensor with a simple etag */
RESOURCE(temperature, METHOD_GET, "sensors/cputemp", "title=\"Temperature status\";rt=\"temperature-c\"");
void
temperature_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  int temperature = temperature_sensor.value(0);

  const uint16_t *accept = NULL;
  int num = REST.get_header_accept(request, &accept);

  if ((num==0) || (num && accept[0]==REST.type.TEXT_PLAIN))
  {
    REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%d", temperature);

    REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer));
  }
  else if (num && (accept[0]==REST.type.APPLICATION_JSON))
  {
    REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'temperature':%d}", temperature);

    REST.set_response_payload(response, buffer, strlen((char *)buffer));
  }
  else
  {
    REST.set_response_status(response, REST.status.UNSUPPORTED_MEDIA_TYPE);
    const char *msg = "Supporting content-types text/plain and application/json";
    REST.set_response_payload(response, msg, strlen(msg));
  }
}
#endif /* PLATFORM_HAS_TEMPERATURE */

/******************************************************************************/
#if REST_RES_BATTERY && defined (PLATFORM_HAS_BATTERY)
/* A simple getter example. Returns the reading from light sensor with a simple etag */
RESOURCE(battery, METHOD_GET, "sensors/battery", "title=\"Battery status\";rt=\"battery-mV\"");
void
battery_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  int battery = battery_sensor.value(0);

  const uint16_t *accept = NULL;
  int num = REST.get_header_accept(request, &accept);

  if ((num==0) || (num && accept[0]==REST.type.TEXT_PLAIN))
  {
    REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%d", battery);

    REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer));
  }
  else if (num && (accept[0]==REST.type.APPLICATION_JSON))
  {
    REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'battery':%d}", battery);

    REST.set_response_payload(response, buffer, strlen((char *)buffer));
  }
  else
  {
    REST.set_response_status(response, REST.status.UNSUPPORTED_MEDIA_TYPE);
    const char *msg = "Supporting content-types text/plain and application/json";
    REST.set_response_payload(response, msg, strlen(msg));
  }
}
#endif /* PLATFORM_HAS_BATTERY */
/******************************************************************************/

void 
hw_init()
{
  led1_off();
}

PROCESS(rest_server_example, "Erbium Example Server");

AUTOSTART_PROCESSES(&rest_server_example, &sensors_process);

PROCESS_THREAD(rest_server_example, ev, data)
{
  PROCESS_BEGIN();
  PRINTF("Starting Erbium Example Server\n");

#ifdef RF_CHANNEL
  PRINTF("RF channel: %u\n", RF_CHANNEL);
#endif
#ifdef IEEE802154_PANID
  PRINTF("PAN ID: 0x%04X\n", IEEE802154_PANID);
#endif

  PRINTF("uIP buffer: %u\n", UIP_BUFSIZE);
  PRINTF("LL header: %u\n", UIP_LLH_LEN);
  PRINTF("IP+UDP header: %u\n", UIP_IPUDPH_LEN);
  PRINTF("REST max chunk: %u\n", REST_MAX_CHUNK_SIZE);

/* if static routes are used rather than RPL */
#if !UIP_CONF_IPV6_RPL && !defined (CONTIKI_TARGET_MINIMAL_NET) && !defined (CONTIKI_TARGET_NATIVE)
  set_global_address();
  configure_routing();
#endif

  /* Initialize the OSD Hardware. */
  hw_init();
  /* Initialize the REST engine. */
  rest_init_engine();

  /* Activate the application-specific resources. */
  rest_activate_resource(&resource_led1);
#if REST_RES_INFO
  rest_activate_resource(&resource_info);
#endif
  /* Activate the application-specific resources. */
#if defined (PLATFORM_HAS_PIR) && (REST_RES_EVENT)
  SENSORS_ACTIVATE(pir_sensor);
  rest_activate_event_resource(&resource_pir);
  PRINTF("ACTIVATE PIR\n");
#endif
#if defined (PLATFORM_HAS_LEDS)
#if REST_RES_LEDS
  rest_activate_resource(&resource_leds);
#endif
#if REST_RES_TOGGLE
  rest_activate_resource(&resource_toggle);
#endif
#endif /* PLATFORM_HAS_LEDS */
#if defined (PLATFORM_HAS_TEMPERATURE) && REST_RES_TEMPERATURE
  SENSORS_ACTIVATE(temperature_sensor);
  rest_activate_resource(&resource_temperature);
#endif
#if defined (PLATFORM_HAS_BATTERY) && REST_RES_BATTERY
  SENSORS_ACTIVATE(battery_sensor);
  rest_activate_resource(&resource_battery);
#endif

  /* Define application-specific events here. */
  while(1) {
    PROCESS_WAIT_EVENT();
#if defined (REST_RES_EVENT)
    if (ev == sensors_event ) {
      PRINTF("EVENT\n");
#if (REST_RES_EVENT && defined (PLATFORM_HAS_PIR))
    if (data == &pir_sensor) {
      PRINTF("PIR EVENT\n");
        /* Call the event_handler for this application-specific event. */
        pir_event_handler(&resource_pir);
        PRINTF("CALL EVENT HANDLER\n");
      }
#endif /* PLATFORM_HAS_PIR */
    }
#endif /* REST_RES_EVENT */
  } /* while (1) */

  PROCESS_END();
}
