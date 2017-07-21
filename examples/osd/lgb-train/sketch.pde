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

uint8_t led_pin=4;
uint8_t led_status;

}

// Merkurboard and BB-L298 connected to it.
//      |---------------------------------------------------------------------------|
//      |            The connections were as follows:                               |
//      |--------------------------------|------------------------------------------|
//      |Connector at Merkurboard Grove 4|      Connector at BB-L298                |
//      |--------------------------------|------------------------------------------|
//      |    Power<3>, 3V                |          CTRL<1>,  +5V                   |
//      |    Power<4>, GND               |          CTRL<8>,  GND                   |
//      |    Power<5>, GND               |          PWR<2>,   GND                   |
//      |    Power<6>, VIN(@12VDC)       |          PWR<1>,   VIN                   |
//      |    DIGITAL<3>,  D3             |          CTRL<5>,  Enable_B              |
//      |    DIGITAL<17>, D17            |          CTRL<6>,  IN3                   |
//      |    DIGITAL<18>, D18            |          CTRL<7>,  IN4                   |
//      |--------------------------------|------------------------------------------|

#define BB_Enable_B	3
#define BB_IN3		17
#define BB_IN4		18

#define IN3_L    digitalWrite(BB_IN3, LOW);
#define IN3_H    digitalWrite(BB_IN3, HIGH);
#define IN4_L    digitalWrite(BB_IN4, LOW);
#define IN4_H    digitalWrite(BB_IN4, HIGH);

#include <Grove_LED_Bar.h>

Grove_LED_Bar bar(9, 8, 1);  // Clock pin, Data pin, Orientation

void setup (void)
{
    // BB-L298
    arduino_pwm_timer_init ();
    pinMode(BB_IN3, OUTPUT);
    pinMode(BB_IN4, OUTPUT);
    IN3_L;
    IN4_L;
    analogWrite(BB_Enable_B, 0); 
    // switch off the led
    pinMode(led_pin, OUTPUT);
    digitalWrite(led_pin, HIGH);
    led_status=0;
    // led bar
    bar.begin();
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
  static int vbar = 0;
  static int speed = 0;
  int sensorValue1 = analogRead(A4);
  int sensorValue2 = analogRead(A5);

  vbar = (1023-sensorValue2)/100;
  if (vbar > 10){
   vbar=0;
  }  
  
  bar.setLevel(vbar);

  speed = (1023-sensorValue2) >> 2;

  IN3_L; IN4_H; analogWrite(BB_Enable_B, speed); 
  analogWrite(led_pin, speed);

  printf("x: %d , y: %d bar: %d speed: %d\n",sensorValue1 ,sensorValue2, vbar, speed);

}
