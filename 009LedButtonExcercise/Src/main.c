/* LED PC13 */
/* Button PA0 */

#include <stdint.h>

const uint32_t RCC_BASE = 0x40023800UL;
const uint32_t GPIOA_BASE = 0x40020000UL;
const uint32_t GPIOC_BASE = 0x40020800UL;
const uint32_t USART2_BASE = 0x40004400UL;
const uint32_t TIM2_BASE = 0x40000000UL;
const uint32_t SYSCFG_BASE = 0x40013800UL;

volatile uint32_t *const AHB1_ENR = (volatile uint32_t*) (RCC_BASE + 0x30UL);
volatile uint32_t *const APB1_ENR = (volatile uint32_t*) (RCC_BASE + 0x40UL);

volatile uint32_t *const GPIOA_MODER = (volatile uint32_t*) (GPIOA_BASE + 0x00UL);
volatile uint32_t *const GPIOA_PUPDR = (volatile uint32_t*) (GPIOA_BASE + 0x0CUL);
volatile uint32_t *const GPIOA_AFRL = (volatile uint32_t*) (GPIOA_BASE + 0x20UL);
volatile uint32_t *const GPIOA_IDR = (volatile uint32_t*) (GPIOA_BASE + 0x10UL);

volatile uint32_t *const GPIOC_MODER = (volatile uint32_t*) (GPIOC_BASE + 0x00UL);
volatile uint32_t *const GPIOC_BSRR = (volatile uint32_t*) (GPIOC_BASE + 0x18UL);

volatile uint32_t *const USART_SR = (volatile uint32_t*) (USART2_BASE + 0x00UL);
volatile uint32_t *const USART_DR = (volatile uint32_t*) (USART2_BASE + 0x04UL);
volatile uint32_t *const USART_BRR = (volatile uint32_t*) (USART2_BASE + 0x08UL);
volatile uint32_t *const USART_CR1 = (volatile uint32_t*) (USART2_BASE + 0x0CUL);

volatile uint32_t *const TIM2_CR1 = (volatile uint32_t*) (TIM2_BASE + 0x00UL);
volatile uint32_t *const TIM2_PSC = (volatile uint32_t*) (TIM2_BASE + 0x28UL);
volatile uint32_t *const TIM2_ARR = (volatile uint32_t*) (TIM2_BASE + 0x2CUL);
volatile uint32_t *const TIM2_CNT = (volatile uint32_t*) (TIM2_BASE + 0x24UL);
volatile uint32_t *const TIM2_SR = (volatile uint32_t*) (TIM2_BASE + 0x10UL);
volatile uint32_t *const TIM2_EGR = (volatile uint32_t*) (TIM2_BASE + 0x14UL);

void delay(uint32_t ms);
void led_on(void);
void led_off(void);
void startup_led_blink(void);
void usart_send_char(const char ch);
void usart_send_str(const char *str);

int main(void) {
	// Enable AHB1 for Port A
	*AHB1_ENR |= (1 << 0);

	// Enable AHB1 (Advanced High-performance bus) Port C
	*AHB1_ENR |= (1 << 2);

	// Enable APB1 (Advanced Peripheral Bus) USART2
	*APB1_ENR |= (1 << 17);

	// Enable TIM2 on APB1
	*APB1_ENR |= (1 << 0);

	// SET PA0 Mode input (default 00) just clear bits to make sure
	*GPIOA_MODER &= ~(3 << 0);
	// Clear first 2 bits
	*GPIOA_PUPDR &= ~(3 << 0);
	// IMPORTANT: If the button shorts to ground, we need a pull-up to 3.3V
	*GPIOA_PUPDR |= (1 << 0); // 1(01) pull up, 2(10) pull down

	// Set AF Mode (10) for pin PA2 (TX)
	*GPIOA_MODER &= ~(3 << 4);
	*GPIOA_MODER |= (2 << 4);

	// SET AF7 mode for USART2 PA2 pin which belongs to AFRL controlled by pins 8-15
	*GPIOA_AFRL &= ~(0xF << 8); // Clear bits 8-15
	*GPIOA_AFRL |= (7 << 8); // Set bits to 0111

	// Set PC13 (LED) Mode Output
	*GPIOC_MODER &= ~(3 << 26); // clear bits 26-27
	*GPIOC_MODER |= (1 << 26); // set output mode to bit 26 (01)

	// SETUP USART2
	// For 16 MHz and 115200: 16000000 / (16 * 115200) = 8.68
	// Mantissa 8 (0x8), Fraction 0.68 * 16 = 11 (0xB). Result: 0x8B.
	*USART_BRR = 0x008B;

	// USART Enable (UE - 13)
	*USART_CR1 |= (1 << 13);
	// Transmitter Enable (TE - 3)
	*USART_CR1 |= (1 << 3);
	// Receiver Enable (RE - 2)
	*USART_CR1 |= (1 << 2);

	// Setup TIM2
	// Prescaler: 16 000 000 / 16 000 = 1000 ticks per second.
	*TIM2_PSC = 16000-1;
	// Force update PSC
	*TIM2_EGR |= (1 << 0);
	// Setting maximum auto reload
	*TIM2_ARR = 0xFFFFFFFF;
	// Enable counter (CEN counter enable)
	*TIM2_CR1 |= (1 << 0);

	usart_send_str("Welcome to the STM32 World!\r\n");

	startup_led_blink();

	for (;;) {
		if (!(*GPIOA_IDR & (1 << 0))) {
			led_on();
			usart_send_str("LED ON!\r\n");
			while (!(*GPIOA_IDR & (1 << 0)));
			delay(1000);
		} else {
			led_off();
		}
	}
}

void delay(uint32_t ms) {
	*TIM2_CNT = 0; // Reset Counter
	while (*TIM2_CNT < ms); // Wait until the counter reads the ms value
}

void led_on(void) {
	*GPIOC_BSRR = (1 << (13 + 16));
}

void led_off(void) {
	*GPIOC_BSRR = (1 << 13);
}

void startup_led_blink(void) {
	usart_send_str("=============================\r\n");

	// Now turn on bit with BSRR
	// the LED is inverted on the board so add +16bits
	led_on();
	usart_send_str("LED ON\r\n");

	// Just toggle the LED
	// wait 1s, turn off, wait .5s turn off, wait .5s turn off
	delay(1000);

	led_off();
	usart_send_str("LED OFF\r\n");
	delay(500);

	led_on();
	usart_send_str("LED ON\r\n");
	delay(500);

	led_off();
	usart_send_str("LED OFF\r\n");
	delay(500);

	led_on();
	usart_send_str("LED ON\r\n");
	delay(500);

	led_off();
	usart_send_str("LED OFF\r\n");
	delay(500);

	usart_send_str("=============================\r\n");
	usart_send_str("Press button on the board to turn the LED ON\r\n");
}

void usart_send_char(const char ch) {
	// TXE (Transmitter Empty bit 7)
	while (!(*USART_SR & (1 << 7)));
	*USART_DR = ch;
}

void usart_send_str(const char *str) {
	while (*str) {
		usart_send_char(*str++);
	}
}
