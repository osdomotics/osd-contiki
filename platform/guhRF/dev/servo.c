/*
 * Copyright (c) 2007, Swedish Institute of Computer Science
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
 */

/*
 * Device driver for the Sensirion SHT1x/SHT7x family of humidity and
 * temperature sensors.
 */

#include "contiki.h"
#include <stdio.h>
#include <dev/servo.h>

#define DEBUG 0

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*
 * servo device
 */

unsigned int servoa=SERVO_INIT;
unsigned int servob=SERVO_INIT;

void
servo_init(void)
{
  // Port E initialization
  // Set Pin 3 and 4 to output mode for OCR1A and OCR2A
  DDRE |= 1<<3 | 1<<4;

  // Timer/Counter 3 initialization
  // Clock source: System Clock
  // Clock value: 2000.000 kHz
  // Mode: Ph. & fr. cor. PWM top=ICR1
  // OC3A output: Connected
  // OC3B output: Connected
  // OC3C output: Connected
  // Noise Canceler: Off
  // Input Capture on Falling Edge
  // Timer3 Overflow Interrupt: Off
  // Input Capture Interrupt: Off
  // Compare A Match Interrupt: Off
  // Compare B Match Interrupt: Off
  // Compare C Match Interrupt: Off

  /* TCCR3A = [COM3A1|COM3A0|COM3B1|COM3B0||FOC3A|FOC3B|WGM31|WGM30] */
  /*                1      0      1      0      1     0     0     0  */
  TCCR3A=0xA8;
  /* TCCR3B = [ ICNC3| ICES3|     -| WGM33||WGM32| CS32| CS31| CS30] */
  /*                0      0      0      1      0     0     1     0  */
  TCCR3B=0x12;
  TCNT3H=0x00;
  TCNT3L=0x00;
  // ICR3 has a computed value of 20,000 - see the chip manual for how this
  // value was derived.
  // 20000 == 0x4e20 so that's what goes into the high and low byte of the ICR3 register
  // alternatively, Codevision would let you just do ICR3 = 20000;
  ICR3H=0x4E;
  ICR3L=0x20;

  // OCR3A will govern the steering servo, OCR3B will govern throttle
  OCR3A = servoa;  // set it to an initial position somewhere in the middle of the 1 to 2ms range

  // OCR3A will govern the steering servo, OCR3B will govern throttle
  OCR3B = servob;  // set it to an initial position somewhere in the middle of the 1 to 2ms range
  // start with motor off - no duty cycle at all
  OCR3CH=0x00;
  OCR3CL=0x00;
}
/*---------------------------------------------------------------------------*/
/*
 * Power of device.
 */
void
servo_off(void)
{

}
/*---------------------------------------------------------------------------*/
/*
 * get servo position
 */
unsigned int
servo_get(unsigned int i)
{
  if(i==0)
    return servoa;
  if(i==1)
    return servob;
  return 0;
}
/*---------------------------------------------------------------------------*/
/*
 * Set servo position
 */
unsigned int
servo_set(unsigned i,unsigned int j)
{
  if(j > SERVO_MAX)
    j=SERVO_MAX;
  if(j < SERVO_MIN)
    j=SERVO_MIN;

  if(i==0)
  {
    servoa=j;
    OCR3A = servoa;
    return 1;
  }
  if(i==1)
  {
    servob=j;
    OCR3B = servob;
    return 1;
  }

  return 0;
}
