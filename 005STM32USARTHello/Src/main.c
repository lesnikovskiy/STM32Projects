#include <stdint.h>
#include <stdio.h>

// --- Регистры тактирования (RCC) ---
#define RCC_BASE      0x40023800
#define RCC_AHB1ENR   *(volatile uint32_t*)(RCC_BASE + 0x30) // Включение GPIO
#define RCC_APB1ENR   *(volatile uint32_t*)(RCC_BASE + 0x40) // Включение UART2

// --- Регистры GPIOA (для пинов PA2, PA3) ---
#define GPIOA_BASE    0x40020000
#define GPIOA_MODER   *(volatile uint32_t*)(GPIOA_BASE + 0x00)
#define GPIOA_AFRL    *(volatile uint32_t*)(GPIOA_BASE + 0x20)

// --- Добавляем регистры для GPIOC (светодиод на PC13) ---
#define GPIOC_BASE    0x40020800
#define GPIOC_MODER   *(volatile uint32_t*)(GPIOC_BASE + 0x00)
#define GPIOC_ODR     *(volatile uint32_t*)(GPIOC_BASE + 0x14)

// --- Регистры USART2 ---
#define USART2_BASE   0x40004400
#define USART2_SR     *(volatile uint32_t*)(USART2_BASE + 0x00) // Status
#define USART2_DR     *(volatile uint32_t*)(USART2_BASE + 0x04) // Data
#define USART2_BRR    *(volatile uint32_t*)(USART2_BASE + 0x08) // Baudrate
#define USART2_CR1    *(volatile uint32_t*)(USART2_BASE + 0x0C) // Control 1

/**
 * Настройка UART2: 115200 бод, 8 бит, 1 стоп-бит, без четности.
 * Частота шины APB1 по умолчанию = 16 МГц (HSI).
 */
void UART2_Init(void) {
	// 1. Включаем тактирование GPIOA и USART2
	RCC_AHB1ENR |= (1 << 0);  // GPIOA EN
	RCC_APB1ENR |= (1 << 17); // USART2 EN

	// 2. Настройка пина PA2 (TX) в режим Alternate Function
	GPIOA_MODER &= ~(3 << 4);  // Очистка бит 4-5
	GPIOA_MODER |= (2 << 4);  // Режим AF (10)

	// Установка AF7 (USART2) для пина PA2 (биты 8-11 в AFRL)
	GPIOA_AFRL &= ~(0xF << 8);
	GPIOA_AFRL |= (7 << 8);

	// 3. Настройка скорости (Baudrate)
	// Для 16 МГц и 115200: 16000000 / (16 * 115200) = 8.68
	// Целая часть 8 (0x8), дробная 0.68 * 16 = 11 (0xB). Итого 0x8B.
	USART2_BRR = 0x008B;

	// 4. Включаем UART, передатчик (TE) и приемник (RE)
	USART2_CR1 |= (1 << 13) | (1 << 3) | (1 << 2);
}

/**
 * Отправка одного символа
 */
void UART2_SendChar(uint8_t ch) {
	// Ждем, пока бит TXE (7-й бит SR) станет 1 (буфер пуст)
	while (!(USART2_SR & (1 << 7)))
		;
	USART2_DR = ch;
}

/**
 * Мост для printf
 */
int _write(int file, char *ptr, int len) {
	for (int i = 0; i < len; i++) {
		UART2_SendChar((uint8_t) *ptr++);
	}
	return len;
}

void LED_Init(void) {
	RCC_AHB1ENR |= (1 << 2); // Turn on GPIOC
	GPIOC_MODER &= ~(3 << 26); // Clear bits 26-27
	GPIOC_MODER |= (1 << 26); // Set OUTPUT Mode (01)
}

void Delay(volatile uint32_t count) {
	while (count--)
		__asm("nop");
}

int main(void) {
	UART2_Init();
	LED_Init();

	printf("UART2 initialized at 115200 baud!\r\n");
	printf("Testing F401 Bare Metal printf...\r\n");

	printf("System Started! LED is blinking...\r\n");

	for (;;) {
		GPIOC_ODR ^= (1 << 13);

		if (!(GPIOC_ODR & (1 << 13))) {
			printf("LED ON\r\n");
		} else {
			printf("LED OFF\r\n");
		}

		Delay(1000000); // Задержка, чтобы глаз успел заметить
	}
}
