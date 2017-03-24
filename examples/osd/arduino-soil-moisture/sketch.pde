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
#include "I2CSoilMoistureSensor.h"

extern "C" {
#include "arduino-process.h"
#include "rest-engine.h"

extern resource_t res_soiltemp,res_soilcap,res_soillight, res_battery;

float  soilcap;
float  soiltemp;
float  soillight;

uint8_t    soiladdr;
uint8_t    soilversion;

char   soilcap_s[8];
char   soiltemp_s[8];
char   soillight_s[8];

I2CSoilMoistureSensor sensor;

#define LED_PIN 4

}

void setup (void)
{
    // switch off the led
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
    // Soil sensor
    Wire.begin();
    sensor.begin(); // reset sensor
    delay(1000); // give some time to boot up
    printf("I2C Soil Moisture Sensor Address: ");
    soiladdr = sensor.getAddress();
    printf("%X\n", soiladdr);
    printf("Sensor Firmware version: ");
    soilversion = sensor.getVersion();   
    printf("%X\n", soilversion);
    sensor.startMeasureLight();
    // init coap resourcen
    rest_init_engine ();
    #pragma GCC diagnostic ignored "-Wwrite-strings"
    rest_activate_resource (&res_soiltemp, "s/temp");
    rest_activate_resource (&res_soilcap, "s/soil");
    rest_activate_resource (&res_soillight, "s/light");
    rest_activate_resource (&res_battery, "s/battery");
    #pragma GCC diagnostic pop
}

// at project-conf.h
// LOOP_INTERVAL		(1 * CLOCK_SECOND)
#define MEASURE_INTERVALL   10

void loop (void)
{

  static int count=0;
  
  count ++;
  switch(count){
    case 1 :
      sensor.startMeasureLight();
    
    break;
    case 4 :
    if(!sensor.isBusy()){ // available since FW 2.3
      // measure the sensors
      soilcap   = sensor.getCapacitance(); //read capacitance register
      soiltemp  = sensor.getTemperature()/(float)10; //temperature register
      soillight = sensor.getLight(0); //request light measurement, read light register
      sensor.sleep(); // available since FW 2.3
      // convert to string
      dtostrf(soilcap , 0, 2, soilcap_s );   
      dtostrf(soiltemp , 0, 2, soiltemp_s );
      dtostrf(soillight , 0, 2, soillight_s );
      // Debug Print
      printf("Temp: %s",soiltemp_s);
      printf("\t\tSoil: %s",soilcap_s);
      printf("\t\tLight: %s\n",soillight_s);
    }      
    break;
    case (MEASURE_INTERVALL+1) :
      count = 0;
    break;
  } 
}
