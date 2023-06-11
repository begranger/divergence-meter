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
    
    // Set Port B to all outputs
    // 0 = output, 1 = input
    TRISA = 0x00;
    
    while (1) {
        // Turn LED on
        PORTA = 0xFF;
    }
}
