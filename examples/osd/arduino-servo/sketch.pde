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
#include "Servo.h"

extern "C" {
#include "arduino-process.h"
#include "rest-engine.h"
#include "net/netstack.h"

extern resource_t res_led, res_battery, res_cputemp, res_servo;
}

uint8_t led_pin=4;
uint8_t led_status;

int potpin = A5;  // analog pin used to connect the potentiometer
int val;    // variable to read the value from the analog pin

Servo servo;  // create servo object to control a servo

void setup (void)
{
    // switch off the led
    pinMode(led_pin, OUTPUT);
    digitalWrite(led_pin, HIGH);
    led_status=0;
    // Servo
    servo.attach(3);  // attaches the servo on pin 9 to the servo object
    // init coap resourcen
    rest_init_engine ();
    #pragma GCC diagnostic ignored "-Wwrite-strings"
    rest_activate_resource (&res_led, "s/led");
    rest_activate_resource (&res_battery, "s/battery");
    rest_activate_resource (&res_cputemp, "s/cputemp");
    rest_activate_resource (&res_servo, "a/servo");
    #pragma GCC diagnostic pop
    
 //   NETSTACK_MAC.off(1);
 //   mcu_sleep_set(128);
}

void loop (void)
{
  val = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023)
  val = map(val, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
  servo.write(val);                  // sets the servo position according to the scaled value
}
