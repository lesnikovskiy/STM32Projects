#ifndef USART_H_
#define USART_H_

#include "stm32f401rct6.h"
#include "tim2.h"

void usart_dma_init(void);
void usart_send_str_dma(const char *str, uint16_t len);

void usart_send_char(const char ch);
void usart_send_str(const char *str);
void usart_send_int(int32_t num);
void usart_send_hex32(uint32_t num);
void usart_send_int_raw(int32_t num);
void usart_send_temp(int32_t temp_x100);

void USART2_IRQHandler(void);

#endif /* USART_H_ */
