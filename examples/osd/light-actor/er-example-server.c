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
#include <avr/eeprom.h>
#include "rest-engine.h"

#define PLATFORM_HAS_LED  1
//#define PLATFORM_HAS_BUTTON  1
#define PLATFORM_HAS_OPTRIAC  1
#define PLATFORM_HAS_TEMPERATURE   1
#define PLATFORM_HAS_BATTERY 1


/* Define which resources to include to meet memory constraints. */
#define REST_RES_MODEL 1
#define REST_RES_NAME 1
#define REST_RES_SW   1
#define REST_RES_RESET   1
#define REST_RES_TIMER   1
#define REST_RES_OPTRIAC 1
#define REST_RES_TEMPERATURE 1
#define REST_RES_LED 1
#define REST_RES_BATTERY 1

#include "pcintkey.h"
#include "statusled.h"

#if defined (PLATFORM_HAS_BUTTON)
#include "dev/button-sensor.h"
#endif
#if defined (PLATFORM_HAS_LED)
#include "dev/leds.h"
#endif
#if defined (PLATFORM_HAS_OPTRIAC)
#include "dev/optriac-sensor.h"
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
uint8_t g_triac_a = 0;
uint8_t g_triac_b = 0;

/******************************************************************************/
#if REST_RES_MODEL
/*
 * Resources are defined by the RESOURCE macro.
 * Signature: resource name, the RESTful methods it handles, and its URI path (omitting the leading slash).
 */

/*
 * A handler function named [resource name]_handler must be implemented for each RESOURCE.
 * A buffer for the response payload is provided through the buffer pointer. Simple resources can ignore
 * preferred_size and offset, but must respect the REST_MAX_CHUNK_SIZE limit for the buffer.
 * If a smaller block size is requested for CoAP, the REST framework automatically splits the data.
 */
void
model_get_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  char message[100];
  int index = 0;
  int length = 0; /*           |<-------->| */

  /* Some data that has the length up to REST_MAX_CHUNK_SIZE. For more, see the chunk resource. */
       // jSON Format
     index += sprintf(message + index,"{\n \"model\" : \"LightActor\"\n");
     index += sprintf(message + index,"}\n");

    length = strlen(message);
    memcpy(buffer, message,length );

  REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
  REST.set_response_payload(response, buffer, length);
}
RESOURCE(res_model, "title=\"model\";rt=\"simple.dev.md\"", model_get_handler, NULL, NULL, NULL);

#endif

/******************************************************************************/
#if REST_RES_SW
/*
 * Resources are defined by the RESOURCE macro.
 * Signature: resource name, the RESTful methods it handles, and its URI path (omitting the leading slash).
 */

/*
 * A handler function named [resource name]_handler must be implemented for each RESOURCE.
 * A buffer for the response payload is provided through the buffer pointer. Simple resources can ignore
 * preferred_size and offset, but must respect the REST_MAX_CHUNK_SIZE limit for the buffer.
 * If a smaller block size is requested for CoAP, the REST framework automatically splits the data.
 */
void
sw_get_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  char message[100];
  int index = 0;
  int length = 0; /*           |<-------->| */

  /* Some data that has the length up to REST_MAX_CHUNK_SIZE. For more, see the chunk resource. */
       // jSON Format
     index += sprintf(message + index,"{\n \"sw\" : \"V1.0\"\n");
     index += sprintf(message + index,"}\n");

    length = strlen(message);
    memcpy(buffer, message,length );

  REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
  REST.set_response_payload(response, buffer, length);
}
RESOURCE(res_sw, "title=\"Software Version\";rt=\"simple.dev.sv\"", sw_get_handler, NULL, NULL, NULL);
#endif

/******************************************************************************/
#if REST_RES_NAME
/*
 * Resources are defined by the RESOURCE macro.
 * Signature: resource name, the RESTful methods it handles, and its URI path (omitting the leading slash).
 */

/* eeprom space */ 
#define P_NAME "Testboard"
#define P_NAME_MAX 17
uint8_t eemem_p_name[P_NAME_MAX] EEMEM = P_NAME;

