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
#include "Barometer.h"

extern "C" {
#include "arduino-process.h"
#include "rest-engine.h"

extern resource_t res_bmp085temp,res_bmp085press,res_bmp085atm,res_bmp085alt, res_battery;

float bmp085temp;
float bmp085press;
float bmp085atm;
float bmp085alt;
char  bmp085temp_s[8];
char  bmp085press_s[8];
char  bmp085atm_s[8];
char  bmp085alt_s[8];

Barometer myBarometer;

#define LED_PIN 4
}

void setup (void)
{
    // switch off the led
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
    // BMP085 sensor
    myBarometer.init();
    // init coap resourcen
    rest_init_engine ();
    #pragma GCC diagnostic ignored "-Wwrite-strings"
    rest_activate_resource (&res_bmp085temp, "s/temp");
    rest_activate_resource (&res_bmp085press, "s/press");
    rest_activate_resource (&res_bmp085atm, "s/atm");
    rest_activate_resource (&res_bmp085alt, "s/alt");
    rest_activate_resource (&res_battery, "s/battery");
    #pragma GCC diagnostic pop
}

// at project-conf.h
// LOOP_INTERVAL		(10 * CLOCK_SECOND)
void loop (void)
{
	mcu_sleep_off();
    bmp085temp = myBarometer.bmp085GetTemperature(myBarometer.bmp085ReadUT()); //Get the temperature, bmp085ReadUT MUST be called first
    bmp085press = myBarometer.bmp085GetPressure(myBarometer.bmp085ReadUP());//Get the temperature
    bmp085alt = myBarometer.calcAltitude(bmp085press); //Uncompensated caculation - in Meters 
    bmp085atm = bmp085press / 101325;
    
    dtostrf(bmp085temp , 0, 2, bmp085temp_s );   
    dtostrf(bmp085press , 0, 2, bmp085press_s );
    dtostrf(bmp085alt , 0, 2, bmp085alt_s );
    dtostrf(bmp085atm , 0, 2, bmp085atm_s );
         
// Debug Print
	printf("Temp: %s",bmp085temp_s);
    printf("\t\tPress: %s\n",bmp085press_s);
    printf("\t\tAltitude: %s\n",bmp085alt_s);
    printf("\t\tatm: %s\n",bmp085atm_s);
  	mcu_sleep_on();    
}
