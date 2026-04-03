#include <stdint.h>

// Base addresses for STM32F401
#define RCC_BASE      0x40023800
#define GPIOA_BASE    0x40020000
#define GPIOC_BASE    0x40020800

// Specific registers
#define RCC_AHB1ENR   *(volatile uint32_t*)(RCC_BASE + 0x30)
#define GPIOA_MODER   *(volatile uint32_t*)(GPIOA_BASE + 0x00)
#define GPIOA_PUPDR   *(volatile uint32_t*)(GPIOA_BASE + 0x0C)
#define GPIOA_IDR     *(volatile uint32_t*)(GPIOA_BASE + 0x10) // Input Data Register
#define GPIOC_MODER   *(volatile uint32_t*)(GPIOC_BASE + 0x00)
#define GPIOC_ODR     *(volatile uint32_t*)(GPIOC_BASE + 0x14)

#define GPIOC_BSRR  *(volatile uint32_t*)(GPIOC_BASE + 0x18)

int main(void) {
	RCC_AHB1ENR |= (1 << 0) | (1 << 2); // Turn on Port A and port C

	// SETUP LED for PC13 OUTPUT
	GPIOC_MODER &= ~(3 << 26); // Clear bits
	GPIOC_MODER |= (1 << 26); // Set as Output

	// Setup button PA0 to Input with pull up
	// by default MODER is for PA0 as Input, but better to setup explicitly
	GPIOA_MODER &= ~(3 << 0);

	// ВАЖНО: Если кнопка замыкает на землю, нам нужна подтяжка к 3.3В (Pull-up)
	// Регистр PUPDR: 01 - Pull-up, 10 - Pull-down. Для PA0 это биты 0-1.
	GPIOA_PUPDR &= ~(3 << 0);
	GPIOA_PUPDR |= (1 << 0); // Turn on internal Pull-up resistor

	while (1) {
		// ПРОВЕРКА БИТА (Чтение кнопки)
		// Мы берем весь регистр и накладываем "маску" на 0-й бит
		if (GPIOA_IDR & (1 << 0)) {
			// Если 0-й бит равен 1 (кнопка НЕ нажата, если есть Pull-up)
			// GPIOC_ODR |= (1 << 13);		// Выключить LED
			GPIOC_BSRR = (1 << 13);
		} else {
			// Если 0-й бит равен 0 (кнопка нажата на землю)
			// GPIOC_ODR &= ~(1 << 13);		// Включить LED
			GPIOC_BSRR = (1 << (13 + 16));
		}
	}

}
