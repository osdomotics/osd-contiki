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

#include <avr/io.h>
#include "contiki.h"
#include "dht11.h"
#include "led.h" // debug

#define udelay(u) clock_delay_usec(u)
#define mdelay(u) clock_delay_msec(u)

// todo: set DHT22 or DHT11 in project file
// define for DHT11 else for DHT22, RHT03 
//   #define DHT11	1

uint8_t DHT_Read_Data(uint16_t *temperature, uint16_t *humidity){

    //data[5] is 8byte table where data come from DHT are stored
    //laststate holds laststate value
    //counter is used to count microSeconds
    uint8_t data[5], laststate = 0, counter = 0, j = 0, i = 0;
    
    //Clear array
    data[0] = data[1] = data[2] = data[3] = data[4] = 0;

    uint8_t volatile sreg;
    sreg = SREG;    /* Save status register before disabling interrupts. */
    cli();          /* Disable interrupts. */

    //Set pin Output
    //Pin High
    DHT_DRIVE();
    mdelay(100);                     //Wait for 100mS
    
    //Send Request Signal
    //Pin Low
    OUTP_0();                      //20ms Low 
    mdelay(20);
    //Pin High
    OUTP_1();
    udelay(40);                      //40us High
    
    //Set pin Input to read Bus
    //Set pin Input
    DHT_RELEASE();
    laststate=DHT_INP();             //Read Pin value
    
    //Repeat for each Transistions
    for (i=0; i<MAXTIMINGS; i++) {
        //While state is the same count microseconds
   //led1_on();
   //led1_off();
 
       while (laststate==DHT_INP()) {
            udelay(1);
            counter++;
            if (counter>254) break;
        }

        if (counter>254) break;
        
        //laststate==_BV(DHT_PIN) checks if laststate was High
        //ignore the first 2 transitions which are the DHT Response
        //if (laststate==_BV(DHT_PIN) && (i > 2)) {
        if ((i&0x01) && (i > 2)) {
            //Save bits in segments of bytes
            //Shift data[] value 1 position left
            //Example. 01010100 if we shift it left one time it will be
            //10101000
 
            data[j/8]<<=1;
            if (counter >= 15) {    //If it was high for more than 40uS
   //led1_on();
                data[j/8]|=1;       //it means it is bit '1' so make a logic
   //led1_off();
            }                       //OR with the value (save it)
            j++;                    //making an OR by 1 to this value 10101000
        }                           //we will have the resault 10101001
                                    //1 in 8-bit binary is 00000001
        //j/8 changes table record every 8 bits which means a byte has been saved
        //so change to next record. 0/8=0 1/8=0 ... 7/8=0 8/8=1 ... 15/8=1 16/8=2
        laststate=DHT_INP();     //save current state
        counter=0;                  //reset counter
        
    }
    SREG = sreg;    /* Enable interrupts. */
    //printf("HUM %d %d %d %d %d %d",data[0],data[1],data[2],data[3],data[4],(uint8_t)(data[0] + data[1] + data[2] + data[3]) );
    //Check if data received are correct by checking the CheckSum
    if ((uint8_t)(data[0] + data[1] + data[2] + data[3]) == data[4]) {
#ifdef DHT11
        *humidity = data[0]*100;
        *temperature = data[2]*100;
#else
        *humidity = ((uint16_t)data[0]<<8 | data[1])*10;
        *temperature = ((uint16_t)data[2]<<8 | data[3])*10;
#endif
        return 0;
    }else{
        *humidity = 2;
        *temperature = 2;
//        uart_puts("\r\nCheck Sum Error");
    }
 
     return 0xff;  // Check Sum Error
}
