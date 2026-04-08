#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include "stm32f401_registers.h"

void UART2_Init(void);
void UART2_SendChar(uint8_t ch);

#endif /* UART_H_ */
