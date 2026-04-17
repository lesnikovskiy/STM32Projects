#include "app.h"
#include "usart.h"

void usart_send_char(const char ch) {
	// TXE (Transmitter Empty bit 7)
	while (!(USART->SR & (1 << 7)));
	USART->DR = ch;
}

void usart_send_str(const char *str) {
	while (*str) {
		usart_send_char(*str++);
	}
}

void usart_send_int(int32_t num) {
	char str[12];
	int i = 0;
	if (num == 0) {
		usart_send_char('0');
		return;
	}

	if (num < 0) {
		usart_send_char('-');
		num = -num;
	}

	while (num > 0) {
		str[i++] = (num % 10) + '0';
		num /= 10;
	}

	while (i--) {
		usart_send_char(str[i]);
	}
}

void usart_send_hex32(uint32_t num) {
    char hex_chars[] = "0123456789ABCDEF";
    usart_send_str("0x");
    for (int i = 7; i >= 0; i--) {
        // Get 4 bits = 1 HEX number and send it
        uint8_t nibble = (num >> (i * 4)) & 0x0F;
        char c = hex_chars[nibble];
        char buf[2] = {c, '\0'};
        usart_send_str(buf);
    }
}

void usart_send_int_raw(int32_t num) {
	char str[11]; // Buffer for 32-bit integer
	int i = 0;

	if (num == 0) {
		usart_send_char('0');
		return;
	}

	if (num < 0) {
		usart_send_char('-');
		num = -num;
	}

	// Convert number to string (reverse order)
	while (num > 0) {
		str[i++] = (num % 10) + '0';
		num /= 10;
	}

	// Send string in correct order
	while (i--) {
		usart_send_char(str[i]);
	}
}

void usart_send_temp(int32_t temp_x100) {
	// Seperate integer and fractional parts
	int32_t integrity = temp_x100 / 100;
	int32_t fractional = temp_x100 % 100;

	// Handle negative values
	if (temp_x100 < 0 && integrity == 0) {
		usart_send_str("-");
	}

	usart_send_int(integrity);
	usart_send_char('.');

	if (fractional < 10 && fractional > -10) {
		usart_send_char('0');
	}

	if (fractional < 0)
		fractional = -fractional;

	usart_send_int(fractional);

	usart_send_str(" C\r\n");
}

void USART2_IRQHandler(void) {
	// Check bit 5 (RxNE)
	if (USART->SR & (1 << 5)) {
		char received = (char) (USART->DR & 0xFF);

		if (received == '\r' || received == '\n') {
			return;
		}

		if (received == '0') {
			global_mode = 0;
			update_mode_settings();
			usart_send_str("Mode: Normal\r\n");
		} else if (received == '1') {
			global_mode = 1;
			update_mode_settings();
			usart_send_str("Mode: Fast\r\n");
		} else if (received == '2') {
			global_mode = 2;
			update_mode_settings();
			usart_send_str("Mode: Slow\r\n");
		} else if (received == '3') {
			global_mode = 3;
			update_mode_settings();
			usart_send_str("Mode: Shim\r\n");
		} else {
			usart_send_str("Unknown command! Use 0, 1, 2 or 3\r\n");
		}

		// Update timer event
		TIM2->EGR |= (1 << 0);
	}
}
