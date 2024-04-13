#ifndef I2C_H
#define	I2C_H

#include <xc.h>
#include <stdbool.h>

#define I2C_WR 0 // I2C transfer direction definition
#define I2C_RD 1

uint8_t i2c_transfer_bytes(bool direction, uint8_t* ctrl_addr, uint8_t prph_addr, uint8_t num_bytes);
uint8_t i2c_sync_intf(uint8_t num_clocks);

#endif	/* I2C_H */

