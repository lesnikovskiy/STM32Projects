#include <stdint.h>

// Base addresses for STM32F401
#define RCC_BASE      0x40023800UL
#define GPIOA_BASE    0x40020000UL
#define GPIOC_BASE    0x40020800UL
#define SYSCFG_BASE   0x40013800UL
#define EXTI_BASE     0x40013C00UL
#define NVIC_BASE     0xE000E100UL

// RCC Registers
#define RCC_AHB1ENR   *(volatile uint32_t*)(RCC_BASE + 0x30)
#define RCC_APB2ENR   *(volatile uint32_t*)(RCC_BASE + 0x44)  // SYSCFG clock

// GPIOA Registers (button PA0)
#define GPIOA_MODER   *(volatile uint32_t*)(GPIOA_BASE + 0x00)
#define GPIOA_PUPDR   *(volatile uint32_t*)(GPIOA_BASE + 0x0C)
#define GPIOA_IDR     *(volatile uint32_t*)(GPIOA_BASE + 0x10)

// GPIOC Registers (LED PC13)
#define GPIOC_MODER   *(volatile uint32_t*)(GPIOC_BASE + 0x00)
#define GPIOC_BSRR    *(volatile uint32_t*)(GPIOC_BASE + 0x18)

// SYSCFG Registers
#define SYSCFG_EXTICR1 *(volatile uint32_t*)(SYSCFG_BASE + 0x08)  // For EXTI0

// EXTI Registers
#define EXTI_IMR      *(volatile uint32_t*)(EXTI_BASE + 0x00)   // Interrupt mask
#define EXTI_FTSR     *(volatile uint32_t*)(EXTI_BASE + 0x0C)   // Falling trigger
#define EXTI_PR       *(volatile uint32_t*)(EXTI_BASE + 0x14)   // Pending register

// NVIC Registers
#define NVIC_ISER0    *(volatile uint32_t*)(NVIC_BASE + 0x00)   // IRQ 0-31

void delay(uint32_t count) {
	for (volatile uint32_t i = 0; i < count; i++) {
		__asm("nop");
	}
}

// ============================================
// IMPORTANT: Declare function BEFORE main
// ============================================
void EXTI0_IRQHandler(void);

// ============================================
// INTERRUPT INITIALIZATION
// ============================================
void Init_Interrupt(void) {
	// 1. Enable SYSCFG clock (required for EXTI!)
	RCC_APB2ENR |= (1 << 14);  // SYSCFGEN = 1

	// 2. Connect EXTI Line 0 to PA0
	// SYSCFG_EXTICR1 controls lines 0-3
	// For Line 0: bits 0-3, value 0000 = PA0
	SYSCFG_EXTICR1 &= ~(0xFU << 0);  // Clear bits 0-3
	SYSCFG_EXTICR1 |= (0x0U << 0);   // 0 = PA0 (already default)

	// 3. Configure EXTI for Line 0
	EXTI_FTSR |= (1 << 0);      // Interrupt on falling edge (button press)
	EXTI_IMR |= (1 << 0);       // Unmask interrupt
	EXTI_PR = (1 << 0);         // Clear flag if pending

	// 4. Enable interrupt in NVIC
	// EXTI0_IRQn = 6 (for STM32F4)
	NVIC_ISER0 = (1 << 6);      // Enable IRQ6

	// Small delay after initialization
	delay(10000);
}

// ============================================
// INTERRUPT SERVICE ROUTINE
// IMPORTANT: __attribute__((used)) prevents compiler removal!
// ============================================
__attribute__((used, section(".text")))
void EXTI0_IRQHandler(void) {
	// Check that interrupt is definitely from Line 0
	if (EXTI_PR & (1 << 0)) {
		// Clear interrupt flag (MANDATORY!)
		EXTI_PR = (1 << 0);

		// Toggle LED
		GPIOC_BSRR = (1 << (13 + 16));  // LED ON

		// Small debounce delay
		delay(50000);

		// Turn off LED
		GPIOC_BSRR = (1 << 13);  // LED OFF
	}
}

// ============================================
// MAIN
// ============================================
int main(void) {
	// 1. Enable GPIOA and GPIOC clocks
	RCC_AHB1ENR |= (1 << 0) | (1 << 2);

	// 2. Configure PC13 as output (LED)
	GPIOC_MODER &= ~(3 << 26);
	GPIOC_MODER |= (1 << 26);

	// 3. Configure PA0 as input with pull-up (button)
	GPIOA_MODER &= ~(3 << 0);   // Input mode
	GPIOA_PUPDR &= ~(3 << 0);
	GPIOA_PUPDR |= (1 << 0);    // Pull-up

	// 4. Initialize interrupts
	Init_Interrupt();

	// 5. Signal that program started (blink LED 2 times)
	for (int i = 0; i < 2; i++) {
		GPIOC_BSRR = (1 << (13 + 16));  // LED ON
		delay(100000);
		GPIOC_BSRR = (1 << 13);         // LED OFF
		delay(100000);
	}

	// 6. Enable global interrupts
	__asm volatile ("cpsie i");

	// 7. Main loop
	while (1) {
		// Main loop: LED blinks slowly to show program is alive
		GPIOC_BSRR = (1 << (13 + 16));  // LED ON
		delay(500000);
		GPIOC_BSRR = (1 << 13);         // LED OFF
		delay(500000);
	}
}
