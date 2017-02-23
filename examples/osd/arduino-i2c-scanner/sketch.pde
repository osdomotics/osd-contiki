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

#include <Wire.h>

extern "C" {
#include "arduino-process.h"
#include "rest-engine.h"
#include "net/netstack.h"

extern resource_t res_led, res_battery, res_cputemp;

uint8_t led_pin=4;
uint8_t led_status;
}

void setup (void)
{
    // switch off the led
    pinMode(led_pin, OUTPUT);
    digitalWrite(led_pin, HIGH);
    led_status=0;
    // i2c
    Wire.begin();
    // init coap resourcen
    rest_init_engine ();
    rest_activate_resource (&res_led, "s/led");
    rest_activate_resource (&res_battery, "s/battery");
    rest_activate_resource (&res_cputemp, "s/cputemp");
    
 //   NETSTACK_MAC.off(1);
    mcu_sleep_set(128);
}

void loop (void)
{
  byte error, address;
  int nDevices;
 
  printf("Scanning...\n");
 
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      printf("I2C device found at address %2X",address); 
      nDevices++;
    }
    else if (error==4)
    {
      printf("Unknow error at address %2X",address);
    }    
  }
  printf("\n");
  if (nDevices == 0)
    printf("No I2C devices found\n");
  else
    printf("done\n");
}
