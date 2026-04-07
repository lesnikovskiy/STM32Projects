#include "delay.h"

/* Simple blocking delay using NOP instructions */
void delay(uint32_t count) {
    for (volatile uint32_t i = 0; i < count; i++) {
        __asm("nop");
    }
}

/* Approximate millisecond delay (calibrate for your clock speed)
   For 84MHz STM32F401, ~42000 iterations = 1ms (adjust as needed) */
void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms; i++) {
        delay(42000);  /* Calibrate this value */
    }
}
