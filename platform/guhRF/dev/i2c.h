/*
 * Copyright (c) 2014, Ingo Gulyas Intembsys
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
 */

 /**
 * \file
 *         I2C driver for ATMEGA128rfa1
 *
 * \author
 *         Ingo Gulyas Intembsys
 *         office@intembsys.at
 *         www.intembsys.at
 */
 

#ifndef I2C_H_
#define I2C_H_


#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////////////////
// CONFIGURATION SECTION:

#define I2C_TD          0			// compiler switch: i2c testdriver code

#define I2C_PORT            PORTD
#define I2C_DDR             DDRD
#define I2C_SCL_PIN         0
#define I2C_SDA_PIN         1

// END OF CONFIGURATION SECTION
////////////////////////////////////////////////////////////////////////////////////////////

#define I2C_INIT()          ({I2C_DDR &= ~((1<<I2C_SCL_PIN) | (1<<I2C_SDA_PIN)); I2C_PORT |= ((1<<I2C_SCL_PIN) | (1<<I2C_SDA_PIN));})
#define I2C_DEINIT()        ({I2C_DDR &= ~((1<<I2C_SCL_PIN) | (1<<I2C_SDA_PIN)); I2C_PORT |= ((1<<I2C_SCL_PIN) | (1<<I2C_SDA_PIN));})

#define I2C_FREQ_STANDARD I2C_FREQ_50KHZ

#if F_CPU == 16000000UL
	#define	I2C_FREQ_400KHZ  0x03
	#define	I2C_FREQ_250KHZ  0x06
	#define	I2C_FREQ_100KHZ  0x12
	#define	I2C_FREQ_50KHZ   0x26
	#define I2C_FREQ_10KHZ   0xC6
#else
	#error "NO I2C FREQUENCY DEFINED -> CHECK F_CPU SETTINGS!"
#endif


#define I2C_STATUS_REG        TWSR
#define I2C_TX_REG            TWDR
#define I2C_RX_REG            TWDR

#define I2C_START()           (TWCR = (1<<TWINT) |(1<<TWSTA) | (1<<TWEN))
#define I2C_STOP()            (TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO))
#define I2C_WRITE_BYTE()      (TWCR = (1<<TWINT) | (1<<TWEN))
#define I2C_READ_BYTE_ACK()   (TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA))
#define I2C_READ_BYTE_NACK()  (TWCR = (1<<TWINT) | (1<<TWEN))
#define I2C_STATUS()          (I2C_STATUS_REG & I2C_STATUS_MASK)

#define I2C_STATUS_MASK       0xF8
#define I2C_STATUS_START      0x08
#define I2C_STATUS_START_REP  0x10
#define I2C_STATUS_SLAW_ACK   0x18
#define I2C_STATUS_DATAW_ACK  0x28
#define I2C_STATUS_SLAR_ACK   0x40
#define I2C_STATUS_DATAR_ACK  0x50
#define I2C_STATUS_DATAR_NACK 0x58

#define I2C_ADR_WR(adr)       ((adr<<1) & ~(1<<0))
#define I2C_ADR_RD(adr)       ((adr<<1) | (1<<0))

#define I2C_CMD_FLAG_NONE     0x00
#define I2C_CMD_FLAG_START    (1<<0)
#define I2C_CMD_FLAG_STOP     (1<<1)


#define I2C_OK                0
#define I2C_ERROR_DRIVER      -1
#define I2C_ERROR_TIMEOUT     -2
#define I2C_ERROR_START       -3
#define I2C_ERROR_WRITE       -4
#define I2C_ERROR_READ        -5


#define I2C_IOCTL_CMD_SET_FREQ 0x01


typedef struct i2c_driver i2c_driver;
struct i2c_driver
{
	int8_t (*ioctl)(const i2c_driver* const me, uint8_t cmd, uint8_t arg);
	int8_t (*read)(const i2c_driver* const me, uint8_t cmd_flags, uint8_t* buffer, uint8_t len);
	int8_t (*write)(const i2c_driver* const me, uint8_t cmd_flags, const uint8_t* data, uint8_t len);
};

i2c_driver* i2c_open(void);
void i2c_close(i2c_driver* const me);
void i2c_reset(void);


#endif /* I2C_H_ */
