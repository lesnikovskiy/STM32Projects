#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include "stm32f401_registers.h"

/* Public functions */
void GPIO_Init(void);
void GPIO_LED_On(void);
void GPIO_LED_Off(void);
void GPIO_LED_Toggle(void);
void GPIO_LED_Blink(uint32_t delay_ms);
void Debug_EnableSleepMode(void);

#endif /* GPIO_H */
