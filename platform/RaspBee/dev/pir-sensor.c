/* Sensor routine */
#include "contiki.h"
#include "lib/sensors.h"
#include "dev/pir-sensor.h"

#include <avr/interrupt.h>
#include "led.h" // debug

const struct sensors_sensor pir_sensor;

static struct timer debouncetimer;
static int status(int type);
static int enabled = 0;
struct sensors_sensor *sensors[1];
unsigned char sensors_flags[1];

#define PIR_BIT INTF6
#define PIR_CHECK_IRQ() (EIFR & PIR_BIT) ? 0 : 1

#define PRINTF(...) printf(__VA_ARGS__)
/*---------------------------------------------------------------------------*/
ISR(INT6_vect)
{

//  leds_toggle(LEDS_YELLOW);
  
  if(PIR_CHECK_IRQ()) {
    if(timer_expired(&debouncetimer)) {
 //   led1_on();
      timer_set(&debouncetimer, CLOCK_SECOND / 4);
      sensors_changed(&pir_sensor);
 //   led1_off();
    }
  }

}
/*---------------------------------------------------------------------------*/

static int
value(int type)
{
 return (PORTE & _BV(PE6) ? 0 : 1) || !timer_expired(&debouncetimer);
 //return 0;
}

static int
configure(int type, int c)
{
	switch (type) {
	case SENSORS_ACTIVE:
		if (c) {
			if(!status(SENSORS_ACTIVE)) {
  //  led1_on();
				timer_set(&debouncetimer, 0);
				DDRE |= (0<<DDRE6); // Set pin as input
				PORTE |= (1<<PORTE6); // Set port PORTE bint 6 with pullup resistor
				EICRB |= (3<<ISC60); // For rising edge
				EIMSK |= (1<<INT6); // Set int
				enabled = 1;
				sei();
  //  led1_off();
			}
		} else {
				enabled = 0;
				EIMSK &= ~(1<<INT6); // clear int
		}
		return 1;
	}
	return 0;
}

static int
status(int type)
{
	switch (type) {
	case SENSORS_ACTIVE:
	case SENSORS_READY:
		return enabled;//(EIMSK & (1<<INT6) ? 0 : 1);//PIR_IRQ_ENABLED();
	}
	return 0;
}

SENSORS_SENSOR(pir_sensor, PIR_SENSOR,
	       value, configure, status);

