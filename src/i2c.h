#ifndef I2C_H
#define	I2C_H

/* Read or write the specified number of bytes to/from a peripheral on I2C bus
 * from/to the given array
 * Inputs:
 *   direction: 1 = read, 0 = write (matches I2C convention)
 *   i2c_addr:  I2C address of the peripheral on the bus (ie how to activate it)
 *   ctrl_addr: pointer to char array in controller to being transfer at
 *   prph_addr: address in the peripheral's memory map to begin transfer at
 *   num_bytes: self-explanatory
 * Returns 1 if successful, 0 if not
 */
bit transfer_bytes(bit            direction,
                   unsigned char  i2c_addr,
                   unsigned char  ctrl_addr,
                   unsigned char* prph_addr,
                   unsigned char  num_bytes);

#endif	/* I2C_H */
