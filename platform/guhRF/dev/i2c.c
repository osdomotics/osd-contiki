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


#include "i2c.h"
#include "contiki-conf.h"
#include <stdint.h>
#include <avr/power.h>
#include <stdbool.h>
#include <stddef.h>

#if I2C_TD != 0
#include <stdio.h>
#include <avr/pgmspace.h>
#include "system_mgmt.h"
#define PRINTD(FORMAT,args...) {sleep_acquire_lock(); printf_P(PSTR(FORMAT),##args); sleep_release_lock();}
#else
#define PRINTD(...)
#endif

#if WITH_RTDEBUG == 1
#include "rtdebug.h"
#define RTDEBUG_PUSH(x) rtdebug_push(x)
#else
#warning "I2C Driver compiling without RTDEBUG!"
#define RTDEBUG_PUSH(x)
#endif

#ifndef TIMEOUT_TIMER
#warning "I2C Driver compiling without TIMEOUT!"
#endif


static int8_t wait_job();
static int8_t wait_stop();

static int8_t i2c_ioctl(const i2c_driver* const me, uint8_t cmd, uint8_t arg);
static int8_t i2c_read(const i2c_driver* const me, uint8_t cmd_flags, uint8_t* buffer, uint8_t len);
static int8_t i2c_write(const i2c_driver* const me, uint8_t cmd_flags, const uint8_t* data, uint8_t len);


// static linkage of member functions
i2c_driver i2c_drv = {i2c_ioctl, i2c_read, i2c_write};
// lock spi if driver opened to prevent further opening access
static volatile bool i2c_lock = false;


///////////////////////////////////////////////////////////////
// global functions
///////////////////////////////////////////////////////////////

i2c_driver* i2c_open(void)
{
	if(i2c_lock == true)
	{
		RTDEBUG_PUSH(RTDEBUG_CODE__I2C_OPEN__DEVICE_BUSY);
		return NULL;
	}	
	 
	i2c_lock = true;
	power_twi_enable();
	I2C_INIT();	

	TWBR = I2C_FREQ_STANDARD;
	TWSR &= ~((1<<TWPS1) | (1<<TWPS0));
	TWSR |= (1<<TWPS0);
	TWCR = (1<<TWEN);
	
	return &i2c_drv;
}

void i2c_close(i2c_driver* const me)
{
	if(me == NULL || i2c_lock == false)
	{
		RTDEBUG_PUSH(RTDEBUG_CODE__I2C_CLOSE__NO_DEVICE);
		return;
	}
	
	i2c_reset();
	
	return;
}

void i2c_reset(void)
{
	TWCR &= ~(1<<TWEN);
	I2C_DEINIT();
	power_twi_disable();
	i2c_lock = false;
	
	return;
}


///////////////////////////////////////////////////////////////
// local helper functions
///////////////////////////////////////////////////////////////

#ifdef TIMEOUT_TIMER
	static int8_t wait_job()
	{
		int8_t status = I2C_OK;	
		uint16_t timeout = TIMEOUT_TIMER_NOW_ADD(I2C_TIMEOUT);
	

		while (!(TWCR & (1<<TWINT)))
		{
			if(!(TIMEOUT_TIMER_LT(TIMEOUT_TIMER_NOW(), timeout)))
			{
				status = I2C_ERROR_TIMEOUT;
				break;
			}	
		}
	
		return status;
	}
	
	static int8_t wait_stop()
	{
		int8_t status = I2C_OK;	
		uint16_t timeout = TIMEOUT_TIMER_NOW_ADD(I2C_TIMEOUT);
	

		while (TWCR & (1<<TWSTO))
		{
			if(!(TIMEOUT_TIMER_LT(TIMEOUT_TIMER_NOW(), timeout)))
			{
				status = I2C_ERROR_TIMEOUT;
				break;
			}	
		}
	
		return status;
	}
