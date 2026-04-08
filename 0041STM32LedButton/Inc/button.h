#ifndef BUTTON_H_
#define BUTTON_H_

#include "stm32f401_registers.h"

void button_init(void);
int is_button_pressed(void);

#endif /* BUTTON_H_ */
