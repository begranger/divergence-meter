//Had to add proc dir to prj-settings bc it wasnt going in for some reason
#include <xc.h>

// Set configuration bits, MSB to LSB
#pragma config CP    = OFF      // Flash mem code protection
#pragma config CPD   = OFF      // Data mem code protection
#pragma config LVP   = ON       // Low voltage programming
#pragma config BOREN = ON       // Brown-out detect/reset
#pragma config MCLRE = ON       // RA5=MCLR
#pragma config PWRTE = ON       // Power-up timer
#pragma config WDTE  = OFF      // Watchdog timer
#pragma config FOSC  = INTOSCIO // Use internal osc, RA6/RA7 are general I/Os

#define _XTAL_FREQ 4000000

void main(void) {
    
    // Set mclk frequency to 4MHz
    PCON |= 1 << 3; // b3 = OSFC, 1 = 4MHz, 0 = 48 kHz
    
    // Setup I/O
    // 0 = output, 1 = input
    TRISA0 = 0b0; // I2C clock (SCL), set to output for now
    TRISA1 = 0b0; // I2C data (SDA), set to output for now
    TRISA2 = 0b1; // SW1 input
    TRISA3 = 0b1; // SW2 input
    TRISA4 = 0b1; // NC, leave as input (output buffer in Hi-Z)
    TRISA5 = 0b1; // MCLR input
    TRISA6 = 0b1; // NC, OSC2 pin
    TRISA7 = 0b1; // NC, OSC1 pin
    //
    TRISB0 = 0b0; // Test LED
    TRISB1 = 0b0; // HVPS enable
    TRISB2 = 0b0; // SERDES clock
    TRISB3 = 0b0; // SERDES blank (active low)
    TRISB4 = 0b0; // SERDES din (also the PGM pin)
    TRISB5 = 0b0; // SERDES latch enable (active low)
    TRISB6 = 0b1; // Programming clock (PGC)
    TRISB7 = 0b1; // Programming data (PGD)
    
    while (1) {
        // Turn LED on
        PORTA = 0xFF;
    }
}
