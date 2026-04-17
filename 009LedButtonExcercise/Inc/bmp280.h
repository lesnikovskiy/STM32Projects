#ifndef BMP280_H_
#define BMP280_H_

#include "i2c.h"

void bmp280_init_sensor(void);
uint8_t bmp280_read_id(void);
void bmp280_write_reg(uint8_t reg, uint8_t value);
int32_t bmp280_read_raw_temp(void);


#endif /* BMP280_H_ */
