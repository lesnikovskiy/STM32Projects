#include "ssd1306.h"
#include "i2c.h"

/*
 * OLED Screen Setup
 * VCC -> 3.3V
 * GND -> Ground
 * SCL -> PB6
 * SDA -> PB7
 *
 * */

void oled_send_cmd(uint8_t cmd) {
	i2c_start_addr(SSD1306_ADDR, 0);
	i2c_write(0x00); // // Co = 0, D/C = 0 (Command)
	i2c_write(cmd);
	i2c_stop();
}

void oled_send_data(uint8_t data) {
	i2c_start_addr(SSD1306_ADDR, 0);
	i2c_write(0x40); // Co = 0, D/C = 1 (Data)
	i2c_write(data);
	i2c_stop();
}

void oled_init(void) {
	oled_send_cmd(0xAE); // Display OFF
	oled_send_cmd(0x20); // Set Memory Addressing Mode
	oled_send_cmd(0x10); // Page addressing mode
	oled_send_cmd(0xB0); // Set Page Start Address for Page Addressing Mode
	oled_send_cmd(0x00); // Set Lower Column Start Address
	oled_send_cmd(0x10); // Set Higher Column Start Address
	oled_send_cmd(0x8D); // Charge Pump Setting
	oled_send_cmd(0x14); // Enable Charge Pump
	oled_send_cmd(0xAF); // Display ON

	oled_clear();
}

void oled_clear(void) {
	for (uint8_t i = 0; i < 8; i++) {
		oled_send_cmd(0xB0 + i); // Page or column
		oled_send_cmd(0x00); // Column 0 low
		oled_send_cmd(0x10); // Column 0 high

		for (uint8_t j = 0; j < 128; j++) {
			oled_send_data(0x00); // Clear pixels
		}
	}
}

void oled_clear_line(uint8_t page) {
	oled_send_cmd(0xB0 + page);
	oled_send_cmd(0x00);
	oled_send_cmd(0x10);
	for (int i = 0; i < 128; i++)
		oled_send_data(0);

	oled_send_cmd(0xB0 + page + 1); // Clear second page of large font
	oled_send_cmd(0x00);
	oled_send_cmd(0x10);
	for (int i = 0; i < 128; i++)
		oled_send_data(0);
}

void oled_clear_temp_area(void) {
	// We clear only pages 3 and 4 where the large font is located
	for (uint8_t p = 3; p <= 4; p++) {
		oled_send_cmd(0xB0 + p);     // Set page
		oled_send_cmd(0x00);         // Set lower column address
		oled_send_cmd(0x10);         // Set higher column address
		for (uint8_t i = 0; i < 128; i++) {
			oled_send_data(0x00);    // Clear 128 pixels horizontally
		}
	}
}

void oled_put_char(uint8_t index) {
	for (uint8_t i = 0; i < 5; i++) {
		oled_send_data(font5x7[index][i]);
	}
	oled_send_data(0x00); // 1px space
}

void oled_put_char_large(uint8_t index, uint8_t page, uint8_t col) {
	// 1. Draw the TOP half of the scaled character (7 bits -> 14 pixels, so 7 bits go to 2 pages)
	oled_send_cmd(0xB0 + page);      // Set top page
	oled_send_cmd(0x00 | (col & 0x0F));        // Set lower column address
	oled_send_cmd(0x10 | ((col >> 4) & 0x0F)); // Set higher column address

	for (uint8_t i = 0; i < 5; i++) {
		uint8_t byte = font5x7[index][i];
		uint8_t res_top = 0;
		// Scale first 4 bits of the original byte to 8 bits
		if (byte & (1 << 0))
			res_top |= (0x03 << 0);
		if (byte & (1 << 1))
			res_top |= (0x03 << 2);
		if (byte & (1 << 2))
			res_top |= (0x03 << 4);
		if (byte & (1 << 3))
			res_top |= (0x03 << 6);

		oled_send_data(res_top); // Send twice to double width
		oled_send_data(res_top);
	}

	// 2. Draw the BOTTOM half of the scaled character
	oled_send_cmd(0xB0 + page + 1);  // Set next page (below)
	oled_send_cmd(0x00 | (col & 0x0F));
	oled_send_cmd(0x10 | ((col >> 4) & 0x0F));

	for (uint8_t i = 0; i < 5; i++) {
		uint8_t byte = font5x7[index][i];
		uint8_t res_bot = 0;
		// Scale next 3-4 bits to the second page
		if (byte & (1 << 4))
			res_bot |= (0x03 << 0);
		if (byte & (1 << 5))
			res_bot |= (0x03 << 2);
		if (byte & (1 << 6))
			res_bot |= (0x03 << 4);
		if (byte & (1 << 7))
			res_bot |= (0x03 << 6);

		oled_send_data(res_bot); // Send twice to double width
		oled_send_data(res_bot);
	}
}

void display_temp_on_oled(int32_t temp) {
	oled_send_cmd(0xB3); // Type second page (column)
	oled_send_cmd(0x00); // from the start of the column
	oled_send_cmd(0x10);

	if (temp < 0) {
		oled_put_char(11); // Minus
	}

	// Supposed temp = 2550 (25.50)
	oled_put_char((temp / 1000) % 10); // Tens digit
	oled_put_char((temp / 100) % 10);  // Units digit
	oled_put_char(10);                 // Decimal point
	oled_put_char((temp / 10) % 10);   // Tenths digit (first decimal place)
	oled_put_char(12);                 // Degree symbol
	oled_put_char(0);                  // Letter 'C' (if added to the font)
}

void display_temp_large(int32_t temp) {
	uint8_t start_col = 30; // Start printing from the middle-ish of the screen
	uint8_t current_page = 3; // Center vertically (uses pages 3 and 4)

	if (temp < 0) {
		oled_put_char_large(11, current_page, start_col); // Minus
		temp = -temp;
	}

	// Scale: each large char is ~12 pixels wide (10 pixels + 2 gap)
	oled_put_char_large((temp / 1000) % 10, current_page, start_col + 0);  // Tens
	oled_put_char_large((temp / 100) % 10, current_page, start_col + 12);  // Units
	oled_put_char_large(10, current_page, start_col + 24);                 // Dot
	oled_put_char_large((temp / 10) % 10, current_page, start_col + 36);   // Tenths
	oled_put_char_large(12, current_page, start_col + 48);                 // Degree sign
	oled_put_char_large(13, current_page, start_col + 60);                 // Letter C (Celsius)
}
