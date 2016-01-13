#ifndef __ADC_ARCH_H__
#define __ADC_ARCH_H__

#include <avr/io.h>

/*
 * Reference voltage
 * The default is 1.6V reference voltage
 * The selected reference voltage is the maximum voltage that can be
 * measured.
 * Directly provide shifted variants so we don't need to shift.
 */
#define ADC_1_5      (2<<6)
#define ADC_1_6      (3<<6)
#define ADC_1_8      (1<<6)
#define ADC_EXTERNAL (0<<6)
#define ADC_DEFAULT  ADC_1_6

/* sometimes it's desirable to decouple setup / finish from sampling */

static inline void adc_setup (uint8_t ref_volt, uint8_t pin)
{
  ADMUX = ref_volt | (pin & 0x7);
  ADCSRA = _BV(ADEN) | _BV(ADPS0) | _BV(ADPS2);
}

static inline int adc_read (void)
{
  ADCSRA |= (1 << ADSC);
  loop_until_bit_is_clear (ADCSRA, ADSC);
  return ADC;
}

static inline void adc_fin (void)
{
    ADCSRA = 0;
    ADMUX  = 0;
}
  
static inline void adc_init (void)
{
    uint8_t temp;
    ADCSRC = 0;
    ADCSRB = 0;
    adc_fin ();
    /*
     * Disable JTAG interface
     * Hardware manual about JTD bit:
     * "In order to avoid unintentional disabling or enabling of the
     * JTAG interface, a timed sequence must be followed when changing
     * this bit: The application software must write this bit to the
     * desired value twice within four cycles to change its value."
     * 15.4.1 "MCUCR - MCU Control Register", p. 219
     */
    temp = MCUCR | (1 << JTD);
    MCUCR = temp;
    MCUCR = temp;
}

int readADC(uint8_t pin);
long readVcc();
int readInternalTemp(void);
void analogReference(uint8_t mode);

#endif /* __ADC_ARCH_H__ */
