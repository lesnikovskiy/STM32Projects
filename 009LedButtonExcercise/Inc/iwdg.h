#ifndef IWDG_H_
#define IWDG_H_

#include "stm32f401rct6.h"

void iwdg_init(void);
void reload_iwdg_counter(void);

#endif /* IWDG_H_ */
