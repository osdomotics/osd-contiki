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
DeviceAddress insideThermometer0, insideThermometer1, insideThermometer2, insideThermometer3, insideThermometer4, insideThermometer5, insideThermometer6, insideThermometer7;

extern resource_t res_dtemp1, res_dtemp2, res_dtemp3, res_dtemp4, res_dtemp5, res_dtemp6, res_dtemp7, res_dtemp8, res_battery;
float d_temp;
char  d_temp_s[8];

// sketch.h
struct dstemp ds1820[8];

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
  if (!dsensors.getAddress(insideThermometer0, 0)) printf("Unable to find address for Device 0\n"); 
  if (!dsensors.getAddress(insideThermometer1, 1)) printf("Unable to find address for Device 1\n"); 
  if (!dsensors.getAddress(insideThermometer2, 2)) printf("Unable to find address for Device 2\n"); 
  if (!dsensors.getAddress(insideThermometer3, 3)) printf("Unable to find address for Device 3\n"); 
  if (!dsensors.getAddress(insideThermometer4, 4)) printf("Unable to find address for Device 4\n"); 
  if (!dsensors.getAddress(insideThermometer5, 5)) printf("Unable to find address for Device 5\n"); 
  if (!dsensors.getAddress(insideThermometer6, 6)) printf("Unable to find address for Device 6\n"); 
  if (!dsensors.getAddress(insideThermometer7, 7)) printf("Unable to find address for Device 7\n"); 

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
  printAddress(insideThermometer0);
  printf("\n");

  printf("Device 1 Address: ");
  printAddress(insideThermometer1);
  printf("\n");

  printf("Device 2 Address: ");
  printAddress(insideThermometer2);
  printf("\n");

  printf("Device 3 Address: ");
  printAddress(insideThermometer3);
  printf("\n");

  printf("Device 4 Address: ");
  printAddress(insideThermometer4);
  printf("\n");

  printf("Device 5 Address: ");
  printAddress(insideThermometer5);
  printf("\n");

  printf("Device 6 Address: ");
  printAddress(insideThermometer6);
  printf("\n");

  printf("Device 7 Address: ");
  printAddress(insideThermometer7);
  printf("\n");

  // set the resolution to 9 bit
  dsensors.setResolution(insideThermometer0, 9);
  dsensors.setResolution(insideThermometer1, 9);
  dsensors.setResolution(insideThermometer2, 9);
  dsensors.setResolution(insideThermometer3, 9);
  dsensors.setResolution(insideThermometer4, 9);
  dsensors.setResolution(insideThermometer5, 9);
  dsensors.setResolution(insideThermometer6, 9);
  dsensors.setResolution(insideThermometer7, 9);

  printf("Device 0 Resolution: ");
  printf("%d",dsensors.getResolution(insideThermometer0)); 
  printf("\n");

  printf("Device 1 Resolution: ");
  printf("%d",dsensors.getResolution(insideThermometer1)); 
  printf("\n");

  printf("Device 2 Resolution: ");
  printf("%d",dsensors.getResolution(insideThermometer2)); 
  printf("\n");

  printf("Device 3 Resolution: ");
  printf("%d",dsensors.getResolution(insideThermometer3)); 
  printf("\n");

  printf("Device 4 Resolution: ");
  printf("%d",dsensors.getResolution(insideThermometer4)); 
  printf("\n");

  printf("Device 5 Resolution: ");
  printf("%d",dsensors.getResolution(insideThermometer5)); 
  printf("\n");

  printf("Device 6 Resolution: ");
  printf("%d",dsensors.getResolution(insideThermometer6)); 
  printf("\n");

  printf("Device 7 Resolution: ");
  printf("%d",dsensors.getResolution(insideThermometer7)); 
  printf("\n");

    // init coap resourcen
    rest_init_engine ();
    #pragma GCC diagnostic ignored "-Wwrite-strings"
    rest_activate_resource (&res_dtemp1, "s/t1/temp");
    rest_activate_resource (&res_dtemp2, "s/t2/temp");
    rest_activate_resource (&res_dtemp3, "s/t3/temp");
    rest_activate_resource (&res_dtemp4, "s/t4/temp");
    rest_activate_resource (&res_dtemp5, "s/t5/temp");
    rest_activate_resource (&res_dtemp6, "s/t6/temp");
    rest_activate_resource (&res_dtemp7, "s/t7/temp");
    rest_activate_resource (&res_dtemp8, "s/t8/temp");
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
      printData(insideThermometer0,0);
      printData(insideThermometer1,1);
      printData(insideThermometer2,2);
      printData(insideThermometer3,3);
      printData(insideThermometer4,4);
      printData(insideThermometer5,5);
      printData(insideThermometer6,6);
      printData(insideThermometer7,7);
      mcu_sleep_on();
   
//  debug only 
}
