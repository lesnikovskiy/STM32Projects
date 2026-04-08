#include "rcc.h"

void RCC_GPIOA_Enable(void) {
	RCC_AHB1ENR |= (1 << 0);
}

void RCC_GPIC_Enable(void) {
	RCC_AHB1ENR |= (1 << 2);
}
