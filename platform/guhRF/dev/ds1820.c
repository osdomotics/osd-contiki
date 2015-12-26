/*
 * Copyright (c) 2005, Swedish Institute of Computer Science
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
 * This file is part of the Contiki operating system.
 *
 * @(#)$Id: ds1820.c,v 1.5 2010/08/25 18:35:52 nifi Exp $
 */
/*
 * Device driver for the Dallas Semiconductor DS1820 chip. Heavily
 * based on the application note 126 "1-Wire Communications Through
 * Software".
 *
 * http://www.maxim-ic.com/appnotes.cfm/appnote_number/126
 */

/*
 * For now we stuff in Moteiv Corporation's unique OUI.
 * From http://www.ethereal.com/distribution/manuf.txt:
 * 00:12:75    Moteiv    # Moteiv Corporation
 *
 * The EUI-64 is a concatenation of the 24-bit OUI value assigned by
 * the IEEE Registration Authority and a 40-bit extension identifier
 * assigned by the organization with that OUI assignment.
 */

#include <avr/io.h>
#include <string.h>
#include "contiki.h"
#include "ds1820.h"


unsigned char ds1820_id[8];
unsigned char ds1820_ok[8];

/* 1-wire is at PortE.3 */
#define SERIAL_ID_PIN_READ PINE
//#define SERIAL_ID_PIN_MASK _BV(PE3)
#define SERIAL_ID_PIN_MASK _BV(PE4)
#define SERIAL_ID_PxOUT PORTE
#define SERIAL_ID_PxDIR DDRE

#define SET_PIN_INPUT() (SERIAL_ID_PxDIR &= ~SERIAL_ID_PIN_MASK)
#define SET_PIN_OUTPUT() (SERIAL_ID_PxDIR |= SERIAL_ID_PIN_MASK)

#define OUTP_0() (SERIAL_ID_PxOUT &= ~SERIAL_ID_PIN_MASK)
#define OUTP_1() (SERIAL_ID_PxOUT |= SERIAL_ID_PIN_MASK) 

#define PIN_INIT() do{  \
                     SET_PIN_INPUT();    \
                     OUTP_0();           \
                   } while(0)


/* Drive the one wire interface low */
#define OW_DRIVE() do {                    \
                     SET_PIN_OUTPUT();     \
                     OUTP_0();             \
                   } while (0)

/* Release the one wire by turning on the internal pull-up. */
#define OW_RELEASE() do {                  \
                       SET_PIN_INPUT();    \
                       OUTP_1();           \
                     } while (0)

/* Read one bit. */
#define INP()  (SERIAL_ID_PIN_READ & SERIAL_ID_PIN_MASK)


/*
 * Delay times in us.
 */
#define tA 6          /* min-5, recommended-6, max-15 */              
#define tB 64         /* min-59, recommended-64, max-N/A */
#define tC 60         /* min-60, recommended-60, max-120 */			
#define tD 10         /* min-5.3, recommended-10, max-N/A */
#define tE 9          /* min-0.3, recommended-9, max-9.3 */
#define tF 55         /* min-50, recommended-55, max-N/A */
#define tG 0          /* min-0, recommended-0, max-0 */
#define tH 480        /* min-480, recommended-480, max-640 */
#define tI 70         /* min-60.3, recommended-70, max-75.3 */
#define tJ 410        /* min-410, recommended-410, max-N/A */
/*---------------------------------------------------------------------------*/
#define udelay(u) clock_delay_usec(u)
/*---------------------------------------------------------------------------*/

