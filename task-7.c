#include <mega16.h>
#asm
// now the lcd module is connected to the port B
.equ __lcd_port = 0x18;
#endasm
// including the header files
#include <lcd.h>
#include <delay.h>
void main(void)
{

    int i;
    lcd_init(16);
    while (1)
    {
        for (i = 0; i < 99; i++)
        {
            lcd_clear();
            lcd_gotoxy(i, 0);
            lcd_putsf(i);
            delay_ms(100);
        }
    };
}