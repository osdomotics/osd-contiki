/**********************************************************************************/
/*    Demo Program for:          						  */
/*	  Board: BB-L298                 					  */
/*    Manufacture: OLIMEX                                                   	  */
/*	  COPYRIGHT (C) 2013							  */
/*    Designed by: Engineer Penko T. Bozhkov                                      */
/*    Module Name    :  main module                                               */
/*    File   Name    :  main.c                                                    */
/*    Revision       :  1.0.0 (initial)                                           */
/*    Date           :  11.09.2013                                                */
/*   Built with Arduino C/C++ Compiler, version: 1.0.5                            */
/**********************************************************************************/

// Description: This simple program is designed by using Olimexino-328 board and BB-L298 connected to it.
//      |---------------------------------------------------------------------------|
//      |            The connections were as follows:                               |
//      |--------------------------------|------------------------------------------|
//      |Connector at Olimexino-328      |      Connector at BB-L298                |
//      |--------------------------------|------------------------------------------|
//      |    Power<3>, 5V                |          CTRL<1>,  +5V                   |
//      |    Power<4>, GND               |          CTRL<8>,  GND                   |
//      |    Power<5>, GND               |          PWR<2>,   GND                   |
//      |    Power<6>, VIN(@12VDC)       |          PWR<1>,   VIN                   |
//      |    DIGITAL<3>, D3              |          CTRL<2>,  Enable_A              |
//      |    DIGITAL<4>, D4              |          CTRL<3>,  IN1                   |
//      |    DIGITAL<5>, D5              |          CTRL<4>,  IN2                   |
//      |    DIGITAL<6>, D6              |          CTRL<5>,  Enable_B              |
//      |    DIGITAL<7>, D7              |          CTRL<6>,  IN3                   |
//      |    DIGITAL<8>, D8              |          CTRL<7>,  IN4                   |
//      |--------------------------------|------------------------------------------|
// A 12VDC 2-phase step engine(rotates 18 degree per step) was connected to the BB-L298 board's connectors MT1 ans MT2.
// When the program is running, the motor makes one 360 degree clockwise rotation(if connected correctly).
// Then one 360 degree counterclockwise rotation. Afterwards the described cycle is constantly repeated.
// The step engine must be connected as follows: Coil<1>*(Begin) to OUT1;  Coil<1>(End) to OUT2
//                                               Coil<2>*(Begin) to OUT3;  Coil<2>(End) to OUT4


// set pin numbers:
const int LED  = 13;
const int Enable_A =  3;    // A low-to-high transition on the STEP input sequences the translator and advances the motor one increment.
const int IN1 = 4;          // Direction of rotation
const int IN2 = 5;          // Mode of operation: Active/Sleep
const int Enable_B = 6;      // Enable/Disable the Driver operation
const int IN3 = 7;          // Reset when active turns off all of the FET outputs
const int IN4 = 8;          // Microstep Select
const int Threshold = 20;

#define IN1_L    digitalWrite(IN1, LOW);
#define IN1_H    digitalWrite(IN1, HIGH);
#define IN2_L    digitalWrite(IN2, LOW);
#define IN2_H    digitalWrite(IN2, HIGH);
#define IN3_L    digitalWrite(IN3, LOW);
#define IN3_H    digitalWrite(IN3, HIGH);
#define IN4_L    digitalWrite(IN4, LOW);
#define IN4_H    digitalWrite(IN4, HIGH);

// Variables will change:
long previousMillis = 0;        // will store last time STEP was updated
int counter = 0;
volatile int index = 1;
int DIR = 0;
long interval = 100;           // interval at which to make a STEP

void setup() {
  // set the digital pin as output:
  pinMode(LED, OUTPUT);
  pinMode(Enable_A, OUTPUT); 
  pinMode(Enable_B, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  
  //Set the state
  digitalWrite(LED, LOW);
  digitalWrite(Enable_A, HIGH);      
  digitalWrite(Enable_B, HIGH);       
  digitalWrite(IN1, LOW);    
  digitalWrite(IN2, LOW);    
  digitalWrite(IN3, LOW);    
  digitalWrite(IN4, LOW);    

}

void loop()
{
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;   
    
    counter++;
    
    if(     index == 1){  IN1_H; IN2_L; IN3_L; IN4_L;  }
    else if(index == 2){  IN1_L; IN2_L; IN3_H; IN4_L;  }
    else if(index == 3){  IN1_L; IN2_H; IN3_L; IN4_L;  }
    else if(index == 4){  IN1_L; IN2_L; IN3_L; IN4_H;  }
      
    if(counter >= Threshold){
      counter = 0; 
      if(DIR){  DIR = 0;  }
      else{     DIR = 1;  }
      if(  digitalRead(LED) == HIGH  ){  digitalWrite(LED, LOW);   }
      else{                              digitalWrite(LED, HIGH);  }
    }
    
    if(DIR){  index++; if(index >4 ){  index = 1;  } }
    else{     index--; if(index <1 ){  index = 4;  } }
   
  }
}