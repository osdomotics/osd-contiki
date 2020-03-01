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
#include "rest-engine.h"
#include "net/netstack.h"
#include "ota-update.h"

extern resource_t res_led, res_battery, res_cputemp, res_linkstats,
                  res_charger_state, res_charger_v_bat, res_charger_v_bat_max,
                  res_charger_v_sol;

uint8_t led_pin = 4;
uint8_t led_status;

uint8_t t_pin = 3;
uint8_t vbat_pin = A0;
uint8_t vsol_pin = A1;
uint8_t count = 0;
uint8_t finished_count = 0;
uint8_t state;

uint16_t v_bat = 0;
uint16_t v_sol = 0;
uint16_t v_bat_max = 0;

#define S_WAIT 0
#define S_LOAD 1
#define S_MEAS 2
#define S_WAIT_BAT_LOW 3


/* how much to scale the ADC readings */
#define V_SCALE 3

/* when to start charging the battery (mV) */
#define BAT_LOW_LEVEL 2700

/* Vsolar must be SOLAR_ABOVE_BATT mV above Vbatt to start charging */
#define SOLAR_ABOVE_BATT 300

/* how many loop() invocations to charge before measure */
#define CHARGE_COUNT 9

/* mV Voltage must drop after reaching max to stop charging 5-10mV for NiMh */
#define MIN_DELTA_PEAK 8

/* how many charge/measure cycles (CHARGE_COUNT + 1) must the voltage be
   below (v_bat_max - MIN_DELTA_PEAK)
*/
#define FINISHED_COUNT 6

}

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
    rest_activate_resource (&res_linkstats, "s/min_rssi");
    rest_activate_resource (&res_charger_v_bat, "s/c/v_bat");
    rest_activate_resource (&res_charger_v_bat_max, "s/c/v_bat_max");
    rest_activate_resource (&res_charger_v_sol, "s/c/v_sol");
    rest_activate_resource (&res_charger_state, "s/c/state");
    OTA_ACTIVATE_RESOURCES();
    #pragma GCC diagnostic pop

    state = S_WAIT;

    // uncomment if you want this node to be a routing node and set the
    // correct options in project-conf.h and contiki-conf.h
    NETSTACK_MAC.off(1);

    // uncoment if you want to set the loop interval at runtime
    //#define LOOP_INTERVAL_AFTER_INIT (60 * CLOCK_SECOND)
    //loop_periodic_set (LOOP_INTERVAL_AFTER_INIT);

    //mcu_sleep_set(128);
    //Serial.begin (115200); // baud rate ignored by contiki
}

void t_on (void) {
  pinMode(t_pin, OUTPUT);
  digitalWrite(t_pin, LOW);
}

void t_off (void) {
  digitalWrite(t_pin, HIGH);
  pinMode(t_pin, INPUT);
}

void charger (void)
{
  v_bat = analogRead(vbat_pin) * 1600L / 1024L * V_SCALE;
  v_sol = analogRead(vsol_pin) * 1600L / 1024L * V_SCALE;

  printf ("v_bat, v_sol = %d, %d, state = ", v_bat, v_sol);
  switch (state) {
    case S_WAIT:
      printf ("S_WAIT");
      break;
    case S_LOAD:
      printf ("S_LOAD");
      break;
    case S_MEAS:
      printf ("S_MEAS");
      break;
    case S_WAIT_BAT_LOW:
      printf ("S_WAIT_BAT_LOW");
      break;
  }
  printf ("\n");

  if (state == S_WAIT) {
    if (v_sol > (v_bat + SOLAR_ABOVE_BATT)) {
      printf ("v_sol > v_bat + SOLAR_ABOVE_BATT -> start loading\n");
      v_bat_max = 0;
      state = S_LOAD;
      count = 0;
      t_on();
    }
  }

  else if (state == S_LOAD) {
    if (count == CHARGE_COUNT) {
      printf ("charge interval passed, turn off and measure\n");
      t_off();
      state = S_MEAS;
      count = 0;
    }
  }

  else if (state == S_MEAS) {
    if (v_sol < (v_bat + SOLAR_ABOVE_BATT)) {
      printf ("v_sol too low !!!\n");
      state = S_WAIT;
    }
    else {
      if (v_bat >= v_bat_max) {
        printf ("v_bat >= max (%d), continue loading\n", v_bat_max);
        v_bat_max = v_bat;
        state = S_LOAD;
        t_on();
        count = 0;
        finished_count = 0;
      }
      else if (v_bat < (v_bat_max - MIN_DELTA_PEAK)) {
        printf ("vbat < (v_bat_max (%d)- MIN_DELTA_PEAK (%d)) %d, wait if this "
                "holds for %d charge rounds\n"
                , v_bat_max, MIN_DELTA_PEAK, v_bat_max - MIN_DELTA_PEAK
                , FINISHED_COUNT);
        finished_count ++;
        if (finished_count == FINISHED_COUNT) {
          printf ("finished loading\n");
          state = S_WAIT_BAT_LOW;
          count = 0;
          finished_count = 0;
        }
        else {
          state = S_LOAD;
          t_on();
          count = 0;
        }
      }
      else {
        printf ("looks like v_bat is somewhere between "
                "(v_bat_max - MIN_DELTA_PEAK) (%d) and v_bat_max (%d), "
                "continue loading\n", v_bat_max - MIN_DELTA_PEAK, v_bat_max);
        state = S_LOAD;
        t_on();
        count = 0;
      }
    }
  }

  else if (state == S_WAIT_BAT_LOW) {
    if (v_bat < BAT_LOW_LEVEL) {
      printf ("battery below BAT_LOW_LEVEL after %d seconds\n", count);
      state = S_WAIT;
      count = 0;
    }
  }

  count ++;
}

void loop (void) {
  charger ();
}
