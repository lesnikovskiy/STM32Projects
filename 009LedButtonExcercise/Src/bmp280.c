#include "bmp280.h"

#define BMP280_I2C_ADDR         0x76
#define BMP280_REG_ID           0xD0  // Chip ID register
#define BMP280_REG_RESET        0xE0  // Software reset register
#define BMP280_REG_STATUS       0xF3  // Device status register (measuring / updating)
#define BMP280_REG_CTRL_MEAS    0xF4  // Measurement control (oversampling and mode)
#define BMP280_REG_CONFIG       0xF5  // Configuration register (filter and standby time)
#define BMP280_REG_TEMP_MSB     0xFA  // Temperature MSB (start of 20-bit data)

void bmp280_init_sensor(void) {
	// Set 0x27 - normal mode, temperature x1, pressure x1
	bmp280_write_reg(0xF4, 0x27);
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
