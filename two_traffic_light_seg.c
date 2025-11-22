#include <mega32.h>
#include <delay.h>

// -----------------------------------
// 7-segment codes for digits 0–9
// -----------------------------------
unsigned char seg[10] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

// -----------------------------------
#define GREEN_TIME 6
#define YELLOW_TIME 3
#define RED_TIME 9 // not used directly, but kept for clarity

#define LED_GREEN 0b00000001
#define LED_YELLOW 0b00000010
#define LED_RED 0b00000100

// -----------------------------------
void show_digit(char port, signed char num)
{
    if (num < 0)
        num = 0;
    if (num > 9)
        num = 9;

    if (port == 'A')
        PORTC = seg[(unsigned char)num];
    else
        PORTD = seg[(unsigned char)num];
}

// -----------------------------------
void main(void)
{
    unsigned char phase;
    signed char t;

    DDRA = 0xFF;
    DDRB = 0xFF;
    DDRC = 0xFF;
    DDRD = 0xFF;

    phase = 0;
    t = GREEN_TIME;

    while (1)
    {
        // =============================
        // SET LED STATES + COUNTDOWN
        // =============================
        switch (phase)
        {
        case 0: // A=Green (6), B=Red (first 6 of 9)
            PORTA = LED_GREEN;
            PORTB = LED_RED;

            show_digit('A', t);
            show_digit('B', t + YELLOW_TIME); // 9..4
            break;

        case 1: // A=Yellow (3), B=Red (last 3)
            PORTA = LED_YELLOW;
            PORTB = LED_RED;

            show_digit('A', t);
            show_digit('B', t);
            break;

        case 2: // A=Red (first 6), B=Green (6)
            PORTA = LED_RED;
            PORTB = LED_GREEN;

            show_digit('A', t + YELLOW_TIME); // 9..4
            show_digit('B', t);
            break;

        case 3: // A=Red (last 3), B=Yellow (3)
            PORTA = LED_RED;
            PORTB = LED_YELLOW;

            show_digit('A', t);
            show_digit('B', t);
            break;
        }

        // 🔹 SPEED CONTROL: smaller value = faster
        delay_ms(500); // was 1000 → now 5x faster

        t--;

        // =============================
        // PHASE TRANSITION
        // =============================
        if (t == 0)
        {
            if (phase == 0)
            {
                phase = 1;
                t = YELLOW_TIME;
            }
            else if (phase == 1)
            {
                phase = 2;
                t = GREEN_TIME;
            }
            else if (phase == 2)
            {
                phase = 3;
                t = YELLOW_TIME;
            }
            else if (phase == 3)
            {
                phase = 0;
                t = GREEN_TIME;
            }
        }
    }
}
