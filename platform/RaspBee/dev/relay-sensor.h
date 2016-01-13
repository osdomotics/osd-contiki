/*
* Copyright (c), Harald Pichler.
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
* @(#)$Id: relay-sensor.h,v 1.0 2013/11/22 19:34:06 nifi Exp $
*/

/**
* \file
* Relay sensor header file for Atmega128rfa1.
* \author
* Harald Pichler <harald@the-develop.net>
*/

#ifndef __RELAY_SENSOR_H__
#define __RELAY_SENSOR_H__

#include "lib/sensors.h"

extern const struct sensors_sensor relay_sensor;

#define RELAY_SENSOR "RELAY"
#define RELAY_SENSOR_1          0
#define RELAY_SENSOR_2          1
#define RELAY_SENSOR_3          2
#define RELAY_SENSOR_4          3
#define RELAY_SENSOR_5          4
#define RELAY_SENSOR_6          5
#define RELAY_SENSOR_7          6
#define RELAY_SENSOR_8          7

/* default pins Arduino-Merkurboard */
#define RELAY_PIN_1          10
#define RELAY_PIN_2          11
#define RELAY_PIN_3          12
#define RELAY_PIN_4          13
#define RELAY_PIN_5          15
#define RELAY_PIN_6          16
#define RELAY_PIN_7          17
#define RELAY_PIN_8          18

#endif /* __RELAY_SENSOR_H__ */ 
