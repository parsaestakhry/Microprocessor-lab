#include <mega32.h>
#asm
.equ __lcd_port = 0x18;
#endasm
#include <lcd.h>
#include <delay.h>
#include <stdio.h>

void main(void)
{
    int i;
    char buffer[4]; // enough for "99\0"

    lcd_init(16); // initialize 16x2 LCD
    lcd_clear();

    while (1)
    {
        for (i = 0; i < 100; i++)
        {
            lcd_clear();              // clear display
            lcd_gotoxy(0, 0);         // first line, first column
            sprintf(buffer, "%d", i); // convert number to string
            lcd_puts(buffer);         // print the string
            delay_ms(500);            // wait half a second
        }
    }
}


