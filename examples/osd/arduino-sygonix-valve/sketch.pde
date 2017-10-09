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
#include "Wire.h"
//#include "U8g2lib.h"
//#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

static uint8_t lc = 0;
}

//uint8_t b1,b2,b3;

extern "C" void __cxa_pure_virtual() { while (1) printf ("xx\n"); }

uint8_t display_off = 0;

Adafruit_SH1106 display(-1);

#define ON 1
#define OFF 0

void user_led (uint8_t state) {
    if (state == ON) {
        digitalWrite (USER_LED_PIN,HIGH);
    } else if (state == OFF) {
        digitalWrite (USER_LED_PIN,LOW);
    }
}

void setup (void)
{
    pinMode (LED_PIN,        OUTPUT);
    pinMode (OLED_ON_PIN,        OUTPUT);
    pinMode (USER_LED_PIN,   OUTPUT);
    pinMode (DIR_UP_PIN,     OUTPUT);
    pinMode (DIR_DOWN_PIN,   OUTPUT);

    pinMode (PULSE_ON_PIN, OUTPUT);
    pinMode (BTN_COM_PIN,    OUTPUT);

    pinMode (BTN_1_PIN,      INPUT_PULLUP);
    pinMode (BTN_2_PIN,      INPUT_PULLUP);
    pinMode (BTN_3_PIN,      INPUT_PULLUP);

    digitalWrite (LED_PIN,      HIGH); // off
    digitalWrite (OLED_ON_PIN,      LOW); // off
    
    digitalWrite (DIR_UP_PIN,     LOW); // off
    digitalWrite (DIR_DOWN_PIN,   LOW); // off

    digitalWrite (BTN_COM_PIN,    LOW); // on
    digitalWrite (PULSE_ON_PIN, HIGH); // on

    user_led (OFF);

    SENSORS_ACTIVATE(button_sensor);

    // init coap resourcen
    rest_init_engine ();
    rest_activate_resource (&res_pulses,    "s/pulses");
    rest_activate_resource (&res_direction, "a/direction");
 //   NETSTACK_MAC.off(1);
}

void print_stats (int8_t dir) {
    
    digitalWrite (OLED_ON_PIN,      HIGH);
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
    display_off = 10;
}

void loop (void)
{
    uint8_t b1,b2,b3;
    mcu_sleep_off();

    if (display_off == 0) {
        digitalWrite (OLED_ON_PIN, LOW);
    } else {
        display_off --;
    }
    
    b1 = digitalRead (BTN_1_PIN);
    b2 = digitalRead (BTN_2_PIN);
    b3 = digitalRead (BTN_3_PIN);

	if (b2 == 0) {
        print_stats (0);
    }

    if (b1 == 0 && b3 == 1) {
        digitalWrite (DIR_UP_PIN,     HIGH); // on
        digitalWrite (DIR_DOWN_PIN,   LOW); // off
        digitalWrite (PULSE_ON_PIN,   HIGH); // on
        print_stats (1);
        user_led (ON);
    } else if (b1 == 1 && b3 == 0) {
        digitalWrite (DIR_UP_PIN,     LOW); // off
        digitalWrite (DIR_DOWN_PIN,   HIGH); // on
        digitalWrite (PULSE_ON_PIN,   HIGH); // on
        print_stats (-1);
        user_led (ON);
    } else {
        digitalWrite (DIR_UP_PIN,     LOW); // off
        digitalWrite (DIR_DOWN_PIN,   LOW); // off
        digitalWrite (PULSE_ON_PIN,   LOW); // off
        user_led (OFF);
    }

    printf ("lbtns: %d, %d, %d, %d\n"
           , b1,b2,b3,button_sensor.value (0)
           );

    mcu_sleep_on();
}
