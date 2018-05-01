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

#include "RGBdriver.h"

#define CLK 3//pins definitions for the driver        
#define DIO 14
RGBdriver Driver(CLK,DIO);

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "contiki.h"
#include "contiki-net.h"
#include "rest-engine.h"
#include "generic_resource.h"
#include "net/netstack.h"

extern volatile uint8_t mcusleepcycle;  // default 16

#define LED_PIN 4
#define WATER_PIN 20
}

uint8_t color_rgb [3] = {0, 0, 0};
uint8_t water_switch = 0;

static uint8_t name_to_offset (const char * name)
{
  uint8_t offset = 0;
  if (0 == strcmp (name, "green")) {
    offset = 1;
  } else if (0 == strcmp (name, "blue")) {
    offset = 2;
  }
  return offset;
}

static size_t
color_to_string
    ( const char *name
    , const char *uri
    , const char *query
    , char *buf
    , size_t bsize
    )
{
  return snprintf (buf, bsize, "%d", color_rgb [name_to_offset (name)]);
}

int color_from_string
    (const char *name, const char *uri, const char *query, const char *s)
{
    color_rgb [name_to_offset (name)] = atoi (s);
    Driver.begin();
    Driver.SetColor(color_rgb [0], color_rgb [1], color_rgb [2]);
    Driver.end();
    return 0;
}

GENERIC_RESOURCE
  ( red
  , RED_LED
  , s
  , 1
  , color_from_string
  , color_to_string
  );

GENERIC_RESOURCE
  ( green
  , GREEN_LED
  , s
  , 1
  , color_from_string
  , color_to_string
  );

GENERIC_RESOURCE
  ( blue
  , BLUE_LED
  , s
  , 1
  , color_from_string
  , color_to_string
  );

static size_t
water_switch_to_string
    ( const char *name
    , const char *uri
    , const char *query
    , char *buf
    , size_t bsize
    )
{
  return snprintf (buf, bsize, "%d", water_switch);
}

GENERIC_RESOURCE
  ( water
  , WATER_SWITCH
  , s
  , 1
  , NULL
  , water_switch_to_string
  );

void setup (void)
{
    // switch off the led
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
    // water switch sensor
    pinMode(WATER_PIN, INPUT);
    digitalWrite(WATER_PIN, HIGH);       // turn on pullup resistors
    // init coap resourcen
    rest_init_engine ();
    
    NETSTACK_MAC.off(1);

    rest_activate_resource (&res_red,   (char *)"led/R");
    rest_activate_resource (&res_green, (char *)"led/G");
    rest_activate_resource (&res_blue,  (char *)"led/B");
    rest_activate_resource (&res_water, (char *)"water");

    Driver.begin();
    Driver.SetColor(color_rgb [0], color_rgb [1], color_rgb [2]);
    Driver.end();
}

void loop (void)
{
    // water sensor test
      water_switch = digitalRead(WATER_PIN);
      printf("water level: %d\n", water_switch);
    // Test 

	static int a=1;
	
	switch(a) {
	case 1: printf("red\n");
      Driver.begin();
      Driver.SetColor(255, 0, 0);
      Driver.end();
      a++;
	 break;
	case 2: printf("green\n");
      Driver.begin();
      Driver.SetColor(0, 255, 0);
      Driver.end();
      a++;
	 break;
	case 3: printf("blue\n");
      Driver.begin();
      Driver.SetColor(0, 0, 255);
      Driver.end();
      a=1;
	 break;
	default: printf("a ist irgendwas\n"); break;
	}

}
