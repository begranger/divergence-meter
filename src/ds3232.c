#include <xc.h>         // device macros
#include <stdbool.h>    // boolean type macro (just unsigned char under hood)
#include "_XTAL_FREQ.h" // need for __delay_*s()

/* On init, we set the output regs on pins RA<1:0> to 0. Therefore by toggling
 * the ports b/w input and output we effectively toggle b/w putting the pin i
 * hi-z to then be pulled up by external resistor, and connecting the pin to 0
 * (pulling the line low). Init leaves both as inputs (line high)
 * e.g. doing SCL = 0 sets RA0 to an output, enabling the buffer and connecting
 * the pin to the 0 in the RA0 output reg. This pulls the line low (sends 0) */
#define SCL TRISA0 // I2C clock
#define SDA TRISA1 // I2C data

#define I2C_SDA_PIN RA1      // Pin to read I2C input from when receiving
#define I2C_ADDR 0x68        // I2C bus address of DS3232 chip (7 LSBs)
#define I2C_TCLK_US_DIV_3 20 // 1/3 of I2C bus clock period in microseconds

bool byte_out(uint8_t dout) {
    // Assumes that output regs on both pins are set to 0
    // Assumes that both pins are configured as outputs (lines low)
    // Assumes that function is entered at/around the time SCL was brought low
    //   (when first called after START)
    
    // Wait for one Tclk/3 step, then begin w data
    __delay_us(I2C_TCLK_US_DIV_3);
    
    // MSB is always transmitted first
    for (uint8_t i = 0; i < 8; i++) {
        // On first iteration, 7-i = 7, so we put MSB at b0 and transfer out
        // On last iteration, 7-i = 0, so we dont shift at all and send b0 asis
        
        SDA = (dout >> (7-i)) & 1;     // change data
        __delay_us(I2C_TCLK_US_DIV_3); // setup time
        SCL = 1;                       // clock rise
        __delay_us(I2C_TCLK_US_DIV_3); // clock high
        SCL = 0;                       // clock fall
        __delay_us(I2C_TCLK_US_DIV_3); // hold time
    }
    
    // Set SDA to 1 (makes pin an input, disables buffer and allows line to be
    // pulled up) so that device can ack or nack, and we can read it
    SDA = 1;
    __delay_us(I2C_TCLK_US_DIV_3);   // setup time (for device)
    SCL = 1;                         // clock rise
    __delay_us(I2C_TCLK_US_DIV_3/2);
    bool ack = I2C_SDA_PIN;          // Sample pin halfway through clock pulse
    __delay_us(I2C_TCLK_US_DIV_3/2);
    SCL = 0;                         // clock fall
    __delay_us(I2C_TCLK_US_DIV_3);   // hold time
    
    // Bring SDA back to 0 (as we found it, SCL is already there), then add
    // another clock third for buffer before returning (makes it symmetric)
    SDA = 0;
    __delay_us(I2C_TCLK_US_DIV_3);
    
    return ack;
}


uint8_t byte_in(bool last_byte) {
    // Assumes that output regs on both pins are set to 0
    // Assumes that both pins are configured as outputs (lines low)

    // Wait for one Tclk/3 step, then pull SDA high, giving control to prph
    __delay_us(I2C_TCLK_US_DIV_3);
    SDA = 1;
    
    // Clock each bit over and store in shift reg
    uint8_t din = 0;
    for (uint8_t i = 0; i < 8; i++) {
        __delay_us(I2C_TCLK_US_DIV_3);
        SCL = 1;
        __delay_us(I2C_TCLK_US_DIV_3/2);
        din |= I2C_SDA_PIN << (7-i);
        __delay_us(I2C_TCLK_US_DIV_3/2);
        SCL = 0;
        __delay_us(I2C_TCLK_US_DIV_3);
    }
    
    // At this time, device should release (pull up) SDA to let us ACK/NACK,
    // so SDA will be set to whatever it was when we were driving, ie 1.
    // Therefore, if last byte, dont need to change to send NACK (1)
    if (!last_byte) { SDA = 0; }
    __delay_us(I2C_TCLK_US_DIV_3);
    SCL = 1;
    __delay_us(I2C_TCLK_US_DIV_3);
    SCL = 0;
    __delay_us(I2C_TCLK_US_DIV_3);
    
    // Set SDA back to 0 to leave it as we found it and add buffer
    SDA = 0;
    __delay_us(I2C_TCLK_US_DIV_3);
    
    return din;
}


