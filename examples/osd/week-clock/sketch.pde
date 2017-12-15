/*
 * Sample arduino sketch using contiki features.
 * We turn the LED off 
 * We allow read the moisture sensor
 * Unfortunately sleeping for long times in loop() isn't currently
 * possible, something turns off the CPU (including PWM outputs) if a
 * Proto-Thread is taking too long. We need to find out how to sleep in
 * a Contiki-compatible way.
 * Note that for a normal arduino sketch you won't have to include any
 * of the contiki-specific files here, the sketch should just work.
 */

extern "C" {
#include "arduino-process.h"
#include "rest-engine.h"
#include "net/netstack.h"
#include "xtime.h"
#include "cron.h"
#include "time_resource.h"
#include "jsonparse.h"
#include "TM1637.h"

int8_t TimeDisp[] = {0x00,0x00,0x00,0x00};
unsigned char ClockPoint = 1;
unsigned char second;
unsigned char minute = 0;
unsigned char hour = 12;

extern resource_t res_led, res_battery, res_cputemp;

uint8_t led_pin=4;
uint8_t led_status;
}

#define CLK 3//pins definitions for TM1637 and can be changed to other ports    
#define DIO 14
TM1637 tm1637(CLK,DIO);

void setup (void)
{
    // switch off the led
    pinMode(led_pin, OUTPUT);
    digitalWrite(led_pin, HIGH);
    led_status=0;
    // 4-Digit Display
    tm1637.set();
    tm1637.init();
    // init coap resourcen
    rest_init_engine ();
    #pragma GCC diagnostic ignored "-Wwrite-strings"
    rest_activate_resource (&res_led, "s/led");
    rest_activate_resource (&res_battery, "s/battery");
    rest_activate_resource (&res_cputemp, "s/cputemp");
    rest_activate_resource (&res_timestamp, "clock/timestamp");
    rest_activate_resource (&res_timezone, "clock/timezone");
    rest_activate_resource (&res_localtime, "clock/localtime");
    rest_activate_resource (&res_utc, "clock/utc");
    #pragma GCC diagnostic pop

    
 //   NETSTACK_MAC.off(1);
    mcu_sleep_set(128);
}

void TimeUpdate(void)
{
  ClockPoint = (~ClockPoint) & 0x01;

  second ++;
  if(second == 60)
  {
    minute ++;
    if(minute == 60)
    {
      hour ++;
      if(hour == 24)hour = 0;
      minute = 0;
    }
    second = 0;
  }
  	
  if(ClockPoint)tm1637.point(POINT_ON);
  else tm1637.point(POINT_OFF); 

  TimeDisp[0] = hour / 10;
  TimeDisp[1] = hour % 10;
  TimeDisp[2] = minute / 10;
  TimeDisp[3] = minute % 10;
  
}

void loop (void)
{
    TimeUpdate();
    tm1637.display(TimeDisp);
}
