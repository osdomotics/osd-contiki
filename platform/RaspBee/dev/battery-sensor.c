/*
* Copyright (c) 2012, BinaryLabs.
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
* @(#)$Id: battery-sensor.h,v 1.1 2012/08/25 19:34:06 nifi Exp $
*/

/**
* \file
* Battery sensor file for Atmega128rfa1.
* \author
* Paulo Louro <paulolouro@binarylabs.dk>
* Harald Pichler <harald@the-develop.net>
*/

/**
*The atmel rf23x radios have a low voltage detector that can be configured in units of 75 millivolts. Here is example *code for the ATmega128rfa1, where the BATMON register is in extended io space [dak664]
*/

#include "dev/battery-sensor.h"
#include "dev/batmon.h"

const struct sensors_sensor battery_sensor;
/*---------------------------------------------------------------------------*/

/**
* \return Voltage on battery measurement with BATMON register.
*/
static int
value(int type)
{

  uint16_t h;
/*
  uint8_t p1;
  BATMON = 16; //give BATMON time to stabilize at highest range and lowest voltage

// Bandgap can't be measured against supply voltage in this chip.
// Use BATMON register instead
  for ( p1=16; p1<31; p1++) {
    BATMON = p1;
    clock_delay_usec(100); // delay needed !!
    if ((BATMON&(1<<BATMON_OK))==0) break;
  }
  h=2550-75*16-75+75*p1; //-75 to take the floor of the 75 mv transition window
*/
  batmon_get_voltage(&h);
  return h;
}
/*---------------------------------------------------------------------------*/
static int
configure(int type, int c)
{
  // No configuration needed. readADC() handles all the config needed.
  batmon_init();
  return type == SENSORS_ACTIVE;
}
/*---------------------------------------------------------------------------*/
static int
status(int type)
{
  // analog sensors are always ready
  return 1;
}
/*---------------------------------------------------------------------------*/
SENSORS_SENSOR(battery_sensor, BATTERY_SENSOR, value, configure, status);
