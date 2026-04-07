#include "interrupts.h"
#include "gpio.h"

/* ============================================
 INTERRUPT INITIALIZATION
 ============================================ */

void Interrupts_Init(void) {
	/* 1. Enable SYSCFG clock (required for EXTI!) */
	RCC_APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	/* 2. Connect EXTI Line 0 to PA0 */
	SYSCFG_EXTICR1 &= ~EXTI0_CONFIG_MASK; /* Clear configuration bits */
	SYSCFG_EXTICR1 |= EXTI0_PORT_A; /* Select Port A for EXTI0 */

	/* 3. Configure EXTI for Line 0 */
	EXTI_FTSR |= EXTI_LINE0; /* Interrupt on falling edge */
	EXTI_IMR |= EXTI_LINE0; /* Unmask interrupt */
	EXTI_PR = EXTI_LINE0; /* Clear pending flag */

	/* 4. Enable interrupt in NVIC */
	NVIC_ISER0 = EXTI0_IRQ_BIT; /* Enable IRQ6 */
}

/* ============================================
 GLOBAL INTERRUPT CONTROL
 ============================================ */

void Interrupts_EnableGlobal(void) {
	__asm volatile ("cpsie i");
	/* Clear PRIMASK - enable interrupts */
}

/* ============================================
 INTERRUPT SERVICE ROUTINE
 ============================================ */

__attribute__((used, section(".text")))
void EXTI0_IRQHandler(void) {
	/* Check that interrupt is definitely from Line 0 */
	if (EXTI_PR & EXTI_LINE0) {
		/* Clear interrupt flag (MANDATORY!) */
		EXTI_PR = EXTI_LINE0;

		/* Toggle LED state */
		static int led_state = 0;

		if (led_state == 0) {
			GPIO_LED_On();
			led_state = 1;
		} else {
			GPIO_LED_Off();
			led_state = 0;
		}

		/* Debounce delay (in production, use timer instead) */
		delay(50000);
	}
}
