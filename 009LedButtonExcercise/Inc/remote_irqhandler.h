#ifndef REMOTE_IRQHANDLER_H_
#define REMOTE_IRQHANDLER_H_

#include <stdint.h>
#include "app.h"
#include "stm32f401rct6.h"
#include "usart.h"
#include "remote_codes.h"

void EXTI1_IRQHandler(void);

#endif /* REMOTE_IRQHANDLER_H_ */
