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

extern resource_t res_led, res_battery, res_cputemp;

uint8_t led_pin=4; // D4 
uint8_t led_status;

#define f1_pin 18  // D18
#define f1_led 17  // D17
#define f2_pin 16  // D16
#define f2_led 15  // D15
#define f3_pin 19  // D19
#define f3_led 20  // D20
#define f4_pin 5   // D5
#define f4_led 6   // D6

uint8_t f1_status=0;
uint8_t f2_status=0;
uint8_t f3_status=0;
uint8_t f4_status=0;
}

void setup (void)
{
    // switch off the led
    pinMode(led_pin, OUTPUT);
    digitalWrite(led_pin, HIGH);
    led_status=0;
    // fx pins
    pinMode(f1_pin,INPUT);
    pinMode(f1_led,OUTPUT);
    digitalWrite(f1_led,HIGH);
    pinMode(f2_pin,INPUT);
    pinMode(f2_led,OUTPUT);
    digitalWrite(f2_led,HIGH);
    pinMode(f3_pin,INPUT);
    pinMode(f3_led,OUTPUT);
    digitalWrite(f3_led,HIGH);
    pinMode(f4_pin,INPUT);
    pinMode(f4_led,OUTPUT);
    digitalWrite(f4_led,HIGH);

    // init coap resourcen
    rest_init_engine ();
    #pragma GCC diagnostic ignored "-Wwrite-strings"
    rest_activate_resource (&res_led, "s/led");
    rest_activate_resource (&res_battery, "s/battery");
    rest_activate_resource (&res_cputemp, "s/cputemp");
    #pragma GCC diagnostic pop

    // uncomment if you want this node to be a routing node and set the
    // correct options in project-conf.h and contiki-conf.h
    //NETSTACK_MAC.off(1);

    // uncoment if you want to set the loop interval at runtime
    //#define LOOP_INTERVAL_AFTER_INIT (60 * CLOCK_SECOND)    
    //loop_periodic_set (LOOP_INTERVAL_AFTER_INIT);

    mcu_sleep_set(128);
}

void loop (void)
{
	f1_status = digitalRead(f1_pin);
	if(f1_status == 1){
		digitalWrite(f1_led,LOW);
	}else{
		digitalWrite(f1_led,HIGH);
	}
	printf("F1: %d\n",f1_status);

        f2_status = digitalRead(f2_pin);
        if(f2_status == 1){
                digitalWrite(f2_led,LOW);
        }else{
                digitalWrite(f2_led,HIGH);
        }
        printf("F2: %d\n",f1_status);

        f3_status = digitalRead(f3_pin);
        if(f3_status == 1){
                digitalWrite(f3_led,LOW);
        }else{
                digitalWrite(f3_led,HIGH);
        }
        printf("F3: %d\n",f3_status);

        f4_status = digitalRead(f4_pin);

        if(f4_status == 1){
                digitalWrite(f4_led,LOW);
        }else{
                digitalWrite(f4_led,HIGH);
        }
        printf("F4: %d\n",f4_status);
}
