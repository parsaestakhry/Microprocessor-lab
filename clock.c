/****************************************************
 * ATmega32 - Silent Clock with Time/Alarm Setting (Text Cursor)
 * Clock: Internal 8 MHz
 * LCD: PORTD
 * Buttons: PORTB.0 (Mode), PORTB.1 (Select), PORTB.2 (Inc)
 ****************************************************/

#include <mega32.h>
#include <lcd.h>
#include <stdio.h>
#include <delay.h>

// LCD port definition (PORTD)
#asm
    .equ __lcd_port = 0x12
#endasm

#define F_CPU 8000000UL

// Calibration for 1Hz
#define OCR1A_1HZ 30249 

// Hardware Definitions
#define BTN_MODE    PINB.0
#define BTN_SEL     PINB.1
#define BTN_INC     PINB.2

// Global Time Variables
volatile unsigned char hh = 0, mm = 0, ss = 0;
volatile bit update_lcd = 0;

// Alarm Variables
unsigned char a_hh = 0, a_mm = 0, a_ss = 0;
bit alarm_enabled = 0;      
bit alarm_triggered = 0;    

// Mode Control
// 0 = Normal Clock Mode
// 1 = Set Current Time Mode 
// 2 = Set Alarm Time Mode
unsigned char system_mode = 0;         
unsigned char cursor_pos = 0; // 0=HH, 1=MM, 2=SS (for setup)

//--------------------------------------------------
// Timer1 Compare A ISR (1 Hz)
//--------------------------------------------------
interrupt [TIM1_COMPA] void timer1_compa_isr(void)
{
    // Only increment the time if we are not in Time Setting Mode
    if (system_mode != 1) { 
        ss++;
        if (ss >= 60) {
            ss = 0;
            mm++;
            if (mm >= 60) {
                mm = 0;
                hh++;
                if (hh >= 24) hh = 0;
            }
        }
    }
    update_lcd = 1;
}

//--------------------------------------------------
// Init Functions
//--------------------------------------------------
void timer1_init_1hz(void)
{
    TCCR1A = 0x00;
    TCNT1 = 0;
    OCR1A = OCR1A_1HZ;
    TCCR1B = (1<<WGM12) | (1<<CS12);
    TIMSK |= (1<<OCIE1A);
}

void io_init(void)
{
    // PORTB.0, .1, .2 as Inputs with Pull-ups (for buttons)
    DDRB &= 0xF8; 
    PORTB |= 0x07;
}

// Helper function to draw the underscore beneath the edited field
void draw_cursor(void)
{
    unsigned char pos = 0;
    
    // Clear the second row (where the time/alarm text is) to reset the cursor
    lcd_gotoxy(0, 1);
    lcd_puts("                "); 

    // Determine position for the time/alarm display on Row 1 (starts at column 6)
    // 00:00:00
    if (cursor_pos == 0) pos = 6;  // Under HH
    if (cursor_pos == 1) pos = 9;  // Under MM
    if (cursor_pos == 2) pos = 12; // Under SS
    
    // Redraw the time/alarm value on the second row
    lcd_gotoxy(6, 1);
    if (system_mode == 1) {
        char buf[17];
        // Must disable interrupts to safely read volatile hh/mm/ss
        #asm("cli")
        sprintf(buf, "%02u:%02u:%02u", hh, mm, ss);
        #asm("sei")
        lcd_puts(buf);
    } else { // system_mode == 2
        char buf[17];
        sprintf(buf, "%02u:%02u:%02u", a_hh, a_mm, a_ss);
        lcd_puts(buf);
    }
    
    // Draw the underscore cursor on Row 1, starting at the position
    lcd_gotoxy(pos, 1); 
    lcd_puts("__"); 
}

