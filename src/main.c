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

//#define SW

void init_regs(void) {
    // Initialize main SFRs
    
    // STATUS - No changes needed
    // OPTION - No changes needed, all PORTB weak-pullups disabled on POR
    // INTCON - No changes needed, interrupt control reg, GIE=0 on POR
    // PIE1   - No changes needed, periph interrupt enable reg, GIE=0, so DC
    // PIR1   - No changes needed, periph interrupt reg, GIE=0, so dont care
    // PCON   - No changes needed, b3=OSFC=mclk, 1=4MHz (POR val), 0=48kHz
    
    // At power-up, the values in the PORTA and PORTB output regs are unknown
    // (i.e. X's), so set them to known value before changing data directions.
    // TRISA and TRISB come up as all 1's (all inputs), so writing to A and B
    // only changes the internal reg value, not the pin output, but thats ok
    PORTA = 0x00;
    PORTB = 0x00;
    
    // Setup I/O directions
    // 0 = output, 1 = input
    TRISA0 = 0b0; // I2C clock (SCL), set to output for now
    TRISA1 = 0b0; // I2C data (SDA), set to output for now
    TRISA2 = 0b1; // SW1 input
    TRISA3 = 0b1; // SW2 input
    TRISA4 = 0b1; // NC, leave as input (output buffer in Hi-Z)
    TRISA5 = 0b1; // MCLR input, MLCRE=on, so pin is dedicated to MCLR/Vpp
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
    
    // Set comparators on pins RA0-3 to off state, so that those pins are
    // treated as digital inputs (Input mode = D, see FIG 5-1 and 10-1 in DS)
    // Note: RA0 and RA1 are outputs right now, but to do I2C we switch them
    // back and forth b/w input and output, so we need them to work as digital
    // inputs when configured that way.
    CMCON |= 0x07;
    
    // Dont need to disable Vref output on RA2 bc VRCON.VROE=0 on POR
    // (see FIG 5-2 and REGISTER 11-1)
    
    // HERE @ 1130p 6/12/23
    // Just: finished making sure we set up each pin on PORTA correctly
    // Next: do PORTB, go through each pin's circuit diagram and make sure
    //       there isnt anything extra we need to to beside setting direction
    
    return;
}

void init_ds3232(void) {
    // Make sure DS3232 (clock chip) is up and running
}

void init(void) {
    init_regs();
    init_ds3232();
    return;
}

void check_buttons(void) {
    //if 
}

void main(void) {
    
    init();
    
    while (1) {
        //RB0 = 1; // Turn on LED
        //RB1 = 1; // Turn on HV
        
        check_buttons(); // See if button is being pressed and react if so
        get_time();      // Get time from clock chip and write to tubes
        __delay_us(100); // Wait for 100us
    }
}
