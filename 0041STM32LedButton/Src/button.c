#include "button.h"

void button_init() {
	// Setup button PA0 to Input mode
	// By default MODER is 00 (Input) for PA0, but explicit setup is safer
	GPIOA_MODER &= ~(3 << 0);

	// IMPORTANT: If the button shorts to ground, we need a pull-up to 3.3V
	// PUPDR Register: 01 - Pull-up, 10 - Pull-down. For PA0 these are bits 0-1.
	GPIOA_PUPDR &= ~(3 << 0);
	GPIOA_PUPDR |= (1 << 0); // Enable internal Pull-up resistor
}

int is_button_pressed(void) {
	return (GPIOA_IDR & (1 << 0)) == 0;
}