/*
 * A handler function named [resource name]_handler must be implemented for each RESOURCE.
 * A buffer for the response payload is provided through the buffer pointer. Simple resources can ignore
 * preferred_size and offset, but must respect the REST_MAX_CHUNK_SIZE limit for the buffer.
 * If a smaller block size is requested for CoAP, the REST framework automatically splits the data.
 */
void
name_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  uint8_t eebuffer[32];
  char message[100];
  int index = 0;
  int length = 0; /*           |<-------->| */
  const char *name = NULL;
  int success = 1;

  switch(REST.get_method_type(request)){
   case METHOD_GET:
     cli();
     eeprom_read_block (eebuffer, &eemem_p_name, sizeof(eemem_p_name));
     sei();
     /* Some data that has the length up to REST_MAX_CHUNK_SIZE. For more, see the chunk resource. */
     // jSON Format
     index += sprintf(message + index,"{\n \"name\" : \"%s\"\n",eebuffer);
     index += sprintf(message + index,"}\n");

     length = strlen(message);
     memcpy(buffer, message,length );

     REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
     REST.set_response_payload(response, buffer, length);
     break;
     
   case METHOD_POST:     
     if (success &&  (length=REST.get_post_variable(request, "name", &name))) {
       PRINTF("name %s\n", name);
       if (length < P_NAME_MAX) {
         memcpy(&eebuffer, name,length);
         eebuffer[length]=0;
         cli();
         eeprom_write_block(&eebuffer,  &eemem_p_name, sizeof(eemem_p_name));
         sei();
       } else {
         success = 0;
       }		   
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
RESOURCE(res_name, "title=\"name\";rt=\"simple.dev.n\"", name_handler, NULL, name_handler, NULL );
#endif


/******************************************************************************/
#if REST_RES_TIMER
/*A simple actuator example*/

/* eeprom space */ 
#define P_TIMER "60"
#define P_TIMER_MAX 10
uint8_t eemem_p_timer[P_TIMER_MAX] EEMEM = P_TIMER;

int gtimer_read(){
  uint8_t eebuffer[32];

  cli();
  eeprom_read_block (eebuffer, &eemem_p_timer, sizeof(eemem_p_timer));
  sei();
  return atoi((const char *)eebuffer);
}

void
timer_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  uint8_t eebuffer[32];
  const char *timer = NULL;
  char message[100];
  int length = 0; /*           |<-------->| */
  int index = 0;
  int success = 1;

  switch(REST.get_method_type(request)){

   case METHOD_GET:
     cli();
     eeprom_read_block (eebuffer, &eemem_p_timer, sizeof(eemem_p_timer));
     sei();
     /* Some data that has the length up to REST_MAX_CHUNK_SIZE. For more, see the chunk resource. */
     // jSON Format
     index += sprintf(message + index,"{\n \"timer\" : \"%s\"\n",eebuffer);
     index += sprintf(message + index,"}\n");

     length = strlen(message);
     memcpy(buffer, message,length );

     REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
     REST.set_response_payload(response, buffer, length);
     break;

   case METHOD_POST:
     if (success &&  (length=REST.get_post_variable(request, "timer", &timer))) {
       PRINTF("name %s\n", timer);
       if (length < P_TIMER_MAX) {
		 memcpy(&eebuffer, timer,length);
         eebuffer[length]=0;     
         cli();
         eeprom_write_block(&eebuffer,  &eemem_p_timer, sizeof(eemem_p_timer));
         sei();
       } else {
         success = 0;
       }
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
RESOURCE(res_timer, "title=\"timer\";rt=\"timer\"", timer_handler, NULL, timer_handler, NULL );
#endif

/******************************************************************************/
#if REST_RES_RESET
/*A simple actuator example*/

/* eeprom space */ 
#define P_RESET "0"
#define P_RESET_MAX 10
uint8_t eemem_p_reset[P_RESET_MAX] EEMEM = P_RESET;

void
reset_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  uint8_t eebuffer[32];
  const char *mode = NULL;
  char message[100];
  int length = 0; /*           |<-------->| */
  int index = 0;
  int reset = 0;
  size_t len = 0;
  int success = 1;

  switch(REST.get_method_type(request)){

   case METHOD_GET:
     cli();
     eeprom_read_block (eebuffer, &eemem_p_reset, sizeof(eemem_p_reset));
     sei();
     /* Some data that has the length up to REST_MAX_CHUNK_SIZE. For more, see the chunk resource. */
     // jSON Format
     index += sprintf(message + index,"{\n \"reset\" : \"%s\"\n",eebuffer);
     index += sprintf(message + index,"}\n");

     length = strlen(message);
     memcpy(buffer, message,length );

     REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
     REST.set_response_payload(response, buffer, length);
     break;

   case METHOD_POST:
     if (success && (len=REST.get_post_variable(request, "mode", &mode))) {
     PRINTF("mode %s\n", mode);
       if (strncmp(mode, "on", len)==0) {
		 length=strlen(P_NAME);  
         memcpy(&eebuffer, P_NAME,length);
         eebuffer[length]=0;		   
         cli();
         eeprom_write_block(&eebuffer,  &eemem_p_name, sizeof(eemem_p_name));
         sei();
		 length=strlen(P_TIMER);  
         memcpy(&eebuffer, P_TIMER,length);
         eebuffer[length]=0;		   
         cli();
         eeprom_write_block(&eebuffer,  &eemem_p_timer, sizeof(eemem_p_timer));         
         eeprom_read_block (eebuffer, &eemem_p_reset, sizeof(eemem_p_reset));
         sei();
         reset= atoi((char*)eebuffer) + 1;
		 length=sprintf((char*)eebuffer,"%d",reset);  
         cli();
         eeprom_write_block(&eebuffer,  &eemem_p_reset, sizeof(eemem_p_reset));
         sei();		   
       } else {
         success = 0;
       }
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
RESOURCE(res_reset, "title=\"reset\";rt=\"reset\"", reset_handler, NULL, reset_handler, NULL );
#endif

/******************************************************************************/
// pcintkey_ext
/*A simple actuator example. read the key button status*/

void
extbutton_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  static char bname1[17]="button1";
  static char bname2[17]="button2";
  static char bname3[17]="button3";
  int success = 1;

  char temp[100];
  int index = 0;
  int length = 0; /*           |<-------->| */
  const char *name = NULL;
  size_t len = 0;

  switch(REST.get_method_type(request)){
   case METHOD_GET:
     // jSON Format
     index += sprintf(temp + index,"{\n \"%s\" : ",bname1);
     if(is_button_ext4())
         index += sprintf(temp + index,"\"on\",\n");
     else
         index += sprintf(temp + index,"\"off\",\n");
     index += sprintf(temp + index,"{\n \"%s\" : ",bname2);
     if(is_button_ext5())
         index += sprintf(temp + index,"\"on\",\n");
     else
         index += sprintf(temp + index,"\"off\",\n");
     index += sprintf(temp + index," \"%s\" : ",bname3);
     if(is_button_ext6())
         index += sprintf(temp + index,"\"on\"\n");
     else
         index += sprintf(temp + index,"\"off\"\n");
     index += sprintf(temp + index,"}\n");

     length = strlen(temp);
     memcpy(buffer, temp,length );

     REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
     REST.set_response_payload(response, buffer, length);

     break;
   case METHOD_POST:

     if (success &&  (len=REST.get_post_variable(request, "name", &name))) {
       PRINTF("name %s\n", name);
       memcpy(bname1, name,len);
       bname1[len]=0;
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
RESOURCE(res_extbutton, "title=\"button\";rt=\"button\"", extbutton_handler, NULL, extbutton_handler, NULL );

/******************************************************************************/
#if REST_RES_LED
/*A simple actuator example, depending on the color query parameter and post variable mode, corresponding led is activated or deactivated*/

void
led1_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  size_t len = 0;
  const char *mode = NULL;
  uint8_t led = 0;
  int success = 1;

  led = LEDS_RED;

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
RESOURCE(res_led1, "title=\"LED: PUT mode=on|off\";rt=\"simple.act.led\"", led1_handler, NULL, led1_handler, NULL );

/*A simple actuator example, depending on the color query parameter and post variable mode, corresponding led is activated or deactivated*/

void
led2_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  size_t len = 0;
  const char *mode = NULL;
  int success = 1;


  if (success && (len=REST.get_post_variable(request, "mode", &mode))) {
    PRINTF("mode %s\n", mode);

    if (strncmp(mode, "on", len)==0) {
      statusled_on();
    } else if (strncmp(mode, "off", len)==0) {
      statusled_off();
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
RESOURCE(res_led2, "title=\"LED: PUT mode=on|off\";rt=\"simple.act.led\"", led2_handler, NULL, led2_handler, NULL );
#endif

#if REST_RES_OPTRIAC
/*A simple actuator example*/

void
optriac_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  const char *mode = NULL;
  static char namea[17]="Triac-a";
  static char nameb[17]="Triac-b";

  char temp[100];
  int index = 0;
  size_t len = 0;
  int success = 1;

  switch(REST.get_method_type(request)){
   case METHOD_GET:
     // jSON Format
     index += sprintf(temp + index,"{\n \"%s\" : ",namea);
     if(optriac_sensor.value(OPTRIAC_SENSOR_1) == 0)
         index += sprintf(temp + index,"\"off\",\n");
     if(optriac_sensor.value(OPTRIAC_SENSOR_1) == 1)
         index += sprintf(temp + index,"\"on\",\n");
     index += sprintf(temp + index," \"%s\" : ",nameb);
     if(optriac_sensor.value(OPTRIAC_SENSOR_2) == 0)
         index += sprintf(temp + index,"\"off\"\n");
     if(optriac_sensor.value(OPTRIAC_SENSOR_2) == 1)
         index += sprintf(temp + index,"\"on\"\n");
     index += sprintf(temp + index,"}\n");

     len = strlen(temp);
     memcpy(buffer, temp,len );

     REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
     REST.set_response_payload(response, buffer, len);
     break;

   case METHOD_PUT:
     if (success && (len=REST.get_post_variable(request, "mode", &mode))) {
     PRINTF("mode %s\n", mode);
       if (strncmp(mode, "on", len)==0) {
         optriac_sensor.configure(OPTRIAC_SENSOR_1,1);
         optriac_sensor.configure(OPTRIAC_SENSOR_2,1);
         statusled_on();
       } else if (strncmp(mode, "off", len)==0) {
         optriac_sensor.configure(OPTRIAC_SENSOR_1,0);
         optriac_sensor.configure(OPTRIAC_SENSOR_2,0);
		 statusled_off();
       } else {
         success = 0;
       }
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
RESOURCE(res_optriac, "title=\"TRIAC, PUT mode=on|off\";rt=\"simple.act.triac\"", optriac_handler, NULL, optriac_handler, NULL );
#endif /* PLATFORM_HAS_OPTRIAC */

/******************************************************************************/
#if REST_RES_TEMPERATURE && defined (PLATFORM_HAS_TEMPERATURE)
/* A simple getter example. Returns the reading from light sensor with a simple etag */

void
temperature_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  int temperature = temperature_sensor.value(0);  
  
  unsigned int accept = -1;
  REST.get_header_accept(request, &accept);

  if(accept == -1 || accept == REST.type.TEXT_PLAIN) 
  {
    REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%d.%02d", temperature/100, temperature % 100);


    REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer));
  }
  else if (accept == REST.type.APPLICATION_JSON)
  {
    REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'temperature':%d.%02d}", temperature/100, temperature % 100);

    REST.set_response_payload(response, buffer, strlen((char *)buffer));
  }
  else
  {
    REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
    const char *msg = "Supporting content-types text/plain and application/json";
    REST.set_response_payload(response, msg, strlen(msg));
  }
}
RESOURCE(res_temperature, "title=\"Temperature status\";rt=\"temperature-c\"", temperature_handler, NULL, NULL, NULL );
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

  if(accept == -1 || accept == REST.type.TEXT_PLAIN) 
  {
    REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%d.%02d", battery/1000, battery % 1000);

    REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer));
  }
  else if (accept == REST.type.APPLICATION_JSON)
  {
    REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{\"battery\":%d.%02d}", battery/1000, battery % 1000);
    
    REST.set_response_payload(response, buffer, strlen((char *)buffer));
  }
  else
  {
    REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
    const char *msg = "Supporting content-types text/plain and application/json";
    REST.set_response_payload(response, msg, strlen(msg));
  }
}
RESOURCE(res_battery, "title=\"Battery status\";rt=\"battery-mV\"", battery_handler, NULL, NULL, NULL );

#endif /* PLATFORM_HAS_BATTERY */
/******************************************************************************/



void 
hw_init()
{
  leds_off(LEDS_RED);
  statusledinit();
  key_init();
}

#define MESURE_INTERVAL		(CLOCK_SECOND/2)

PROCESS(rest_server_example, "Erbium Example Server");

AUTOSTART_PROCESSES(&rest_server_example, &sensors_process);

PROCESS_THREAD(rest_server_example, ev, data)
{
  static struct etimer ds_periodic_timer;
  static int ext4=0;
  static int ext5=0;
  static int ext6=0;
//  ext4 = is_button_ext4();
//  ext5 = is_button_ext5();
//  ext6 = is_button_ext6();
  
	  
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
#if REST_RES_MODEL
  rest_activate_resource(&res_model,"p/model");
#endif
#if REST_RES_SW
  rest_activate_resource(&res_sw,"p/sw");
#endif
#if REST_RES_NAME
  rest_activate_resource(&res_name,"p/name");
#endif
#if REST_RES_RESET
  rest_activate_resource(&res_reset,"p/reset");
#endif
#if REST_RES_TIMER
  rest_activate_resource(&res_timer,"a/timer");
#endif

  rest_activate_resource(&res_extbutton,"s/extbutton");
  /* Activate the application-specific resources. */
#if REST_RES_OPTRIAC
  SENSORS_ACTIVATE(optriac_sensor);
  rest_activate_resource(&res_optriac,"a/optriac");
#endif
#if defined (PLATFORM_HAS_LED)
#if REST_RES_LED
  rest_activate_resource(&res_led1,"a/led1");
  rest_activate_resource(&res_led2,"a/led2");
#endif
#endif /* PLATFORM_HAS_LED */
#if defined (PLATFORM_HAS_TEMPERATURE) && REST_RES_TEMPERATURE
  SENSORS_ACTIVATE(temperature_sensor);
  rest_activate_resource(&res_temperature,"s/cputemp");
#endif
#if defined (PLATFORM_HAS_BATTERY) && REST_RES_BATTERY
  SENSORS_ACTIVATE(battery_sensor);
  rest_activate_resource(&res_battery,"s/battery");
#endif

  etimer_set(&ds_periodic_timer, MESURE_INTERVAL);
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
    /* Button Tric Logic */
    if(etimer_expired(&ds_periodic_timer)) {
        PRINTF("Periodic %d %d\n",ext5,ext6);
	if(ext5 != is_button_ext5()) {
	  ext5 = is_button_ext5();
          PRINTF("Toggle Triac A\n");
          // Toggle Triac A
          if(optriac_sensor.value(OPTRIAC_SENSOR_1) == 0){
            optriac_sensor.configure(OPTRIAC_SENSOR_1,1);
            led1_on();
          }else{
            optriac_sensor.configure(OPTRIAC_SENSOR_1,0);
            led1_off();
          }
	}
	if(ext6 != is_button_ext6()) {
	  ext6 = is_button_ext6();
          PRINTF("Toggle Triac B\n");
          // Toggle Triac B
          if(optriac_sensor.value(OPTRIAC_SENSOR_2) == 0){
            optriac_sensor.configure(OPTRIAC_SENSOR_2,1);
            led2_on();
          }else{
            optriac_sensor.configure(OPTRIAC_SENSOR_2,0);
            led2_off();
          }
	}
      etimer_reset(&ds_periodic_timer);
    }
  } /* while (1) */

  PROCESS_END();
}
