#include "system.h"
#include "gpio.h"
#include "interrupts.h"
#include "delay.h"
#include "uart.h"

void System_Init(void) {
	Enable_Sleep_Debugging();

	GPIO_Init();
	UART2_Init();
	Interrupts_Init();
}

void App_Run(void) {
	/* UART Message */
	UART2_SendString("UART Initialized\r\n");

	/* Signal startup */
	for (int i = 0; i < 2; i++) {
		GPIO_LED_On();
		UART2_SendString("LED ON\r\n");
		delay(100000);

		GPIO_LED_Off();
		UART2_SendString("LED OFF\r\n");
		delay(100000);
	}

	Interrupts_EnableGlobal();

	while (1) {
		__asm volatile ("wfi");
	}
}
