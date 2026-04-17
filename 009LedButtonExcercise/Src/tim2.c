#include "app.h"
#include "tim2.h"

void update_mode_settings(void) {
	if (global_mode == 3) {
		TIM2->PSC = 16 - 1;
		TIM2->ARR = 100 - 1;
	} else {
		// Prescaler: 16 000 000 / 16 000 = 1000 ticks per second.
		TIM2->PSC = 16000 - 1;
		if (global_mode == 0) {
			// Setting auto reload every 500ms
			TIM2->ARR = 500 - 1;
		} else if (global_mode == 1) {
			TIM2->ARR = 100 - 1;
		} else {
			TIM2->ARR = 2000 - 1;
		}
	}

	// Force update PSC
	TIM2->EGR |= (1 << 0);
}
