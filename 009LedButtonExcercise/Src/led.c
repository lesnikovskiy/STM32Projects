#include "led.h"

void led_on(void) {
	GPIOC->BSRR = (1 << (13 + 16));
}

void led_off(void) {
	GPIOC->BSRR = (1 << 13);
}

void startup_led_blink(void) {
	usart_send_str("=============================\r\n");

	// Now turn on bit with BSRR
	// the LED is inverted on the board so add +16bits
	led_on();
	usart_send_str("LED ON\r\n");

	// Just toggle the LED
	// wait 1s, turn off, wait .5s turn off, wait .5s turn off
	delay(1000);

	led_off();
	usart_send_str("LED OFF\r\n");
	delay(500);

	led_on();
	usart_send_str("LED ON\r\n");
	delay(500);

	led_off();
	usart_send_str("LED OFF\r\n");
	delay(500);

	led_on();
	usart_send_str("LED ON\r\n");
	delay(500);

	led_off();
	usart_send_str("LED OFF\r\n");
	delay(500);

	usart_send_str("=============================\r\n");
	usart_send_str("Select mode 0 (Normal) 1 (FAST) 2 (SLOW) 3 (SHIM)\r\n");
}
