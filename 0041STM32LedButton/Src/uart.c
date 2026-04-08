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
