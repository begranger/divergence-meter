// Had to add proc dir to prj-settings bc it wasnt going in for some reason
#include <xc.h>         // device macros
#include "_XTAL_FREQ.h" // need for __delay_*s()
#include "pic.h"        // pic init and subroutines
#include "ds3232.h"     // clock chip init and subroutines

void check_buttons(void) {
    //if 
}

void blink(uint8_t n) {
    // max val of n = 255, i takes on 0->254, so max num blinks = 255
    for (uint8_t i = 0; i < n; i++) {
        RB0 = 1;         // Turn on LED
        __delay_ms(500); // Wait for 0.5s
        RB0 = 0;         // Turn off LED
        __delay_ms(500); // Wait for 0.5s
    }
    return;
}

void main(void) {
    
    init_pic();
    uint8_t rv = init_ds3232();
    
    while (1) {
        blink(100);
        //RB0 = 1; 
        
        //check_buttons(); // See if button is being pressed and react if so
        //get_time();      // Get time from clock chip and write to tubes
        //__delay_us(100); // Wait for 100us
    }
}