static int
owreset(void)
{
  int result;

  OW_DRIVE();
  udelay(tH);     /* 480 < tH < 640 */
  OW_RELEASE();    /* Releases the bus */
  udelay(tI);
  result = INP();
  udelay(tJ);
  return result;
}
/*---------------------------------------------------------------------------*/
static void
owwriteb(unsigned byte)
{
  int i = 7;

  do {
    if(byte & 0x01) {
      OW_DRIVE();
      udelay(tA);
      OW_RELEASE();    /* Releases the bus */
      udelay(tB);
    } else {
      OW_DRIVE();
      udelay(tC);
      OW_RELEASE();    /* Releases the bus */
      udelay(tD);
    }
    if(i == 0) {
      return;
    }
    i--;
    byte >>= 1;
  } while(1);
}
/*---------------------------------------------------------------------------*/
static unsigned
owreadb(void)
{
  unsigned result = 0;
  int i = 7;

  do {
    OW_DRIVE();
    udelay(tA);
    OW_RELEASE();	/* Releases the bus */
    udelay(tE);
    if (INP()){
      result |= 0x80;	/* LSbit first */
    }
    udelay(tF);
    if(i == 0) {
      return result;
    }
    i--;
    result >>= 1;
  } while(1);
}
/*---------------------------------------------------------------------------*/
/* Polynomial ^8 + ^5 + ^4 + 1 */
static unsigned
crc8_add(unsigned acc, unsigned byte)
{
  int i;
  acc ^= byte;
  for(i = 0; i < 8; i++) {
    if(acc & 1) {
      acc = (acc >> 1) ^ 0x8c;
    } else {
      acc >>= 1;
    }
  }
  return acc;
}
/*---------------------------------------------------------------------------*/
int
ds1820_init()
{
  int i;
  unsigned family, crc, acc;

  PIN_INIT();

  if(owreset() == 0) {	/* Something pulled down 1-wire. */

    owwriteb(0x33);		/* Read ROM command. */
    family = owreadb();
    /* We receive 6 bytes in the reverse order, LSbyte first. */
    for(i = 7; i >= 2; i--) {
      ds1820_id[i] = owreadb();
    }
    crc = owreadb();

    /* Verify family DS1820 and that CRC match. */
    if(family != 0x10) {
      goto fail;
    }
    acc = crc8_add(0x0, family);
    for(i = 7; i >= 2; i--) {
      acc = crc8_add(acc, ds1820_id[i]);
    }
    if(acc == crc) {
      ds1820_id[0] = 0x00;
      ds1820_id[1] = 0x00;
      ds1820_id[2] = 0x00;
      return 1;			/* Success! */
    }
  } else {
  }


 fail:
  memset(ds1820_id, 0x0, sizeof(ds1820_id));
  return 0;			/* Fail! */
}
/*---------------------------------------------------------------------------*/
int
ds1820_temp()
{
    ds1820_convert();
// wait max 750ms pin lo
    clock_wait(CLOCK_SECOND);
    ds1820_read();
    return 1;
}

int
ds1820_convert()
{
  unsigned i;

  PIN_INIT();
  for(i=0;i<3;i++){
  if(owreset() == 0) {	/* Something pulled down 1-wire. */
    owwriteb(0xCC);		/* Skip ROM command. */
    owwriteb(0x44);		/* Convert T command. */
    OW_RELEASE();    /* Releases the bus */
    return 1;
  } else {
  }
  }
  return 0;			/* Fail! */
}
/*---------------------------------------------------------------------------*/
int
ds1820_read()
{
  int i;
  unsigned crc, acc;

  if(owreset() == 0) {	/* Something pulled down 1-wire. */
    owwriteb(0xCC);		/* Skip ROM command. */
    owwriteb(0xBE);		/* Read Scratchpad command. */
    /* We receive 8 bytes in the reverse order, LSbyte first. */
    for(i = 0; i < 8; i++) {
      ds1820_id[i] = owreadb();
    }
      crc = owreadb();

    acc=0;
    for(i = 0; i < 8; i++) {
      acc = crc8_add(acc, ds1820_id[i]);
    }
    if(acc == crc) {
      // store temp
      for(i = 0; i < 8; i++) {
         ds1820_ok[i]=ds1820_id[i];
      }
      return 1;			/* Success! */
    } else {
        return 0;			/* Fail! */
    }
  } else {
    return 0;			/* Fail! */
  }
  return 1;			/* Fail! */
}
