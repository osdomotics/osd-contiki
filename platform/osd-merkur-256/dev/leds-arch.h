/*
 *  osd-merkur Platforms have just one green LED on PIN E5
 */

#ifndef __LEDS_ARCH_H__
#define __LEDS_ARCH_H__

#define LED_OUT PORTE

#define BIT_GREEN _BV(PE5)

#define LED_GREEN_OFF() LED_OUT &= ~BIT_GREEN
#define LED_GREEN_ON()  LED_OUT |=  BIT_GREEN

#define LED_GREEN_TOGGLE() LED_OUT ^= BIT_GREEN

#define LEDS_ON() LED_OUT &= ~BIT_GREEN
#define LEDS_OFF() LED_OUT |=   BIT_GREEN


void leds_arch_init(void);
unsigned char leds_arch_get(void);
void leds_arch_set(unsigned char leds);

#endif /* __LEDS_ARCH_H__ */
