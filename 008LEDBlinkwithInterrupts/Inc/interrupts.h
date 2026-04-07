#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>
#include "delay.h"
#include "stm32f401_registers.h"

/* Public functions */
void Interrupts_Init(void);
void Interrupts_EnableGlobal(void);

/* ISR declaration - MUST match startup file */
void EXTI0_IRQHandler(void);

#endif /* INTERRUPTS_H */
