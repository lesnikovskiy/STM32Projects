#include <stdint.h>
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

	UART2_SendString("The UART has been initialized\n");

	int initial_state = is_button_pressed();

	UART2_SendString("The BUTTON initial state is ");
	UART2_SendInt(initial_state);
	UART2_SendString("\r\n");

	if (initial_state)
		led_on();
	else
		led_off();

	static int last_button_state;
	last_button_state = initial_state;

	while (1) {
		int current_state = is_button_pressed();

		if (current_state != last_button_state) {
			if (current_state == 1) {
				led_on();
				UART2_SendString("LED ON\r\n");
			} else {
				led_off();
				UART2_SendString("LED OFF\r\n");
			}

			last_button_state = current_state;
		}

		for (int i = 0; i < 50000; i++);
	}
}
