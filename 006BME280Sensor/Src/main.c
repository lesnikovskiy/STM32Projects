#include <stdint.h>
#include <stdio.h>

#define RCC_BASE      0x40023800
#define RCC_AHB1ENR   *(volatile uint32_t*)(RCC_BASE + 0x30)
#define RCC_APB1ENR   *(volatile uint32_t*)(RCC_BASE + 0x40)

// Registers for UART
#define GPIOA_BASE    0x40020000
#define GPIOA_MODER   *(volatile uint32_t*)(GPIOA_BASE + 0x00)
#define GPIOA_AFRL    *(volatile uint32_t*)(GPIOA_BASE + 0x20)

// Registers for I2C
#define GPIOB_BASE    0x40020400
#define GPIOB_MODER   *(volatile uint32_t*)(GPIOB_BASE + 0x00)
#define GPIOB_AFRL    *(volatile uint32_t*)(GPIOB_BASE + 0x20)
#define GPIOB_OTYPER  *(volatile uint32_t*)(GPIOB_BASE + 0x04)
#define GPIOB_PUPDR   *(volatile uint32_t*)(0x4002040C)

#define USART2_BASE   0x40004400
#define USART2_SR     *(volatile uint32_t*)(USART2_BASE + 0x00)
#define USART2_DR     *(volatile uint32_t*)(USART2_BASE + 0x04)
#define USART2_BRR    *(volatile uint32_t*)(USART2_BASE + 0x08)
#define USART2_CR1    *(volatile uint32_t*)(USART2_BASE + 0x0C)

#define I2C1_BASE     0x40005400
#define I2C1_CR1      *(volatile uint32_t*)(I2C1_BASE + 0x00)
#define I2C1_CR2      *(volatile uint32_t*)(I2C1_BASE + 0x04)
#define I2C1_DR       *(volatile uint32_t*)(I2C1_BASE + 0x10)
#define I2C1_SR1      *(volatile uint32_t*)(I2C1_BASE + 0x14)
#define I2C1_SR2      *(volatile uint32_t*)(I2C1_BASE + 0x18)
#define I2C1_CCR      *(volatile uint32_t*)(I2C1_BASE + 0x1C)
#define I2C1_TRISE    *(volatile uint32_t*)(I2C1_BASE + 0x20)

void UART2_Init(void) {
	RCC_AHB1ENR |= (1 << 0); // GPIOA EN
	RCC_APB1ENR |= (1 << 17); // USART2 EN

	// PA2 (TX) => Alternate Function AF7
	// AF7 connects USART2 periferals
	GPIOA_MODER &= ~(3 << 4); // Reset bits 4 and 5
	GPIOA_MODER |= (2 << 4); // Set AF (10)
	GPIOA_AFRL &= ~(0xF << 8); // 0xF = 1111
	GPIOA_AFRL |= (7 << 8); // 7 = 0111

	// Setup Baud Rate 115200: 16MHz / (16 * 115200) = 8.68 -> BRR = 0x8B
	USART2_BRR = 0x008B;
	USART2_CR1 |= (1 << 3); // (TE) (Transmit Enable)
	USART2_CR1 |= (1 << 13); // UE (UART Enable)
}

void UART2_SendChar(char ch) {
	while (!(USART2_SR & (1 << 7))); // Wait until TXE=1
	USART2_DR = ch;
}

int _write(int file, char *ptr, int len) {
	for (int i = 0; i < len; i++) {
		UART2_SendChar(*ptr++);
	}
	return len;
}

void I2C1_Init(void) {
	RCC_AHB1ENR |= (1 << 1); // GPIOB EN
	RCC_APB1ENR |= (1 << 21); // I2C1 EN

	// Enable pull-up resistors first to ensure lines default to "High" (1)
	GPIOB_PUPDR &= ~((3 << 12) | (3 << 14));
	GPIOB_PUPDR |= ((1 << 12) | (1 << 14));

	// PB6, PB7 -> Alternate Function AF4, Open-Drain
	GPIOB_MODER &= ~((3 << 12) | (3 << 14));
	GPIOB_MODER |= ((2 << 12) | (2 << 14));
	GPIOB_OTYPER |= ((1 << 6) | (1 << 7));
	GPIOB_AFRL |= ((4 << 24) | (4 << 28));

	I2C1_CR1 &= ~(1 << 0); // Turn OFF I2C
	I2C1_CR2 = 16; // Frequencey 16MHz

	// Configuration for 100 kHz (Standard Mode):
	// CCR = PCLK1 / (StandardMode_Speed * 2) = 16,000,000 / (100,000 * 2) = 80
	I2C1_CCR = 80;
	// TRISE = (Max Rise Time / Period APB1) + 1 = (1000ns / 62.5ns) + 1 = 17
	I2C1_TRISE = 17;

	I2C1_CR1 |= (1 << 0);  // Turn ON
}

