/****************************************************
 * LM35 Temperature Display (Clean Version)
 * ATmega32 - CodeVisionAVR
 ****************************************************/

#include <mega32.h>
#include <lcd.h>
#include <delay.h>
#include <stdio.h>

#define ADC_CHANNEL 0 // LM35 on ADC0 (PA0)

#asm
.equ __lcd_port = 0x12
#endasm

    void ADC_Init(void)
{
    // AVCC as reference, ADC0 selected
    ADMUX = 0x40;

    // Enable ADC, prescaler = 128
    ADCSRA = 0x87;

    // PA0 as input, no pull-up
    DDRA &= ~(1 << 0);
    PORTA &= ~(1 << 0);
}

unsigned int ADC_Read(unsigned char channel)
{
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);

    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC))
        ;

    return ADCW;
}

void main(void)
{
    unsigned int adc_value;
    unsigned long temp_x100;
    unsigned int temp_int, temp_frac;
    char lcd_buffer[17];

    lcd_init(16);
    ADC_Init();

    lcd_clear();

    while (1)
    {
        adc_value = ADC_Read(ADC_CHANNEL);

        // Convert ADC ? °C * 100
        temp_x100 = (unsigned long)adc_value * 50000UL / 1024;

        temp_int = temp_x100 / 100;
        temp_frac = temp_x100 % 100;

        lcd_gotoxy(0, 0);
        sprintf(lcd_buffer, "Temp: %u.%02u C", temp_int, temp_frac);
        lcd_puts(lcd_buffer);

        delay_ms(500);
    }
}
