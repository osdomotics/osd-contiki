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
#include "DHT.h"

extern "C" {
#include "arduino-process.h"
#include "rest-engine.h"

extern resource_t res_htu21dtemp, res_htu21dhum, res_battery;

#define LED_PIN 4

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +3.3V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) o
#define DHTPIN 9     // what digital pin we're connected to
DHT dht(DHTPIN, DHTTYPE);

float dht_hum;
float dht_temp;
char  dht_hum_s[8];
char  dht_temp_s[8];

}

void setup (void)
{
	printf("DHT Sensor\n");
    // switch off the led
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
    // DHT sensor
    dht.begin();
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
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    dht_hum = dht.readHumidity();
    // Read temperature as Celsius (the default)
    dht_temp = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(dht_hum) || isnan(dht_temp)) {
      printf("Failed to read from DHT sensor!\n");
      return;
    }

    dtostrf(dht_temp , 0, 2, dht_temp_s );   
    dtostrf(dht_hum , 0, 2, dht_hum_s );
      
//  debug only   
    printf("Temp: '%s'",dht_temp_s);
    printf("\t\tHum: '%s'\n",dht_hum_s);
}