void BMP280_WakeUp(void) {
	I2C1_CR1 |= (1 << 8); // START
	while (!(I2C1_SR1 & (1 << 0)));
	I2C1_DR = (0x76 << 1);
	while (!(I2C1_SR1 & (1 << 1)));
	(void) I2C1_SR2;

	I2C1_DR = 0xF4;
	while (!(I2C1_SR1 & (1 << 7))); // 7 bit is NOSTRETCH

	/*
	 * 0x2E = 001 011 10
	 * 0x2F = 001 011 11
	 * 001 Temperature oversampling x1
	 * 011 Pressure oversampling x4
	 * 10 Mode: Forced
	 * 11 Mode: Normal
	 * */
	//I2C1_DR = 0x2E; // Wake up in forced mode!!!
	I2C1_DR = 0x2F; // Wake up in normal mode!!!
	while (!(I2C1_SR1 & (1 << 7))); // 7 bit is TxE

	I2C1_CR1 |= (1 << 9); // STOP
}

// Temperature data
uint16_t dig_T1;
int16_t dig_T2;
int16_t dig_T3;

// Pressure data
uint16_t dig_P1;
int16_t dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
int32_t t_fine; // common variable to calculcate pressure;

void BMP280_ReadCalibration(void) {
	uint8_t data[24];

	// Read 6 bytes starting from 0x88
	I2C1_CR1 |= (1 << 8); // START
	while (!(I2C1_SR1 & (1 << 0)));
	I2C1_DR = (0x76 << 1);
	while (!(I2C1_SR1 & (1 << 1)));
	(void) I2C1_SR2;

	I2C1_DR = 0x88; // Start calibration register
	while (!(I2C1_SR1 & (1 << 7))); // Wait until we sent data out TxE
	while (!(I2C1_SR1 & (1 << 2))); // Wait BTF (important before the restart)

	I2C1_CR1 |= (1 << 8); // ReSTART
	while (!(I2C1_SR1 & (1 << 0))); //
	I2C1_DR = (0x76 << 1) | 1;
	while (!(I2C1_SR1 & (1 << 1)));
	(void) I2C1_SR2;

	I2C1_CR1 |= (1 << 10); // Turn on ACK

	for (int i = 0; i < 24; i++) {
		if (i == 23) {
			I2C1_CR1 &= ~(1 << 10); // Send NACK for the last byte
			I2C1_CR1 |= (1 << 9);   // Generate STOP condition immediately
		}

		// Wait for byte to appear in DR
		while (!(I2C1_SR1 & (1 << 6))); // Wait RxNE

		// Read the byte
		data[i] = I2C1_DR;
	}

	// Temperature: Merging bytes (Little Endian - Least Significant Byte comes first)
	dig_T1 = (uint16_t) ((data[1] << 8) | data[0]);
	dig_T2 = (int16_t) ((data[3] << 8) | data[2]);
	dig_T3 = (int16_t) ((data[5] << 8) | data[4]);
	// Pressure
	dig_P1 = (uint16_t) ((data[7] << 8) | data[6]);
	dig_P2 = (int16_t) ((data[9] << 8) | data[8]);
	dig_P3 = (int16_t) ((data[11] << 8) | data[10]);
	dig_P4 = (int16_t) ((data[13] << 8) | data[12]);
	dig_P5 = (int16_t) ((data[15] << 8) | data[14]);
	dig_P6 = (int16_t) ((data[17] << 8) | data[16]);
	dig_P7 = (int16_t) ((data[19] << 8) | data[18]);
	dig_P8 = (int16_t) ((data[21] << 8) | data[20]);
	dig_P9 = (int16_t) ((data[23] << 8) | data[22]);
}

int32_t BMP280_Compensate_T(int32_t adc_T) {
	int32_t var1, var2, T;
	var1 = ((((adc_T >> 3) - ((int32_t) dig_T1 << 1))) * ((int32_t) dig_T2))
			>> 11;
	var2 = (((((adc_T >> 4) - ((int32_t) dig_T1))
			* ((adc_T >> 4) - ((int32_t) dig_T1))) >> 12) * ((int32_t) dig_T3))
			>> 14;

	int32_t t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;

	return T;
}

