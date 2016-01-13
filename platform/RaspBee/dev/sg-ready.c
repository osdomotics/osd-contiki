/*
 *  Copyright (c) 2015 Bernhard Trinnes
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/**
 * \file
 *
 * \brief
 *      Smart Grid Ready Module - guhRF
 *
 * \author
 *      Bernhard Trinnes bernhard.trinnes@guh.guru
 *
 */

#include "sg-ready.h"
#define LATCH_TIME 3000 // time in micro seconds

#define RELAY1_ON     (1<<DDRB4)
#define RELAY1_OFF    (1<<DDRB5)
#define RELAY2_ON     (1<<DDRB6)
#define RELAY2_OFF    (1<<DDRB7)
#define RELAY1_FB     (1<<DDRD5)
#define RELAY2_FB     (1<<DDRD7)

/**
 * \addtogroup relay
 * \{
*/
/*---------------------------------------------------------------------------*/
/**
 * \brief  init RELAY PINS - direction & pull-ups
*/
void
releay_init()
{
   /*2 latching dual coal relay
   RELAY 1 Coil 1   PB4 - ON
           Coil 2   PB5 - OFF
           Feedback PD5
   RELAY 2 Coil 1   PB6 - ON
           Coil 2   PB7 - OFF
           Feedback PD7

   */ 
   DDRB |= (RELAY1_ON | RELAY1_OFF | RELAY2_ON | RELAY2_OFF);
   PORTB &= ~(RELAY1_ON | RELAY1_OFF | RELAY2_ON | RELAY2_OFF);

   DDRD &= ~(RELAY1_FB | RELAY2_FB);
   PORTD |= (RELAY1_FB | RELAY2_FB);
}
/**
 * \brief 
*/

void
set_state(uint8_t state)
{
   /* State 1 - Relays 1:0
      State 2 - Relays 0:0  
      State 3 - Relays 0:1
      State 4 - Relays 1:1
   */
   uint8_t i = 0;
   uint8_t current_state;
   current_state = get_state();
   while ( i<=3 || (current_state != state)){
      i++;
      switch_difference(current_state, state);
      current_state = get_state();  
   } 
}
/**
 * \brief 
*/


uint8_t
get_state()
{
   uint8_t state;
   /* Pull up -> Pin high = Relay Open */
   if ((~PIND & RELAY1_FB) && (~PIND & RELAY2_FB)){ 
      state = 4;
   }else if  (~PIND & RELAY1_FB){ 
      state = 1; 
   } else if (~PIND & RELAY2_FB){
      state = 3;
   } else {
      state = 2;
   }
   return state;
}
/**
 * \brief 
*/

void 
switch_difference(uint8_t old, uint8_t new)
{
   switch(old) {
      case 1: 
          switch(new) {
              case 2: 
                 PORTB |=   RELAY1_OFF;
                 PORTB &= ~(RELAY1_ON | RELAY2_ON | RELAY2_OFF);
                 clock_delay_usec (LATCH_TIME);
                 PORTB &= ~(RELAY1_OFF); 
              break;
              case 3: 
                 PORTB |=  (RELAY1_OFF | RELAY2_ON );
                 PORTB &= ~(RELAY1_ON  | RELAY2_OFF);
                 clock_delay_usec (LATCH_TIME);
                 PORTB &= ~(RELAY1_OFF | RELAY2_ON);   
              break;
              case 4: 
                 PORTB |=   RELAY2_ON;
                 PORTB &= ~(RELAY1_ON | RELAY1_OFF | RELAY2_OFF);
                 clock_delay_usec (LATCH_TIME);
                 PORTB &= ~(RELAY2_ON); 
              break;
          }
      break;
      case 2: 
          switch(new) {
              case 1: 
                 PORTB |=   RELAY1_ON;
                 PORTB &= ~(RELAY1_OFF | RELAY2_ON | RELAY2_OFF);
                 clock_delay_usec (LATCH_TIME);
                 PORTB &= ~(RELAY1_ON); 
              break;
              case 3: 
                 PORTB |=  (RELAY2_ON);
                 PORTB &= ~(RELAY1_ON | RELAY1_OFF | RELAY2_OFF);
                 clock_delay_usec (LATCH_TIME);
                 PORTB &= ~(RELAY2_ON); 
              break;
              case 4: 
                 PORTB |=  (RELAY1_ON  | RELAY2_ON);
                 PORTB &= ~(RELAY1_OFF | RELAY2_OFF);
                 clock_delay_usec (LATCH_TIME);
                 PORTB &= ~(RELAY1_ON  | RELAY2_ON); 
              break;
          }
  
      break;
      case 3:
          switch(new) {
              case 1: 
                 PORTB |=  (RELAY1_ON  | RELAY2_OFF);
                 PORTB &= ~(RELAY1_OFF | RELAY2_ON);
                 clock_delay_usec (LATCH_TIME);
                 PORTB &= ~(RELAY1_ON  | RELAY2_OFF); 
              break;
              case 2: 
                 PORTB |=   RELAY2_OFF;
                 PORTB &= ~(RELAY1_ON  | RELAY1_OFF | RELAY2_ON);
                 clock_delay_usec (LATCH_TIME);
                 PORTB &= ~(RELAY2_OFF);   
              break;
              case 4: 
                 PORTB |=   RELAY1_ON;
                 PORTB &= ~(RELAY1_OFF | RELAY2_ON | RELAY2_OFF);
                 clock_delay_usec (LATCH_TIME);
                 PORTB &= ~(RELAY1_ON); 
              break;
          }
      break;
      case 4: 
          switch(new) {
              case 1: 
                 PORTB |=   RELAY2_OFF;
                 PORTB &= ~(RELAY1_ON | RELAY1_OFF | RELAY2_ON);
                 clock_delay_usec (LATCH_TIME);
                 PORTB &= ~(RELAY2_OFF); 
              break;
              case 2: 
                 PORTB |=  (RELAY1_OFF | RELAY2_OFF);
                 PORTB &= ~(RELAY1_ON  | RELAY2_ON );
                 clock_delay_usec (LATCH_TIME);
                 PORTB &= ~(RELAY1_OFF | RELAY2_OFF);  
              break;
              case 3: 
                 PORTB |=   RELAY1_OFF;
                 PORTB &= ~(RELAY1_ON | RELAY2_ON | RELAY2_OFF);
                 clock_delay_usec (LATCH_TIME);
                 PORTB &= ~(RELAY1_OFF); 
              break;
          }
      break;
   }
}

