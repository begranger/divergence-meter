#include <xc.h> // device macros

// Set configuration bits, MSB to LSB
#pragma config CP    = OFF      // Flash mem code protection
#pragma config CPD   = OFF      // Data mem code protection
#pragma config LVP   = ON       // Low voltage programming
#pragma config BOREN = ON       // Brown-out detect/reset
#pragma config MCLRE = ON       // RA5=MCLR
#pragma config PWRTE = ON       // Power-up timer
#pragma config WDTE  = OFF      // Watchdog timer
#pragma config FOSC  = INTOSCIO // Use internal osc, RA6/RA7 are general I/Os

void init_pic(void) {
    // Core SFRs --------------------------------------------------------------
    // STATUS - No changes needed
    // OPTION - No changes needed, all PORTB weak-pullups disabled on POR
    // INTCON - No changes needed, interrupt control reg, GIE=0 on POR
    // PIE1   - No changes needed, periph interrupt enable reg, GIE=0, so DC
    // PIR1   - No changes needed, periph interrupt reg, GIE=0, so dont care
    // PCON   - No changes needed, b3=OSFC=mclk, 1=4MHz (POR val), 0=48kHz

    // Port A -----------------------------------------------------------------
    // At power-up, the values in the PORTA and PORTB output regs are unknown
    // (i.e. X's), so set them to known value before changing data directions.
    // TRISA and TRISB come up as all 1's (all inputs), so writing to A and B
    // only changes the internal reg value, not the pin output, but thats ok
    PORTA = 0x00;
    
    // Pin directions: 0 = output, 1 = input
    TRISA0 = 0b1; // I2C clock (SCL), set as input to pullup line for now
    TRISA1 = 0b1; // I2C data (SDA), set as input to pullup line for now
    TRISA2 = 0b1; // SW1 input
    TRISA3 = 0b1; // SW2 input
    TRISA4 = 0b1; // NC, leave as input (output buffer in Hi-Z)
    TRISA5 = 0b1; // MCLR input, MLCRE=on, so pin is dedicated to MCLR/Vpp
    TRISA6 = 0b1; // NC, OSC2 pin
    TRISA7 = 0b1; // NC, OSC1 pin

    // Pin circuits
    // RA<3:0> are configured as inputs. To function as such, we must set
    // the comparators on pins RA<3:0> to off state, so that those pins are
    // treated as digital inputs (Input mode = D, see FIG 5-1 and 10-1 in DS).
    // On POR, the comparators come up in the Reset state which treats the pins
    // as analog inputs and I assume sets 'Analog Input Mode' in Figs 5-{1,2,3}
    // to 1, thus disabling the digital FF we want to use to sample the input.
    // Therefore we have to turn them off via write of non-default value 0x07.
    // Note: to do I2C we switch RA<1:0> back and forth b/w input and output,
    // so we need them to work as digital inputs when configured that way.
    CMCON |= 0x07;
    // Dont need to disable Vref output on RA2 bc VRCON.VROE=0 on POR
    // (see FIG 5-2 and REGISTER 11-1)
    // Dont care about Comparator Output mux on RA3 bc pin is an configured as
    // in input, so the buffer is open.
    // Dont care about anything in RA4 bc its a NC.
    // RA5 cannot be written to or read from, dedicated to MCLR line
    // Dont care about anything in RA<7:6> bc they're NCs.

    // Port B -----------------------------------------------------------------
    PORTB = 0x00; // Set output reg to known state

    // Pin directions
    TRISB0 = 0b0; // Test LED
    TRISB1 = 0b0; // HVPS enable
    TRISB2 = 0b0; // SERDES clock
    TRISB3 = 0b0; // SERDES blank (active low)
    TRISB4 = 0b0; // SERDES din (also the PGM pin)
    TRISB5 = 0b0; // SERDES latch enable (active low)
    TRISB6 = 0b1; // Programming clock (PGC)
    TRISB7 = 0b1; // Programming data (PGD)

    // Pin circuits
    // All PORTB pins have an optional internal weak-pullup. However, its
    // turned off by default, i.e. NotRBPU=1 on POR, so I assume the gate is
    // left open and Vdd is cut off from the circuit there, so we dont need
    // to do anything with it for any of the pins.
    // For RA0, dont care about interrupt line out bc GIE=0 on POR.
    // For RA<2:1>, SPEN is 0 on POR, so the USART lines are not selected in
    // in the mux that drives output buffer. Not sure what '!Peripheral OE' is
    // but its a dont-care bc TRIS Latch is 0 (bc output), so the AND output is
    // always zero and therefore the output buffer is always closed (enabled).
    // For RB3, I cant figure out what they mean by putting 'CCP1CON' as the
    // output mux select line, since thats a whole register, not a field in one.
    // I'm going to assume that whatever it means, the CCP output is not
    // not selected after POR, and the chip takes Data Latch as output by
    // default- may need to verify this in hardware (TODO). This pin also has
    // the '!Peripheral OE' signal, which is again a dont care bc TRIS Latch is
    // a 0 (bc output), so the output buffer is always enabled.
    // Pin RB4 is the PGM pin. This program configures the pin as an output,
    // however, the LVP bit is enabled by default (and we dont turn it off), so
    // the output buffer will remain off, even though we've set TRISB4 = 0.
    // Unless we get a PICkit to do HVP, this pin must remain an input. And we
    // need it to be an output bc of the board traces... so need PICkit. Don't
    // care about the RBIF circuitry be GIE = 0
    // Pin RB5 is all good as an output, dont care about RBIF circuitry for
    // same reason as RB4 above
    // Pins RB<7:6> are configured as inputs. Output buffer is disabled bc
    // TRIS Latch = 1. Bit T1OSCEN is 0 on POR, so the TMR1 Oscillator buffer
    // is disabled and therefor not driving pin (in contention w input). Need
    // to be inputs bc they are the programming clock and data lines, although
    // they might get overriden in that mode regardless of TRIS bits, so are
    // probably don't cares. Also don't care about the RBIF circuitry for the
    // same reason as RB4 and RB5 above.
    // All pins done!
    return;
}

