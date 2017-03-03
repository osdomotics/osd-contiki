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

#include <OneWire.h>
#include "DallasTemperature.h"

extern "C" {

#include "arduino-process.h"
#include "rest-engine.h"
#include "sketch.h"

extern volatile uint8_t mcusleepcycle;  // default 16

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 3
#define TEMPERATURE_PRECISION 9

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature dsensors(&oneWire);

// arrays to hold device addresses
DeviceAddress insideThermometer, outsideThermometer;

extern resource_t res_dtemp1, res_dtemp2, res_battery;
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

  // assign address manually.  the addresses below will beed to be changed
  // to valid device addresses on your bus.  device address can be retrieved
  // by using either oneWire.search(deviceAddress) or individually via
  // dsensors.getAddress(deviceAddress, index)
  //insideThermometer = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };
  //outsideThermometer   = { 0x28, 0x3F, 0x1C, 0x31, 0x2, 0x0, 0x0, 0x2 };

  // search for devices on the bus and assign based on an index.  ideally,
  // you would do this to initially discover addresses on the bus and then 
  // use those addresses and manually assign them (see above) once you know 
  // the devices on your bus (and assuming they don't change).
  // 
  // method 1: by index
  if (!dsensors.getAddress(insideThermometer, 0)) printf("Unable to find address for Device 0\n"); 
  if (!dsensors.getAddress(outsideThermometer, 1)) printf("Unable to find address for Device 1\n"); 

  // method 2: search()
  // search() looks for the next device. Returns 1 if a new address has been
  // returned. A zero might mean that the bus is shorted, there are no devices, 
  // or you have already retrieved all of them.  It might be a good idea to 
  // check the CRC to make sure you didn't get garbage.  The order is 
  // deterministic. You will always get the same devices in the same order
  //
  // Must be called before search()
  //oneWire.reset_search();
  // assigns the first address found to insideThermometer
  //if (!oneWire.search(insideThermometer)) Serial.println("Unable to find address for insideThermometer");
  // assigns the seconds address found to outsideThermometer
  //if (!oneWire.search(outsideThermometer)) Serial.println("Unable to find address for outsideThermometer");

  // show the addresses we found on the bus
  printf("Device 0 Address: ");
  printAddress(insideThermometer);
  printf("\n");

  printf("Device 1 Address: ");
  printAddress(outsideThermometer);
  printf("\n");

  // set the resolution to 9 bit
  dsensors.setResolution(insideThermometer, 9);
  dsensors.setResolution(outsideThermometer, 9);

  printf("Device 0 Resolution: ");
  printf("%d",dsensors.getResolution(insideThermometer)); 
  printf("\n");

  printf("Device 1 Resolution: ");
  printf("%d",dsensors.getResolution(outsideThermometer)); 
  printf("\n");
    // init coap resourcen
    rest_init_engine ();
    #pragma GCC diagnostic ignored "-Wwrite-strings"
    rest_activate_resource (&res_dtemp1, "s/t1/temp");
    rest_activate_resource (&res_dtemp2, "s/t2/temp");
    rest_activate_resource (&res_battery, "s/batter");
    #pragma GCC diagnostic pop
}

// at project-conf.h
// LOOP_INTERVAL		(10 * CLOCK_SECOND)
void loop (void)
{
      mcu_sleep_off();
      // call sensors.requestTemperatures() to issue a global temperature 
      // request to all devices on the bus
      printf("Requesting temperatures...");
      dsensors.requestTemperatures();
      printf("DONE\n");

      // print the device information
      printData(insideThermometer,0);
      printData(outsideThermometer,1);
      mcu_sleep_on();
   
//  debug only 
}
