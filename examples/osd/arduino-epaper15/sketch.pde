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
#include "imagedata.h"

extern resource_t res_led, res_battery, res_cputemp;

uint8_t led_pin=4;
uint8_t led_status;
}

#define COLORED     0
#define UNCOLORED   1

unsigned char image[1024];
Paint paint(image, 0, 0);    // width should be the multiple of 8 
Epd epd;

long second=-3;

void setup (void)
{
    // switch off the led
    pinMode(led_pin, OUTPUT);
    digitalWrite(led_pin, HIGH);
    led_status=0;

    // init coap resourcen
    rest_init_engine ();
    #pragma GCC diagnostic ignored "-Wwrite-strings"
    rest_activate_resource (&res_led, "s/led");
    rest_activate_resource (&res_battery, "s/battery");
    rest_activate_resource (&res_cputemp, "s/cputemp");
    #pragma GCC diagnostic pop
    
 //   NETSTACK_MAC.off(1);
 //    mcu_sleep_set(128);
 
    printf("e-Paper init");
    // e-paper init
    if (epd.Init(lut_full_update) != 0) {
    printf(" failed");
    return;
    }

  /** 
   *  there are 2 memory areas embedded in the e-paper display
   *  and once the display is refreshed, the memory area will be auto-toggled,
   *  i.e. the next action of SetFrameMemory will set the other memory area
   *  therefore you have to clear the frame memory twice.
   */

    epd.ClearFrameMemory(0xFF);   // bit set = white, bit reset = black
    epd.DisplayFrame();
    epd.ClearFrameMemory(0xFF);   // bit set = white, bit reset = black
    epd.DisplayFrame();
    
}

void loop (void)
{
  char time_string[] = {'0', '0', ':', '0', '0', '\0'};
  
    second ++;
    
    if(second==-2){
      if (epd.Init(lut_partial_update) != 0) {
         printf("e-Paper init failed");
         return;
      }
	}
    if(second==-1){
      epd.SetFrameMemory(IMAGE_DATA);
      epd.DisplayFrame();
	}
    if(second==0){
      epd.SetFrameMemory(IMAGE_DATA);
      epd.DisplayFrame();    		
	}
    if(second > 0){
      time_string[0] = second / 60 / 10 + '0';
      time_string[1] = second / 60 % 10 + '0';
      time_string[3] = second % 60 / 10 + '0';
      time_string[4] = second % 60 % 10 + '0';

      paint.SetWidth(32);
      paint.SetHeight(96);
      paint.SetRotate(ROTATE_270);

      paint.Clear(UNCOLORED);
      paint.DrawStringAt(0, 4, time_string, &Font24, COLORED);
      epd.SetFrameMemory(paint.GetImage(), 80, 72, paint.GetWidth(), paint.GetHeight());
      epd.DisplayFrame();
    }
    printf("%d ",second);    
}
