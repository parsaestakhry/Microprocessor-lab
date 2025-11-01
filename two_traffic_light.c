#include <mega32.h>
#include <delay.h>

// 7-segment data for digits 0–9 (common cathode)
unsigned char seg_data[10] = {
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

// Light phase durations (in seconds)
#define GREEN_TIME 5
#define YELLOW_TIME 2
#define RED_TIME 5

// Function to display the same countdown on both 7-seg displays
void countdown_dual(int A_time, int B_time)
{
    int i;
    // Count down from the given times (whichever is greater)
    for (i = (A_time > B_time ? A_time : B_time); i >= 0; i--)
    {
        if (i <= A_time && i >= 0)
            PORTC = seg_data[i]; // Display for Road A
        if (i <= B_time && i >= 0)
            PORTD = seg_data[i]; // Display for Road B
        delay_ms(1000);
    }
}

// Function to display only one countdown while keeping the other steady
void display_countdown(int A_count, int B_count, int seconds)
{
    int i;
    for (i = seconds; i >= 0; i--)
    {
        if (i < 10)
            PORTC = seg_data[i];
        else
            PORTC = 0x00;
        if (B_count < 10)
            PORTD = seg_data[B_count];
        delay_ms(1000);
        B_count--;
    }
}

void main(void)
{
    // Configure ports as output
    DDRA = 0xFF; // Road A LEDs
    DDRB = 0xFF; // Road B LEDs
    DDRC = 0xFF; // 7-seg Road A
    DDRD = 0xFF; // 7-seg Road B

    while (1)
    {
        // -----------------------------
        // 1️⃣ Road A: Green | Road B: Red
        // -----------------------------
        PORTA = 0b00000001; // A Green ON
        PORTB = 0b00000100; // B Red ON

        for (int i = GREEN_TIME; i >= 0; i--)
        {
            PORTC = seg_data[i];            // Road A countdown
            PORTD = seg_data[RED_TIME - i]; // Road B red countdown increasing
            delay_ms(1000);
        }

        // -----------------------------
        // 2️⃣ Road A: Yellow | Road B: Red
        // -----------------------------
        PORTA = 0b00000010; // A Yellow ON
        PORTB = 0b00000100; // B Red ON

        for (int i = YELLOW_TIME; i >= 0; i--)
        {
            PORTC = seg_data[i];
            PORTD = seg_data[YELLOW_TIME - i];
            delay_ms(1000);
        }

        // -----------------------------
        // 3️⃣ Road A: Red | Road B: Green
        // -----------------------------
        PORTA = 0b00000100; // A Red ON
        PORTB = 0b00000001; // B Green ON

        for (int i = GREEN_TIME; i >= 0; i--)
        {
            PORTC = seg_data[RED_TIME - i]; // A red countdown increasing
            PORTD = seg_data[i];            // B green countdown
            delay_ms(1000);
        }

        // -----------------------------
        // 4️⃣ Road A: Red | Road B: Yellow
        // -----------------------------
        PORTA = 0b00000100; // A Red ON
        PORTB = 0b00000010; // B Yellow ON

        for (int i = YELLOW_TIME; i >= 0; i--)
        {
            PORTC = seg_data[YELLOW_TIME - i];
            PORTD = seg_data[i];
            delay_ms(1000);
        }
    }
}
