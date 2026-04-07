#ifndef DELAY_H
#define DELAY_H

#include <stdint.h>

/* Public functions */
void delay(uint32_t count);
void delay_ms(uint32_t ms);  /* Approximate, based on loop count */

#endif /* DELAY_H */
