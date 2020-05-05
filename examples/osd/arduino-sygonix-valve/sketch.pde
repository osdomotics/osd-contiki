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

extern resource_t res_battery, res_cputemp, res_smallest_rssi, res_nbt, res_routes;

extern "C" void __cxa_pure_virtual() { while (1) printf ("xx\n"); }

uint8_t b1,b2,b3;
int16_t fade_out_counter;
int32_t idle_counter;

enum states state = INIT_OPENING;
enum states next_state = IDLE;
enum states old_state = IDLE;

int16_t direction = 0;
int16_t total_pulses = 1;
int16_t current_position = 1;

short int interrupt = 0;
int pulseCount = 0, last_pulse_count = 0;

#define PULSE_PIN 3

extern int8_t rf230_last_rssi,rf230_smallest_rssi;

Adafruit_SH1106 display(-1);

void pulseCounter (void)
{
  pulseCount++;
}

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

    pinMode (PULSE_PIN, INPUT);
    digitalWrite(PULSE_PIN, HIGH);
    attachInterrupt(interrupt, pulseCounter, FALLING);

    // init coap resourcen
    rest_init_engine ();

    #pragma GCC diagnostic ignored "-Wwrite-strings"
    OTA_ACTIVATE_RESOURCES();
    rest_activate_resource (&res_battery,   "s/battery");
    rest_activate_resource (&res_cputemp,   "s/cputemp");
    rest_activate_resource (&res_pulses,    "s/pulses");
    rest_activate_resource (&res_valve,     "s/valve");
    rest_activate_resource (&res_status,    "s/status");
    rest_activate_resource (&res_command,   "a/command");
    rest_activate_resource (&res_smallest_rssi, "s/min_rssi");
    rest_activate_resource (&res_nbt,           "s/nbt");
    rest_activate_resource (&res_routes,        "s/routes");
    #pragma GCC diagnostic pop
    //NETSTACK_MAC.off(1);
}

void print_stats (int8_t dir) {
    digitalWrite (OLED_ON_PIN, HIGH);
    display.begin(SH1106_EXTERNALVCC, 0x3c, false);
    display.clearDisplay ();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.print("pulses : ");
    display.println (pulseCount);
    if (dir == OPEN) {
        display.println ("dir: open");
    } else if (dir == CLOSE) {
        display.println ("dir: close");
    }
    display.print("last rssi: ");
    display.println (rf230_last_rssi);
    display.print("min  rssi: ");
    display.println (rf230_smallest_rssi);

    display.display();
}

void valve (uint8_t direction_) {
  if (direction_ == CLOSE) {
    printf ("valve(CLOSE)\n");
    digitalWrite (DIR_UP_PIN,     HIGH); // on
    digitalWrite (DIR_DOWN_PIN,   LOW); // off
    digitalWrite (PULSE_ON_PIN,   HIGH); // on
    direction = -1;
  } else if (direction_ == OPEN) {
    printf ("valve(OPEN)\n");
    digitalWrite (DIR_UP_PIN,     LOW); // off
    digitalWrite (DIR_DOWN_PIN,   HIGH); // on
    digitalWrite (PULSE_ON_PIN,   HIGH); // on
    direction = 1;
  } else if (direction_ == STOP){
    printf ("valve(STOP)\n");
    digitalWrite (DIR_UP_PIN,     LOW); // off
    digitalWrite (DIR_DOWN_PIN,   LOW); // off
    digitalWrite (PULSE_ON_PIN,   LOW); // off
    direction = 0;
  }
}

