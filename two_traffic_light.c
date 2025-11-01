#include <mega32.h>
#include <delay.h>

// define delay times (in milliseconds)
#define GREEN_DELAY 5000
#define YELLOW_DELAY 2000
#define RED_DELAY 5000

void main(void)
{
    // Set PORTA and PORTB as output
    DDRA = 0xFF;
    DDRB = 0xFF;

    while (1)
    {
        // -----------------------------
        // 1️⃣ Road A: Green, Road B: Red
        // -----------------------------
        PORTA.0 = 1; // A Green ON
        PORTA.1 = 0;
        PORTA.2 = 0;
        PORTB.0 = 0;
        PORTB.1 = 0;
        PORTB.2 = 1; // B Red ON
        delay_ms(GREEN_DELAY);

        // -----------------------------
        // 2️⃣ Road A: Yellow, Road B: Red
        // -----------------------------
        PORTA.0 = 0;
        PORTA.1 = 1; // A Yellow ON
        PORTA.2 = 0;
        PORTB.0 = 0;
        PORTB.1 = 0;
        PORTB.2 = 1; // B Red ON
        delay_ms(YELLOW_DELAY);

        // -----------------------------
        // 3️⃣ Road A: Red, Road B: Green
        // -----------------------------
        PORTA.0 = 0;
        PORTA.1 = 0;
        PORTA.2 = 1; // A Red ON
        PORTB.0 = 1; // B Green ON
        PORTB.1 = 0;
        PORTB.2 = 0;
        delay_ms(GREEN_DELAY);

        // -----------------------------
        // 4️⃣ Road A: Red, Road B: Yellow
        // -----------------------------
        PORTA.0 = 0;
        PORTA.1 = 0;
        PORTA.2 = 1; // A Red ON
        PORTB.0 = 0;
        PORTB.1 = 1; // B Yellow ON
        PORTB.2 = 0;
        delay_ms(YELLOW_DELAY);
    }
}