#else
	static int8_t wait_job()
	{
		while (!(TWCR & (1<<TWINT)));
		return I2C_OK;
	}
	
	static int8_t wait_stop()
	{
		while (TWCR & (1<<TWSTO));
		return I2C_OK;
	}
#endif


///////////////////////////////////////////////////////////////
// member functions
///////////////////////////////////////////////////////////////

static int8_t i2c_ioctl(const i2c_driver* const me, uint8_t cmd, uint8_t arg)
{
	if(me == NULL || i2c_lock == false)
	{
		RTDEBUG_PUSH(RTDEBUG_CODE__I2C_IOCTL__DEVICE_CLOSED);
		return I2C_ERROR_DRIVER;
	}
		
	if(cmd == I2C_IOCTL_CMD_SET_FREQ)
	{
		switch(arg)
		{
			case I2C_FREQ_400KHZ:	TWBR = I2C_FREQ_400KHZ; break;
			case I2C_FREQ_250KHZ:	TWBR = I2C_FREQ_250KHZ; break;
			case I2C_FREQ_100KHZ:	TWBR = I2C_FREQ_100KHZ; break;
			case I2C_FREQ_50KHZ:	TWBR = I2C_FREQ_50KHZ; break;
			case I2C_FREQ_10KHZ:	TWBR = I2C_FREQ_10KHZ; break;
			default:
			{
				RTDEBUG_PUSH(RTDEBUG_CODE__I2C_IOCTL__ARG_INVALID);
				return I2C_ERROR_DRIVER;
			}			
		}
	}
	else
	{
		RTDEBUG_PUSH(RTDEBUG_CODE__I2C_IOCTL__CMD_INVALID);
		return I2C_ERROR_DRIVER;
	}
	
	return I2C_OK;
}

static int8_t i2c_read(const i2c_driver* const me, uint8_t cmd_flags, uint8_t* buffer, uint8_t len)
{
	uint8_t i = 0;
	int8_t status = I2C_OK;
	
	
	if(me == NULL || i2c_lock == false)
	{
		RTDEBUG_PUSH(RTDEBUG_CODE__I2C_READ__DEVICE_CLOSED);
		return I2C_ERROR_DRIVER;
	}	
	
	if((len > 0) && (buffer == NULL))
	{
		RTDEBUG_PUSH(RTDEBUG_CODE__I2C_READ__ERROR_NULLPOINTER);
		return I2C_ERROR_DRIVER;
	}
	
	do 
	{
		if(cmd_flags & I2C_CMD_FLAG_START)
		{
			I2C_START();
			if(wait_job() != I2C_OK)
			{
				RTDEBUG_PUSH(RTDEBUG_CODE__I2C_READ__START_TIMEOUT);
				status = I2C_ERROR_TIMEOUT;
				break;
			}
			if((I2C_STATUS() != I2C_STATUS_START) && (I2C_STATUS() != I2C_STATUS_START_REP))
			{
				RTDEBUG_PUSH(RTDEBUG_CODE__I2C_READ__START_ERROR);
				status = I2C_ERROR_START;
				break;
			}
			PRINTD("I2C-RD-START\n");
		}
		
		if(len == 0) break;
		
		for(i=0; i<(len-1); i++)
		{
			I2C_READ_BYTE_ACK();
			if(wait_job() != I2C_OK)
			{
				RTDEBUG_PUSH(RTDEBUG_CODE__I2C_READ__READ_BYTE_ACK_TIMEOUT);
				status = I2C_ERROR_TIMEOUT;
				break;
			}
			if(I2C_STATUS() != I2C_STATUS_DATAR_ACK)
			{
				RTDEBUG_PUSH(RTDEBUG_CODE__I2C_READ__READ_BYTE_ACK_ERROR);
				status = I2C_ERROR_READ;
				break;
			}
			buffer[i] = I2C_RX_REG;
			PRINTD("I2C-RD-RACK: 0x%02X\n", buffer[i]);
		}
		
		I2C_READ_BYTE_NACK();
		if(wait_job() != I2C_OK)
		{
			RTDEBUG_PUSH(RTDEBUG_CODE__I2C_READ__READ_BYTE_NACK_TIMEOUT);
			status = I2C_ERROR_TIMEOUT;
			break;
		}
		if(I2C_STATUS() != I2C_STATUS_DATAR_NACK)
		{
			RTDEBUG_PUSH(RTDEBUG_CODE__I2C_READ__READ_BYTE_NACK_ERROR);
			status = I2C_ERROR_READ;
			break;
		}
		buffer[i] = I2C_RX_REG;
		PRINTD("I2C-RD-RNACK: 0x%02X\n", buffer[i]);
		
	} while (0);
	
	if(cmd_flags & I2C_CMD_FLAG_STOP)
	{
		I2C_STOP();
		if(wait_stop() != I2C_OK)
		{
			RTDEBUG_PUSH(RTDEBUG_CODE__I2C_READ__STOP_TIMEOUT);
			status = I2C_ERROR_TIMEOUT;
		}
		PRINTD("I2C-RD_STOP\n");
	}
	
	return status;
}

