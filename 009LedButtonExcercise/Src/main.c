/* LED PC13 */
/* Button PA0 */

#include <stdint.h>

const uint32_t RCC_BASE = 0x40023800UL;
const uint32_t GPIOA_BASE = 0x40020000UL;
const uint32_t GPIOC_BASE = 0x40020800UL;

volatile uint32_t *ahb1_enr;

volatile uint32_t *gpioa_moder;
volatile uint32_t *gpioa_pupdr;
volatile uint32_t *gpioa_idr;

volatile uint32_t *gpioc_moder;
volatile uint32_t *gpioc_bsrr;

void delay(volatile uint32_t count);
void led_on(void);
void led_off(void);
void startup_led_blink(void);

int main(void) {
	ahb1_enr = (volatile uint32_t*) (RCC_BASE + 0x30);

	gpioa_moder = (volatile uint32_t*) (GPIOA_BASE + 0x00);
	gpioa_pupdr = (volatile uint32_t*) (GPIOA_BASE + 0x0C);
	gpioa_idr = (volatile uint32_t*) (GPIOA_BASE + 0x10);

	gpioc_moder = (volatile uint32_t*) (GPIOC_BASE + 0x00);
	gpioc_bsrr = (volatile uint32_t*) (GPIOC_BASE + 0x18);

	// Enable AHB1 for Port A
	*ahb1_enr |= (1 << 0);

	// Enable AHB1 (Advanced High-performance bus) Port C
	*ahb1_enr |= (1 << 2);

	// SET PA0 Mode input (default 00) just clear bits to make sure
	*gpioa_moder &= ~(3 << 0);
	// Clear first 2 bits
	*gpioa_pupdr &= ~(3 << 0);
	// IMPORTANT: If the button shorts to ground, we need a pull-up to 3.3V
	*gpioa_pupdr |= (1 << 0); // 1(01) pull up, 2(10) pull down

	// Set PC13 Mode Output
	*gpioc_moder &= ~(3 << 26); // clear bits 26-27
	*gpioc_moder |= (1 << 26); // set output mode to bit 26 (01)

	startup_led_blink();

	for (;;) {
		if (*gpioa_idr & (1 << 0)) {
			led_off();
		} else {
			led_on();
		}
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
