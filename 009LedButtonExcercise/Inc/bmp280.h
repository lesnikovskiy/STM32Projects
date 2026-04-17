#ifndef BMP280_H_
#define BMP280_H_

#include "i2c.h"

#define BMP280_I2C_ADDR         0x76
#define BMP280_REG_ID           0xD0  // Chip ID register
#define BMP280_REG_RESET        0xE0  // Software reset register
#define BMP280_REG_STATUS       0xF3  // Device status register (measuring / updating)
#define BMP280_REG_CTRL_MEAS    0xF4  // Measurement control (oversampling and mode)
#define BMP280_REG_CONFIG       0xF5  // Configuration register (filter and standby time)
#define BMP280_REG_TEMP_MSB     0xFA  // Temperature MSB (start of 20-bit data)
#define BMP280_REG_CALIB_START  0x88  // Start of calibration data (dig_T1)

typedef struct {
	uint16_t dig_T1; // Calibration coefficient T1
	int16_t dig_T2; // Calibration coefficient T2
	int16_t dig_T3; // Calibration coefficient T3
} BMP280_Calib_Data;

void bmp280_init_sensor(void);
uint8_t bmp280_read_id(void);
void bmp280_write_reg(uint8_t reg, uint8_t value);
int32_t bmp280_read_raw_temp(void);
void bmp280_read_calibration(void);
int32_t bmp280_compensate_T(int32_t adc_T);

#endif /* BMP280_H_ */
