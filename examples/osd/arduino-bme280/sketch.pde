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
#include <Adafruit_BME280.h>

extern "C" {
#include "arduino-process.h"
#include "rest-engine.h"
#include "ota-update.h"

extern resource_t res_bmptemp, res_bmphum, res_bmppress,res_bmpatm,res_bmpalt, res_battery,res_nbt,
  res_routes,
  res_smallest_rssi;

float bmptemp;
float bmppress;
float bmpatm;
float bmpalt;
float bmphum;
char  bmptemp_s[8];
char  bmppress_s[10];
char  bmpatm_s[8];
char  bmpalt_s[8];
char  bmphum_s[8];

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme; // I2C

#define LED_PIN 4
}

void setup (void)
{
    // switch off the led
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);

    bool status;
    
    // default settings
    status = bme.begin();  
    if (!status) {
        printf("Could not find a valid BME280 sensor, check wiring, address, sensor ID!\n");
        printf("SensorID was: 0x%lx\n",bme.sensorID());
        printf("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        printf("   ID of 0x56-0x58 represents a BMP 280,\n");
        printf("        ID of 0x60 represents a BME 280.\n");
        printf("        ID of 0x61 represents a BME 680.\n");
        while (1);
    } else {
        printf ("BME280 found with id: 0x%lx\n", bme.sensorID());
    }

    // init coap resourcen
    rest_init_engine ();
    #pragma GCC diagnostic ignored "-Wwrite-strings"
    rest_activate_resource (&res_bmptemp, "s/temp");
    rest_activate_resource (&res_bmphum, "s/hum");
    rest_activate_resource (&res_bmppress, "s/press");
    rest_activate_resource (&res_bmpatm, "s/atm");
    rest_activate_resource (&res_bmpalt, "s/alt");
    rest_activate_resource (&res_battery, "s/battery");
    rest_activate_resource (&res_nbt,           "s/nbt");
    rest_activate_resource (&res_routes,        "s/routes");
    rest_activate_resource (&res_smallest_rssi, "s/min_rssi");
    OTA_ACTIVATE_RESOURCES();
    #pragma GCC diagnostic pop
}

// at project-conf.h
// LOOP_INTERVAL		(10 * CLOCK_SECOND)
void loop (void)
{
    bmptemp = bme.readTemperature();
    bmppress = bme.readPressure();
    bmpalt = bme.readAltitude(SEALEVELPRESSURE_HPA);
    bmpatm = bme.readPressure() / 100.0F;
    bmphum = bme.readHumidity();
    
    dtostrf(bmptemp , 0, 2, bmptemp_s );   
    dtostrf(bmppress , 0, 2, bmppress_s );
    dtostrf(bmpalt , 0, 2, bmpalt_s );
    dtostrf(bmpatm , 0, 2, bmpatm_s );
    dtostrf(bmphum , 0, 2, bmphum_s );
         
// Debug Print
//    printf("Temp: %s\n",bmptemp_s);
//    printf("Press: %s\n",bmppress_s);
//    printf("Altitude: %s\n",bmpalt_s);
//    printf("atm: %s\n",bmpatm_s);
//    printf("hum: %s\n",bmphum_s);   
}
