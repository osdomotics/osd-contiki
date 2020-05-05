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
#include "contiki.h"
#include "contiki-net.h"
#include "arduino-process.h"
#include "rest-engine.h"
#include "ota-update.h"

extern resource_t res_soiltemp,res_soilcap,res_soillight, res_battery;

float  soilcap;
float  soiltemp;
float  soillight;

uint8_t    soiladdr;
uint8_t    soilversion;

char   soilcap_s[16];
char   soiltemp_s[16];
char   soillight_s[16];

I2CSoilMoistureSensor sensor;

#define LED_PIN 4
#define PWR_PIN 7

// connect a mosfet to pwr pin to switch power supply to soil moisture sensor

#define SOIL_ON digitalWrite(PWR_PIN,HIGH);
#define SOIL_OFF digitalWrite(PWR_PIN,LOW);
}

void setup (void)
{
    // switch off the led
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
    pinMode (PWR_PIN, OUTPUT);
    Wire.begin();
    SOIL_OFF
    rest_init_engine ();
    #pragma GCC diagnostic ignored "-Wwrite-strings"
    rest_activate_resource (&res_soiltemp, "s/temp");
    rest_activate_resource (&res_soilcap, "s/soil");
    rest_activate_resource (&res_soillight, "s/light");
    rest_activate_resource (&res_battery, "s/battery");
    OTA_ACTIVATE_RESOURCES();
    #pragma GCC diagnostic pop

    //NETSTACK_MAC.off(1);
}

// at project-conf.h
// LOOP_INTERVAL		(3 * CLOCK_SECOND)
#define MEASURE_INTERVALL   120

void loop (void)
{

  static int count    = 0;
  static int measured = 0;

  count ++;

  switch(count){
    case 1 :
      printf ("power on sensor\n");
      SOIL_ON
      break;

    case 2 :
      printf ("sensor.begin\n");
      sensor.begin (false);
      break;

    case 3 :
      printf("I2C Soil Moisture Sensor Address: ");
      soiladdr = sensor.getAddress();
      printf("%X\n", soiladdr);
      printf("Sensor Firmware version: ");
      soilversion = sensor.getVersion();
      printf("%X\n", soilversion);
      printf ("start measure\n");
      sensor.startMeasureLight();
      break;

    case (MEASURE_INTERVALL) :
      printf ("start again in next round\n");
      measured = 0;
      count = 0;
      break;

    default :
      if (measured == 0) {
        printf ("check if sensor ready\n");
        if (!sensor.isBusy ()) { // available since FW 2.3
          printf ("sensor is ready, read measurements:\n");
          // measure the sensors
          soilcap   = sensor.getCapacitance(); //read capacitance register
          soiltemp  = sensor.getTemperature()/(float)10; //temperature register
          soillight = sensor.getLight(false); //request light measurement, read light register
          sensor.sleep(); // available since FW 2.3
          // convert to string
          dtostrf(soilcap   , 3, 2, soilcap_s   );
          dtostrf(soiltemp  , 3, 2, soiltemp_s  );
          dtostrf(soillight , 3, 2, soillight_s );
          // Debug Print
          printf("Temp: %s",soiltemp_s);
          printf("\t\tSoil: %s",soilcap_s);
          printf("\t\tLight: %s\n",soillight_s);
          printf ("turn power off\n");
          SOIL_OFF
          measured = 1;
        }
      } else {
        //printf ("wait...\n");
      }
      break;
  }
}
