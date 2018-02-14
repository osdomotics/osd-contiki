/*
 * Copyright (C) 2011, Matthias Kovatsch and other contributors.
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
 *      Erbium (Er) SERVO REST Engine example (with CoAP-specific code)
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 *      Harald Pichler <harald@the-develop.net>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "rest-engine.h"


/* Define which resources to include to meet memory constraints. */
#define REST_RES_INFO 1
#define REST_RES_SERVO 1
#define REST_RES_T4_SERVO 0
#define REST_RES_LEDS 0
#define REST_RES_TOGGLE 0
#define REST_RES_BATTERY 1

#if defined (PLATFORM_HAS_BUTTON)
#include "dev/button-sensor.h"
#endif
#if defined (PLATFORM_HAS_LEDS)
#include "dev/leds.h"
#endif
#if defined (PLATFORM_HAS_SERVO)
#include "dev/servo-sensor.h"
#endif
#if defined (PLATFORM_HAS_T4_SERVO)
#include "dev/t4-servo-sensor.h"
#endif
#if defined (PLATFORM_HAS_TEMPERATURE)
#include "dev/temperature-sensor.h"
#endif
#if defined (PLATFORM_HAS_BATTERY)
#include "dev/battery-sensor.h"
#endif


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

/******************************************************************************/

#if REST_RES_INFO
/*
 * A handler function named [resource name]_handler must be implemented for each RESOURCE.
 * A buffer for the response payload is provided through the buffer pointer. Simple resources can ignore
 * preferred_size and offset, but must respect the REST_MAX_CHUNK_SIZE limit for the buffer.
 * If a smaller block size is requested for CoAP, the REST framework automatically splits the data.
 */
void
info_get_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  char message[100];
  int index = 0;
  int length = 0; /*           |<-------->| */

  /* Some data that has the length up to REST_MAX_CHUNK_SIZE. For more, see the chunk resource. */
       // jSON Format
     index += sprintf(message + index,"{\n \"Version\" : \"V1.0pre1\",\n");
     index += sprintf(message + index," \"name\" : \"6lowpan-SERVO\"\n");
     index += sprintf(message + index,"}\n");

    length = strlen(message);
    memcpy(buffer, message,length );

  REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
  REST.set_response_payload(response, buffer, length);
}

/*
 * Resources are defined by the RESOURCE macro.
 * Signature: resource name, the RESTful methods it handles, and its URI path (omitting the leading slash).
 */
RESOURCE(res_info, "title=\"Info\";rt=\"text\"", info_get_handler, NULL, NULL, NULL);

#endif

