#include <stdint.h>
#include "gpio.h"
#include "interrupts.h"
#include "delay.h"

/* ============================================
 MAIN APPLICATION
 ============================================ */

int main(void) {
	/* 1. Initialize hardware peripherals */
	GPIO_Init();
	Interrupts_Init();

	/* 2. Signal that program started (blink LED 2 times) */
	for (int i = 0; i < 2; i++) {
		GPIO_LED_On();
		delay(100000);
		GPIO_LED_Off();
		delay(100000);
	}

	/* 3. Enable global interrupts */
	Interrupts_EnableGlobal();

	/* 4. Main loop - idle */
	while (1) {
		/* Application idle - could enter low power mode */
		__asm("nop");
	}
}
