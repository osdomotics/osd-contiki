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

#ifdef OSDPLUG
#define PLATFORM_HAS_LED  1
//#define PLATFORM_HAS_BUTTON  1
#define PLATFORM_HAS_OPTRIAC  1
#define PLATFORM_HAS_TEMPERATURE   1
#define PLATFORM_HAS_BATTERY 1
#endif

#if (defined (OSDLIGHT) || defined (OSDSHUTTER))
#define PLATFORM_HAS_LED  1
#define PLATFORM_HAS_BUTTON  1
#define PLATFORM_HAS_OPTRIAC  1
#define PLATFORM_HAS_TEMPERATURE   1
#define PLATFORM_HAS_BATTERY 1
#endif

/* Define which resources to include to meet memory constraints. */
#define REST_RES_MODEL 1
#define REST_RES_NAME 1
#define REST_RES_SW   1
#define REST_RES_RESET   1
#define REST_RES_TIMER   1
#define REST_RES_OPTRIAC 1
#define REST_RES_TEMPERATURE 1
#define REST_RES_EVENT 0
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


/* For CoAP-specific example: not required for normal RESTful Web service. */
#if WITH_COAP == 3
#include "er-coap-03.h"
#elif WITH_COAP == 7
#include "er-coap-07.h"
#elif WITH_COAP == 12
#include "er-coap-12.h"
#elif WITH_COAP == 13
#include "er-coap-13.h"
#else
#warning "Erbium example without CoAP-specifc functionality"
#endif /* CoAP-specific example */

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
RESOURCE(model, METHOD_GET, "p/model", "title=\"model\";rt=\"simple.dev.mdl\"");

/*
 * A handler function named [resource name]_handler must be implemented for each RESOURCE.
 * A buffer for the response payload is provided through the buffer pointer. Simple resources can ignore
 * preferred_size and offset, but must respect the REST_MAX_CHUNK_SIZE limit for the buffer.
 * If a smaller block size is requested for CoAP, the REST framework automatically splits the data.
 */
void
model_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  char message[100];
  int index = 0;
  int length = 0; /*           |<-------->| */

  /* Some data that has the length up to REST_MAX_CHUNK_SIZE. For more, see the chunk resource. */
       // jSON Format
#ifdef OSDPLUG
     index += sprintf(message + index,"{\n \"model\" : \"PingThePlug\"\n");
#endif
#ifdef OSDLIGHT
     index += sprintf(message + index,"{\n \"model\" : \"Light-Actor\"\n");
#endif
#ifdef OSDSHUTTER
     index += sprintf(message + index,"{\n \"model\" : \"Shutter-Control\n");
#endif
     index += sprintf(message + index,"}\n");

    length = strlen(message);
    memcpy(buffer, message,length );

  REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
  REST.set_response_payload(response, buffer, length);
}
#endif

/******************************************************************************/
#if REST_RES_SW
/*
 * Resources are defined by the RESOURCE macro.
 * Signature: resource name, the RESTful methods it handles, and its URI path (omitting the leading slash).
 */
RESOURCE(sw, METHOD_GET, "p/sw", "title=\"Software Version\";rt=\"simple.dev.sv\"");

/*
 * A handler function named [resource name]_handler must be implemented for each RESOURCE.
 * A buffer for the response payload is provided through the buffer pointer. Simple resources can ignore
 * preferred_size and offset, but must respect the REST_MAX_CHUNK_SIZE limit for the buffer.
 * If a smaller block size is requested for CoAP, the REST framework automatically splits the data.
 */
void
sw_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  char message[100];
  int index = 0;
  int length = 0; /*           |<-------->| */

  /* Some data that has the length up to REST_MAX_CHUNK_SIZE. For more, see the chunk resource. */
       // jSON Format
     index += sprintf(message + index,"{\n \"sw\" : \"V0.9\"\n");
     index += sprintf(message + index,"}\n");

    length = strlen(message);
    memcpy(buffer, message,length );

  REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
  REST.set_response_payload(response, buffer, length);
}
#endif

/******************************************************************************/
#if REST_RES_NAME
/*
 * Resources are defined by the RESOURCE macro.
 * Signature: resource name, the RESTful methods it handles, and its URI path (omitting the leading slash).
 */
RESOURCE(name,  METHOD_POST | METHOD_GET, "p/name", "title=\"name\";rt=\"simple.dev.n\"");
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
#endif


