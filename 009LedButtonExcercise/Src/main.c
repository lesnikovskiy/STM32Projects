/* LED PC13 */
/* Button PA0 */

#include <stdint.h>

const uint32_t RCC_BASE = 0x40023800UL;
const uint32_t GPIOA_BASE = 0x40020000UL;
const uint32_t GPIOC_BASE = 0x40020800UL;
const uint32_t USART2_BASE = 0x40004400UL;

volatile uint32_t *ahb1_enr;
volatile uint32_t *apb1_enr;

volatile uint32_t *gpioa_moder;
volatile uint32_t *gpioa_pupdr;
volatile uint32_t *gpioa_afrl;
volatile uint32_t *gpioa_idr;

volatile uint32_t *gpioc_moder;
volatile uint32_t *gpioc_bsrr;

volatile uint32_t *usart_sr;
volatile uint32_t *usart_dr;
volatile uint32_t *usart_brr;
volatile uint32_t *usart_cr1;

void delay(volatile uint32_t count);
void led_on(void);
void led_off(void);
void startup_led_blink(void);
void usart_send_char(const char ch);
void usart_send_str(const char *str);

int main(void) {
	ahb1_enr = (volatile uint32_t*) (RCC_BASE + 0x30);
	apb1_enr = (volatile uint32_t*) (RCC_BASE + 0x40);

	gpioa_moder = (volatile uint32_t*) (GPIOA_BASE + 0x00);
	gpioa_pupdr = (volatile uint32_t*) (GPIOA_BASE + 0x0C);
	gpioa_afrl = (volatile uint32_t*) (GPIOA_BASE + 0x20);
	gpioa_idr = (volatile uint32_t*) (GPIOA_BASE + 0x10);

	gpioc_moder = (volatile uint32_t*) (GPIOC_BASE + 0x00);
	gpioc_bsrr = (volatile uint32_t*) (GPIOC_BASE + 0x18);

	usart_sr = (volatile uint32_t*) (USART2_BASE + 0x00);
	usart_dr = (volatile uint32_t*) (USART2_BASE + 0x04);
	usart_brr = (volatile uint32_t*) (USART2_BASE + 0x08);
	usart_cr1 = (volatile uint32_t*) (USART2_BASE + 0x0C);

	// Enable AHB1 for Port A
	*ahb1_enr |= (1 << 0);

	// Enable AHB1 (Advanced High-performance bus) Port C
	*ahb1_enr |= (1 << 2);

	// Enable APB1 (Advanced Peripheral Bus) USART2
	*apb1_enr |= (1 << 17);

	// SET PA0 Mode input (default 00) just clear bits to make sure
	*gpioa_moder &= ~(3 << 0);
	// Clear first 2 bits
	*gpioa_pupdr &= ~(3 << 0);
	// IMPORTANT: If the button shorts to ground, we need a pull-up to 3.3V
	*gpioa_pupdr |= (1 << 0); // 1(01) pull up, 2(10) pull down

	// Set AF Mode (10) for pin PA2 (TX)
	*gpioa_moder &= ~(3 << 4);
	*gpioa_moder |= (2 << 4);

	// SET AF7 mode for USART2 PA2 pin which belongs to AFRL controlled by pins 8-15
	*gpioa_afrl &= ~(0xF << 8); // Clear bits 8-15
	*gpioa_afrl |= (7 << 8); // Set bits to 0111

	// Set PC13 (LED) Mode Output
	*gpioc_moder &= ~(3 << 26); // clear bits 26-27
	*gpioc_moder |= (1 << 26); // set output mode to bit 26 (01)

	// SETUP USART2
	// For 16 MHz and 115200: 16000000 / (16 * 115200) = 8.68
	// Mantissa 8 (0x8), Fraction 0.68 * 16 = 11 (0xB). Result: 0x8B.
	*usart_brr = 0x008B;

	// USART Enable (UE - 13)
	*usart_cr1 |= (1 << 13);
	// Transmitter Enable (TE - 3)
	*usart_cr1 |= (1 << 3);
	// Receiver Enable (RE - 2)
	*usart_cr1 |= (1 << 2);

	usart_send_str("Welcome to the STM32 World!\r\n");

	startup_led_blink();

	for (;;) {
		if (*gpioa_idr & (1 << 0)) {
			delay(50000);
			led_off();
			// usart_send_str("LED OFF!\r\n");
			delay(50000);
		} else {
			delay(50000);
			led_on();
			// usart_send_str("LED ON!\r\n");
			delay(50000);
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
	usart_send_str("=============================\r\n");

	// Now turn on bit with BSRR
	// the LED is inverted on the board so add +16bits
	led_on();
	usart_send_str("LED ON\r\n");

	// Just toggle the LED
	// wait 1s, turn off, wait .5s turn off, wait .5s turn off
	delay(1000000);

	led_off();
	usart_send_str("LED OFF\r\n");
	delay(500000);

	led_on();
	usart_send_str("LED ON\r\n");
	delay(500000);

	led_off();
	usart_send_str("LED OFF\r\n");
	delay(500000);

	led_on();
	usart_send_str("LED ON\r\n");
	delay(500000);

	led_off();
	usart_send_str("LED OFF\r\n");
	delay(500000);

	usart_send_str("=============================\r\n");
	usart_send_str("Press button on the board to turn the LED ON\r\n");
}

void usart_send_char(const char ch) {
	// TXE (Transmitter Empty bit 7)
	while(!(*usart_sr & (1 << 7)));
	*usart_dr = ch;
}

void usart_send_str(const char *str) {
	while (*str) {
		usart_send_char(*str++);
	}
}
