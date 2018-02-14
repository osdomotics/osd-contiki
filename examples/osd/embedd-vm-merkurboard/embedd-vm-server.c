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
#include "contiki.h"
#include "contiki-net.h"
#include "rest-engine.h"


/* Define which resources to include to meet memory constraints. */
#define REST_RES_INFO 1
#define REST_RES_EVM 1
#define REST_RES_BATTERY 1
#define REST_RES_LEDS 1
#define REST_RES_TOGGLE 1


#if defined (PLATFORM_HAS_BUTTON)
#include "dev/button-sensor.h"
#endif
#if defined (PLATFORM_HAS_LEDS)
#include "dev/leds.h"
#endif
#if defined (PLATFORM_HAS_BATTERY)
#include "dev/battery-sensor.h"
#endif

// embedd-vm
#include "embedvm.h"


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
     index += sprintf(message + index,"{\n \"version\" : \"V0.2\",\n");
     index += sprintf(message + index," \"name\" : \"embedd-vm demo\"\n");
     index += sprintf(message + index,"}\n");

    length = strlen(message);
    memcpy(buffer, message,length );

  REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
  REST.set_response_payload(response, buffer, length);
}
RESOURCE(res_info, "title=\"Info\";rt=\"text\"", info_get_handler, NULL, NULL, NULL);
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
RESOURCE(res_toggle, "title=\"Red LED\";rt=\"Control\"", toggle_handler, toggle_handler, leds_handler, NULL);
#endif
#endif /* PLATFORM_HAS_LEDS */

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
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%d", battery);

    REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer));
  }
  else if (accept == REST.type.APPLICATION_JSON) {
    REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'battery':%d}", battery);

    REST.set_response_payload(response, buffer, strlen((char *)buffer));
  }
  else
  {
    REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
    const char *msg = "Supporting content-types text/plain and application/json";
    REST.set_response_payload(response, msg, strlen(msg));
  }
}
RESOURCE(res_battery, "title=\"Battery status\";rt=\"battery-mV\"", battery_handler, NULL, NULL, NULL);
#endif /* PLATFORM_HAS_BATTERY */

#include "vmcode.hdr"
#define UNUSED __attribute__((unused))

uint8_t vm_mem[256] = { EMBEDVM_SECT_SRAM_DATA };
struct embedvm_s vm = { };
// Global Variable coap answear
 char gmessage[100];
 int gindex = 0;
/******************************************************************************/
#if defined (PLATFORM_HAS_EVM)
/******************************************************************************/
#if REST_RES_EVM
/*A simple actuator example, depending on the color query parameter and post variable mode, corresponding led is activated or deactivated*/
void
evm_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  size_t len = 0;
  const char *player = NULL;
  int success = 1;
  //int length = 0; /*           |<-------->| */
  int16_t val = 0;
  uint16_t old_ip=0;

  switch(REST.get_method_type(request)){
   case METHOD_GET:
     gindex=0;
     gindex += sprintf(gmessage + gindex,"Number-Quiz!\nI think at a Number between 0 and 9999 :-)\n");
     break;
   case METHOD_PUT:
     if ((len=REST.get_post_variable(request, "player", &player))) {
        PRINTF("player %.*s\n", len, player);
	gindex=0;
	val = atoi(player);
	embedvm_push(&vm, 1);
	embedvm_push(&vm, val);
	old_ip=vm.ip;
	embedvm_interrupt(&vm, EMBEDVM_SYM_play);
  	while (vm.ip != old_ip) {
	//	printf("<%X>",vm.ip);
		embedvm_exec(&vm);
  	}
	embedvm_pop(&vm);
	embedvm_pop(&vm);

     } else {
       success = 0;
     }
     break;
  default:
    success = 0;
  }
  
  unsigned int accept = -1;
  REST.get_header_accept(request, &accept);
  if(accept == -1 || accept == REST.type.TEXT_PLAIN) {
    if(success){
      //length = strlen(gmessage);
      memcpy(buffer, gmessage,gindex );
      buffer[gindex]=0;
      REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
      REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer));
    } else {
      REST.set_response_status(response, REST.status.BAD_REQUEST);
    }
  }
  else
  {
    REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
    const char *msg = "Supporting content-types text/plain";
    REST.set_response_payload(response, msg, strlen(msg));
  }
}
RESOURCE(res_evm, "title=\"Embedd-VM, POST/PUT player=0/9999\";rt=\"Embedd-VM\"", evm_handler, evm_handler, evm_handler, NULL);
#endif
#endif


