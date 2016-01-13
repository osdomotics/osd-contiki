/*
* Copyright (c) , Harald Pichler.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
* 3. Neither the name of the Institute nor the names of its contributors
* may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
* SUCH DAMAGE.
*
* @(#)$Id: relay-sensor.c,v 1.0 2013/11/22 19:34:06 nifi Exp $
*/

/**
* \file
* relay sensor header file for Atmega128rfa1.
* \author
* Harald Pichler <harald@the-develop.net>
*/

#include "contiki.h"
#include "dev/relay.h"
#include "dev/relay-sensor.h"

#define PRINTF(...) printf(__VA_ARGS__)

const struct sensors_sensor relay_sensor;
static int status(int type);
static int enabled = 0;
static int relay[8]={0,0,0,0,0,0,0,0};
static int relaypin[8]={RELAY_PIN_1,RELAY_PIN_2,RELAY_PIN_3,RELAY_PIN_4,RELAY_PIN_5,RELAY_PIN_6,RELAY_PIN_7,RELAY_PIN_8};
/*---------------------------------------------------------------------------*/
static int
value(int type)
{
    return relay[type];
}
/*---------------------------------------------------------------------------*/
static int
configure(int type, int c)
{
  switch(type) {
  case SENSORS_ACTIVE:
    if(c) {
      if(!status(SENSORS_ACTIVE)) {
		relay_init(relaypin[RELAY_SENSOR_1]);
		relay_init(relaypin[RELAY_SENSOR_2]);
		relay_init(relaypin[RELAY_SENSOR_3]);
		relay_init(relaypin[RELAY_SENSOR_4]);
		relay_init(relaypin[RELAY_SENSOR_5]);
		relay_init(relaypin[RELAY_SENSOR_6]);
		relay_init(relaypin[RELAY_SENSOR_7]);
		relay_init(relaypin[RELAY_SENSOR_8]);
        enabled = 1;
      }
    } else {
      enabled = 1;
    }
    break;
  case RELAY_SENSOR_1:
  case RELAY_SENSOR_2:
  case RELAY_SENSOR_3:
  case RELAY_SENSOR_4:
  case RELAY_SENSOR_5:
  case RELAY_SENSOR_6:
  case RELAY_SENSOR_7:
  case RELAY_SENSOR_8:
    if(c==0){
	relay_off(relaypin[type]);
	relay[type]=0;
    }else{
	relay_on(relaypin[type]);
	relay[type]=1;
    };
    break;
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
static int
status(int type)
{
  switch(type) {
  case SENSORS_ACTIVE:
  case SENSORS_READY:
    return enabled;
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
SENSORS_SENSOR(relay_sensor, RELAY_SENSOR, value, configure, status); 
