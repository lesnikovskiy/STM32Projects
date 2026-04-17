#include "i2c.h"

void i2c_init(void) {
	I2C->CR1 &= ~(1 << 0); // turn off before the setup

	// Frequency 16MHz
	I2C->CR2 = 16;

	// Setup 100kHz (Standard Mode)
	// CCR = 16 000 000 / (2 * 100 000) = 80
	I2C->CCR = 80;

	// Maximum trigger rise (1000ns / 62.5ns + 1)
	I2C->TRISE = 17;

	I2C->CR1 |= (1 << 0); // turn on
}

void i2c_start_addr(uint8_t addr, uint8_t read) {
	I2C->CR1 |= (1 << 8); // START bit
	while (!(I2C->SR1 & (1 << 0))); // Wait SB bit

	I2C->DR = (addr << 1) | read; // R/W bit
	while (!(I2C->SR1 & (1 << 1))); // Wait ADDR bit (address accepted)
	(void) I2C->SR2; // Clear ADDR bit by reading SR2
}

void i2c_write(uint8_t data) {
	while (!(I2C->SR1 & (1 << 7))); // Wait TxE (Data Register Empty)
	I2C->DR = data;
	while (!(I2C->SR1 & (1 << 2))); // Wait BTF (Byte Transfer Finish)
}

uint8_t i2c_read(int ack) {
	if (ack)
		I2C->CR1 |= (1 << 10); // Send ACK
	else
		I2C->CR1 &= ~(1 << 10); // Send NACK

	while (!(I2C->SR1 & (1 << 6))); // Wait RxNE (Data Register not empty)

	return (uint8_t) I2C->DR;
}

void i2c_stop(void) {
	I2C->CR1 |= (1 << 9); // STOP
}
