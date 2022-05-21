#include <avr/io.h>
#include "peri.h"

void init_peri()
{
    // PB0..PB4 , PB5 -> output
    DDRB |= (1<<PB4)|(1<<PB3)|(1<<PB2)|(1<<PB1)|(1<<PB0);

    // PC0..PC2 -> input
    DDRC &= ~((1<<PC1)|(1<<PC0));

    // enable pull-up resistor on PC0
    //PORTC |= (1<<PC0);

}
void set_buzzer(uint8_t state) {
    if (state)
        PORTB |= (1<<4);
    else
        PORTB &= ~(1<<4);
}

void set_led(uint8_t pin, uint8_t state)
{
    if (pin > 3) return;
    if (state)
        PORTB |= (1<<pin);
    else
        PORTB &= ~(1<<pin);
}

uint16_t read_adc(uint8_t channel)
{
    ADMUX = (0<<REFS1)|(1<<REFS0) // use VCC as reference voltage
          | (0<<ADLAR)            // store result to the right of ADCH/ADCL
          | (channel & 0b1111);   // point MUX to the specified channel

    ADCSRA = (1<<ADEN)            // enable ADC
           | (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0) // set speed to 1/128 of system clock
           | (1<<ADSC);           // start conversion

    // wait until ADSC bit becomes 0, indicating complete conversion
    while ((ADCSRA & (1<<ADSC)))
       ;

    return ADCL + ADCH*256;
}
