#ifndef SSD1306_H_
#define SSD1306_H_

#include <stdint.h>
#include "ssd1306_fonts.h"

#define SSD1306_ADDR 0x3C

void oled_send_cmd(uint8_t cmd);
void oled_send_data(uint8_t data);
void oled_send_data_burst(uint8_t *data, uint16_t size);

void oled_init(void);
void oled_clear(void);
void oled_clear_line(uint8_t page);
void oled_clear_temp_area(void);
void oled_put_char(uint8_t index);
void oled_put_char_large(uint8_t index, uint8_t page, uint8_t col);

void display_temp_on_oled(int32_t temp);
void display_temp_large(int32_t temp);

#endif /* SSD1306_H_ */
