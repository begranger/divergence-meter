#include <xc.h>

// On init, we set the output regs on pins RA<1:0> to 0. Therefore by toggling
// the ports b/w input and output we effectively toggle b/w connecting putting
// the pin in hi-z to then be pulled up by external resistor, and connecting
// the pin to 0 (pulling the line low). Init leaves both as inputs (line high)
// e.g. doing SCL = 0 sets RA0 to an output, enabling the buffer and connecting
// the pin to the 0 in the RA0 output reg. This pulls the line low (sends 0)
#define SCL TRISA0 // I2C clock
#define SDA TRISA1 // I2C data

#define _XTAL_FREQ 4000000

bit byte_out(unsigned char byte) {
    // Assumes that output regs on both pins are set to 0
    // Assumes that both pins are configured as outputs (lines low)
    // Assumes that function is entered at/around the time SCL was brought low
    
    // MSB is always transmitted first
    for (unsigned char i = 0; i < 8; i++) {
        // On first iteration, 7-i = 7, so we put MSB at b0 and transfer out
        // On last iteration, 7-i = 0, so we dont shift at all and send b0 asis
        
        // Wait half a low-clock time (quarter Tclk) then change data
        HERE @ 11p 6/16/23 - draw timing of data and clock, then maybe define
        macros for the various time intervals, eg T_L1, T_L2, etc
        
        
    }
    
}

bit transfer_bytes(bit direction, unsigned char i2c_addr, unsigned char ctrl_addr, unsigned char* prph_addr, unsigned char num_bytes) {
    // Assumes that output regs on both pins are set to 0
    // Assumes that both pins are configured as inputs (lines high)
    
    // Signal transfer-start by bringing SDA low while SCL is high, then bring
    // SCL low as well
    SDA = 0;
    __delay_us(20); // t_HD:STA
    SCL = 0;
    
    // Send out the I2C address and R/W flag to select peripheral
    byte_out((i2c_addr << 1) | direction);

}



