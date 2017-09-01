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
#include "lib/settings.h"
#include "shell.h"
#include "serial-shell.h"

extern resource_t res_led, res_battery, res_cputemp;

uint8_t led_pin=4;
uint8_t led_status;
}

settings_status_t status;
uint16_t panid;
char hostname[30];
uint16_t channel;


void setup (void)
{
	settings_iter_t iter;
	int i;
    // switch off the led
    pinMode(led_pin, OUTPUT);
    digitalWrite(led_pin, HIGH);
    led_status=0;
    // settings manager
    /* Basic setting of parameters */
    status = settings_set_uint16(SETTINGS_KEY_PAN_ID, 0xABCD);
    if(SETTINGS_STATUS_OK != status) {
      printf("settings-example: `set` failed: %d\n", status);
    }
    status = settings_set_uint8(SETTINGS_KEY_CHANNEL, 11);
    if(SETTINGS_STATUS_OK != status) {
      printf("settings-example: `set` failed: %d\n", status);
    }
    status = settings_set_cstr(SETTINGS_KEY_HOSTNAME, "contiki.local");
    if(SETTINGS_STATUS_OK != status) {
      printf("settings-example: `set` failed: %d\n", status);
    }
    /* Basic getting of parameters */
    panid = settings_get_uint16(SETTINGS_KEY_PAN_ID, 0);
    if(0xABCD != panid) {
      printf("settings-example: `get` failed: value mismatch.\n");
    }
    channel = settings_get_uint16(SETTINGS_KEY_CHANNEL, 0);
    if(26 != channel) {
      printf("settings-example: `get` failed: value mismatch.\n");
    }

    if(!settings_get_cstr(SETTINGS_KEY_HOSTNAME, 0, hostname, sizeof(hostname))) {
      printf("settings-example: `get` failed: settings_get_cstr returned NULL\n");
    } else if(strcmp(hostname, "contiki.local") != 0) {
      printf("settings-example: `get` failed: value mismatch.\n");
    }
      /*************************************************************************/
  /* Adding multiple values with the same key */

  for(i = 0; i < 10; i++) {
    settings_add_uint8(TCC('e','x'), i + 20);
  }

  /*************************************************************************/
  /* Reading multiple values with the same key */

  for(i = 0; i < 10; i++) {
    if(settings_get_uint8(TCC('e', 'x'), i) != i + 20) {
      printf("settings-example: `get` failed: value mismatch.\n");
    }
  }

  /*************************************************************************/
  /* Iterating thru all settings */

  for(iter = settings_iter_begin(); iter; iter = settings_iter_next(iter)) {
    settings_length_t len = settings_iter_get_value_length(iter);
    eeprom_addr_t addr = settings_iter_get_value_addr(iter);
    uint8_t byte;

    union {
      settings_key_t key;
      char bytes[0];
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
    // Seriell Shell
    serial_shell_init();
    shell_blink_init();
    shell_ps_init();
    shell_reboot_init();
    shell_text_init();
    shell_time_init();
  
#if COFFEE
  shell_coffee_init();
  shell_file_init();
#endif
  

    // init coap resourcen
    rest_init_engine ();
    #pragma GCC diagnostic ignored "-Wwrite-strings"
    rest_activate_resource (&res_led, "s/led");
    rest_activate_resource (&res_battery, "s/battery");
    rest_activate_resource (&res_cputemp, "s/cputemp");
    #pragma GCC diagnostic pop
    
 //   NETSTACK_MAC.off(1);
 //   mcu_sleep_set(128);
}

void loop (void)
{


}
