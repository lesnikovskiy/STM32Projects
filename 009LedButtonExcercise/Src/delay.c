#include "delay.h"

void delay(uint32_t ms) {
	while (ms--) {
		TIM3->CNT = 0; // Reset Counter
		while (TIM3->CNT < 1); // Wait exactly 1ms
	}
}
