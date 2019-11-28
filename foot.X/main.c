#include <xc.h>
#include <stdint.h>
#include "seven.h"

/**
 * RC0 => clock 74ls373 display digit
 * RC1 => clock 74ls373 row digit
 * PORTB => display digit/row (74ls373 display and 74ls373 row(
 * PORTA => bit 0 => analog in; bit 1-2 optical coupler responsible for inout counting
 * RA3 => step motor clock
 */

//#include <pic16f872.h>
//typedef uint16_t word;
//word __at 0x2007 CONFIG = _HS_OSC & _WDT_OFF & _PWRTE_OFF & _BODEN_ON & _LVP_OFF & _CPD_OFF & _WRT_ENABLE_OFF & _DEBUG_OFF & _CP_OFF;

#pragma config FOSC=HS, WDTE=OFF, PWRTE=OFF, BOREN=ON, LVP=OFF, CP=OFF, WRT=OFF, DEBUG=OFF, CPD=OFF

#define TMR_VALUE 128

void sleepx(int count)
{
    int i, j;
    
    for (j=0; j<count; j++)
        for (i=0; i<100; i++)
            ;
}

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

/* interrupt service routine */
void __interrupt() tcInt(void)
{   
    if (TMR0IF) {
        time ++;
        
        switch (state) {
        case 0:
            PORTB = display;
            RC4 = 0;        // DISPLAY
            state = 1;
            break;
            
        case 1:
            RC4 = 1;
            state = 2;
            break;
            
        case 2:
            PORTB = ~leds;
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
            dot = PORTCbits.RC3;
            RC3 = 1;

            switch(kstate) {
            case 0:                 // not pressed              
                if (!dot)
                    kstate++;
                else {
                    scan = scan << 1;
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
                    } else
                        leds = scan;
                    kstate++;
                } else
                    kstate = 0;
                break;
                
            case 3:                // wait release
                if (dot)           // trigger event
                    kstate = 0;    //release
                break;
            }
            state = 0;
            break;    
        }
            
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
    
    TRISA = 0;          // PORTA = output
    TRISB = 0;          // PORTB = output
    TRISC = 4;          // PORTC = output RC3 = input
   
    RC3 = 1;
    RC4 = 1;
    RC5 = 1;
    
    GIE = 1;
    
    display = to7(bank);
    
    while(1)
        ;
}

