#include "gpio.h"
#include "delay.h"

/* ============================================
 GPIO INITIALIZATION
 ============================================ */

void GPIO_Init(void) {
	/* Enable GPIO clocks */
	RCC_AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN;

	/* Configure PC13 as output (LED) */
	GPIOC_MODER &= ~LED_PIN_MASK; /* Clear mode bits */
	GPIOC_MODER |= (GPIO_MODE_OUTPUT << (LED_PIN * 2)); /* Set output mode */

	/* Configure PA0 as input with pull-up (button) */
	GPIOA_MODER &= ~BUTTON_PIN_MASK; /* Clear mode bits */
	GPIOA_MODER |= (GPIO_MODE_INPUT << (BUTTON_PIN * 2)); /* Set input mode */

	/* Configure pull-up on PA0 */
	GPIOA_PUPDR &= ~BUTTON_PULL_MASK; /* Clear pull bits */
	GPIOA_PUPDR |= (GPIO_PUPDR_PULLUP << (BUTTON_PIN * 2)); /* Enable pull-up */
}

/* ============================================
 LED CONTROL FUNCTIONS
 ============================================ */

void GPIO_LED_On(void) {
	GPIOC_BSRR = LED_BSRR_RESET; /* Reset = 0 = LED ON (active low) */
}

void GPIO_LED_Off(void) {
	GPIOC_BSRR = LED_BSRR_SET; /* Set = 1 = LED OFF (active low) */
}

void GPIO_LED_Toggle(void) {
	/* Read current state and invert */
	if (GPIOC_ODR & (1 << LED_PIN)) {
		GPIO_LED_On();
	} else {
		GPIO_LED_Off();
	}
}

void GPIO_LED_Blink(uint32_t delay_ms) {
	GPIO_LED_On();
	delay(delay_ms);
	GPIO_LED_Off();
	delay(delay_ms);
}

/* ============================================
 BUTTON READ FUNCTION (optional)
 ============================================ */

uint8_t GPIO_Button_IsPressed(void) {
	/* Button is pressed when pin reads 0 (pull-up to ground) */
	return !(GPIOA_IDR & (1 << BUTTON_PIN));
}

void Debug_EnableSleepMode(void) {
	// This is CRITICAL for debugging with WFI!
	DBGMCU_CR |= DBGMCU_CR_DBG_SLEEP | DBGMCU_CR_DBG_STOP; // 0 bit DBG_SLEEP, 1 bit DBG_STOP, 2 bit DBG_STANDBY
}
