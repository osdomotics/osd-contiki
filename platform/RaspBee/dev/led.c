/*
 *  Copyright (c) 2012  Bernhard Trinnes
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
 *      This file provides RaspBee LED support.
 *
 * \author
 *      Bernhard Trinnes bernhard.trinnes@guh.guru
 *
 */

#include "led.h"

/**
 * \addtogroup relay
 * \{
*/
/*---------------------------------------------------------------------------*/

/**
 * \brief Turns  LED1 on.
*/
void
led1_on(void)
{
    PORTD &= ~(1<<PIND7);
    DDRD  |=  (1<<PIND7);
}

/*---------------------------------------------------------------------------*/

/**
 *  \brief Turns LED1 off.
*/
void
led1_off(void)
{
    PORTD |= (1<<PIND7);
    DDRD  |= (1<<PIND7);
}
/*---------------------------------------------------------------------------*/

void
led2_on(void)
{
    PORTG &= ~(1<<PING2);
    DDRG  |=  (1<<PING2);
}

/*---------------------------------------------------------------------------*/

/**
 *  \brief Turns LED2 off.
*/
void
led2_off(void)
{
    PORTG |= (1<<PING2);
    DDRG  |= (1<<PING2);
}
/*---------------------------------------------------------------------------*/
