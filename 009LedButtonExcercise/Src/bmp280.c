#include "bmp280.h"

BMP280_Calib_Data calib;

void bmp280_init_sensor(void) {
	// Set 0x27 - normal mode, temperature x1, pressure x1
	bmp280_write_reg(BMP280_REG_CTRL_MEAS, 0x27);
}

uint8_t bmp280_read_id(void) {
	uint8_t id;

	i2c_start_addr(BMP280_I2C_ADDR, 0);
	i2c_write(BMP280_REG_ID);

	i2c_start_addr(BMP280_I2C_ADDR, 1); // Restart + Read
	id = i2c_read(0); // Read and send NACK
	i2c_stop();

	return id;
}

void bmp280_write_reg(uint8_t reg, uint8_t value) {
	i2c_start_addr(BMP280_I2C_ADDR, 0); // Write Mode
	i2c_write(reg);
	i2c_write(value);
	i2c_stop();
}

int32_t bmp280_read_raw_temp(void) {
	uint8_t msb, lsb, xlsb;

	// Read from register 0xFA (Temperature)
	i2c_start_addr(BMP280_I2C_ADDR, 0); // Write mode
	i2c_write(BMP280_REG_TEMP_MSB);

	// Restart and read the 3 bytes
	i2c_start_addr(BMP280_I2C_ADDR, 1); // Read mode

	msb = i2c_read(1); // Read MSB, send ACK
	lsb = i2c_read(1); // Read LSB, send ACK
	xlsb = i2c_read(0); // Read XLSB, send NACK
	i2c_stop();

	int32_t raw_temp = (int32_t) ((msb << 12) | (lsb << 4) | (xlsb >> 4));

	return raw_temp;
}

void bmp280_read_calibration(void) {
	uint8_t buf[6];

	i2c_start_addr(BMP280_I2C_ADDR, 0);
	i2c_write(BMP280_REG_CALIB_START);

	i2c_start_addr(BMP280_I2C_ADDR, 1);
	for (int i = 0; i < 5; i++) {
		buf[i] = i2c_read(1); // Read with ACK
	}
	buf[5] = i2c_read(0); // Read with NACK
	i2c_stop();

	calib.dig_T1 = (uint16_t) ((buf[1] << 8) | buf[0]);
	calib.dig_T2 = (int16_t) ((buf[3] << 8) | buf[2]);
	calib.dig_T3 = (int16_t) ((buf[5] << 8) | buf[4]);
}

int32_t bmp280_compensate_T(int32_t adc_T) {
	int32_t var1, var2, T;
	var1 = ((((adc_T >> 3) - ((int32_t) calib.dig_T1 << 1))) * ((int32_t) calib.dig_T2)) >> 11;
	var2 = (((((adc_T >> 4) - ((int32_t) calib.dig_T1)) * ((adc_T >> 4) - ((int32_t) calib.dig_T1))) >> 12)
			* ((int32_t) calib.dig_T3)) >> 14;

	int32_t t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	return T;
}
