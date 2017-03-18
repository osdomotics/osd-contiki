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
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

extern "C" {
#include "arduino-process.h"
#include "rest-engine.h"

extern resource_t res_bmptemp,res_bmppress,res_bmpatm,res_bmpalt, res_battery;

float bmptemp;
float bmppress;
float bmpatm;
float bmpalt;
char  bmptemp_s[8];
char  bmppress_s[10];
char  bmpatm_s[8];
char  bmpalt_s[8];;

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BMP280 bme; // I2C

#define LED_PIN 4
}

void setup (void)
{
    bool status;
    
    // switch off the led
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
    // default settings
    status = bme.begin(0x76);
    if (!status) {
        printf("Could not find a valid BMP280 sensor, check wiring!");
    }
    // init coap resourcen
    rest_init_engine ();
    #pragma GCC diagnostic ignored "-Wwrite-strings"
    rest_activate_resource (&res_bmptemp, "s/temp");
    rest_activate_resource (&res_bmppress, "s/press");
    rest_activate_resource (&res_bmpatm, "s/atm");
    rest_activate_resource (&res_bmpalt, "s/alt");
    rest_activate_resource (&res_battery, "s/battery");
    #pragma GCC diagnostic pop
    mcu_sleep_set(128);
}

// at project-conf.h
// LOOP_INTERVAL		(30 * CLOCK_SECOND)
void loop (void)
{
    bmptemp = bme.readTemperature();
    bmppress = bme.readPressure();
    bmpalt = bme.readAltitude(SEALEVELPRESSURE_HPA);
    bmpatm = bme.readPressure() / 100.0F;
    
    dtostrf(bmptemp , 0, 2, bmptemp_s );   
    dtostrf(bmppress , 0, 2, bmppress_s );
    dtostrf(bmpalt , 0, 2, bmpalt_s );
    dtostrf(bmpatm , 0, 2, bmpatm_s );
   

// Debug Print
/*
    printf("Temp: %s\n",bmptemp_s);
    printf("Press: %s\n",bmppress_s);
    printf("Altitude: %s\n",bmpalt_s);
    printf("atm: %s\n",bmpatm_s);   
*/
}
