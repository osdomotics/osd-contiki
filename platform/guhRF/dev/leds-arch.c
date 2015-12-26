/*
* Copyright (c) 2005, Swedish Institute of Computer Science.
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
* This file is part of the Contiki operating system.
*
* $Id: leds-arch.c,v 1.1 2006/06/17 22:41:31 adamdunkels Exp $
*/

/**
* \file
* Leds arch. for STK600-Atmega128rfa1.
* \author
* Paulo Louro <paulolouro@binarylabs.dk>
*/


 // WARNING : Blink function is disabled on the core leds.c file
 
#include "contiki.h"
#include "dev/leds.h"
#include "leds-arch.h"

/*---------------------------------------------------------------------------*/
void
leds_arch_init(void)
{
DDRE|=(1<<DDRE5);	// Set pins as output since leds are mounted on the VDD bus
LEDS_OFF();
}
/*---------------------------------------------------------------------------*/
unsigned char
leds_arch_get(void)
{
  return ((LED_OUT & BIT_RED) ? 0 : LEDS_RED)
    | ((LED_OUT & BIT_GREEN) ? 0 : LEDS_GREEN)
    | ((LED_OUT & BIT_YELLOW) ? 0 : LEDS_YELLOW);

}
/*---------------------------------------------------------------------------*/
void
leds_arch_set(unsigned char leds)
{
  if(leds & LEDS_GREEN) {
    LED_OUT &= ~BIT_GREEN;
  } else {
    LED_OUT |= BIT_GREEN;
  }
  if(leds & LEDS_YELLOW) {
    LED_OUT &= ~BIT_YELLOW;
  } else {
    LED_OUT |= BIT_YELLOW;
  }
  if(leds & LEDS_RED) {
    LED_OUT &= ~BIT_RED;
  } else {
    LED_OUT |= BIT_RED;
  }
}
/*---------------------------------------------------------------------------*/