#if defined (PLATFORM_HAS_SERVO)
/*A simple actuator example. read the servo status*/
void
servo_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  int success = 1;

  char temp[100];
  int index = 0;
  int length = 0; /*           |<-------->| */
  const char *value = NULL;
  size_t len = 0;

  int servo1 = servo_sensor.value(0);
  int servo2 = servo_sensor.value(1);

  switch(REST.get_method_type(request)){
   case METHOD_GET:
     // jSON Format
     index += sprintf(temp + index,"{\n \"servo1\" : \"%d\",\n",servo1);
     index += sprintf(temp + index,"\n \"servo2\" : \"%d\"\n",servo2);
     index += sprintf(temp + index,"}\n");

     length = strlen(temp);
     memcpy(buffer, temp,length );

     REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
     REST.set_response_payload(response, buffer, length);

     break;
   case METHOD_PUT:

     if (success &&  (len=REST.get_post_variable(request, "servo1", &value))) {
       PRINTF("servo1 %s\n", value);
       servo_sensor.configure(0,atoi(value));
     } else {
       success = 0;
     }
     if (success &&  (len=REST.get_post_variable(request, "servo2", &value))) {
       PRINTF("servo2 %s\n", value);
       servo_sensor.configure(1,atoi(value));
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
RESOURCE(res_servo, "title=\"Servo\";rt=\"servo\"", servo_handler, NULL, servo_handler, NULL );
#endif

#if defined (PLATFORM_HAS_T4_SERVO)
void
t4_servo_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  int success = 1;

  char temp[100];
  int index = 0;
  int length = 0; /*           |<-------->| */
  const char *value = NULL;
  size_t len = 0;

  int red   = t4_servo_sensor.value(0);
  int green = t4_servo_sensor.value(1);
  int blue  = t4_servo_sensor.value(2);
  int white = t4_servo_sensor.value(3);

  switch(REST.get_method_type(request)){
   case METHOD_GET:
     // jSON Format
     index += sprintf(temp + index,"{\n \"red\" : \"%d\",",  red);
     index += sprintf(temp + index, "\n \"green\" : \"%d\",", green);
     index += sprintf(temp + index, "\n \"blue\" : \"%d\",",  blue);
     index += sprintf(temp + index, "\n \"white\" : \"%d\"", white);
     index += sprintf(temp + index,"}\n");

     length = strlen(temp);
     memcpy(buffer, temp, length);

     REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
     REST.set_response_payload(response, buffer, length);

     break;

   case METHOD_POST:
     success = 0;
     break;

   case METHOD_PUT:
     // Note that we have to set all at once:
     // red=255&green=255&blue=255&white=255
     // otherwise we get "bad request"
     if (success &&  (len=REST.get_post_variable(request, "red", &value)))
     {
       t4_servo_sensor.configure(0,atoi(value));
     } else {
       success = 0;
     }

     if (success &&  (len=REST.get_post_variable(request, "green", &value))) {
       t4_servo_sensor.configure(1,atoi(value));
     } else {
       success = 0;
     }

     if (success &&  (len=REST.get_post_variable(request, "blue", &value))) {
       t4_servo_sensor.configure(2,atoi(value));
     } else {
       success = 0;
     }

     if (success &&  (len=REST.get_post_variable(request, "white", &value))) {
       t4_servo_sensor.configure(3,atoi(value));
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
RESOURCE(res_t4_servo,"title=\"Timer4Servo\";rt=\"t4_servo\"", t4_servo_handler, te4_servo_handler, te4_servo_handler, NULL );
#endif


/******************************************************************************/
#if defined (PLATFORM_HAS_LEDS)
/******************************************************************************/
#if REST_RES_LEDS
/*A simple actuator example, depending on the color query parameter and post variable mode, corresponding led is activated or deactivated*/
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
RESOURCE(res_leds, "title=\"LEDs: ?color=r|g|b, POST/PUT mode=on|off\";rt=\"Control\"", NULL, leds_handler, leds_handler, NULL);
#endif

/******************************************************************************/
#if REST_RES_TOGGLE
/* A simple actuator example. Toggles the red led */
void
toggle_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  leds_toggle(LEDS_RED);
}
RESOURCE(res_toggle, "title=\"Red LED\";rt=\"Control\"", toggle_handler, toggle_handler, toggle_handler, NULL);
#endif
#endif /* PLATFORM_HAS_LEDS */

/******************************************************************************/

/******************************************************************************/
#if REST_RES_TEMPERATURE && defined (PLATFORM_HAS_TEMPERATURE)
/* A simple getter example. Returns the reading from light sensor with a simple etag */
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
RESOURCE(res_temperature, "title=\"Temperature status\";rt=\"temperature-c\"", temperature_handler, NULL, NULL, NULL);
#endif /* PLATFORM_HAS_TEMPERATURE */

/******************************************************************************/
#if REST_RES_BATTERY && defined (PLATFORM_HAS_BATTERY)
/* A simple getter example. Returns the reading from light sensor with a simple etag */
void
battery_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  int battery = battery_sensor.value(0);

  unsigned int accept = -1;
  REST.get_header_accept(request, &accept);

  if(accept == -1 || accept == REST.type.TEXT_PLAIN) {
    REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%d.%02d", battery/1000, battery % 1000);

    REST.set_response_payload(response, buffer, strlen((char *)buffer));
  } else if(accept == REST.type.APPLICATION_JSON) {
    REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'battery':%d.%02d}", battery/1000, battery % 1000);

    REST.set_response_payload(response, buffer, strlen((char *)buffer));
  } else {
    REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
    const char *msg = "Supporting content-types text/plain and application/json";
    REST.set_response_payload(response, msg, strlen(msg));
  }
}
RESOURCE(res_battery, "title=\"Battery status\";rt=\"battery-mV\"", battery_handler, NULL, NULL, NULL);
#endif /* PLATFORM_HAS_BATTERY */
/******************************************************************************/

void 
hw_init()
{
#if defined (PLATFORM_HAS_LEDS)
 leds_off(LEDS_RED);
#endif
}

PROCESS(rest_server_example, "Erbium Example Server");

AUTOSTART_PROCESSES(&rest_server_example, &sensors_process);

PROCESS_THREAD(rest_server_example, ev, data)
{
  PROCESS_BEGIN();
  PRINTF("Starting Erbium Example Server\n");
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
#if REST_RES_INFO
  rest_activate_resource(&res_info, "info");
#endif
#if defined (PLATFORM_HAS_LEDS)
#if REST_RES_LEDS
  rest_activate_resource(&res_leds, "actuators/leds");
#endif
#if REST_RES_TOGGLE
  rest_activate_resource(&res_toggle, "actuators/toggle");
#endif
#endif /* PLATFORM_HAS_LEDS */
#if defined (PLATFORM_HAS_TEMPERATURE) && REST_RES_TEMPERATURE
  SENSORS_ACTIVATE(temperature_sensor);
  rest_activate_resource(&res_temperature, "sensors/cputemp");
#endif
#if defined (PLATFORM_HAS_BATTERY) && REST_RES_BATTERY
  SENSORS_ACTIVATE(battery_sensor);
  rest_activate_resource(&res_battery, "sensors/battery");
#endif
#if defined (PLATFORM_HAS_SERVO) && REST_RES_SERVO
  SENSORS_ACTIVATE(servo_sensor);
  rest_activate_resource(&res_servo, "actuators/servo");
#endif
#if defined (PLATFORM_HAS_T4_SERVO) && REST_RES_T4_SERVO
  SENSORS_ACTIVATE(t4_servo_sensor);
  rest_activate_resource(&res_t4_servo, "actuators/t4_servo");
#endif

  /* Define application-specific events here. */
  while(1) {
    PROCESS_WAIT_EVENT();
  } /* while (1) */

  PROCESS_END();
}
