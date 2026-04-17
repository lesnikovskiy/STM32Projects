#ifndef I2C_H_
#define I2C_H_

#include "stm32f401rct6.h"

void i2c_init(void);
void i2c_start_addr(uint8_t addr, uint8_t read);
void i2c_write(uint8_t data);
uint8_t i2c_read(int ack);
void i2c_stop(void);

#endif /* I2C_H_ */
