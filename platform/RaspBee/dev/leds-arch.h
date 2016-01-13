/*
	guhRF Platform has just one green LED on PIN E5

*/

#ifndef __LEDS_ARCH_H__
#define __LEDS_ARCH_H__

#define LED_OUT PORTE

#define BIT_RED   _BV(PE5)
#define BIT_BLUE   BIT_RED
#define BIT_GREEN  BIT_RED
#define BIT_YELLOW BIT_RED

#define LED_GREEN_ON() LED_OUT &= ~BIT_GREEN
#define LED_GREEN_OFF() LED_OUT |= BIT_GREEN
#define LED_YELLOW_ON() LED_OUT &= ~BIT_YELLOW
#define LED_YELLOW_OFF() LED_OUT |= BIT_YELLOW
#define LED_RED_ON() LED_OUT &= ~BIT_RED
#define LED_RED_OFF() LED_OUT |= BIT_RED


#define LED_GREEN_TOGGLE() LED_OUT ^= BIT_GREEN
#define LED_YELLOW_TOGGLE() LED_OUT ^= BIT_YELLOW
#define LED_RED_TOGGLE() LED_OUT ^= BIT_RED

#define LEDS_ON() LED_OUT &= ~(BIT_BLUE | BIT_GREEN | BIT_RED)
#define LEDS_OFF() LED_OUT |= (BIT_BLUE | BIT_GREEN | BIT_RED)


void leds_arch_init(void);
unsigned char leds_arch_get(void);
void leds_arch_set(unsigned char leds);

#endif /* __LEDS_ARCH_H__ */
