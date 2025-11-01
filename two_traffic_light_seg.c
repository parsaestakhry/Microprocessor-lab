#include <mega32.h> // Include header for ATmega32 registers
#include <delay.h>  // Include delay functions (CodeVisionAVR library)

// ----------------------------
// Define 7-segment digit patterns (common cathode)
// Each value corresponds to segments a–g
// Bit = 1 → LED ON
// ----------------------------
unsigned char seg[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

// ----------------------------
// Define traffic light durations (in seconds)
// ----------------------------
#define GREEN_TIME 5  // Green light duration
#define YELLOW_TIME 2 // Yellow light duration
#define RED_TIME 5    // Red light duration

// ----------------------------
// Function: show_digit()
// Displays a number (0–9) on a chosen 7-segment
// port = 'A' → show on Road A’s 7-segment (PORTC)
// port = 'B' → show on Road B’s 7-segment (PORTD)
// ----------------------------
void show_digit(char port, char num)
{
    if (port == 'A')
        PORTC = seg[num]; // Output the digit pattern to 7-segment A
    else
        PORTD = seg[num]; // Output the digit pattern to 7-segment B
}

// ----------------------------
// MAIN FUNCTION
// ----------------------------
void main(void)
{
    // ----------------------------
    // Configure port directions
    // ----------------------------
    DDRA = 0xFF; // PORTA → Output (controls LEDs for Road A)
    DDRB = 0xFF; // PORTB → Output (controls LEDs for Road B)
    DDRC = 0xFF; // PORTC → Output (7-segment display for Road A)
    DDRD = 0xFF; // PORTD → Output (7-segment display for Road B)

    // Infinite loop — keeps repeating the traffic cycle
    while (1)
    {
        signed char t; // Variable for countdown loops

        // ------------------------------------------------------
        // 1️⃣ ROAD A: GREEN | ROAD B: RED
        // ------------------------------------------------------
        PORTA = 0b00000001;               // Road A Green ON (PA0)
        PORTB = 0b00000100;               // Road B Red ON (PB2)
        for (t = GREEN_TIME; t >= 0; t--) // Count down from 5 → 0
        {
            show_digit('A', t); // Display countdown on Road A 7-seg
            show_digit('B', 0); // Show 0 on Road B (red light)
            delay_ms(1000);     // Wait 1 second
        }

        // ------------------------------------------------------
        // 2️⃣ ROAD A: YELLOW | ROAD B: RED
        // ------------------------------------------------------
        PORTA = 0b00000010;                // Road A Yellow ON (PA1)
        PORTB = 0b00000100;                // Road B Red ON (PB2)
        for (t = YELLOW_TIME; t >= 0; t--) // Count down from 2 → 0
        {
            show_digit('A', t); // Display countdown on Road A
            show_digit('B', 0); // Keep Road B showing 0
            delay_ms(1000);     // Wait 1 second
        }

        // ------------------------------------------------------
        // 3️⃣ ROAD A: RED | ROAD B: GREEN
        // ------------------------------------------------------
        PORTA = 0b00000100;               // Road A Red ON (PA2)
        PORTB = 0b00000001;               // Road B Green ON (PB0)
        for (t = GREEN_TIME; t >= 0; t--) // Count down from 5 → 0
        {
            show_digit('A', 0); // Show 0 for Road A (red)
            show_digit('B', t); // Display countdown on Road B
            delay_ms(1000);     // Wait 1 second
        }

        // ------------------------------------------------------
        // 4️⃣ ROAD A: RED | ROAD B: YELLOW
        // ------------------------------------------------------
        PORTA = 0b00000100;                // Road A Red ON (PA2)
        PORTB = 0b00000010;                // Road B Yellow ON (PB1)
        for (t = YELLOW_TIME; t >= 0; t--) // Count down from 2 → 0
        {
            show_digit('A', 0); // Road A stays red (shows 0)
            show_digit('B', t); // Display countdown on Road B
            delay_ms(1000);     // Wait 1 second
        }

        // After yellow, loop repeats — switching sides again
    }
}