/******************************************************************************/
#if REST_RES_TIMER
/*A simple actuator example*/
RESOURCE(timer, METHOD_GET | METHOD_POST , "a/timer", "title=\"TIMER, POST timer=XXX\";rt=\"Control\"");
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
#endif

/******************************************************************************/
#if REST_RES_RESET
/*A simple actuator example*/
RESOURCE(reset, METHOD_GET | METHOD_POST , "p/reset", "title=\"RESET, POST mode=on\";rt=\"Control\"");
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
#endif

/******************************************************************************/
// pcintkey_ext
/*A simple actuator example. read the key button status*/
RESOURCE(extbutton, METHOD_GET | METHOD_POST , "s/extbutton",  "title=\"ext.Button\";rt=\"Text\"");
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

/******************************************************************************/
#if REST_RES_LED
/*A simple actuator example, depending on the color query parameter and post variable mode, corresponding led is activated or deactivated*/
RESOURCE(led1, METHOD_POST | METHOD_PUT , "a/led1", "title=\"LED: POST/PUT mode=on|off\";rt=\"simple.act.led\"");

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

/*A simple actuator example, depending on the color query parameter and post variable mode, corresponding led is activated or deactivated*/
RESOURCE(led2, METHOD_POST | METHOD_PUT , "a/led2", "title=\"LED: POST/PUT mode=on|off\";rt=\"simple.act.led\"");

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
#endif

/******************************************************************************/
#if (defined PLATFORM_HAS_OPTRIAC)
/******************************************************************************/
#if REST_RES_OPTRIAC
/*A simple actuator example*/
RESOURCE(optriac, METHOD_GET | METHOD_POST | METHOD_PUT , "a/optriac", "title=\"TRIAC: ?type=a|b, POST/PUT mode=on|off\";rt=\"Control\"");

void
optriac_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  const char *type = NULL;
  const char *mode = NULL;
  static char namea[17]="Triac-a";
  static char nameb[17]="Triac-b";

  char temp[100];
  int index = 0;
  size_t len = 0;

  uint8_t triac = 0;
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
     success = 0;
     break;
   case METHOD_POST:
  if ((len=REST.get_query_variable(request, "type", &type))) {
    PRINTF("type %.*s\n", len, type);

    if (strncmp(type, "a", len)==0) {
      triac = OPTRIAC_SENSOR_1;
    } else if(strncmp(type,"b", len)==0) {
      triac = OPTRIAC_SENSOR_2;
    } else {
      triac = OPTRIAC_SENSOR_1;
    }
  } else {
    success = 0;
  }

  if (success && (len=REST.get_post_variable(request, "mode", &mode))) {
    PRINTF("mode %s\n", mode);

    if (strncmp(mode, "on", len)==0) {
	  if (triac == OPTRIAC_SENSOR_1){
        g_triac_a = 1;
      } else {
        g_triac_b = 1;
      }  
    } else if (strncmp(mode, "off", len)==0) {
	  if (triac == OPTRIAC_SENSOR_1){
        g_triac_a = 0;
      } else {
        // Triac B off
        g_triac_b = 0;
      }  
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
#endif
/******************************************************************************/
#endif /* PLATFORM_HAS_OPTRIAC */


/******************************************************************************/
#if REST_RES_TEMPERATURE && defined (PLATFORM_HAS_TEMPERATURE)
/* A simple getter example. Returns the reading from light sensor with a simple etag */
RESOURCE(temperature, METHOD_GET, "s/cputemp", "title=\"Temperature status\";rt=\"temperature-c\"");
void
temperature_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  int temperature = temperature_sensor.value(0);

  const uint16_t *accept = NULL;
  int num = REST.get_header_accept(request, &accept);

  if ((num==0) || (num && accept[0]==REST.type.TEXT_PLAIN))
  {
    REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%d.%02d", temperature/100, temperature % 100);


    REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer));
  }
  else if (num && (accept[0]==REST.type.APPLICATION_JSON))
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
#endif /* PLATFORM_HAS_TEMPERATURE */

