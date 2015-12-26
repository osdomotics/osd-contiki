/*
  pins_arduino.h - Pin definition functions for Arduino
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2014 Harald Pichler

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  $Id: wiring.h 249 2014-04-18 17:35:12Z pichler $
*/

/*
	This version of pins_arduino.h is for the Merkur Dev Board r1
	Harald Pichler 2014 Apr 18
*/

#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <avr/pgmspace.h>

#define NUM_DIGITAL_PINS            15
#define NUM_ANALOG_INPUTS           6
#define analogInputToDigitalPin(p)  ((p < NUM_ANALOG_INPUTS) ? (p) + NUM_DIGITAL_PINS  : -1)
#define digitalPinHasPWM(p)         ((p) == 2 ||(p) == 3 ||(p) == 4 ||(p) == 14 )

// Dev board specific defines: RF RX and TX LEDs:
#define RXLED_DDR  DDRB
#define RXLED_PORT PORTB
#define RXLED_POS  PB6

#define TXLED_DDR  DDRB
#define TXLED_PORT PORTB
#define TXLED_POS  PB7  

const static uint8_t SS   = 10;
const static uint8_t MOSI = 11;
const static uint8_t MISO = 13;
const static uint8_t SCK  = 12;

const static uint8_t SDA = 9;
const static uint8_t SCL = 8;
const static uint8_t LED = 4;
const static uint8_t LED1 = 4;
const static uint8_t LED2 = 5;

const static uint8_t A0 = 7;
const static uint8_t A1 = 6;
const static uint8_t A2 = 5;
const static uint8_t A3 = 4;
const static uint8_t A4 = 0;
const static uint8_t A5 = 1;

// A majority of the pins are NOT PCINTs, SO BE WARNED (i.e. you cannot use them as receive pins)
// Only pins available for RECEIVE (TRANSMIT can be on any pin):
// Pins: 10, 11, 12, 13, 14

#define digitalPinToPCICR(p)    ( (((p) >= 10) && ((p) <= 14)) || ? (&PCICR) : ((uint8_t *)0) )

#define digitalPinToPCICRbit(p) ( 0 ) 

#define digitalPinToPCMSK(p)    ( (((p) >= 10) && ((p) <= 14))  ? (&PCMSK0) : ((uint8_t *)0) )

#define digitalPinToPCMSKbit(p) ( ((p) == 10) ? 6 : \
                                ( ((p) == 11) ? 5 : \
                                ( ((p) == 12) ? 1 : \
                                ( ((p) == 13) ? 3 : \
                                ( ((p) == 14) ? 7 : \
                                0 ) ) ) ) )

#ifdef ARDUINO_MAIN

const uint16_t PROGMEM port_to_mode_PGM[] = {
	NOT_A_PORT,
	NOT_A_PORT,
	(uint16_t)&DDRB,
	NOT_A_PORT,
	(uint16_t)&DDRD,
	(uint16_t)&DDRE,
	(uint16_t)&DDRF,
	(uint16_t)&DDRG,
	NOT_A_PORT,
	NOT_A_PORT,
	NOT_A_PORT,
	NOT_A_PORT,
	NOT_A_PORT,
};

const uint16_t PROGMEM port_to_output_PGM[] = {
	NOT_A_PORT,
	NOT_A_PORT,
	(uint16_t)&PORTB,
	NOT_A_PORT,
	(uint16_t)&PORTD,
	(uint16_t)&PORTE,
	(uint16_t)&PORTF,
	(uint16_t)&PORTG,
	NOT_A_PORT,
	NOT_A_PORT,
	NOT_A_PORT,
	NOT_A_PORT,
	NOT_A_PORT,
};

const uint16_t PROGMEM port_to_input_PGM[] = {
	NOT_A_PIN,
	NOT_A_PIN,
	NOT_A_PIN,
	(uint16_t)&PINC,
	(uint16_t)&PIND,
	(uint16_t)&PINE,
	(uint16_t)&PINF,
	(uint16_t)&PING,
	NOT_A_PIN,
	NOT_A_PIN,
	NOT_A_PIN,
	NOT_A_PIN,
	NOT_A_PIN,
};

