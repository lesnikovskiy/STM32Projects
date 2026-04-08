#include "led.h"

void led_init(void) {
	// SETUP LED for PC13 OUTPUT
	GPIOC_MODER &= ~(3 << 26); // Clear bits
	GPIOC_MODER |= (1 << 26); // Set as Output
}

void led_on(void) {
	GPIOC_BSRR = (1 << (13 + 16));
}

void led_off(void) {
	GPIOC_BSRR = (1 << 13);

}