static int8_t i2c_write(const i2c_driver* const me, uint8_t cmd_flags, const uint8_t* data, uint8_t len)
{
	uint8_t i = 0;
	int8_t status = I2C_OK;
	
	
	if(me == NULL || i2c_lock == false)
	{
		RTDEBUG_PUSH(RTDEBUG_CODE__I2C_WRITE__DEVICE_CLOSED);
		return I2C_ERROR_DRIVER;
	}
	
	if((len > 0) && (data == NULL))
	{
		RTDEBUG_PUSH(RTDEBUG_CODE__I2C_WRITE__ERROR_NULLPOINTER);
		return I2C_ERROR_DRIVER;
	}
	
	do 
	{		
		if(cmd_flags & I2C_CMD_FLAG_START)
		{
			I2C_START();
			if(wait_job() != I2C_OK)
			{
				RTDEBUG_PUSH(RTDEBUG_CODE__I2C_WRITE__START_TIMEOUT);
				status = I2C_ERROR_TIMEOUT;
				break;
			}
			if((I2C_STATUS() != I2C_STATUS_START) && (I2C_STATUS() != I2C_STATUS_START_REP))
			{
				RTDEBUG_PUSH(RTDEBUG_CODE__I2C_WRITE__START_ERROR);
				status = I2C_ERROR_START;
				break;
			}
			PRINTD("I2C-WR-START\n");
		}
		
		
	
		for(i=0; i<len; i++)
		{
			I2C_TX_REG = data[i];
			I2C_WRITE_BYTE();
			if(wait_job() != I2C_OK)
			{
				RTDEBUG_PUSH(RTDEBUG_CODE__I2C_WRITE__WRITE_BYTE_TIMEOUT);
				status = I2C_ERROR_TIMEOUT;
				break;
			}
			if((I2C_STATUS() != I2C_STATUS_DATAW_ACK) && (I2C_STATUS() != I2C_STATUS_SLAW_ACK) && (I2C_STATUS() != I2C_STATUS_SLAR_ACK))
			{
				RTDEBUG_PUSH(RTDEBUG_CODE__I2C_WRITE__WRITE_BYTE_ERROR);
				status = I2C_ERROR_WRITE;
				break;
			}
			PRINTD("I2C-WR-BYTE: 0x%02X\n", data[i]);
		}

	} while (0);
	
	if(cmd_flags & I2C_CMD_FLAG_STOP)
	{
		I2C_STOP();
		if(wait_stop() != I2C_OK)
		{
			RTDEBUG_PUSH(RTDEBUG_CODE__I2C_WRITE__STOP_TIMEOUT);
			status = I2C_ERROR_TIMEOUT;
		}
		PRINTD("I2C-WR-STOP\n");
	}
	
	return status;
}
