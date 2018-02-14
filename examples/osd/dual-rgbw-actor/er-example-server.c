/*
 * Copyright (C) 2011-2014, Matthias Kovatsch and other contributors.
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
 *      Erbium (Er) Dual RGBW Led REST Engine example (with CoAP-specific code)
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 *      Harald Pichler <harald@the-develop.net>
 *      Marcus Priesch <marcus@priesch.co.at>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"

#include "dev/t4-servo-sensor.h"

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
     index += sprintf(message + index," \"name\" : \"Dual RGBW Led Actor\"\n");
     index += sprintf(message + index,"}\n");

    length = strlen(message);
    memcpy(buffer, message,length );

  REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
  REST.set_response_payload(response, buffer, length);
}

RESOURCE(t4_servo, METHOD_GET | METHOD_PUT , "a/dual_rgbw_led", "title=\"Dual RGBW Led Actor\";rt=\"dual_rgbw_led\"");
void
t4_servo_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  int success = 1;

  char temp[100];
  int index = 0;
  int length = 0;
  const char *value = NULL;
  size_t len = 0;

  int r1 = t4_servo_sensor.value(0);
  int g1 = t4_servo_sensor.value(1);
  int b1 = t4_servo_sensor.value(2);
  int w1 = t4_servo_sensor.value(3);
  int r2 = t4_servo_sensor.value(4);
  int g2 = t4_servo_sensor.value(5);
  int b2 = t4_servo_sensor.value(6);
  int w2 = t4_servo_sensor.value(7);

  switch(REST.get_method_type(request)){
   case METHOD_GET:
     // jSON Format
     index += sprintf(temp + index,"{\"r1\":%d,", r1);
     index += sprintf(temp + index, "\"g1\":%d,", g1);
     index += sprintf(temp + index, "\"b1\":%d,", b1);
     index += sprintf(temp + index, "\"w1\":%d,", w1);
     index += sprintf(temp + index, "\"r2\":%d,", r2);
     index += sprintf(temp + index, "\"g2\":%d,", g2);
     index += sprintf(temp + index, "\"b2\":%d,", b2);
     index += sprintf(temp + index, "\"w2\":%d,", w2);
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
     if (success &&  (len=REST.get_post_variable(request, "r1", &value))) {
       t4_servo_sensor.configure(0,(unsigned char) atoi(value));
     } else {
       success = 0;
     }

     if (success &&  (len=REST.get_post_variable(request, "g1", &value))) {
       t4_servo_sensor.configure(1,(unsigned char) atoi(value));
     } else {
       success = 0;
     }

     if (success &&  (len=REST.get_post_variable(request, "b1", &value))) {
       t4_servo_sensor.configure(2,(unsigned char) atoi(value));
     } else {
       success = 0;
     }

     if (success &&  (len=REST.get_post_variable(request, "w1", &value))) {
       t4_servo_sensor.configure(3,(unsigned char) atoi(value));
     } else {
       success = 0;
     }

     if (success &&  (len=REST.get_post_variable(request, "r2", &value)))
     {
       t4_servo_sensor.configure(4,(unsigned char) atoi(value));
     } else {
       success = 0;
     }

     if (success &&  (len=REST.get_post_variable(request, "g2", &value))) {
       t4_servo_sensor.configure(5,(unsigned char) atoi(value));
     } else {
       success = 0;
     }

     if (success &&  (len=REST.get_post_variable(request, "b2", &value))) {
       t4_servo_sensor.configure(6,(unsigned char) atoi(value));
     } else {
       success = 0;
     }

     if (success &&  (len=REST.get_post_variable(request, "w2", &value))) {
       t4_servo_sensor.configure(7,(unsigned char) atoi(value));
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

void
hw_init()
{
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
  rest_activate_resource(&resource_info);

  SENSORS_ACTIVATE(t4_servo_sensor);
  rest_activate_resource(&resource_t4_servo);

  /* Define application-specific events here. */
  while(1) {
    PROCESS_WAIT_EVENT();
  }

  PROCESS_END();
}
