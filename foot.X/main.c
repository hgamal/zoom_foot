#include <xc.h>
#include <stdint.h>
#include "seven.h"

/**
 * PORTB => display digit/row leds
 * RA2 => bit 3 => input
 * RC4 => bit 5 => DISPLAY strobe
 * RC5 => bit 6 => LEDS strobe
 */

//#include <pic16f872.h>
//typedef uint16_t word;
//word __at 0x2007 CONFIG = _HS_OSC & _WDT_OFF & _PWRTE_OFF & _BODEN_ON & _LVP_OFF & _CPD_OFF & _WRT_ENABLE_OFF & _DEBUG_OFF & _CP_OFF;

#pragma config FOSC=HS, WDTE=OFF, PWRTE=OFF, BOREN=ON, LVP=OFF, CP=OFF, WRT=OFF, DEBUG=OFF, CPD=OFF

#define TMR_VALUE 128

#ifdef NEVER
void sleepx(int count)
{
    int i, j;
    
    for (j=0; j<count; j++)
        for (i=0; i<100; i++)
            ;
}
#endif

uint8_t display;
uint8_t state = 0;
uint8_t dot = 0;
uint8_t leds = 0;
uint8_t count = 0;
uint32_t time = 0;
uint32_t old = 0;
uint8_t scan = 1;
uint8_t bank = 0;
uint8_t kstate = 0;
uint8_t display_on = 0;

void inline keyb_state_machine()
{
    dot = PORTAbits.RA2;

    switch(kstate) {
    case 0:                 // not pressed              
        if (!dot)
            kstate++;
        else {
            scan <<= 1;
            if (scan > 16)
                scan = 1;
        }
        break;

    case 1:                // first press
        if (!dot)
            kstate++;
        else
            kstate = 0;
        break;

    case 2:               // yes it is pressed
        if (!dot) {       // trigger event
            if (scan == 16) {
                bank = (bank + 1) % 16;
                display = to7(bank);
            } else {
                if (leds == scan)
                    leds &= ~scan;
                else
                    leds = scan;
            }
            kstate++;
        } else
            kstate = 0;
        break;

    case 3:                // wait release
        if (dot)           // trigger event
            kstate = 0;    //release
        break;
    }
}

void inline display_state_machine()
{
    switch (state) {
    case 0:
        PORTB = display;
        RC4 = 0;        // DISPLAY
        if (++display_on == 5) {
            state = 1;
            display_on = 0;
        }
        break;

    case 1:
        RC4 = 1;
        state = 2;
        break;

    case 2:
        PORTB = ~(leds & ~16);
        RC5 = 0;
        state = 3;
        break;

    case 3:
        RC5 = 1;
        state = 4;
        break;

    case 4:
        PORTB = ~scan;
        state = 5;
        break;

    case 5:
        keyb_state_machine();
        state = 6;
        break;
            
    case 6:
        PORTB = ~(leds & 16);
        RC5 = 0;
        state = 7;
        break;
        
    case 7:
        state = 0;
        RC5 = 1;
        break;
    }

}

/* interrupt service routine */
void __interrupt() tcInt(void)
{   
    if (TMR0IF) {
        time ++;
        if (time % 200 == 0) {
            if (leds & 16)
                leds &= ~16;
            else
                leds |= 16;
        }
        
        display_state_machine();
            
        TMR0 = TMR_VALUE;
        TMR0IF = 0;
    }
}

void main() {
    // OPTION_REG 
    T0CS = 0;           // TMR0 Clock Source Select bit = Internal instruction cycle clock (CLKOUT)
    
    TMR0IE = 1;         // TMR0 Overflow Interrupt Enable bit = Enables the TMR0 interrupt
    
    PSA = 0;            // Assign prescaler to Timer0 Module
    PS2 = 1;            // PS{2:1:0} = 111 => TMR0 Rate = 1:256
    PS1 = 0;
    PS0 = 0;
    TMR0 = TMR_VALUE;

    nRBPU = 0;          // PORTB Pull-up Enable bit = PORTB pull-ups are enabled by individual port latch values
    ADCON1 = 0x8e;      // PORTA = digital
    
    TRISA = 4;          // PORTA = output, RA2 = input
    TRISB = 0;          // PORTB = output
    TRISC = 0;          // PORTC = output
   
    RC4 = 1;
    RC5 = 1;
    
    GIE = 1;
    
    display = to7(bank);
    
    while(1)
        ;
}

