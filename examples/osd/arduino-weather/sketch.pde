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
#include "Adafruit_HTU21DF.h"
#include "BH1750FVI.h"

extern "C" {
#include "arduino-process.h"
#include "rest-engine.h"

extern resource_t res_bh1750, res_htu21dtemp, res_htu21dhum, res_bmp085press,res_bmp085atm,res_bmp085alt, res_battery;

float bmp085temp;
float bmp085press;
float bmp085atm;
float bmp085alt;
char  bmp085temp_s[8];
char  bmp085press_s[8];
char  bmp085atm_s[8];
char  bmp085alt_s[8];

Barometer myBarometer;

float htu21d_hum;
float htu21d_temp;
char  htu21d_hum_s[8];
char  htu21d_temp_s[8];

Adafruit_HTU21DF htu = Adafruit_HTU21DF();

uint16_t lux;

BH1750FVI lightMeter;

#define LED_PIN 4
}

void setup (void)
{
    // switch off the led
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
    // BH1750 sensor
    Wire.begin();
    lightMeter.begin();
    // BMP085 sensor
    myBarometer.init();
    // htu21d sensor
    if (!htu.begin()) {
      printf("Couldn't find sensor htu21d !");
    }
    // init coap resourcen
    rest_init_engine ();
    #pragma GCC diagnostic ignored "-Wwrite-strings"
    rest_activate_resource (&res_bmp085press, "s/press");
    rest_activate_resource (&res_bmp085atm, "s/atm");
    rest_activate_resource (&res_bmp085alt, "s/alt");
    rest_activate_resource (&res_htu21dtemp, "s/temp");
    rest_activate_resource (&res_htu21dhum, "s/hum");
    rest_activate_resource (&res_bh1750, "s/lux");
    rest_activate_resource (&res_battery, "s/battery");
    #pragma GCC diagnostic ignored "-Wwrite-strings"
}

// at project-conf.h
// LOOP_INTERVAL		(10 * CLOCK_SECOND)
void loop (void)
{
	// BMP085 Sensor
    bmp085temp = myBarometer.bmp085GetTemperature(myBarometer.bmp085ReadUT()); //Get the temperature, bmp085ReadUT MUST be called first
    bmp085press = myBarometer.bmp085GetPressure(myBarometer.bmp085ReadUP());//Get the temperature
    bmp085alt = myBarometer.calcAltitude(bmp085press); //Uncompensated caculation - in Meters 
    bmp085atm = bmp085press / 101325;
    
    dtostrf(bmp085temp , 0, 2, bmp085temp_s );   
    dtostrf(bmp085press , 0, 2, bmp085press_s );
    dtostrf(bmp085alt , 0, 2, bmp085alt_s );
    dtostrf(bmp085atm , 0, 2, bmp085atm_s );
    
    // HTU21d Sensor
    htu21d_temp = htu.readTemperature();
    htu21d_hum = htu.readHumidity();
    dtostrf(htu21d_temp , 0, 2, htu21d_temp_s );   
	dtostrf(htu21d_hum , 0, 2, htu21d_hum_s );
    
    // BH1750
    lux = lightMeter.getLightLevel();     
    
// Debug Print
	printf("BMP085\n");
    printf("Press: %s\n",bmp085press_s);
    printf("Altitude: %s\n",bmp085alt_s);
    printf("atm: %s\n",bmp085atm_s);
    printf("HTU21d\n");
    printf("Temp: %s\n",htu21d_temp_s);
    printf("Hum: %s\n",htu21d_hum_s);
    printf("BH1750\n");    
    printf("Lux: %d\n",lux);
}
