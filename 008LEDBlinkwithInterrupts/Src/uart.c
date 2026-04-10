#include "uart.h"

/**
 * UART2 Configuration: 115200 baud, 8 bits, 1 stop bit, no parity.
 * Default APB1 bus frequency = 16 MHz (HSI).
 */
void UART2_Init(void) {
	// 1. Enable clocking for GPIOA and USART2
	RCC_AHB1ENR |= (1 << 0);  // GPIOA EN
	RCC_APB1ENR |= (1 << 17); // USART2 EN

	// 2. Configure pin PA2 (TX) to Alternate Function mode
	GPIOA_MODER &= ~(3 << 4);  // Clear bits 4-5
	GPIOA_MODER |= (2 << 4);  // AF Mode (10)

	// Set AF7 (USART2) for pin PA2 (bits 8-11 in AFRL)
	GPIOA_AFRL &= ~(0xF << 8);
	GPIOA_AFRL |= (7 << 8);

	// Configure pin PA3 (RX)
	GPIOA_MODER &= ~(3 << 6); // Clear bits 6-7 for PA3
	GPIOA_MODER |= (2 << 6); // AF mode (2 = 10)

	// Set AF7 for pin PA3
	GPIOA_AFRL &= ~(0xF << 12); // pins 12 - 15 (F = 1111)
	GPIOA_AFRL |= (7 << 12); // AF7 (7 = 0111)

	// 3. Baud rate configuration
	// For 16 MHz and 115200: 16000000 / (16 * 115200) = 8.68
	// Mantissa 8 (0x8), Fraction 0.68 * 16 = 11 (0xB). Result: 0x8B.
	USART2_BRR = 0x008B;

	// 4. Enable UART, transmitter (TE), and receiver (RE)
	USART2_CR1 |= (1 << 13) | (1 << 3) | (1 << 2);
}

void UART2_SendChar(uint8_t ch) {
	// Wait until TXE bit (bit 7 of SR) becomes 1 (buffer is empty)
	while (!(USART2_SR & (1 << 7)));
	USART2_DR = ch;
}

void UART2_SendString(char *str) {
	while (*str) {
		UART2_SendChar(*str++);
	}
}

void UART2_SendInt(int num) {
	char str[12]; // Maximum for int32 including \0
	int i = 0;

	if (num == 0) {
		UART2_SendChar('0');
		return;
	}

	if (num < 0) {
		UART2_SendChar('-');
		if (num == -2147483648) {
			UART2_SendString("2147483648");
			return;
		}
		num = -num;
	}

	while (num > 0) {
		str[i++] = (num % 10) + '0';
		num /= 10;
	}

	while (i > 0) {
		UART2_SendChar(str[--i]);
	}
}