uint32_t BMP280_Compensate_P(int32_t adc_P) {
	int64_t var1, var2, p;
	var1 = ((int64_t) t_fine) - 128000;
	var2 = var1 * var1 * (int64_t) dig_P6;
	var2 = var2 + ((var1 * (int64_t) dig_P5) << 17);
	var2 = var2 + (((int64_t) dig_P4) << 35);
	var1 = ((var1 * var1 * (int64_t) dig_P3) >> 8)
			+ ((var1 * (int64_t) dig_P2) << 12);
	var1 = (((((int64_t) 1) << 47) + var1)) * ((int64_t) dig_P1) >> 33;
	if (var1 == 0)
		return 0; // avoid division by zero

	p = 1048576 - adc_P;
	p = (((p << 31) - var2) * 3125) / var1;
	var1 = (((int64_t) dig_P9) * (p >> 13) * (p >> 13)) >> 25;
	var2 = (((int64_t) dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((int64_t) dig_P7) << 4);

	return (uint32_t) p; // Pressure in Q24.8 format (divide by 256 for Pascals)
}

uint8_t BMP280_ReadID(void) {
	// 1. Start
	I2C1_CR1 |= (1 << 8); // 8 start bit
	while (!(I2C1_SR1 & (1 << 0))); // 0 SB bit

	// 2. Sensor Address (0x76 << 1) + Write (0)
	I2C1_DR = (0x76 << 1);
	while (!(I2C1_SR1 & (1 << 1))); // 2 bit ADDRESS
	(void) I2C1_SR2; // Clear ADDRESS flag

	// 3. Register ID (0xD0)
	I2C1_DR = 0xD0;
	while (!(I2C1_SR1 & (1 << 7))); // 7 TxE bit

	// 4. Restart for reading
	I2C1_CR1 |= (1 << 8); // 8 START bit
	while (!(I2C1_SR1 & (1 << 0))); // 0 SB bit

	// 5. Address (0x76 << 1) + Read (1)
	I2C1_DR = (0x76 << 1) | 1;
	while (!(I2C1_SR1 & (1 << 1))); // 2 bit ADDRESS

	// 6. Reading of a byte and stop
	I2C1_CR1 &= ~(1 << 10); // NACK reading only one byte (10 bit is ACK)
	// Read SR1 and SRW to clear ADDR
	(void) I2C1_SR1;
	(void) I2C1_SR2;

	I2C1_CR1 |= (1 << 9); // 9 bit is STOP
	while (!(I2C1_SR1 & (1 << 6))); // 6 bit is RxNE

	return I2C1_DR;
}

void BMP280_ReadRawData(uint32_t *rawPres, uint32_t *rawTemp) {
	uint8_t data[6];

	// 1. Start and set address (Write)
	I2C1_CR1 |= (1 << 8); // 8 bit is for START
	while (!(I2C1_SR1 & (1 << 0)));
	I2C1_DR = (0x76 << 1);
	while (!(I2C1_SR1 & (1 << 1)));
	(void) I2C1_SR2;

	// 2. Register of the temperature (0xFA)
	// 2. Use register of the pressure it is the first (0xF7)
	I2C1_DR = 0xF7;
	while (!(I2C1_SR1 & (1 << 7)));

	// 3. Restart and set address (Read)
	I2C1_CR1 |= (1 << 8); // Restart
	while (!(I2C1_SR1 & (1 << 0)));
	I2C1_DR = (0x76 << 1) | 1;
	while (!(I2C1_SR1 & (1 << 1)));
	(void) I2C1_SR2;

	// 4. Read 3 bytes (algorythm for several bytes)
	I2C1_CR1 |= (1 << 10); // Turn on ACK (confirm received)

	for (int i = 0; i < 6; i++) {
		if (i == 5) {
			I2C1_CR1 &= ~(1 << 10); // NACK
			I2C1_CR1 |= (1 << 9); // STOP
		}
		while (!(I2C1_SR1 & (1 << 6)));
		data[i] = I2C1_DR;
	}

	// 1 byte MSB, 2 byte LSB, 3 byte XLSB
	*rawPres = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);
	*rawTemp = (data[3] << 12) | (data[4] << 4) | (data[5] >> 4);
}

void Delay(volatile uint32_t count) {
	while (count--)
		__asm("nop");
}

int main(void) {
	UART2_Init();
	I2C1_Init();

	printf("\r\n--- STM32 F401 BMP280 Started ---\r\n");

	uint8_t id = BMP280_ReadID();
	printf("Sensor ID: 0x%02X\r\n", id);

	BMP280_ReadCalibration();
	BMP280_WakeUp();

	Delay(100000);

	/* Loop forever */
	for (;;) {
		uint32_t rawP, rawT;
		BMP280_ReadRawData(&rawP, &rawT);

		int32_t temp = BMP280_Compensate_T(rawT);
		uint32_t pressure = BMP280_Compensate_P(rawP);
		uint32_t pa = pressure / 256; // Pressure in Pascal

		// Convert Pascals to mmHg without using float:
		// 1 Pa = 0.00750062 mmHg -> approximately 75 / 10000
		// Multiply by 75 and divide by 10000 to get the integer part
		uint32_t mmhg_int = (pa * 75) / 10000;
		uint32_t mmhg_frac = ((pa * 75) % 10000) / 100;

		// temp is in hundredths of a degree (e.g., 2438 = 24.38 C)
		// Output the integer and fractional parts
		printf("T: %ld.%02ld C | P: %lu Pa | %lu.%02lu mmHg\r\n",
				(int32_t) (temp / 100), (int32_t) (temp % 100), (uint32_t) pa,
				(uint32_t) mmhg_int, (uint32_t) mmhg_frac);

		Delay(1000000);
	}
}
