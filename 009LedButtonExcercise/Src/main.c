/* LED PC13 */
/* Button PA0 */

#include <stdint.h>

uint32_t rcc_base = 0x40023800UL;
uint32_t gpioc_base = 0x40020800UL;

volatile uint32_t *ahb1_enr;

volatile uint32_t *gpioc_moder;
volatile uint32_t *gpioc_bsrr;

void delay(volatile uint32_t count);
void led_on(void);
void led_off(void);
void startup_led_blink(void);

int main(void) {
	ahb1_enr = (volatile uint32_t*) (rcc_base + 0x30);

	gpioc_moder = (volatile uint32_t*) (gpioc_base + 0x00);
	gpioc_bsrr = (volatile uint32_t*) (gpioc_base + 0x18);

	// Enable AHB1 (Advanced High-performance bus) Port C
	*ahb1_enr |= (1 << 2);
	// Set PC13 Mode Output
	*gpioc_moder &= ~(3 << 26); // clear bits 26-27
	*gpioc_moder |= (1 << 26); // set output mode to bit 26 (01)

	startup_led_blink();

	for (;;) {
		__asm("nop");
	}
}

void delay(volatile uint32_t count) {
	for (int i = 0; i < count; i++) {
		__asm("nop");
	}
}
void led_on(void) {
	*gpioc_bsrr = (1 << (13 + 16));
}

void led_off(void) {
	*gpioc_bsrr = (1 << 13);
}

void startup_led_blink(void) {
	// Now turn on bit with BSRR
	// the LED is inverted on the board so add +16bits
	led_on();

	// Just toggle the LED
	// wait 1s, turn off, wait .5s turn off, wait .5s turn off
	delay(1000000);

	led_off();
	delay(500000);

	led_on();
	delay(500000);

	led_off();
	delay(500000);

	led_on();
	delay(500000);

	led_off();
	delay(500000);
}
