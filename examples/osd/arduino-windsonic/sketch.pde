/*
 * Sample arduino sketch using contiki features.
 * Unfortunately sleeping for long times in loop() isn't currently
 * possible, something turns off the CPU (including PWM outputs) if a
 * Proto-Thread is taking too long. We need to find out how to sleep in
 * a Contiki-compatible way.
 * Note that for a normal arduino sketch you won't have to include any
 * of the contiki-specific files here, the sketch should just work.
 */


extern "C" {
#include <stdio.h>
#include "arduino-process.h"
#include "rest-engine.h"
#include "ota-update.h"
#include "net/netstack.h"

#define DEBUG 0

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

extern resource_t
  res_wind,
  res_wind_speed_max,
  res_wind_status,
  res_wind_speed,
  res_wind_direction,
  res_power_supply;

#define STX 0x02
#define ETX 0x03
#define STRING_TERMINATOR 0x00

#define LED_PIN 4

}

int   state             = 0;
char  ws_status         [32];
int   ws_speed_hi       = 0;
int   ws_speed_lo       = 0;
char  ws_unit           [8];
int   ws_direction      = 0;
int   supply_voltage_hi = 0;
int   supply_voltage_lo = 0;
int   ws_max_speed      = 0;
int   ws_max_speed_lo   = 0;
int   ws_max_speed_hi   = 0;

int wind_speeds [WIND_SPEEDS]; // every 2 seconds we get a value
int wind_speed_idx = 0;

uint32_t idx;

int  d, count, msg_length, state0_counter = 0;
unsigned char msg_buffer [128], checksum_buffer [3], checksum;

void set_no_data (void)
{
  PRINTF ("set no data\n");
  strcpy (ws_status, "No data from sensor");
  ws_speed_lo     = 0;
  ws_speed_hi     = 0;
  ws_direction    = 0;
  ws_max_speed    = 0;
  ws_max_speed_hi = 0;
  ws_max_speed_lo = 0;
  wind_speed_idx  = 0;
  strcpy (ws_unit, "");

  for (idx = 0; idx < WIND_SPEEDS; idx++)
    wind_speeds [idx] = 0;

}

void setup (void)
{
    // switch off the led
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);

    state = 0;

    // init coap resourcen
    rest_init_engine ();

#pragma GCC diagnostic ignored "-Wwrite-strings"
    rest_activate_resource (&res_wind,           "s/wind");
    rest_activate_resource (&res_wind_speed,     "s/wind_speed");
    rest_activate_resource (&res_wind_speed_max, "s/wind_speed_max");
    rest_activate_resource (&res_wind_direction, "s/wind_direction");
    rest_activate_resource (&res_wind_status,    "s/wind_status");
    rest_activate_resource (&res_power_supply,   "s/power_supply");
    OTA_ACTIVATE_RESOURCES();
#pragma GCC diagnostic pop

    NETSTACK_MAC.off(1);
    mcu_sleep_set(0);
    Serial1.begin (9600);

    set_no_data ();
}

