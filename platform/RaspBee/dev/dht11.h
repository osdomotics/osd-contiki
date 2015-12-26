/*
 DHT-11 Library
 (c) Created by Charalampos Andrianakis on 18/12/11.
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

/* DHT 1-wire is at PortE.6 */
#define DHT_PIN_READ PINE
#define DHT_PIN_MASK _BV(PE6)
#define DHT_PxOUT PORTE
#define DHT_PxDIR DDRE

#define SET_PIN_INPUT() (DHT_PxDIR &= ~DHT_PIN_MASK)
#define SET_PIN_OUTPUT() (DHT_PxDIR |= DHT_PIN_MASK)

#define OUTP_0() (DHT_PxOUT &= ~DHT_PIN_MASK)
#define OUTP_1() (DHT_PxOUT |= DHT_PIN_MASK) 

#define PIN_INIT() do{  \
                     SET_PIN_INPUT();    \
                     OUTP_0();           \
                   } while(0)


/* Drive the one wire interface hight */
#define DHT_DRIVE() do {                    \
                     SET_PIN_OUTPUT();     \
                     OUTP_1();             \
                   } while (0)

/* Release the one wire by turning on the internal pull-up. */
#define DHT_RELEASE() do {                  \
                       SET_PIN_INPUT();    \
                       OUTP_1();           \
                     } while (0)

/* Read one bit. */
#define DHT_INP()  (DHT_PIN_READ & DHT_PIN_MASK)

//The packet size is 40bit but each bit consists of low and high state
//so 40 x 2 = 80 transitions. Also we have 2 transistions DHT response
//and 2 transitions which indicates End Of Frame. In total 84
#define MAXTIMINGS  84

//This is the main function which requests and reads the packet
uint8_t DHT_Read_Data(uint16_t *temperature, uint16_t *humidity);
