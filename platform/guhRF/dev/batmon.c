#include "contiki.h"
#include "batmon.h"
#include <util/delay.h>




int8_t batmon_init()
{
	return 0;
}	

int8_t batmon_get_voltage(uint16_t* voltage)
{
	uint16_t volt = 0;
	uint16_t resolution = 75;
	uint16_t offset = 2550;
	int8_t ctr = 0;
	
	BATMON = 0 | _BV(BATMON_HR);
	_delay_us(2);
	
	if(BATMON & _BV(BATMON_OK))
	{
		// voltage above 2.550 V
		resolution = 75;
		offset = 2550;
		for(ctr=15; ctr>=0; ctr--)
		{
			BATMON = (BATMON & 0xF0) | (ctr);
			_delay_us(2);
			if(BATMON & _BV(BATMON_OK)) break;
		}
	}
	else
	{
		// voltage below 2.550 V
		resolution = 50;
		offset = 1700;
		
		BATMON &= ~_BV(BATMON_HR);
		
		for(ctr=15; ctr>=0; ctr--)
		{
			BATMON = (BATMON & 0xF0) | (ctr);
			_delay_us(2);
			if(BATMON & _BV(BATMON_OK)) break;
		}
	}
	
	volt = resolution*ctr+offset;
	*voltage=volt; 
	
	return 0;
}