/******************************************************************************/
#if REST_RES_BATTERY && defined (PLATFORM_HAS_BATTERY)
/* A simple getter example. Returns the reading from light sensor with a simple etag */
RESOURCE(battery, METHOD_GET, "s/battery", "title=\"Battery status\";rt=\"battery-mV\"");
void
battery_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  int battery = battery_sensor.value(0);

  const uint16_t *accept = NULL;
  int num = REST.get_header_accept(request, &accept);

  if ((num==0) || (num && accept[0]==REST.type.TEXT_PLAIN))
  {
    REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%d.%02d", battery/1000, battery % 1000);

    REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer));
  }
  else if (num && (accept[0]==REST.type.APPLICATION_JSON))
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
  static struct etimer triac_off_timer;
  static uint8_t state=0;
  static int ext4=0;
  static int ext5=0;
  static int ext6=0;
//  ext4 = is_button_ext4();
//  ext5 = is_button_ext5();
//  ext6 = is_button_ext6();
  
	  
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
#if REST_RES_MODEL
  rest_activate_resource(&resource_model);
#endif
#if REST_RES_SW
  rest_activate_resource(&resource_sw);
#endif
#if REST_RES_NAME
  rest_activate_resource(&resource_name);
#endif
#if REST_RES_RESET
  rest_activate_resource(&resource_reset);
#endif
#if REST_RES_TIMER
  rest_activate_resource(&resource_timer);
#endif

  rest_activate_resource(&resource_extbutton);
  /* Activate the application-specific resources. */
#if REST_RES_OPTRIAC
  SENSORS_ACTIVATE(optriac_sensor);
  rest_activate_resource(&resource_optriac);
#endif

#if defined (PLATFORM_HAS_LED)
#if REST_RES_LED
  rest_activate_resource(&resource_led1);
  rest_activate_resource(&resource_led2);
#endif
#endif /* PLATFORM_HAS_LED */
#if defined (PLATFORM_HAS_TEMPERATURE) && REST_RES_TEMPERATURE
  SENSORS_ACTIVATE(temperature_sensor);
  rest_activate_resource(&resource_temperature);
#endif
#if defined (PLATFORM_HAS_BATTERY) && REST_RES_BATTERY
  SENSORS_ACTIVATE(battery_sensor);
  rest_activate_resource(&resource_battery);
#endif

  etimer_set(&ds_periodic_timer, MESURE_INTERVAL);
  /* Define application-specific events here. */
  
  while(1) {
    PROCESS_WAIT_EVENT();
    /* Button Tric Logic */

    if(etimer_expired(&ds_periodic_timer)) {
        PRINTF("Periodic \n");
        PRINTF("extb %d %d\n",ext5,ext6);
        PRINTF("g_triac %d %d\n",g_triac_a,g_triac_b);

	if( is_button_ext5()) {
        g_triac_a = 1;
	}
	if( is_button_ext6()) {
        g_triac_b = 1;		
	}
	

#if (defined (OSDSHUTTER))    
    PRINTF("State: %d\n",state);		          
	switch(state)
	    {
        case  0:
	    if( g_triac_a == 1) {
          PRINTF("Triac A\n");
          g_triac_a = 0;
          // Triac B off
          optriac_sensor.configure(OPTRIAC_SENSOR_2,0);
          statusled_off();
          state = 1;
        }
	    if( g_triac_b == 1) {
          PRINTF("Triac B\n");
          g_triac_b = 0;
          // Triac A off        
          optriac_sensor.configure(OPTRIAC_SENSOR_1,0);
          leds_off(LEDS_RED);
          state = 2;
        }
        break;
        case 1:
          // Triac A on 
          optriac_sensor.configure(OPTRIAC_SENSOR_1,1);
          leds_on(LEDS_RED);
          etimer_set(&triac_off_timer, gtimer_read()*CLOCK_SECOND);
          state=0; 
        break;    
        case 2:
          // Triac B on 
          optriac_sensor.configure(OPTRIAC_SENSOR_2,1);
          statusled_on();
          etimer_set(&triac_off_timer, gtimer_read()*CLOCK_SECOND);
          state=0;         
        break;    
        default: state = 0;
  	} // switch
#endif
      etimer_reset(&ds_periodic_timer);
    }
  
    if(etimer_expired(&triac_off_timer)) {
        PRINTF("Triac off timer\n");
        // Triac A off
        optriac_sensor.configure(OPTRIAC_SENSOR_1,0);
        leds_off(LEDS_RED);
        g_triac_a = 0;
        // Triac B off
        optriac_sensor.configure(OPTRIAC_SENSOR_2,0);
        statusled_off();
        g_triac_b = 0;
    }
    
  } /* while (1) */

  PROCESS_END();
}