//--------------------------------------------------
// Display Logic
//--------------------------------------------------
void refresh_display(void)
{
    char buf[17];
    
    // Copy time safely for display
    unsigned char t_h, t_m, t_s;
    #asm("cli")
    t_h = hh; t_m = mm; t_s = ss;
    #asm("sei")

    // --- Mode 0: NORMAL CLOCK ---
    if (system_mode == 0) 
    {
        lcd_gotoxy(0,0);
        sprintf(buf, "Time: %02u:%02u:%02u", t_h, t_m, t_s);
        lcd_puts(buf);

        lcd_gotoxy(0,1);
        if (alarm_triggered) 
        {
            sprintf(buf, "** ALARM ACTIVATED **");
        }
        else if (alarm_enabled) 
        {
            sprintf(buf, "Alarm: ON %02u:%02u", a_hh, a_mm);
        }
        else               
        {
            lcd_puts("Alarm: OFF      ");
        }
        lcd_puts(buf);
    }
    
    // --- Setup Modes (1 and 2) ---
    else 
    {
        lcd_gotoxy(0,0);
        
        // 1. Determine the top line text (clear field label)
        if (system_mode == 1) {
            // SET CURRENT TIME mode
            if (cursor_pos == 0) lcd_puts("SET TIME: HOUR  ");
            else if (cursor_pos == 1) lcd_puts("SET TIME: MINUTE");
            else if (cursor_pos == 2) lcd_puts("SET TIME: SECONDS");
        }
        else { 
            // SET ALARM TIME mode (system_mode == 2)
            if (cursor_pos == 0) lcd_puts("SET ALARM: HOUR ");
            else if (cursor_pos == 1) lcd_puts("SET ALARM: MINUTE");
            else if (cursor_pos == 2) lcd_puts("SET ALARM: SECONDS");
        }
        
        // 2. Redraw the time/alarm value AND the cursor on line 1
        draw_cursor();
    }
}

//--------------------------------------------------
// MAIN 
//--------------------------------------------------
void main(void)
{
    io_init();
    lcd_init(16);
    lcd_clear();
    
    timer1_init_1hz();
    
    #asm("sei")

    while (1) 
    {
        // 1. Check Alarm Trigger
        if (system_mode == 0 && alarm_enabled && !alarm_triggered)
        {
            if (hh == a_hh && mm == a_mm && ss == a_ss) {
                alarm_triggered = 1;
            }
        }

        // 2. Update LCD on Timer Tick
        if (update_lcd) {
            // Only refresh display if in Normal Mode (to prevent setting flicker)
            if (system_mode == 0) { 
                refresh_display();
            }
            update_lcd = 0; 
        }
        
        // 3. Button Logic 

        // --- Button: MODE (PB0) ---
        if (BTN_MODE == 0) {
            delay_ms(20);
            if (BTN_MODE == 0) {
                // Cycle through modes: 0 -> 1 -> 2 -> 0...
                system_mode++;
                if (system_mode > 2) system_mode = 0;
                
                cursor_pos = 0;
                alarm_triggered = 0; 
                
                lcd_clear();
                refresh_display(); 
                while(BTN_MODE == 0);
            }
        }

        // --- Button: SELECT (PB1) ---
        if (BTN_SEL == 0) {
            delay_ms(20);
            if (BTN_SEL == 0) {
                if (system_mode > 0) { 
                    cursor_pos++;
                    if (cursor_pos > 2) cursor_pos = 0;
                    refresh_display(); // REFRESH: Update label and cursor position
                }
                while(BTN_SEL == 0);
            }
        }

        // --- Button: INCREMENT (PB2) ---
        if (BTN_INC == 0) {
            delay_ms(20);
            if (BTN_INC == 0) {
                if(alarm_triggered) {
                    alarm_triggered = 0;
                    refresh_display();
                }
                
                // --- Setting Logic ---
                if (system_mode == 1) { 
                    #asm("cli") 
                    if (cursor_pos == 0) { hh++; if (hh >= 24) hh = 0; }
                    else if (cursor_pos == 1) { mm++; if (mm >= 60) mm = 0; }
                    else if (cursor_pos == 2) { ss = 0; } 
                    #asm("sei")
                } 
                else if (system_mode == 2) { 
                    if (cursor_pos == 0) { a_hh++; if (a_hh >= 24) a_hh = 0; }
                    else if (cursor_pos == 1) { a_mm++; if (a_mm >= 60) a_mm = 0; }
                    else if (cursor_pos == 2) { a_ss++; if (a_ss >= 60) a_ss = 0; }
                } 
                else { // Mode 0: TOGGLE ALARM
                    alarm_enabled = !alarm_enabled;
                }
                
                // REFRESH: Show the new number immediately after incrementing
                refresh_display();
                
                // Delay while pressed to allow slow scrolling
                delay_ms(100); 
                while(BTN_INC == 0); 
            }
        }
    }
}