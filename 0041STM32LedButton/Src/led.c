#include "led.h"

void led_init(void) {
	// SETUP LED for PC13 OUTPUT
	GPIOC_MODER &= ~(3 << 26); // Clear bits
	GPIOC_MODER |= (1 << 26); // Set as Output
}

void led_on(void) {
	// Reset pin to 0 (GND) to let current flow through the LED
	GPIOC_BSRR = (1 << (13 + 16));
}

void led_off(void) {
	// Set pin to 1 (3.3V), no current flows (potential difference is 0)
	GPIOC_BSRR = (1 << 13);
}
