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

	static int last_button_state = 1;

	while (1) {
		int current_state = is_button_pressed();

		if (current_state != last_button_state) {
			if (current_state == 1) {
				led_on();
				printf("LED ON\r\n");
			} else {
				led_off();
				printf("LED OFF\r\n");
			}

			last_button_state = current_state;
		}

		for (int i = 0; i < 50000; i++);
	}
}