/* Read or write the specified number of bytes to/from a peripheral on I2C bus
 * from/to the given array
 * Inputs:
 *   direction: 1 = read, 0 = write (matches I2C convention)
 *   ctrl_addr: pointer to uint8_t array in controller to being transfer at
 *   prph_addr: address in the peripheral's memory map to begin transfer at
 *   num_bytes: self-explanatory
 * Returns 0 if successful, error code if not */
uint8_t transfer_bytes(bool direction, uint8_t* ctrl_addr, uint8_t prph_addr, uint8_t num_bytes) {
    // Assumes that output regs on both pins are set to 0
    // Assumes that both pins are configured as inputs (lines high)
    // Assumes that bus has been in idle longer than t_BUF = 4.7us
    
    // Signal transfer-start by bringing SDA low while SCL is high, then
    // bring SCL low as well
    SDA = 0;
    __delay_us(I2C_TCLK_US_DIV_3); // t_HD:STA, must be > 4us, use Tclk/3
    SCL = 0;
    
    // Send out the I2C address and R/W flag to select peripheral
    // Note: We always jamset mem pointer when reading. Doing
    // so requires 2 transfers, the first being a write of the
    // address to move pointer to. Therefore ALWAYS send the first
    // I2C address as a write transfer (hence '| 0') below
    // Note: ack is active low, so if zero is returned then all ok
    if (byte_out((I2C_ADDR << 1) | 0)) {
        return 1;
    }

    // Set addr pointer in device
    if (byte_out(prph_addr)) {
        return 2;
    }

    if (direction) { // if read

        // Execute repeated-start to then begin read
        __delay_us(I2C_TCLK_US_DIV_3); // buffer
        SDA = 1;
        __delay_us(I2C_TCLK_US_DIV_3); // t_SU:DAT (even though not transmitting data)
        SCL = 1;
        __delay_us(I2C_TCLK_US_DIV_3); // t_SU:STA > 4.7us
        SDA = 0;
        __delay_us(I2C_TCLK_US_DIV_3); // t_HD:STA > 4.0us
        SCL = 0;

        // Transmit I2C address again, this time w R/!W = 1
        if (byte_out((I2C_ADDR << 1) | 1)) {
            return 3;
        }

        // read all bytes into buffer, sending a NACK (1) on the last one
        // so that device knows to release SDA so we can generate STOP
        for (uint8_t i = 0; i < num_bytes-1; i++) {
            ctrl_addr[i] = byte_in(0);
        }
        ctrl_addr[num_bytes-1] = byte_in(1);
    }
    else { // write
    
        for (uint8_t i = 0; i < num_bytes; i++) {
            if (byte_out(ctrl_addr[i])) { return i+4; }
        }
    }
    
    // Generate STOP and return (assumes both lines are low at this point)
    // stop condition leaves bus in idle (both lines high)
    __delay_us(I2C_TCLK_US_DIV_3); // buffer
    SCL = 1;
    __delay_us(I2C_TCLK_US_DIV_3); // t_SU:STO > 4.7us
    SDA = 1;
    __delay_us(I2C_TCLK_US_DIV_3); // buffer
    return 0; 
}


uint8_t sync_i2c(uint8_t num_clocks) {
    // [Re]sync the I2C bus to a known state. Return 0 if successful, and a
    // postive error code if not.
    // Assumes that output regs on both pins are set to 0
    // Does NOT assume anything about the state of SCL and SDA
    
    // Set SCL high, and 'release' SDA by setting pin to high impedence (SDA=1)
    // Then the only way for SDA to be read as zero is if the DS3232 is pulling
    // it low bc it's desynced or in some weird state. 
    SCL = 1;
    SDA = 1;
    
    // Toggle SCL a bunch and then check if the DS3232 has released SDA (if it
    // was even holding it), and the value on the pin becomes 1. Note that the
    // datasheet says to toggle SCL ~until~ SDA becomes 1, but we just do one
    // set of toggles, check and return either way. This is bc I dont want to
    // get stuck in a loop here- just tell caller that it didnt work, and they
    // can try again if they want and/or do some other error handling
    if (num_clocks == 0) {return 1;}
    for (uint8_t i = 1; i <= num_clocks; i++) {
        __delay_us(I2C_TCLK_US_DIV_3);
        SCL = 0;
        __delay_us(I2C_TCLK_US_DIV_3*2);
        SCL = 1;
    } // Leaves loop w SCL at 1, same as it started
    
    // Pause before checking SDA in case it goes high after last clock
    __delay_us(I2C_TCLK_US_DIV_3);
    
    // If SDA is still not high (DS3232 pulling it down), sync failed
    if (!I2C_SDA_PIN) {return 2;}
            
    // Otherwise, it has returned to high. And since SCL is also already 1, we
    // return successful and w SCL+SDA in state expected by transfer_bytes
    return 0;
}
