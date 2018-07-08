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

extern "C" {
#include "arduino-process.h"
#include "valve.h"
#include "ota-update.h"
#include "Wire.h"
//#include "U8g2lib.h"
//#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
}

extern resource_t res_battery, res_cputemp;

extern "C" void __cxa_pure_virtual() { while (1) printf ("xx\n"); }

uint8_t b1,b2,b3;
uint16_t pulses, last_pulses;
int16_t fade_out_counter;
int32_t idle_counter;

enum states state = FULLY_CLOSING;

Adafruit_SH1106 display(-1);

void setup (void)
{
    pinMode (LED_PIN,        OUTPUT);
    pinMode (OLED_ON_PIN,    OUTPUT);
    pinMode (USER_LED_PIN,   OUTPUT);
    pinMode (DIR_UP_PIN,     OUTPUT);
    pinMode (DIR_DOWN_PIN,   OUTPUT);

    pinMode (PULSE_ON_PIN,   OUTPUT);
    pinMode (BTN_COM_PIN,    OUTPUT);

    pinMode (BTN_1_PIN,      INPUT_PULLUP);
    pinMode (BTN_2_PIN,      INPUT_PULLUP);
    pinMode (BTN_3_PIN,      INPUT_PULLUP);

    digitalWrite (LED_PIN,       HIGH); // off
    digitalWrite (OLED_ON_PIN,    LOW); // off

    digitalWrite (DIR_UP_PIN,     LOW); // off
    digitalWrite (DIR_DOWN_PIN,   LOW); // off

    digitalWrite (BTN_COM_PIN,    LOW); // on
    digitalWrite (PULSE_ON_PIN,  HIGH); // on

    SENSORS_ACTIVATE(button_sensor);

    // init coap resourcen
    rest_init_engine ();

    #pragma GCC diagnostic ignored "-Wwrite-strings"
    OTA_ACTIVATE_RESOURCES();
    rest_activate_resource (&res_battery,   "s/battery");
    rest_activate_resource (&res_cputemp,   "s/cputemp");
    rest_activate_resource (&res_pulses,    "s/pulses");
    rest_activate_resource (&res_direction, "a/direction");
    rest_activate_resource (&res_command,   "a/command");
    #pragma GCC diagnostic pop
    NETSTACK_MAC.off(1);
}

void print_stats (int8_t dir) {
    digitalWrite (OLED_ON_PIN, HIGH);
    display.begin(SH1106_EXTERNALVCC, 0x3c, false);
    display.clearDisplay ();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.print("pulses : ");
    display.println (button_sensor.value (0));
    if (dir == -1) {
        display.println ("dir: open");
    } else if (dir == 1) {
        display.println ("dir: close");
    }
    display.display();
}

void valve (uint8_t direction) {
  if (direction == CLOSE) {
    // close
    digitalWrite (DIR_UP_PIN,     HIGH); // on
    digitalWrite (DIR_DOWN_PIN,   LOW); // off
    digitalWrite (PULSE_ON_PIN,   HIGH); // on
  } else if (direction == OPEN) {
    // open
    digitalWrite (DIR_UP_PIN,     LOW); // off
    digitalWrite (DIR_DOWN_PIN,   HIGH); // on
    digitalWrite (PULSE_ON_PIN,   HIGH); // on
  } else if (direction == STOP){
    // stop
    digitalWrite (DIR_UP_PIN,     LOW); // off
    digitalWrite (DIR_DOWN_PIN,   LOW); // off
    digitalWrite (PULSE_ON_PIN,   LOW); // off
  }
}

void loop (void)
{
    b1 = digitalRead (BTN_1_PIN);
    b2 = digitalRead (BTN_2_PIN);
    b3 = digitalRead (BTN_3_PIN);
    pulses = button_sensor.value (0);

    printf ("%d, %d, %d, %d, %d, %d, %d\n"
           , b1,b2,b3,state,pulses,fade_out_counter,(int16_t)idle_counter
           );

    if (b1 == 0 || b2 == 0 || b3 == 0) {
      state = MANUAL;
    }

    switch (state) {
      case MANUAL :
      loop_periodic_set (LOOP_INTERVAL);
      if (b1 == 0 && b3 == 1) {
        valve (CLOSE);
        print_stats (1);
      } else if (b1 == 1 && b3 == 0) {
        valve (OPEN);
        print_stats (-1);
      } else {
        valve (STOP);
        print_stats (0);
        fade_out_counter =
          (DISPLAY_FADE_OUT_SECONDS*CLOCK_SECOND) / LOOP_INTERVAL;
        state = MANUAL_FADE_OUT;
      }
      break;

      case MANUAL_FADE_OUT :
      //print_stats (0);
      if (--fade_out_counter <= 0) {
        state = IDLE;
        digitalWrite (OLED_ON_PIN, LOW);
      }
      break;

      case IDLE :
      idle_counter = WAIT_TO_CLOSE_SECONDS;
      idle_counter = idle_counter * CLOCK_SECOND;
      idle_counter = idle_counter / LOOP_INTERVAL_SLOW;
      loop_periodic_set (LOOP_INTERVAL_SLOW);
      state = WAIT_TO_CLOSE;
      break;

      case WAIT_TO_CLOSE :
      if (--idle_counter <= 0 ) {
        state = FULLY_CLOSING;
        loop_periodic_set (LOOP_INTERVAL);
      }
      break;

      case FULLY_CLOSING :
      case FULLY_OPENING :
      mcu_sleep_off();
      loop_periodic_set (LOOP_INTERVAL);
      last_pulses = pulses;
      if (state == FULLY_CLOSING)
        valve (CLOSE);
      else
        valve (OPEN);
      state = WAIT_END;
      break;

      case WAIT_END :
      if (pulses == last_pulses) {
        valve (STOP);
        //button_sensor.configure(4711,0);
        mcu_sleep_on();
        state = IDLE;
      }
      last_pulses = pulses;
    }
}
