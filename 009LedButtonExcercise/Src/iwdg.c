#include "iwdg.h"

void iwdg_init(void) {
	IWDG->KR = 0x5555;  // Enable access to PR and RLR registers (Unlock)
	IWDG->PR = 0x04;    // Prescaler: divider by 64 (32kHz / 64 = 500 Hz)
	IWDG->RLR = 1000;   // Reload value: 1000 / 500Hz = 2.0 seconds timeout
	IWDG->KR = 0xAAAA;  // Refresh the counter with RLR value (Pet the dog)
	IWDG->KR = 0xCCCC;  // Start the Independent Watchdog (Enable)
}

void reload_iwdg_counter(void) {
	// Reload IWDG counter to prevent reset (Keep-alive)
	IWDG->KR = 0xAAAA;
}
