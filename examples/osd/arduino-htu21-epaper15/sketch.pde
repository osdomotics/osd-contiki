/*
 * Sample e-paper arduino sketch using contiki features.
 * We turn the LED off
 *
 * Note that for a normal arduino sketch you won't have to include any
 * of the contiki-specific files here, the sketch should just work.
 */

extern "C" {
#include "arduino-process.h"
#include "rest-engine.h"
#include "net/netstack.h"
#include <SPI.h>
#include <epd1in54.h>
#include <Wire.h>
#include "Adafruit_HTU21DF.h"
#include "jsonparse.h"
#include "dev/batmon.h"
#include "ota-update.h"

extern resource_t res_led, res_battery, res_cputemp;

uint8_t led_pin=4;
uint8_t led_status;

void __cxa_pure_virtual() { while (1); }

}

#include <Adafruit_GFX.h>
#include <Fonts/arial_big.h>
#include <Fonts/arial_small.h>
#include <Fonts/arial_xsmall.h>

#define COLORED     0
#define UNCOLORED   1

Epd epd;

Adafruit_HTU21DF htu = Adafruit_HTU21DF();

GFXcanvas1 canvas = GFXcanvas1 (200, 200);

extern resource_t res_htu21dtemp, res_htu21dhum, res_battery;

char flag = 1;

// needed by the resource
char  htu21d_hum_s[8];
char  htu21d_temp_s[8];

void setup (void)
{
    // switch off the led
    pinMode(led_pin, OUTPUT);
    digitalWrite(led_pin, HIGH);
    led_status=0;
    // htu21d sensor
    if (!htu.begin()) {
      printf("Couldn't find sensor!");
    }
    // init coap resourcen
    rest_init_engine ();
    #pragma GCC diagnostic ignored "-Wwrite-strings"
    rest_activate_resource (&res_htu21dtemp, "s/temp");
    rest_activate_resource (&res_htu21dhum, "s/hum");
    rest_activate_resource (&res_led, "s/led");
    rest_activate_resource (&res_battery, "s/battery");
    rest_activate_resource (&res_cputemp, "s/cputemp");
    OTA_ACTIVATE_RESOURCES();
    #pragma GCC diagnostic pop

 //   NETSTACK_MAC.off(1);
    mcu_sleep_set(128);

    epd.Init (lut_full_update);
    epd.ClearFrameMemory(0xFF);   // bit set = white, bit reset = black
    epd.DisplayFrame();

    flag = 1;
}

void loop (void)
{
    float htu21d_hum;
    float htu21d_temp;

    uint16_t battery_voltage;
    char buf [20];
    char htu_buf [20];

    htu21d_temp = htu.readTemperature();
    htu21d_hum = htu.readHumidity();

    dtostrf(htu21d_temp , 0, 2, htu21d_temp_s );
    dtostrf(htu21d_hum , 0, 2, htu21d_hum_s );

    dtostrf(htu21d_temp , 0, 1, htu_buf );
    snprintf (buf, 12, "  LT: %s", htu_buf);

    batmon_get_voltage(&battery_voltage);

    switch (flag) {
     case 1 :
      epd.ClearFrameMemory(0xFF);   // bit set = white, bit reset = black
      epd.DisplayFrame();
       break;

     case 2 :
      epd.Init (lut_partial_update);
      canvas.fillScreen(0xffff);
      canvas.setTextColor(0x0000);

      canvas.setFont(&arialbd12pt7b);
      canvas.setCursor (14, 30);
      canvas.print (F("WOHNZIMMER"));
      //canvas.print (((float)battery_voltage)/1000, 1);

      canvas.setFont(&arialbd48pt7b);
      canvas.setCursor(10, 125);
      canvas.print (htu21d_temp, 1);

      canvas.setFont(&arialbd20pt7b);
      canvas.setCursor(50, 175);
      canvas.print (htu21d_hum, 1);
      canvas.println (F("%"));

      epd.SetFrameMemory(canvas.getBuffer(), 0, 0, 200, 200);
      epd.DisplayFrame();
      loop_periodic_set (LOOP_INTERVAL_AFTER_INIT);
      epd.Sleep();
     break;
    }

    flag++;

    if (flag == 3) flag = 2;

    printf ("\n");

}
