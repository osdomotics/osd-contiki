/* Sensor routine */

#include "lib/sensors.h"
#include "dev/button-sensor.h"

#include <avr/interrupt.h>
#include "led.h" // debug

// define CONF_BUTTON_SENSOR_FUNC to one of the two below to get different
// behavior. (currently only works on avr cpu)
// note there is no debouncing in the COUNTER case !

#define BUTTON_SENSOR_FUNC_STATE   1
#define BUTTON_SENSOR_FUNC_COUNTER 2

#ifdef CONF_BUTTON_SENSOR_FUNC
#define BUTTON_SENSOR_FUNC CONF_BUTTON_SENSOR_FUNC
#else
#define BUTTON_SENSOR_FUNC BUTTON_SENSOR_FUNC_STATE
#endif

#if BUTTON_SENSOR_FUNC == BUTTON_SENSOR_FUNC_STATE
#define DEBOUNCE_TIME CLOCK_SECOND/16
#endif

const struct sensors_sensor button_sensor;

#if BUTTON_SENSOR_FUNC == BUTTON_SENSOR_FUNC_STATE
static struct timer debouncetimer;
volatile static int bstate;
volatile static int bstatei;
#elif BUTTON_SENSOR_FUNC == BUTTON_SENSOR_FUNC_COUNTER
static unsigned int pulses = 0;
#endif

static int status(int type);
static int enabled = 0;
struct sensors_sensor *sensors[1];
unsigned char sensors_flags[1];

#define BUTTON_BIT INTF4
#define BUTTON_CHECK_IRQ() (EIFR & BUTTON_BIT) ? 0 : 1

#define PRINTF(...) printf(__VA_ARGS__)
/*---------------------------------------------------------------------------*/

/* todo make it arduino compatible
ISR(INT4_vect)
{
  if(BUTTON_CHECK_IRQ()) {
#if BUTTON_SENSOR_FUNC == BUTTON_SENSOR_FUNC_STATE
    bstatei = (PINE & _BV(PE4) ? 0 : 1);
    if(timer_expired(&debouncetimer)) {
//  led1_on();
      timer_set(&debouncetimer, DEBOUNCE_TIME);
      bstate = (PINE & _BV(PE4) ? 0 : 1);
      if(bstate == bstatei){
#elif BUTTON_SENSOR_FUNC == BUTTON_SENSOR_FUNC_COUNTER
        pulses ++;
#endif
        sensors_changed(&button_sensor);
#if BUTTON_SENSOR_FUNC == BUTTON_SENSOR_FUNC_STATE
      }
 //   led1_off();
    }
#endif
  }
}
*/
/*---------------------------------------------------------------------------*/

static int
value(int type)
{
#if BUTTON_SENSOR_FUNC == BUTTON_SENSOR_FUNC_STATE
 //return (PINE & _BV(PE4) ? 0 : 1) || !timer_expired(&debouncetimer);

 return bstate;
#elif BUTTON_SENSOR_FUNC == BUTTON_SENSOR_FUNC_COUNTER
  return pulses;
#endif
}

static int
configure(int type, int c)
{
  switch (type) {
    case SENSORS_ACTIVE:
      if (c) {
        if(!status(SENSORS_ACTIVE)) {
#if BUTTON_SENSOR_FUNC == BUTTON_SENSOR_FUNC_STATE
  //  led1_on();
          timer_set(&debouncetimer, 0);
#endif
          DDRE |= (0<<DDE4); // Set pin as input
          PORTE |= (1<<PORTE4); // Set port PORTE bint 5 with pullup resistor
          EICRB |= (1<<ISC40); // For falling edge
          EIMSK |= (1<<INT4); // Set int
          enabled = 1;
          sei();
#if BUTTON_SENSOR_FUNC == BUTTON_SENSOR_FUNC_STATE
  //  led1_off();
#endif
        }
      } else {
        enabled = 0;
        EIMSK &= ~(1<<INT4); // clear int
      }
#if BUTTON_SENSOR_FUNC == BUTTON_SENSOR_FUNC_STATE
      return 1;
#elif BUTTON_SENSOR_FUNC == BUTTON_SENSOR_FUNC_COUNTER
      break;

    default :
      pulses = c;
      break;
#endif
    }
  return 0;
}

static int
status(int type)
{
  switch (type) {
    case SENSORS_ACTIVE:
    case SENSORS_READY:
		return enabled;//(EIMSK & (1<<INT5) ? 0 : 1);//BUTTON_IRQ_ENABLED();
  }
  return 0;
}

SENSORS_SENSOR(button_sensor, BUTTON_SENSOR,
	       value, configure, status);