void loop (void)
{
    b1 = digitalRead (BTN_1_PIN);
    b2 = digitalRead (BTN_2_PIN);
    b3 = digitalRead (BTN_3_PIN);

    printf ("b: %d, %d, %d s: ", b1,b2,b3);
    
    if (old_state == IDLE                  ) printf ("IDLE                  ");
    if (old_state == INIT_OPENING          ) printf ("INIT_OPENING          ");
    if (old_state == WAIT_INIT_FULLY_CLOSED) printf ("WAIT_INIT_FULLY_CLOSED");
    if (old_state == FULLY_CLOSING         ) printf ("FULLY_CLOSING         ");
    if (old_state == FULLY_OPENING         ) printf ("FULLY_OPENING         ");
    if (old_state == WAIT_END              ) printf ("WAIT_END              ");
    if (old_state == WAIT_TO_CLOSE         ) printf ("WAIT_TO_CLOSE         ");

    if (state == IDLE                  ) printf ("IDLE                  ");
    if (state == INIT_OPENING          ) printf ("INIT_OPENING          ");
    if (state == WAIT_INIT_FULLY_CLOSED) printf ("WAIT_INIT_FULLY_CLOSED");
    if (state == FULLY_CLOSING         ) printf ("FULLY_CLOSING         ");
    if (state == FULLY_OPENING         ) printf ("FULLY_OPENING         ");
    if (state == WAIT_END              ) printf ("WAIT_END              ");
    if (state == WAIT_TO_CLOSE         ) printf ("WAIT_TO_CLOSE         ");

    if (next_state == IDLE                  ) printf ("IDLE                  ");
    if (next_state == INIT_OPENING          ) printf ("INIT_OPENING          ");
    if (next_state == WAIT_INIT_FULLY_CLOSED) printf ("WAIT_INIT_FULLY_CLOSED");
    if (next_state == FULLY_CLOSING         ) printf ("FULLY_CLOSING         ");
    if (next_state == FULLY_OPENING         ) printf ("FULLY_OPENING         ");
    if (next_state == WAIT_END              ) printf ("WAIT_END              ");
    if (next_state == WAIT_TO_CLOSE         ) printf ("WAIT_TO_CLOSE         ");

    printf (", p: %d, %d, %d, %d, c: %d, %d rssi: %d, %d\n"
           , total_pulses, pulseCount, current_position, direction
           , fade_out_counter, (int16_t)idle_counter
           , rf230_last_rssi, rf230_smallest_rssi
           );

    if (  (state == IDLE || state == WAIT_TO_CLOSE)
       && (b2 == 1 && b3 == 0)
       ) {
      printf ("manually closing\n");
      state = FULLY_CLOSING;
      next_state = WAIT_END;
    }

    if (  (state == IDLE || state == WAIT_TO_CLOSE)
       && (b2 == 0 && b3 == 1)
       ) {
      printf ("manually opening\n");
      state = FULLY_OPENING;
      next_state = WAIT_END;
    }

    switch (state) {
      case IDLE :
        printf ("IDLE\n");
	pulseCount = 0;
        idle_counter = WAIT_TO_CLOSE_SECONDS;
        idle_counter = idle_counter * CLOCK_SECOND;
        idle_counter = idle_counter / LOOP_INTERVAL_SLOW;
        loop_periodic_set (LOOP_INTERVAL_SLOW);
        state = WAIT_TO_CLOSE;
        digitalWrite (OLED_ON_PIN, LOW);
        mcu_sleep_enable();
      break;

      case WAIT_TO_CLOSE :
        printf ("WAIT_TO_CLOSE\n");
        if (--idle_counter <= 0 ) {
          state = FULLY_CLOSING;
          next_state = WAIT_END;
        }
        digitalWrite (OLED_ON_PIN, LOW);
      break;

      case FULLY_CLOSING :
      case FULLY_OPENING :
        printf ("FULLY_CLOSING or FULLY_OPENING\n");
        old_state = state;
        mcu_sleep_disable();
        loop_periodic_set (LOOP_INTERVAL);
        if (state == FULLY_CLOSING) {
          print_stats (CLOSE);
          valve (CLOSE);
        } else {
          print_stats (OPEN);
          valve (OPEN);
        }
        state = next_state;
      break;

      case INIT_OPENING :
        printf ("INIT_OPENING\n");
        mcu_sleep_disable();
        loop_periodic_set (LOOP_INTERVAL);
        valve (OPEN);
        old_state = state;
        state = WAIT_END;
      break;

      case WAIT_END :
      case WAIT_INIT_FULLY_CLOSED:
        printf ("WAIT_END or WAIT_INIT_FULLY_CLOSED\n");
        if (pulseCount == last_pulse_count) {
	  printf ("stop\n");
          valve (STOP);
          if (old_state == INIT_OPENING) {
            printf ("1\n");
            print_stats (OPEN);
	    pulseCount = 0;
            last_pulse_count = pulseCount;
            state = FULLY_CLOSING;
            next_state = WAIT_INIT_FULLY_CLOSED;
            break;
          }
          else if (state == WAIT_INIT_FULLY_CLOSED) {
            printf ("2\n");
            print_stats (CLOSE);
            total_pulses = pulseCount;
            pulseCount = 0;
            last_pulse_count = pulseCount;
            current_position = 0;
            state = IDLE;
	    next_state = IDLE;
            break;
          } else {
            printf ("3\n");
            if (old_state == FULLY_CLOSING) {
              pulseCount = 0;
              current_position = 0;
            }
            if (old_state == FULLY_OPENING) {
              pulseCount = 0;
              current_position = 100;
            }
            last_pulse_count = pulseCount;
            state = IDLE;
	    next_state = IDLE;
            break;
          }
        }
        if (old_state == FULLY_CLOSING) {
          print_stats (CLOSE);
        }
        if (old_state == FULLY_OPENING) {
          print_stats (OPEN);
        }
        last_pulse_count = pulseCount;
    } // switch state
}
