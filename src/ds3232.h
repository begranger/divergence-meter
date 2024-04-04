#ifndef DS3232_H
#define	DS3232_H

#include <xc.h>
#include <stdbool.h>

uint8_t transfer_bytes(bool direction, uint8_t* ctrl_addr, uint8_t prph_addr, uint8_t num_bytes);

#endif	/* DS3232_H */