void loop (void)
{
  switch (state) {
    case 0 :
      PRINTF ("state0\n");
      strcpy (ws_status, "0");
      state0_counter ++;
      if (state0_counter > 10) {
        set_no_data ();
        state0_counter = 0;
      }
      // wait for stx
      while (Serial1.available () > 0) {
        d = Serial1.read ();
        PRINTF ("%d ", d);
        if (d == STX) {
          state = 1;
          count = 0;
          break;
        }
      }
      if (state == 0)
        break;

    case 1 :
      PRINTF ("state1\n");
      strcpy (ws_status, "1");
      state0_counter = 0;

      // wait for etx and read message
      while (Serial1.available () > 0) {
        d = Serial1.read ();
        PRINTF ("%d ", d);
        if (d == ETX) {
          msg_length = count;
          msg_buffer [count] = STRING_TERMINATOR;
          count = 0;
          checksum = 0;
          state = 2;
          break;
        } else {
          msg_buffer [count] = (unsigned char) d;
          count ++;
          if (count > 127) {
            state = 0;
            break;
          }
        }
      }
      if (state == 1 || state == 0)
        break;

    case 2 :
      PRINTF ("state2\n");
      strcpy (ws_status, "2");
      // read checksum
      while (Serial1.available () > 0) {
        d = Serial1.read ();
        PRINTF ("%d ", d);
        PRINTF (" %d (%d)\n", d, count);
        checksum_buffer [count] = (unsigned char) (d & 0xff);
        count ++;
        if (count == 2) {
          checksum_buffer [count] = STRING_TERMINATOR;
          state = 3;
          break;
        }
      }
      if (state == 2)
        break;

    case 3 :
      unsigned int checksum_rx;
      int cnt;

      PRINTF ("state3\n");
      strcpy (ws_status, "3");
      // calc and compare checksum
      checksum = 0;
      sscanf ((const char*)checksum_buffer, "%2X", &checksum_rx);
      for (cnt = 0; cnt < msg_length; cnt++) {
        checksum ^= msg_buffer [cnt];
      }

      if (checksum_rx != checksum) {
        PRINTF ("checksum mismatch %02x != %02x\n", checksum, checksum_rx);
        sprintf (ws_status, "CS: %02x!=%02x,%s", checksum, checksum_rx,msg_buffer);
        state = 0;
        break;
      } else {
        state = 4;
      }

    case 4 :
      // parse message Q,079,000.08,M,00,
      char unit [1];
      int status;

      PRINTF ("state4\n");
      strcpy (ws_status, "4");
      PRINTF ("parse messge: %s\n", msg_buffer);
      sscanf
        ( (const char*) msg_buffer
        , "Q,%3d,%3d.%2d,%c,%2d"
        , &ws_direction
        , &ws_speed_hi
        , &ws_speed_lo
        , unit
        , &status
        );

      switch (unit[0]) {
        case 'M' :
          strcpy (ws_unit, "m/s");
          break;

        case 'N' :
          strcpy (ws_unit, "knots");
          break;

        case 'P' :
          strcpy (ws_unit, "mph");
          break;

        case 'K' :
          strcpy (ws_unit, "km/h");
          break;

        case 'F' :
          strcpy (ws_unit, "fpm");
          break;
      }

      switch (status) {
        case 0x00 :
          strcpy (ws_status, "OK");
          break;

        case 0x01 :
          strcpy (ws_status, "Axis 1 failed");
          break;

        case 0x02 :
          strcpy (ws_status, "Axis 2 failed");
          break;

        case 0x03 :
          strcpy (ws_status, "Axis 1+2 failed");
          break;

        case 0x08 :
          strcpy (ws_status, "NVM error");
          break;

        case 0x09 :
          strcpy (ws_status, "ROM error");
          break;
      }

      wind_speeds [wind_speed_idx] = ws_speed_hi * 100 + ws_speed_lo;
      wind_speed_idx ++;
      if (wind_speed_idx > WIND_SPEEDS)
        wind_speed_idx = 0;

      ws_max_speed = 0;
      for (idx = 0; idx < WIND_SPEEDS; idx++) {
        if (wind_speeds [idx] > ws_max_speed)
          ws_max_speed = wind_speeds [idx];
      }

      ws_max_speed_lo = ws_max_speed % 100;
      ws_max_speed_hi = ws_max_speed / 100;

      PRINTF ("speed: %d.%02d %s, dir: %d, status: %s, max: %d.%02d, Vs: %d.%02d\n"
              , ws_speed_hi, ws_speed_lo, ws_unit, ws_direction, ws_status
              , ws_max_speed_hi, ws_max_speed_lo, supply_voltage_hi
	      , supply_voltage_lo
	      );

      state = 0;
      break;
  } /* end switch (state) */

  idx = analogRead (A1);
  idx *= 1000;
  idx /= 572;
  supply_voltage_lo = idx % 100;
  supply_voltage_hi = idx / 100;
} /* end loop */
