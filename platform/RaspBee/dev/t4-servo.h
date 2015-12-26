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
**    t4-servo
**
** Purpose
**    Implements software pwm on any portpins via timer 4
**
**
** Revision Dates
**    31-Mar-2013 (MPR) Creation
**    12-Mar-2014 (MPR) Factored to support configurable amount of pwm's
      ««revision-date»»···
**--
*/

#ifndef __T4_SERVO_H__
#define __T4_SERVO_H__

typedef struct struct_servo_channel
{
  unsigned char port;
  unsigned char ddr;
  unsigned char pin;
  unsigned char duty;
} servo_channel_type;

void t4_servo_init(void);
void t4_servo_off(void);

int t4_servo_get(unsigned int channel);
int t4_servo_set(unsigned int channel, unsigned char duty);

#endif /* __T4_SERVO_H__ */
