#include <stdint.h>

// RCC Registers (Clock Control)
// Correct RCC_BASE for STM32L476 is 0x40021000
#define RCC_BASE              (0x40021000UL)
#define RCC_AHB2ENR           (*(volatile uint32_t *)(RCC_BASE + 0x4C))

// GPIOA Registers
// GPIOA_BASE on AHB2 is 0x48000000
#define GPIOA_BASE            (0x48000000UL)
#define GPIOA_MODER           (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_BSRR            (*(volatile uint32_t *)(GPIOA_BASE + 0x18))

#define GPIOAEN               (1U << 0)
#define LED_PIN               (1U << 5)  // PA5

void delay(volatile uint32_t count) {
	while (count--) {
		__asm("nop");
	}
}

int main(void) {
	// 1. Enable clock for GPIO Port A
	// Now pointing to the correct address 0x4002104C
	RCC_AHB2ENR |= GPIOAEN;

	// 2. Configure PA5 as Output (set bits 11:10 to '01')
	GPIOA_MODER &= ~(3U << 10);
	GPIOA_MODER |= (1U << 10);

	while (1) {
		// 3. LED ON
		GPIOA_BSRR = LED_PIN;
		delay(400000);

		// 4. LED OFF
		GPIOA_BSRR = (LED_PIN << 16);
		delay(400000);
	}
}
