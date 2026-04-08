#include <stdint.h>

// Base addresses for STM32F401
#define RCC_BASE      0x40023800
#define GPIOC_BASE    0x40020800

// Specific registers
#define RCC_AHB1ENR   *(volatile uint32_t*)(RCC_BASE + 0x30)
#define GPIOC_MODER   *(volatile uint32_t*)(GPIOC_BASE + 0x00)
#define GPIOC_ODR     *(volatile uint32_t*)(GPIOC_BASE + 0x14)

// Адреса регистров SysTick (из Programming Manual)
#define STK_CTRL  *(volatile uint32_t*)0xE000E010
#define STK_LOAD  *(volatile uint32_t*)0xE000E014
#define STK_VAL   *(volatile uint32_t*)0xE000E018

volatile uint32_t msTicks = 0;

void SysTick_Init(uint32_t ticks) {
	STK_LOAD = ticks - 1;
	STK_VAL = 0;
	// Bits: 0 - Enable, 1 - TickInt (interrupt), 2 - Clock Source (Processor)
	STK_CTRL = (1 << 0) | (1 << 1) | (1 << 2);
}

void SysTick_Handler(void) {
	msTicks++;
}

void delay(uint32_t ms) {
	uint32_t startTick = msTicks;
	while ((msTicks - startTick) < ms);
}

int main(void) {
	// 1. Инициализация системного таймера (16000 тактов = 1мс при 16МГц)
	SysTick_Init(16000);

	// 2. Enable GPIOC port clock (LED is on C13)
	// AHB1ENR register, bit 2 corresponds to Port C
	RCC_AHB1ENR |= (1 << 2);

	// 2. Configure C13 as output (set bits 26-27 to 01)
	GPIOC_MODER &= ~(3 << 26); // Clear mode bits for pin 13
	GPIOC_MODER |= (1 << 26); // Set mode to General Purpose Output

	while (1) {
		GPIOC_ODR ^= (1 << 13); // Toggle the LED pin
		delay(500);
	}
}
