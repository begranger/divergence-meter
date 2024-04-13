// Had to add proc dir to prj-settings bc it wasnt going in for some reason
#include <xc.h>         // device macros
#include "_XTAL_FREQ.h" // need for __delay_*s()
#include "pic.h"        // pic init and subroutines
#include "i2c.h"        // for debug, wont need eventually bc everything in rtc
#include "ds3232.h"     // clock chip subroutines


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


void blink_bits(uint8_t to_blink) {
    // MSB first, 0=short, 1=long, P=5000ms
    for (uint8_t i = 0; i < 8; i++) {
        if ((to_blink >> (7-i)) & 1) {
            RB0 = 1; // Long
            __delay_ms(2000);
            RB0 = 0;
            __delay_ms(1000);
        }
        else {
            RB0 = 1; // Short
            __delay_ms(500);
            RB0 = 0;
            __delay_ms(2500);
        }
    }
}


void main(void) {
    
    init_pic();
    while (i2c_sync_intf(50)) {} // Bring I2C bus into known state
    
    __delay_ms(5000);
    
    uint8_t current_time[3] = {0, 0, 0};
    i2c_transfer_bytes(0x68, 0, &current_time, 0x00, 3);
    
    if (ds3232_get_time(current_time)) {RB0 = 1; while (1) {}}
    blink_bits(current_time[1]);
    
    if (ds3232_incr_min()) {RB0 = 1; while (1) {}}
    __delay_ms(5000);
    
    if (ds3232_get_time(current_time)) {RB0 = 1; while (1) {}}
    blink_bits(current_time[1]);

    // Turn on LED to indicate startup completed successfully
    //RB0 = 1;
    
//    uint8_t dout = 0b10101010;
//    uint8_t din  = 0;
//    
//    transfer_bytes(WR, &dout, 0x14, 1);
//    __delay_ms(1000);
//    transfer_bytes(RD, &din, 0x00, 1);
//    __delay_ms(1000);
//    blink_bits(din);
    
    while (1) {
        //blink(100);
        //RB0 = 1; 
        
        //check_buttons(); // See if button is being pressed and react if so
        //get_time();      // Get time from clock chip and write to tubes
        //__delay_us(100); // Wait for 100us
    }
}
