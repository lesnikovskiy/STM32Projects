#include "system.h"
#include "gpio.h"
#include "interrupts.h"
#include "delay.h"

void System_Init(void) {
	GPIO_Init();
	Interrupts_Init();
}

void App_Run(void) {
	/* Signal startup */
	for (int i = 0; i < 2; i++) {
		GPIO_LED_On();
		delay(100000);
		GPIO_LED_Off();
		delay(100000);
	}

	Interrupts_EnableGlobal();

	while (1) {
		__asm("nop");
	}
}
