#include <xc.h>         // device macros
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

char byte_out(char byte) {
    // Assumes that output regs on both pins are set to 0
    // Assumes that both pins are configured as outputs (lines low)
    // Assumes that function is entered at/around the time SCL was brought low
    //   (when first called after START)
    
    // Wait for one Tclk/3 step, then begin w data
    __delay_us(I2C_TCLK_US_DIV_3);
    
    // MSB is always transmitted first
    for (char i = 0; i < 8; i++) {
        // On first iteration, 7-i = 7, so we put MSB at b0 and transfer out
        // On last iteration, 7-i = 0, so we dont shift at all and send b0 asis
        
        SDA = (byte >> (7-i)) & 1;     // change data
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
    char ack = I2C_SDA_PIN;          // Sample pin halfway through clock pulse
    __delay_us(I2C_TCLK_US_DIV_3/2);
    SCL = 0;                         // clock fall
    __delay_us(I2C_TCLK_US_DIV_3);   // hold time
    
    // Bring SDA back to 0 (as we found it, SCL is already there), then add
    // another clock third for buffer before returning (makes it symmetric)
    SDA = 0;
    __delay_us(I2C_TCLK_US_DIV_3);
    return ack;
}


char byte_in(last_byte) {
    // Assumes that output regs on both pins are set to 0
    // Assumes that both pins are configured as outputs (lines low)

    // Wait for one Tclk/3 step, then pull SDA high, giving control to prph
    __delay_us(I2C_TCLK_US_DIV_3);
    SDA = 1;
    
    // Clock each bit over and store in shift reg
    char shift_ref = 0;
    for (char i = 0;, i < num_bytes; i++) {
        __delay_us(I2C_TCLK_US_DIV_3);
        SCL = 1;
        __delay_us(I2C_TCLK_US_DIV_3/2);
        shift_ref |= I2C_SDA_PIN << (7-i);
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
    
    return shift_reg;
}


/* Read or write the specified number of bytes to/from a peripheral on I2C bus
 * from/to the given array
 * Inputs:
 *   direction: 1 = read, 0 = write (matches I2C convention)
 *   ctrl_addr: pointer to char array in controller to being transfer at
 *   prph_addr: address in the peripheral's memory map to begin transfer at
 *   num_bytes: self-explanatory
 * Returns 0 if successful, error code if not */
char transfer_bytes(bit direction, char* ctrl_addr, char prph_addr, char num_bytes) {
    // Assumes that output regs on both pins are set to 0
    // Assumes that both pins are configured as inputs (lines high)
    
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
    if (byte_out((I2C_ADDR << 1) | 0) {
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
        if (byte_out((I2C_ADDR << 1) | 1) {
            return 3;
        }

        // read all bytes into buffer, sending a NACK (1) on the last one
        // so that device knows to release SDA so we can generate STOP
        for (char i = 0; i < num_bytes-1; i++) {
            ctrl_addr[i] = byte_in(0);
        }
        ctrl_addr[num_bytes-1] = byte_in(1);
    }
    else { // write
    
        for (char i = 0; i < num_bytes; i++) {
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


char init_ds3232(void) {
    // Make sure DS3232 (clock chip) is up and running
    return 0;
}
