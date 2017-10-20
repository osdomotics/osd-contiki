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
#include "net/netstack.h"

extern resource_t res_dhtatemp, res_dhtahum, res_battery;
extern resource_t res_dhtbtemp, res_dhtbhum;
extern resource_t res_dhtctemp, res_dhtchum;

#define LED_PIN 4

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +3.3V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) o
#define DHTPINA 7     // what digital pin we're connected to
DHT dhta(DHTPINA, DHTTYPE);

#define DHTPINB 8     // what digital pin we're connected to
DHT dhtb(DHTPINB, DHTTYPE);

#define DHTPINC 9     // what digital pin we're connected to
DHT dhtc(DHTPINC, DHTTYPE);

float dhta_hum;
float dhta_temp;
char  dhta_hum_s[8];
char  dhta_temp_s[8];

float dhtb_hum;
float dhtb_temp;
char  dhtb_hum_s[8];
char  dhtb_temp_s[8];

float dhtc_hum;
float dhtc_temp;
char  dhtc_hum_s[8];
char  dhtc_temp_s[8];

}

void setup (void)
{
//	NETSTACK_MAC.off(1); // reciever always on
	printf("DHT Sensor\n");
    // switch off the led
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
    // DHT sensor
    dhta.begin();
    dhtb.begin();
    dhtc.begin();
    // init coap resourcen
    rest_init_engine ();
#pragma GCC diagnostic ignored "-Wwrite-strings"
    rest_activate_resource (&res_dhtatemp, "s/tempa");
    rest_activate_resource (&res_dhtahum, "s/huma");
    rest_activate_resource (&res_dhtbtemp, "s/tempb");
    rest_activate_resource (&res_dhtbhum, "s/humb");
    rest_activate_resource (&res_dhtctemp, "s/tempc");
    rest_activate_resource (&res_dhtchum, "s/humc");
    rest_activate_resource (&res_battery, "s/battery");
#pragma GCC diagnostic pop    
    mcu_sleep_set(128); // Power consumtion 278uA; average over 20 minutes
}

// at project-conf.h
// LOOP_INTERVAL		(30 * CLOCK_SECOND)
void loop (void)
{
    int errdht=0;
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    dhta_hum = dhta.readHumidity();
    dhtb_hum = dhtb.readHumidity();
    dhtc_hum = dhtc.readHumidity();
    // Read temperature as Celsius (the default)
    dhta_temp = dhta.readTemperature();
    dhtb_temp = dhtb.readTemperature();
    dhtc_temp = dhtc.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(dhta_hum) || isnan(dhta_temp)) {
      printf("Failed to read from DHTa sensor!\n");
      errdht=1;
    }
    if (isnan(dhtb_hum) || isnan(dhtb_temp)) {
      printf("Failed to read from DHTb sensor!\n");
      errdht=1;
    }
    if (isnan(dhtc_hum) || isnan(dhtc_temp)) {
      printf("Failed to read from DHTc sensor!\n");
      errdht=1;
    }
    if (errdht){
		return;
	}

    dtostrf(dhta_temp , 0, 2, dhta_temp_s );   
    dtostrf(dhta_hum , 0, 2, dhta_hum_s );

    dtostrf(dhtb_temp , 0, 2, dhtb_temp_s );   
    dtostrf(dhtb_hum , 0, 2, dhtb_hum_s );

    dtostrf(dhtc_temp , 0, 2, dhtc_temp_s );   
    dtostrf(dhtc_hum , 0, 2, dhtc_hum_s );
      
//  debug only   
    printf("Tempa: '%s'",dhta_temp_s);
    printf("\t\tHuma: '%s'\n",dhta_hum_s);

    printf("Tempb: '%s'",dhtb_temp_s);
    printf("\t\tHumb: '%s'\n",dhtb_hum_s);

    printf("Tempc: '%s'",dhtc_temp_s);
    printf("\t\tHumc: '%s'\n",dhtc_hum_s);
}
