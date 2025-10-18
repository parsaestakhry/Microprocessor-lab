#include <mega32.h>
#include <delay.h>

unsigned char x[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};
void main(void)
{
    char i;

    DDRA = 0xff;

    while (1)
    {
        for (i = 0; i < 10; i++)
        {
            PORTA = x[i];
            delay_ms(100);
        }
    }
}