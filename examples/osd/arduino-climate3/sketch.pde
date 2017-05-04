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
#include <OneWire.h>
#include "DallasTemperature.h"

extern "C" {

#include "arduino-process.h"
#include "rest-engine.h"
#include "sketch.h"

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 3
#define TEMPERATURE_PRECISION 9

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature dsensors(&oneWire);

// arrays to hold device addresses
DeviceAddress outsideThermometer;

Adafruit_HTU21DF htu = Adafruit_HTU21DF();

extern resource_t res_htu21dtemp, res_htu21dhum, res_dtemp1, res_battery;
float htu21d_hum;
float htu21d_temp;
char  htu21d_hum_s[8];
char  htu21d_temp_s[8];

float d_temp;
char  d_temp_s[8];
// sketch.h
struct dstemp ds1820[7];

#define LED_PIN 4
}
// main functions to print information about a device
void printAddress(uint8_t* adress)
{
 printf("%02X",adress[0]);
 printf("%02X",adress[1]);
 printf("%02X",adress[2]);
 printf("%02X",adress[3]);
 printf("%02X",adress[4]);
 printf("%02X",adress[5]);
 printf("%02X",adress[6]);
 printf("%02X",adress[7]);
}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress,int index)
{
 d_temp = dsensors.getTempC(deviceAddress);
 dtostrf(d_temp , 0, 2, d_temp_s );
 printf("Temp C: ");
 printf("%s",d_temp_s);
 // copy to structure
 ds1820[index].ftemp=d_temp;
 strcpy(ds1820[index].stemp, d_temp_s);
}

void printData(DeviceAddress deviceAddress, int index)
{
 printf("Device Address: ");
 printAddress(deviceAddress);
 printf(" ");
 printTemperature(deviceAddress,index);
 printf("\n");
}

void setup (void)
{
    // switch off the led
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
    // ds1820 sensor
    printf("Dallas Temperature IC Control Library Demo");
    // Start up the library
    dsensors.begin();
    // locate devices on the bus
    printf("Locating devices...\n");
    printf("Found ");
    printf("%d",dsensors.getDeviceCount());
    printf(" devices.\n");
    // report parasite power requirements
    printf("Parasite power is: "); 
    if (dsensors.isParasitePowerMode()) printf("ON\n");
    else printf("OFF\n");
    if (!dsensors.getAddress(outsideThermometer, 0)) printf("Unable to find address for Device 0\n"); 
    // show the addresses we found on the bus
    printf("Device 0 Address: ");
    printAddress(outsideThermometer);
    printf("\n");
    // set the resolution to 9 bit
    dsensors.setResolution(outsideThermometer, 9);
    printf("Device 0 Resolution: ");
    printf("%d",dsensors.getResolution(outsideThermometer)); 
    printf("\n");
    
    // htu21d sensor
    if (!htu.begin()) {
      printf("Couldn't find sensor!");
    }
    // init coap resourcen
    rest_init_engine ();
    #pragma GCC diagnostic ignored "-Wwrite-strings"
    rest_activate_resource (&res_htu21dtemp, "s/temp");
    rest_activate_resource (&res_htu21dhum, "s/hum");
    rest_activate_resource (&res_dtemp1, "s/tempd");
    rest_activate_resource (&res_battery, "s/battery");
    #pragma GCC diagnostic pop
}

// at project-conf.h
// LOOP_INTERVAL		(20 * CLOCK_SECOND)
void loop (void)
{  
      dsensors.requestTemperatures();
      // print the device information
      printData(outsideThermometer,0);
      
      htu21d_temp = htu.readTemperature();
      htu21d_hum = htu.readHumidity();
     
      dtostrf(htu21d_temp , 0, 2, htu21d_temp_s );   
      dtostrf(htu21d_hum , 0, 2, htu21d_hum_s );
      
//  debug only   
      printf("Temp: %s",htu21d_temp_s);
      printf("\t\tHum: %s\n",htu21d_hum_s);
}
