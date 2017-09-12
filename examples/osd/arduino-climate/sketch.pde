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
#include "Adafruit_HTU21DF.h"

extern "C" {
#include "arduino-process.h"
#include "rest-engine.h"

Adafruit_HTU21DF htu = Adafruit_HTU21DF();

extern resource_t res_htu21dtemp, res_htu21dhum, res_battery;
float htu21d_hum;
float htu21d_temp;
char  htu21d_hum_s[8];
char  htu21d_temp_s[8];

#define LED_PIN 4
}

void setup (void)
{
    // switch off the led
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
    // htu21d sensor
    if (!htu.begin()) {
      printf("Couldn't find sensor!");
    }
    // init coap resourcen
    rest_init_engine ();
#pragma GCC diagnostic ignored "-Wwrite-strings"
    rest_activate_resource (&res_htu21dtemp, "s/temp");
    rest_activate_resource (&res_htu21dhum, "s/hum");
    rest_activate_resource (&res_battery, "s/battery");
#pragma GCC diagnostic pop    
    mcu_sleep_set(128); // Power consumtion 278uA; average over 20 minutes
}

// at project-conf.h
// LOOP_INTERVAL		(30 * CLOCK_SECOND)
void loop (void)
{
    htu21d_temp = htu.readTemperature();
    htu21d_hum = htu.readHumidity();
    dtostrf(htu21d_temp , 0, 2, htu21d_temp_s );   
    dtostrf(htu21d_hum , 0, 2, htu21d_hum_s );
      
//  debug only   
//  printf("Temp: '%s'",htu21d_temp_s);
//  printf("\t\tHum: '%s'\n",htu21d_hum_s);
}
