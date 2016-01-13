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
* @(#)$Id: servo-sensor.c,v 1.0 2013/02/20 19:34:06 nifi Exp $
*/

/**
* \file
* Servo sensor header file for Atmega128rfa1.
* \author
* Harald Pichler <harald@the-develop.net>
*/

#include "contiki.h"
#include "dev/servo.h"
#include "dev/servo-sensor.h"

#define PRINTF(...) printf(__VA_ARGS__)

const struct sensors_sensor servo_sensor;
static int status(int type);
static int enabled = 0;

/*---------------------------------------------------------------------------*/
static int
value(int type)
{
  switch(type) {
  case SERVO_SENSOR_A:
    return servo_get(0);;

    /* Total Solar Radiation. */
  case SERVO_SENSOR_B:
    return servo_get(1);
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
static int
configure(int type, int c)
{
  switch(type) {
  case SENSORS_ACTIVE:
    if(c) {
      if(!status(SENSORS_ACTIVE)) {
	servo_init();
        enabled = 1;
      }
    } else {
      servo_off();
      enabled = 1;
    }
    break;
  case SERVO_SENSOR_A:
    servo_set(0,c);
    break;
  case SERVO_SENSOR_B:
    servo_set(1,c);
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
SENSORS_SENSOR(servo_sensor, SERVO_SENSOR, value, configure, status); 