int16_t mem_read(uint16_t addr, bool is16bit, void *ctx)
{
	if (addr + (is16bit ? 1 : 0) >= sizeof(vm_mem))
		return 0;
	if (is16bit)
		return (vm_mem[addr] << 8) | vm_mem[addr+1];
	return vm_mem[addr];
}

void mem_write(uint16_t addr, int16_t value, bool is16bit, void *ctx)
{
	if (addr + (is16bit ? 1 : 0) >= sizeof(vm_mem))
		return;
	if (is16bit) {
		vm_mem[addr] = value >> 8;
		vm_mem[addr+1] = value;
	} else
		vm_mem[addr] = value;
}

extern uint8_t rng_get_uint8(void);
int16_t call_user(uint8_t funcid, uint8_t argc, int16_t *argv, void *ctx)
{
	if (funcid == 0)
		return (random()%99)+1;

	if (funcid == 1 && argc >= 1) {
		if (argv[0] > 0){
                        gindex += sprintf(gmessage + gindex,"Try larger numbers.\n");
			PRINTF("Try larger numbers.\n");
		}else{
			gindex += sprintf(gmessage + gindex,"Try smaler numbers.\n");
			PRINTF("Try smaller numbers.\n");
		}
		return 0;
	}
	if (funcid == 2) {
                gindex += sprintf(gmessage + gindex,"This is correct!\n");
		PRINTF("This is correct!\n");
		return 0;
	}
	if (funcid == 3) {
                gindex += sprintf(gmessage + gindex,"\nYou currently have %d points.\n", mem_read(EMBEDVM_SYM_points, true, ctx));
		PRINTF("\n");
		PRINTF("You currently have %d points.\n", mem_read(EMBEDVM_SYM_points, true, ctx));
		return 0;
	}
	if (funcid == 4) {
		PRINTF("\n");
		PRINTF("D: %d\n", mem_read(EMBEDVM_SYM_dbg, true, ctx));
		return 0;
	}
	if (funcid == 10) {
		PRINTF("D: LED-RED off\n");
                leds_off(LEDS_RED);
		return 0;
	}
	if (funcid == 11) {
		PRINTF("D: LED-RED on\n");
                leds_on(LEDS_RED);
		return 0;
	}
	if (funcid == 12) {
		PRINTF("D: LED-RED Toggle\n");
                leds_toggle(LEDS_RED);
		return 0;
	}
	return 1;
}

void vm_setup()
{
	PRINTF("Initializing embedded-vm...\n");

	vm.ip = EMBEDVM_SYM_main;
	vm.sp = vm.sfp = sizeof(vm_mem);
	vm.mem_read = &mem_read;
	vm.mem_write = &mem_write;
	vm.call_user = &call_user;

	embedvm_push(&vm, 0);
  	while (vm.ip > 0) {
	//	printf("<%X>",vm.ip);
		embedvm_exec(&vm);
  	}
}

void 
hw_init()
{
#if defined (PLATFORM_HAS_LEDS)
  leds_off(LEDS_RED);
#endif
  vm_setup();
}

PROCESS(rest_server_example, "Erbium Example Server");
AUTOSTART_PROCESSES(&rest_server_example);

PROCESS_THREAD(rest_server_example, ev, data)
{
  PROCESS_BEGIN();

  PRINTF("Starting Embedd-VM Example Server\n");
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
  rest_activate_resource(&res_leds,"a/leds");
#endif
#if REST_RES_TOGGLE
  rest_activate_resource(&res_toggle,"a/toggle");
#endif
#endif /* PLATFORM_HAS_LEDS */
#if REST_RES_EVM
  rest_activate_resource(&res_evm,"a/evm");
#endif
#if defined (PLATFORM_HAS_BATTERY) && REST_RES_BATTERY
  SENSORS_ACTIVATE(battery_sensor);
  rest_activate_resource(&res_battery,"s/battery");
#endif

  while(1) {
    PROCESS_WAIT_EVENT();

  } /* while (1) */
  PROCESS_END();
}
