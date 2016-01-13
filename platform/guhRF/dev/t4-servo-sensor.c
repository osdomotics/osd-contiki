/*
** Copyright (C) 2013-2014 Marcus Priesch, All rights reserved
** In Prandnern 31, A--2122 Riedenthal, Austria. office@priesch.co.at
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the Institute nor the names of its contributors
**    may be used to endorse or promote products derived from this software
**    without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
** OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
** HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
** OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
** SUCH DAMAGE.
**
**++
** Name
**    t4-servo-sensor
**
** Purpose
**    Implements a sensor around t4servo.c
**
**
** Revision Dates
**    31-Mar-2013 (MPR) Creation
**    12-Mar-2014 (MPR) Factored to support configurable amount of pwm's
**    ««revision-date»»···
**--
*/

#include "contiki.h"
#include "dev/t4-servo.h"
#include "dev/t4-servo-sensor.h"

const struct sensors_sensor t4_servo_sensor;

static int status(int type);
static int enabled = 0;

static int value (int channel)
  {
    if (channel >= SERVO_COUNT)
      return -1;
    else if (channel < 0)
      return -2;
    else
      return t4_servo_get (channel);
  }

static int configure (int type, int c) // type, c: SENSORS_ACTIVE, 1 -> act.
                                       // type, c: SENSORS_ACTIVE, 0 -> deact.
  {
    switch (type)
      {
        case SENSORS_ACTIVE :
          if (c == 0)
            {
              t4_servo_off ();
            }
          else if (c == 1)
            {
              t4_servo_init ();
            }
          break;

        default :
          if (type >= SERVO_COUNT)
            return -1;
          else
            t4_servo_set (type, c);
          break;

      }
    return 0;
  }

static int status(int type)
  {
    switch (type)
      {
        case SENSORS_ACTIVE:
        case SENSORS_READY:
          return enabled;
      }
    return 0;
  }

SENSORS_SENSOR(t4_servo_sensor, T4_SENSOR_NAME, value, configure, status);
