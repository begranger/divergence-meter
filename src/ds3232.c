#include <xc.h>      // device macros
#include "i2c.h"     // need for RD/WR defs and functions

#define DS3232_I2C_ADDR 0x68 // I2C bus address of DS3232 chip (7 LSBs)


uint8_t ds3232_get_time(uint8_t* current_time) {
    // Return current time in given array of 3 uint8s: [sec, min, hour]
    // Each element in BCD format matching DS3232 datasheet. Did not convert
    // them to regular binary bc keeping them in BCD actually makes figuring 
    // out what value to set each tube to easier (which is the point of BCD)
    // Returns error code if something went wrong, zero otherwise

    // Get reg vals from DS3232
    return i2c_transfer_bytes(DS3232_I2C_ADDR, I2C_RD, current_time, 0x00, 3);
}


uint8_t ds3232_incr_min(void) {
    // Add one to the minute counter in DS3232, return error code if fail
    
    // Get current minute count
    uint8_t current_min = 0;
    if (i2c_transfer_bytes(DS3232_I2C_ADDR, I2C_RD, &current_min, 0x01, 1)) {return 1;}
    
    // Extract BCD components
    uint8_t current_1min  = current_min & 16;
    uint8_t current_10min = current_min >> 4;
    
    uint8_t next_1min  = 0;
    uint8_t next_10min = 0;
    
    // Calculate next counts
    if (current_1min == 9) {
        
        next_1min = 0;
        
        if (current_10min == 5) {
            next_10min = 0;
        }
        else {
            next_10min = current_10min + 1;
        }
    }
    else {
        next_1min  = current_1min + 1;
        next_10min = current_10min;
    }
    
    // Bitpack and write into DS3232
    uint8_t next_min = (next_10min << 4) | next_1min;
    if (i2c_transfer_bytes(DS3232_I2C_ADDR, I2C_WR, &next_min, 0x01, 1)) {return 2;}
    
    return 0;
}


uint8_t ds3232_incr_hour(void) {
    // Add one to the hour counter in DS3232, return error code if fail
    // Note: function will always set 12/!24 bit to 1, and !AM/PM to 0, so the
    //       value of those bits in the input is overwritten
}
