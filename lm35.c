/****************************************************
 * Measure Room Temperature using LM35 & ATmega32
 * Compiler: CodeVisionAVR
 ****************************************************/

#include <mega32.h>
#include <lcd.h>
#include <delay.h>
#include <stdio.h>

#define ADC_CHANNEL 0 // LM35 connected to ADC0 (PA0)
#define VREF 5.0      // Reference voltage

#asm
.equ __lcd_port = 0x18
#endasm

    // Function to initialize ADC
    void ADC_Init(void)
{
    ADMUX = 0x40;  // AVCC as reference, ADC0 selected
    ADCSRA = 0x87; // Enable ADC, prescaler=128 (for 16MHz → 125kHz ADC clock)
}

// Function to read ADC value
unsigned int ADC_Read(unsigned char channel)
{
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F); // Select ADC channel
    ADCSRA |= (1 << ADSC);                     // Start conversion
    while (ADCSRA & (1 << ADSC))
        ;        // Wait for conversion to finish
    return ADCW; // Return 10-bit result
}

void main(void)
{
    unsigned int adc_value;
    unsigned int temp_int, temp_frac;
    char lcd_buffer[16];

    lcd_init(16); // Initialize 16x2 LCD
    ADC_Init();   // Initialize ADC

    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_putsf("Room Temp Meter"); // Static title

    while (1)
    {
        adc_value = ADC_Read(ADC_CHANNEL);
        // Convert ADC to temperature in °C with 2 decimal precision
        temp_int = (adc_value * 500) / 1024;           // Integer part (°C * 100)
        temp_frac = ((adc_value * 5000) / 1024) % 100; // Fractional part (2 decimals)

        lcd_gotoxy(0, 1);
        sprintf(lcd_buffer, "Temp: %u.%02u C", temp_int, temp_frac); // Manually format
        lcd_puts(lcd_buffer);

        delay_ms(1000);
    }
}
