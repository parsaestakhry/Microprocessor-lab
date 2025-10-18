#include <mega32.h>
#include <delay.h>

// Common Cathode seven-segment codes for digits 0–9
unsigned char seg_code[] = {
    0x7E, // 0
    0x30, // 1
    0x6D, // 2
    0x79, // 3
    0x33, // 4
    0x5B, // 5
    0x5F, // 6
    0x70, // 7
    0x7F, // 8
    0x7B  // 9
};

void main(void)
{
    unsigned char i, j;
    char up = 1; // Direction flag (1 = up, 0 = down)

    DDRA = 0xFF; // PORTA → tens digit (7-seg)
    DDRB = 0xFF; // PORTB → ones digit (7-seg)

    i = 0; // tens
    j = 0; // ones

    while (1)
    {
        // Display the two digits
        PORTA = seg_code[i];
        PORTB = seg_code[j];
        delay_ms(300);

        if (up)
        {
            // Counting up
            j++;
            if (j > 9)
            {
                j = 0;
                i++;
                if (i > 9)
                {
                    i = 9;
                    j = 9;
                    up = 0; // Switch direction to down
                }
            }
        }
        else
        {
            // Counting down
            if (j == 0)
            {
                j = 9;
                if (i > 0)
                    i--;
                else
                {
                    i = 0;
                    j = 0;
                    up = 1; // Switch direction to up
                }
            }
            else
            {
                j--;
            }
        }
    }
}
