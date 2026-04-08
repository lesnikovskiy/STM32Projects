#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include "stm32f401_registers.h"

void UART2_Init(void);
void UART2_SendChar(uint8_t ch);
void UART2_SendString(char *str);
void UART2_SendInt(int num);

#endif /* UART_H_ */
