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
#include <epdpaint.h>
#include <Wire.h>
#include "Adafruit_HTU21DF.h"
#include "jsonparse.h"
#include "dev/batmon.h"
#include "ota-update.h"

extern resource_t res_led, res_battery, res_cputemp;

uint8_t led_pin=4;
uint8_t led_status;
}

#define COLORED     0
#define UNCOLORED   1

unsigned char image[1024];
Paint paint(image, 0, 0);    // width should be the multiple of 8
Epd epd;

Adafruit_HTU21DF htu = Adafruit_HTU21DF();

extern resource_t res_htu21dtemp, res_htu21dhum, res_battery;

char flag = 0;

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
}

void loop (void)
{
    float htu21d_hum;
    float htu21d_temp;

    uint16_t battery_voltage;
    char buf [20];
    char htu_buf [20];

    if (flag == 0) {
        epd.ClearFrameMemory(0xFF);   // bit set = white, bit reset = black
        epd.DisplayFrame();
        flag = 1;
    } else {
        epd.Init (lut_partial_update);
        paint.SetWidth(4+17*10);
        paint.SetHeight(32);
        //paint.SetRotate(ROTATE_180);

        htu21d_temp = htu.readTemperature();
        htu21d_hum = htu.readHumidity();
        dtostrf(htu21d_temp , 0, 2, htu21d_temp_s );
        dtostrf(htu21d_hum , 0, 2, htu21d_hum_s );

        dtostrf(htu21d_temp , 0, 1, htu_buf );
        snprintf (buf,12,"  LT: %s", htu_buf);
        paint.Clear(UNCOLORED);
        paint.DrawStringAt(0, 4, buf, &Font24, COLORED);
        epd.SetFrameMemory(paint.GetImage(), 0, 53, paint.GetWidth(), paint.GetHeight());

        dtostrf(htu21d_hum , 0, 1, htu_buf );
        snprintf (buf,12,"  RF: %s", htu_buf);
        paint.Clear(UNCOLORED);
        paint.DrawStringAt(0, 4, buf, &Font24, COLORED);
        epd.SetFrameMemory(paint.GetImage(), 0, 83, paint.GetWidth(), paint.GetHeight());

        batmon_get_voltage(&battery_voltage);
        dtostrf ((float)battery_voltage / (float)1000,0,1,htu_buf);
        snprintf (buf,12," BAT:  %s", htu_buf);
        paint.Clear(UNCOLORED);
        paint.DrawStringAt(0, 4, buf, &Font24, COLORED);
        epd.SetFrameMemory(paint.GetImage(), 8, 113, paint.GetWidth(), paint.GetHeight());
        epd.DisplayFrame();
        epd.Sleep();

        if (flag == 1) {
            printf ("reset timer to %d... \n", LOOP_INTERVAL_AFTER_INIT);
            loop_periodic_set (LOOP_INTERVAL_AFTER_INIT);
            flag = 2;
        }
    }
}
