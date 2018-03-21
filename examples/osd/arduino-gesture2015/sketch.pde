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

#include <Wire.h>
#include "paj7620.h"

extern "C" {
#include "arduino-process.h"
#include "rest-engine.h"

extern resource_t res_battery, res_gesture;

#define LED_PIN 4

char gesture[64];
/* 
Notice: When you want to recognize the Forward/Backward gesture or other continuous gestures, your gestures' reaction time must less than GES_REACTION_TIME(0.8s). 
        You also can adjust the reaction time according to the actual circumstance.
*/
#define GES_REACTION_TIME		800
#define GES_QUIT_TIME			1000
}

void setup (void)
{
    // switch off the led
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);

    // init coap resourcen
    rest_init_engine ();
	uint8_t error = 0;

	printf("\nPAJ7620U2 TEST DEMO: Recognize 15 gestures.\n");

	error = paj7620Init();			// initialize Paj7620 registers
	if (error) 
	{
		printf("INIT ERROR,CODE: %d \n",error);
	}
	else
	{
		printf("INIT OK\n");
	}
	printf("Please input your gestures:\n");
	
	#pragma GCC diagnostic ignored "-Wwrite-strings"
    rest_activate_resource (&res_battery, "s/battery");
    rest_activate_resource (&res_gesture, "s/gesture");
    #pragma GCC diagnostic pop
}

// at project-conf.h
// LOOP_INTERVAL		(10 * CLOCK_SECOND)
void loop (void)
{
	uint8_t data = 0, data1 = 0, error; 

	error = paj7620ReadReg(0x43, 1, &data);				// Read Bank_0_Reg_0x43/0x44 for gesture result.
	if (!error) 
	{
		switch (data) 									// When different gestures be detected, the variable 'data' will be set to different values by paj7620ReadReg(0x43, 1, &data).
		{
			case GES_RIGHT_FLAG:
				delay(GES_REACTION_TIME);
				paj7620ReadReg(0x43, 1, &data);
				if(data == GES_LEFT_FLAG) 
				{
					sprintf(gesture,"Right-Left");
				}
				else if(data == GES_FORWARD_FLAG) 
				{
					sprintf(gesture,"Forward");
					delay(GES_QUIT_TIME);
				}
				else if(data == GES_BACKWARD_FLAG) 
				{
					sprintf(gesture,"Backward");
					delay(GES_QUIT_TIME);
				}
				else
				{
					sprintf(gesture,"Right");
					printf("%s\n",gesture);
				}          
				break;
			case GES_LEFT_FLAG:
				delay(GES_REACTION_TIME);
				paj7620ReadReg(0x43, 1, &data);
				if(data == GES_RIGHT_FLAG) 
				{
					sprintf(gesture,"Left-Right");
				}
				else if(data == GES_FORWARD_FLAG) 
				{
					sprintf(gesture,"Forward");
					delay(GES_QUIT_TIME);
				}
				else if(data == GES_BACKWARD_FLAG) 
				{

					sprintf(gesture,"Backward");
					delay(GES_QUIT_TIME);
				}
				else
				{
					sprintf(gesture,"Left");
					printf("%s\n",gesture);
				}          
				break;
				break;
			case GES_UP_FLAG:
				delay(GES_REACTION_TIME);
				paj7620ReadReg(0x43, 1, &data);
				if(data == GES_DOWN_FLAG) 
				{
					sprintf(gesture,"Up-Down");
				}
				else if(data == GES_FORWARD_FLAG) 
				{
					sprintf(gesture,"Forward");
					delay(GES_QUIT_TIME);
				}
				else if(data == GES_BACKWARD_FLAG) 
				{
					sprintf(gesture,"Backward");
					delay(GES_QUIT_TIME);
				}
				else
				{
					sprintf(gesture,"Up");
				}
				break;
			case GES_DOWN_FLAG:
				delay(GES_REACTION_TIME);
				paj7620ReadReg(0x43, 1, &data);
				if(data == GES_UP_FLAG) 
				{
					sprintf(gesture,"Down-Up");
				}
				else if(data == GES_FORWARD_FLAG) 
				{
					sprintf(gesture,"Forward");
					delay(GES_QUIT_TIME);
				}
				else if(data == GES_BACKWARD_FLAG) 
				{
					sprintf(gesture,"Backward");
					delay(GES_QUIT_TIME);
				}
				else
				{
					sprintf(gesture,"Down");
				}
				break;
			case GES_FORWARD_FLAG:
				delay(GES_REACTION_TIME);
				paj7620ReadReg(0x43, 1, &data);
				if(data == GES_BACKWARD_FLAG) 
				{
					sprintf(gesture,"Forward-Backward");
					delay(GES_QUIT_TIME);
				}
				else
				{
					sprintf(gesture,"Forward");
					delay(GES_QUIT_TIME);
				}
				break;
			case GES_BACKWARD_FLAG:		  
				delay(GES_REACTION_TIME);
				paj7620ReadReg(0x43, 1, &data);
				if(data == GES_FORWARD_FLAG) 
				{
					sprintf(gesture,"Backward-Forward");
					delay(GES_QUIT_TIME);
				}
				else
				{
					sprintf(gesture,"Backward");
					delay(GES_QUIT_TIME);
				}
				break;
			case GES_CLOCKWISE_FLAG:
				sprintf(gesture,"Clockwise");
				break;
			case GES_COUNT_CLOCKWISE_FLAG:
				sprintf(gesture,"anti-clockwise");
				break;  
			default:
				paj7620ReadReg(0x44, 1, &data1);
				if (data1 == GES_WAVE_FLAG) 
				{
					sprintf(gesture,"wave");
				}
				break;
		}
		printf("%s\n",gesture); // Debug Print
	}
}
