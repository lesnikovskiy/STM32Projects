/* LED PC13 */
/* Button PA0 */

#include <stdint.h>

int main(void) {
	uint32_t rcc_base = 0x40023800UL;
	volatile uint32_t *ahb1_enr = (volatile uint32_t*) (rcc_base + 0x30);

	uint32_t gpioc_base = 0x40020800UL;
	volatile uint32_t *gpioc_moder = (volatile uint32_t*) (gpioc_base + 0x00);
	volatile uint32_t *gpioc_bsrr = (volatile uint32_t*) (gpioc_base + 0x18);

	// Enable AHB1 (Advanced High-performance bus) Port C
	*ahb1_enr |= (1 << 2);
	// Set PC13 Mode Output
	*gpioc_moder &= ~(3 << 26); // clear bits 26-27
	*gpioc_moder |= (1 << 26); // set output mode to bit 26 (01)

	// Now turn on bit with BSRR
	// the LED is inverted on the board so add +16bits
	*gpioc_bsrr = (1 << (13 + 16));

	for (;;);
}
