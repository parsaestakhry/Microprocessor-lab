#include <mega32.h>
#include <delay.h>
#include <stdio.h>

// Define min and max temperature
#define MIN_TEMP 25
#define MAX_TEMP 30

// Define control pins
#define HEATER PORTD.0
#define FAN PORTD.1

// Function to read temperature from LM35 sensor
float read_temperature()
{
    unsigned int adc_value;
    float temperature;

    // Start ADC conversion on channel 0
    ADMUX = 0x00;          // Select ADC0 channel
    ADCSRA |= (1 << ADSC); // Start conversion
    while (ADCSRA & (1 << ADSC))
        ; // Wait for conversion to finish

    adc_value = ADCW;                        // Read ADC value
    temperature = (adc_value * 4.88) / 10.0; // Convert to °C (LM35 gives 10mV/°C)
    return temperature;
}

void main(void)
{
    float temperature;

    // Initialize ADC
    ADMUX = 0x00;                                       // AREF reference, ADC0 channel
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1); // Enable ADC, prescaler = 64

    // Configure PORTD as output for heater and fan
    DDRD = 0x03; // PD0 and PD1 output
    PORTD = 0x00;

    while (1)
    {
        temperature = read_temperature();

        if (temperature < MIN_TEMP)
        {
            // Temperature too low → Turn ON heater, Turn OFF fan
            PORTD |= (1 << 0);  // Heater ON
            PORTD &= ~(1 << 1); // Fan OFF
        }
        else if (temperature > MAX_TEMP)
        {
            // Temperature too high → Turn ON fan, Turn OFF heater
            PORTD |= (1 << 1);  // Fan ON
            PORTD &= ~(1 << 0); // Heater OFF
        }
        else
        {
            // Temperature within range → Turn OFF both
            PORTD &= ~((1 << 0) | (1 << 1));
        }

        delay_ms(1000); // Wait 1 second before next reading
    }
}
