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
#include "serial-shell.h"
#include "shell-merkur.h"

#include "lib/settings.h"

extern resource_t res_led, res_battery, res_cputemp;

uint8_t led_pin=4;
uint8_t led_status;
}

void setup (void)
{
    // switch off the led
    pinMode(led_pin, OUTPUT);
    digitalWrite(led_pin, HIGH);
    led_status=0;
    // Seriell Shell
    serial_shell_init();
    shell_merkur_init();  
    // init coap resourcen
    rest_init_engine ();
    #pragma GCC diagnostic ignored "-Wwrite-strings"
    rest_activate_resource (&res_led, "s/led");
    rest_activate_resource (&res_battery, "s/battery");
    rest_activate_resource (&res_cputemp, "s/cputemp");
    #pragma GCC diagnostic pop
    mcu_sleep_set(128);
}

void loop (void)
{
  settings_iter_t iter;

  /*************************************************************************/
  /* Iterating thru all settings */

  for(iter = settings_iter_begin(); iter; iter = settings_iter_next(iter)) {
    settings_length_t len = settings_iter_get_value_length(iter);
    eeprom_addr_t addr = settings_iter_get_value_addr(iter);
    uint8_t byte;

    union {
      settings_key_t key;
      char bytes[2];
    } u;

    u.key = settings_iter_get_key(iter);

    if(u.bytes[0] >= 32 && u.bytes[0] < 127
       && u.bytes[1] >= 32 && u.bytes[1] < 127
    ) {
      printf("settings-example: [%c%c] = <",u.bytes[0],u.bytes[1]);
    } else {
      printf("settings-example: <0x%04X> = <",u.key);
    }

    for(; len; len--, addr++) {
      eeprom_read(addr, &byte, 1);
      printf("%02X", byte);
      if(len != 1) {
        printf(" ");
      }
    }

    printf(">\n");
  }
  printf("settings-example: Done.\n");
}
