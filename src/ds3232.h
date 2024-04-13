#ifndef DS3232_H
#define	DS3232_H

uint8_t ds3232_get_time(uint8_t* current_time);
uint8_t ds3232_incr_min(void);
uint8_t ds3232_incr_hour(void);

#endif	/* DS3232_H */

