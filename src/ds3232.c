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
    
    // Signal transfer-start by bringing SDA low while SCL is high, then bring
    // SCL low as well
    SDA = 0;
    __delay_us(I2C_TCLK_US_DIV_3); // t_HD:STA, must be > 4us, use Tclk/3
    SCL = 0;
    
    // Send out the I2C address and R/W flag to select peripheral
    char rv = byte_out((I2C_ADDR << 1) | direction);

}


char init_ds3232(void) {
    // Make sure DS3232 (clock chip) is up and running
    return 0x00;
}
