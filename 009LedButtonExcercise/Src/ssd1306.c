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

void oled_send_data_burst(uint8_t *data, uint16_t size) {
	i2c_start_addr(SSD1306_ADDR, 0);
	i2c_write(0x40); // Co = 0, D/C = 1 (Data)

	for (uint16_t i = 0; i < size; i++) {
		i2c_write(data[i]);
	}

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
	uint8_t buffer[128] = { 0 };

	// We clear only pages 3 and 4 where the large font is located
	for (uint8_t p = 3; p <= 4; p++) {
		oled_send_cmd(0xB0 + p);           // Set page
		oled_send_cmd(0x00);               // Set lower column address
		oled_send_cmd(0x10);               // Set higher column address
		oled_send_data_burst(buffer, 128); // Clear 128 pixels horizontally
	}
}

void oled_put_char(uint8_t index) {
	for (uint8_t i = 0; i < 5; i++) {
		oled_send_data(font5x7[index][i]);
	}
	oled_send_data(0x00); // 1px space
}

void oled_put_char_large(uint8_t index, uint8_t page, uint8_t col) {
	uint8_t buf[14]; // 7 column font * 2 = 10 bytes

	// 1. Draw the TOP half of the scaled character (7 bits -> 14 pixels, so 7 bits go to 2 pages)
	oled_send_cmd(0xB0 + page);      // Set top page
	oled_send_cmd(0x00 | (col & 0x0F));        // Set lower column address
	oled_send_cmd(0x10 | ((col >> 4) & 0x0F)); // Set higher column address

	for (uint8_t i = 0; i < 7; i++) {
		uint8_t byte = 0;
		if (index == 15) {
			byte = font5x7[index][i];
		} else if (i < 5) {
			byte = font5x7[index][i];
		}

		uint8_t res_top = 0;
		// Upscale remaining bits to 8 bits
		if (byte & (1 << 0))
			res_top |= (0x03 << 0);
		if (byte & (1 << 1))
			res_top |= (0x03 << 2);
		if (byte & (1 << 2))
			res_top |= (0x03 << 4);
		if (byte & (1 << 3))
			res_top |= (0x03 << 6);

		buf[i * 2] = res_top;
		buf[i * 2 + 1] = res_top;
	}

	oled_send_data_burst(buf, 14);

	// 2. Draw BOTTOM half (IMPORTANT: Don't forget this part)
	oled_send_cmd(0xB0 + page + 1);
	oled_send_cmd(0x00 | (col & 0x0F));
	oled_send_cmd(0x10 | ((col >> 4) & 0x0F));

	for (uint8_t i = 0; i < 7; i++) {
		uint8_t byte = 0;
		if (index == 15) {
			byte = font5x7[index][i];
		} else if (i < 5) {
			byte = font5x7[index][i];
		}

		uint8_t res_bot = 0;
		if (byte & (1 << 4))
			res_bot |= (0x03 << 0);
		if (byte & (1 << 5))
			res_bot |= (0x03 << 2);
		if (byte & (1 << 6))
			res_bot |= (0x03 << 4);
		if (byte & (1 << 7))
			res_bot |= (0x03 << 6);

		buf[i * 2] = res_bot;
		buf[i * 2 + 1] = res_bot;
	}

	oled_send_data_burst(buf, 14);
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
	// Positioning
	uint8_t thermo_col = 4;   // Thermometer position
	uint8_t start_col = 18;   // Digits position
	uint8_t current_page = 3; // Center vertically (uses pages 3 and 4)

	// 1. Draw thermometer icon
	oled_put_char_large(15, current_page, thermo_col);

	// 2. Draw Minus sign or clear it if temp > 0
	if (temp < 0) {
		oled_put_char_large(11, current_page, start_col); // Minus
		temp = -temp;
	} else {
		oled_put_char_large(14, current_page, start_col);
	}

	// Scale: Digits with STEP 15 (to leave a small gap)
	oled_put_char_large((temp / 1000) % 10, current_page, start_col + 15);  // Tens
	oled_put_char_large((temp / 100) % 10, current_page, start_col + 30);  // Units
	oled_put_char_large(10, current_page, start_col + 45);                 // Dot
	oled_put_char_large((temp / 10) % 10, current_page, start_col + 55);   // Tenths
	oled_put_char_large(12, current_page, start_col + 70);                 // Degree sign
	oled_put_char_large(13, current_page, start_col + 85);                 // Letter C (Celsius)

	// 3. Draw a space AFTER to clean up any trailing pixels
//	oled_put_char_large(14, current_page, start_col + 84);
}
