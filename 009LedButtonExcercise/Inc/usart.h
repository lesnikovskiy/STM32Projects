#ifndef USART_H_
#define USART_H_

#include "stm32f401rct6.h"
#include "tim2.h"

void usart_send_char(const char ch);
void usart_send_str(const char *str);
void usart_send_int(int32_t num);

void USART2_IRQHandler(void);

#endif /* USART_H_ */
