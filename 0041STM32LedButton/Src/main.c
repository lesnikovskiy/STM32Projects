#include <stdint.h>
#include <stdio.h>
#include "rcc.h"
#include "led.h"
#include "button.h"
#include "uart.h"

int main(void) {
	RCC_GPIOA_Enable();
	RCC_GPIOC_Enable();

	led_init();

	button_init();

	UART2_Init();

	printf("The UART has been initialized\n");

	while (1) {
		if (is_button_pressed()) {
			led_on();
			printf("LED ON\n");
		} else {
			led_off();
			printf("LED OFF\n");

		}
	}
}
