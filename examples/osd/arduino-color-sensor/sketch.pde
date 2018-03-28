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
#include "Adafruit_TCS34725.h"

extern "C" {
#include "arduino-process.h"
#include "rest-engine.h"

extern resource_t  res_battery;

#define LED_PIN 4
}

/* Connect SCL    to analog 5
   Connect SDA    to analog 4
   Connect VDD    to 3.3V DC
   Connect GROUND to common ground */
   
/* Initialise with default values (int time = 2.4ms, gain = 1x) */
// Adafruit_TCS34725 tcs = Adafruit_TCS34725();

/* Initialise with specific int time and gain values */
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

void setup (void)
{
    // switch off the led
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
	if (tcs.begin()) {
		printf("Found sensor\n");
	} else {
		printf("No TCS34725 found ... check your connections{n");
	}
    // init coap resourcen
    rest_init_engine ();
#pragma GCC diagnostic ignored "-Wwrite-strings"
    rest_activate_resource (&res_battery, "s/battery");
#pragma GCC diagnostic pop    
    mcu_sleep_set(128); // Power consumtion 278uA; average over 20 minutes
}

// at project-conf.h
// LOOP_INTERVAL		(30 * CLOCK_SECOND)
void loop (void)
{
  uint16_t r, g, b, c, colorTemp, lux;
  
  tcs.getRawData(&r, &g, &b, &c);
  colorTemp = tcs.calculateColorTemperature(r, g, b);
  lux = tcs.calculateLux(r, g, b);
  
  printf("CTemp: %d K - ",colorTemp);
  printf("Lux: %d - ",lux);
  printf("R: %d ",r);
  printf("G: %d ",g);
  printf("B: %d ",b);
  printf("C: %d ",c);
  printf("\n");      
}
