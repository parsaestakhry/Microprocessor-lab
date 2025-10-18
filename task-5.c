// 2 seven seg

#include <mega32.h>
#include <delay.h>

unsigned char x[] = {0x7E,
                     0x30,
                     0x6D,
                     0x79,
                     0x33,
                     0x5B,
                     0x5F,
                     0x70,
                     0x7F,
                     0x7B};
void main(void)
{
    char i, j;
    DDRA = 0xff;
    DDRB = 0xff;
    while (1)
    {
        for (i = 0; i < 10; i++)
        {
            PORTA = x[i];
            delay_ms(400);
            for (j = 0; j < 10; j++)
            {
                PORTB = x[j];
                delay_ms(400);
            }
        }
    }
}