const uint8_t PROGMEM digital_pin_to_port_PGM[] = {
	// PORTLIST		
	// -------------------------------------------		
	PE	, // PE 1 ** 0 ** D0 / USART0_TX	
	PE	, // PE 0 ** 1 ** D1 / USART0_RX	
	PE	, // PE 3 ** 2 ** D2 / PWM
	PE	, // PE 4 ** 3 ** D3 / PWM
	PE	, // PE 5 ** 4 ** D4 / PWM / LED1 / LED 
	PE	, // PE 6 ** 5 ** D5 / LED2
	PD	, // PD 3 ** 6 ** D6 / USART1_TX
	PD	, // PD 2 ** 7 ** D7 / USART1_RX
	PD	, // PD 0 ** 8 ** D8 / I2C_SCL
	PD	, // PD 1 ** 9 ** D9 / I2C_SDA
	PB	, // PB 0 ** 10 ** D10 / SPI_SSN
	PB	, // PB 2 ** 11 ** D11 / SPI_MOSI
	PB	, // PB 1 ** 12 ** D12 / SPI_SCK
	PB	, // PB 3 ** 13 ** D13 / SPI_MISO
	PB	, // PB 4 ** 14 ** D14 / PWM 
	PF	, // PF 7 ** 15 ** A0 / D15
	PF	, // PF 6 ** 16 ** A1 / D16	
	PF	, // PF 5 ** 17 ** A2 / D17	
	PF	, // PF 4 ** 18 ** A3 / D18	
	PF	, // PF 0 ** 19 ** A4 / D19
	PF	, // PF 1 ** 20 ** A5 / D20
//	PB	, // PB 6 ** 34 ** D34 / LED1 / LED / PWM
//	PB	, // PB 7 ** 35 ** D35 / LED2 / PWM
//	PE	, // PE 2 ** 2 ** D2
//	PE	, // PE 7 ** 7 ** D7
//	PB	, // PB 5 ** 8 ** D8 / PWM
//	PG	, // PG 0 ** 16 ** D16
//	PG	, // PG 1 ** 17 ** D17
//	PG	, // PG 2 ** 18 ** D18
//	PG	, // PG 5 ** 19 ** D19 / PWM
//	PD	, // PD 4 ** 22 ** D22
//	PD	, // PD 5 ** 23 ** D23
//	PD	, // PD 6 ** 24 ** D24
//	PD	, // PD 7 ** 25 ** D25
//	PF	, // PF 2 ** 28 ** A2 / D28
//	PF	, // PF 3 ** 29 ** A3 / D29
};

const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[] = {
	// PIN IN PORT		
	// -------------------------------------------		
	_BV( 1 )	, // PE 1 ** 0 ** USART0_TX	
	_BV( 0 )	, // PE 0 ** 1 ** USART0_RX	
	_BV( 3 )	, // PE 3 ** 2 ** D3 / PWM
	_BV( 4 )	, // PE 4 ** 3 ** D4 / PWM
	_BV( 5 )	, // PE 5 ** 4 ** D5 / PWM
	_BV( 6 )	, // PE 6 ** 5 ** D6
	_BV( 3 )	, // PD 3 ** 6 ** D21 / USART1_TX
	_BV( 2 )	, // PD 2 ** 7 ** D20 / USART1_RX
	_BV( 0 )	, // PD 0 ** 8 ** D15 / I2C_SCL
	_BV( 1 )	, // PD 1 ** 9 ** D14 / I2C_SDA
	_BV( 0 )	, // PB 0 ** 10 ** D10 / SPI_SSN
	_BV( 2 )	, // PB 2 ** 11 ** D11 / SPI_MOSI
	_BV( 1 )	, // PB 1 ** 12 ** D13 / SPI_SCK
	_BV( 3 )	, // PB 3 ** 13 ** D12 / SPI_MISO
	_BV( 4 )	, // PB 4 ** 14 ** D9 / PWM
	_BV( 7 )	, // PF 7 ** 15 ** A0 / D33
	_BV( 6 )	, // PF 6 ** 16 ** A1 / D32
	_BV( 5 )	, // PF 5 ** 17 ** A2 / D31
	_BV( 4 )	, // PF 4 ** 18 ** A3 / D30			
	_BV( 0 )	, // PF 0 ** 19 ** A4 / D26
	_BV( 1 )	, // PF 1 ** 20 ** A5 / D27
//	_BV( 2 )	, // PE 2 ** 2 ** D2
//	_BV( 7 )	, // PE 7 ** 7 ** D7
//	_BV( 5 )	, // PB 5 ** 8 ** D8 / PWM
//	_BV( 0 )	, // PG 0 ** 16 ** D16
//	_BV( 1 )	, // PG 1 ** 17 ** D17	
//	_BV( 2 )	, // PG 2 ** 18 ** D18	
//	_BV( 5 )	, // PG 5 ** 19 ** D19 / PWM
//	_BV( 4 )	, // PD 4 ** 22 ** D22
//	_BV( 5 )  	, // PD 5 ** 23 ** D23
//	_BV( 6 )  	, // PD 6 ** 24 ** D24
//	_BV( 7 )	, // PD 7 ** 25 ** D25
//	_BV( 2 )	, // PF 2 ** 28 ** A2 / D28
//	_BV( 3 )	, // PF 3 ** 29 ** A3 / D29
//	_BV( 6 )	, // PB 6 ** 34 ** D34 / LED1 / LED / PWM
//	_BV( 7 )	, // PB 7 ** 35 ** D35 / LED2 / PWM
};

#endif

#endif
