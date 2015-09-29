/*
 * File:   display.c
 * Author: neilchloearmstrong
 *
 * Created on September 27, 2015, 10:44 AM
 */
#include "display.h"
#define _XTAL_FREQ 48000000

/*
    PIN_RS = RC0
    PIN_RW = RC1
    PIN_EN = RC2
    PIN_D4 = RA1
    PIN_D5 = RA2
    PIN_D6 = RA3
    PIN_D7 = RA4
 */

extern void delay_ms(unsigned ms);

enum pins
{
    PIN_RS = 0,
    PIN_RW,
    PIN_EN,
    PIN_D4,
    PIN_D5,
    PIN_D6,
    PIN_D7
};

enum directions
{
    OUTPUT = 0,
    INPUT,
};

enum values
{
    LOW  = 0,
    HIGH,
};

void set_pin_mode(enum pins pin, enum directions direction)
{
    switch(pin)
    {
        case PIN_RS:
            TRISC0 = direction;
            break;
        case PIN_RW:
            TRISC1 = direction;
            break;
        case PIN_EN:
            TRISC2 = direction;
            break;
        case PIN_D4:
            TRISA1 = direction;
            break;
        case PIN_D5:
            TRISA2 = direction;
            break;
        case PIN_D6:
            TRISA3 = direction;
            break;
        case PIN_D7:
            TRISA4 = direction;
            break;
    }
}

void set_pin_value(enum pins pin, enum values value)
{
    switch(pin)
    {
        case PIN_RS:
            LATC0 = value;
            break;
        case PIN_RW:
            LATC1 = value;
            break;
        case PIN_EN:
            LATC2 = value;
            break;
        case PIN_D4:
            LATA1 = value;
            break;
        case PIN_D5:
            LATA2 = value;
            break;
        case PIN_D6:
            LATA3 = value;
            break;
        case PIN_D7:
            LATA4 = value;
            break;
    }
}

void display_send(unsigned char value, enum values pin_value);

void display_setup()
{
    set_pin_value(PIN_RS, LOW);
    set_pin_value(PIN_RW, LOW);
    set_pin_value(PIN_EN, LOW);
    
    set_pin_mode(PIN_RS, OUTPUT);
    set_pin_mode(PIN_RW, OUTPUT);
    set_pin_mode(PIN_EN, OUTPUT);
    
    for (int i = 0; i < 4; i++)
        set_pin_value(PIN_D4+i, LOW);
    
    for (int i = 0; i < 4; i++)
        set_pin_mode(PIN_D4+i, OUTPUT);
    
    display_command(0x28);
    display_command(0x0C);
    display_command(0x06);
    display_clear();
}

void display_clear()
{
    display_command(0x01);
    __delay_ms(2);
}

void display_cursor_reset()
{
    display_command(0x02);
    __delay_ms(2);
}

unsigned char row_offsets[] = { 
    0x00, 0x40, 0x14, 0x54 
};
void display_cursor_set(unsigned col, unsigned row)
{
    display_command(0x80 | (col + row_offsets[row%4]));
    __delay_us(40);
}

void display_print(char * txt, unsigned length)
{
    for (int i = 0 ; i < length ; ++i)
        display_write(txt[i]);
}
void display_prints(const char * txt)
{
    while(*txt) {
        display_write(*txt);
        ++txt;
    }
}

void display_write(unsigned char value)
{
    display_send(value, HIGH);
    __delay_us(50);
}

void display_command(unsigned char command)
{
    display_send(command, LOW);
}

static inline void display_tx(unsigned char value, unsigned shift)
{
    int i;
    
    for (int i = 0; i < 4; i++)
        set_pin_value(PIN_D4+i, (value >> (i + shift)) & 0x01);
    __delay_us(20);

    set_pin_value(PIN_EN, HIGH);
    __delay_us(10);
    set_pin_value(PIN_EN, LOW);
    __delay_us(20);
}

void display_send(unsigned char value, enum values pin_value)
{
    set_pin_value(PIN_RS, pin_value);
    set_pin_value(PIN_RW, LOW);
    __delay_us(10);
    
    /* for function set command, tx first 4 bits twice */
    if ((value & 0xF0) == 0x20) {
        display_tx(value, 4);
    }
    
    /* tx high 4 bits */
    display_tx(value, 4);
    
    /* tx low 4 bits */
    display_tx(value, 0);
    
    set_pin_value(PIN_RS, LOW);
    __delay_us(10);
    for (int i = 0; i < 4; i++)
        set_pin_value(PIN_D4+i, LOW);
}
