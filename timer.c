/****************************************************
 * ATmega32 - HH:MM:SS TIMER
 * Clock: Internal 8 MHz
 * LCD: PORTD via lcd.h + __lcd_port
 ****************************************************/

#include <mega32.h>
#include <lcd.h>
#include <stdio.h>

// LCD port definition (PORTD)
#asm
    .equ __lcd_port = 0x12
#endasm

#define F_CPU 8000000UL

// Timer1 settings:
// 8 MHz / 256 = 31250 Hz
// 1 second => OCR1A = 31250 - 1 = 31249
// ideal would be 31249 for true 8MHz
#define OCR1A_1HZ 30249   // 31249 * (230/240) ≈ 29947

volatile unsigned char hh = 0;
volatile unsigned char mm = 0;
volatile unsigned char ss = 0;
volatile bit update_lcd = 0;

//--------------------------------------------------
// Timer1 Compare A ISR (1 Hz)
//--------------------------------------------------
interrupt [TIM1_COMPA] void timer1_compa_isr(void)
{
    ss++;
    if (ss >= 60) {
        ss = 0;
        mm++;
        if (mm >= 60) {
            mm = 0;
            hh++;
            if (hh >= 100) hh = 0;
        }
    }
    update_lcd = 1;
}

//--------------------------------------------------
// Timer1 init: CTC, 1 Hz
//--------------------------------------------------
void timer1_init_1hz(void)
{
    TCCR1A = 0x00;
    TCCR1B = 0x00;

    TCNT1 = 0;
    OCR1A = OCR1A_1HZ;

    // CTC mode (WGM12=1), prescaler=256 (CS12=1)
    TCCR1B = (1<<WGM12) | (1<<CS12);

    // Enable compare match A interrupt
    TIMSK |= (1<<OCIE1A);
}

//--------------------------------------------------
// LCD print
//--------------------------------------------------
void lcd_show_time(unsigned char h, unsigned char m, unsigned char s)
{
    char buf[17];
    sprintf(buf, "%02u:%02u:%02u", h, m, s);
    lcd_gotoxy(0,0);
    lcd_puts(buf);
}

//--------------------------------------------------
// MAIN
//--------------------------------------------------
void main(void)
{
    // LCD init (16x2)
    lcd_init(16);
    lcd_clear();
    lcd_puts("00:00:00");

    timer1_init_1hz();

    #asm
        sei
    #endasm

    while (1) {
        if (update_lcd) {
            unsigned char h, m, s;

            #asm
                cli
            #endasm

            h = hh;
            m = mm;
            s = ss;
            update_lcd = 0;

            #asm
                sei
            #endasm

            lcd_show_time(h, m, s);
        }
    }
}
