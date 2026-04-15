/* LED PC13 */
/* Button PA0 */

#include <stdint.h>

const uint32_t RCC_BASE = 0x40023800UL;
const uint32_t GPIOA_BASE = 0x40020000UL;
const uint32_t GPIOC_BASE = 0x40020800UL;
const uint32_t USART2_BASE = 0x40004400UL;
const uint32_t TIM2_BASE = 0x40000000UL;
const uint32_t TIM3_BASE = 0x40000400UL;
const uint32_t SYSCFG_BASE = 0x40013800UL;
const uint32_t EXTI_BASE = 0x40013C00UL;

volatile uint32_t *const DBGMCU_CR = (volatile uint32_t*) 0xE0042004UL;

volatile uint32_t *const AHB1_ENR = (volatile uint32_t*) (RCC_BASE + 0x30UL);
volatile uint32_t *const APB1_ENR = (volatile uint32_t*) (RCC_BASE + 0x40UL);
volatile uint32_t *const APB2_ENR = (volatile uint32_t*) (RCC_BASE + 0x44UL);

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
volatile uint32_t *const TIM2_DIER = (volatile uint32_t*) (TIM2_BASE + 0x0CUL);

volatile uint32_t *const TIM3_CR1 = (volatile uint32_t*) (TIM3_BASE + 0x00);
volatile uint32_t *const TIM3_PSC = (volatile uint32_t*) (TIM3_BASE + 0x28UL);
volatile uint32_t *const TIM3_ARR = (volatile uint32_t*) (TIM3_BASE + 0x2CUL);
volatile uint32_t *const TIM3_CNT = (volatile uint32_t*) (TIM3_BASE + 0x24UL);
volatile uint32_t *const TIM3_EGR = (volatile uint32_t*) (TIM3_BASE + 0x14UL);

volatile uint32_t *const EXTI_IMR = (volatile uint32_t*) (EXTI_BASE + 0x00UL);
volatile uint32_t *const EXTI_FTSR = (volatile uint32_t*) (EXTI_BASE + 0x0CUL);
volatile uint32_t *const EXTI_PR = (volatile uint32_t*) (EXTI_BASE + 0x14UL);

volatile uint32_t *const SYSCFG_EXTICR1 = (volatile uint32_t*) (SYSCFG_BASE + 0x08UL);

// NVIC ISER0 (Interrupt Set-Enable Register) interrupts 0 - 31 bits
// TIM2 is at 28 position
volatile uint32_t *const NVIC_ISER0 = (volatile uint32_t*) 0xE000E100UL;
volatile uint32_t *const NVIC_ISER1 = (volatile uint32_t*) 0xE000E104UL;

void delay(uint32_t ms);
void led_on(void);
void led_off(void);
void usart_send_char(const char ch);
void usart_send_str(const char *str);
void TIM2_IRQHandler(void);
void EXTI0_IRQHandler(void);
void USART2_IRQHandler(void);
void startup_led_blink(void);

int main(void) {
	// Enable Debugger in sleep/stop/standby modes not to brick the board
	*DBGMCU_CR |= (1 << 0) | (1 << 1) | (1 << 2);

	// Enable AHB1 for Port A
	*AHB1_ENR |= (1 << 0);

	// Enable AHB1 (Advanced High-performance bus) Port C
	*AHB1_ENR |= (1 << 2);

	// Enable APB1 (Advanced Peripheral Bus) USART2
	*APB1_ENR |= (1 << 17);

	// Enable TIM2 on APB1
	*APB1_ENR |= (1 << 0);

	// Enable TIM3 on APB1
	*APB1_ENR |= (1 << 1);

	// Enable SYSCFG
	*APB2_ENR |= (1 << 14);

	// SET PA0 Mode input (default 00) just clear bits to make sure
	*GPIOA_MODER &= ~(3 << 0);
	// Clear first 2 bits
	*GPIOA_PUPDR &= ~(3 << 0);
	// IMPORTANT: If the button shorts to ground, we need a pull-up to 3.3V
	*GPIOA_PUPDR |= (1 << 0); // 1(01) pull up, 2(10) pull down

	// Set AF Mode (10) for pin PA2 (TX)
	*GPIOA_MODER &= ~(3 << 4);
	*GPIOA_MODER |= (2 << 4);
	// Set AF Mode (10) for PA3 (RX)
	*GPIOA_MODER &= ~(3 << 6);
	*GPIOA_MODER |= (2 << 6);

	// SET AF7 mode for USART2 PA2 pin which belongs to AFRL controlled by pins 8-15
	*GPIOA_AFRL &= ~(0xF << 8); // Clear bits 8-15
	*GPIOA_AFRL |= (7 << 8); // Set bits to 0111
	// Set AF7 mode for PA3
	*GPIOA_AFRL &= ~(0xF << 12);
	*GPIOA_AFRL |= (7 << 12);

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
	// Enable RxNEIE (Receive not empty interrupt enable)
	*USART_CR1 |= (1 << 5);

	// Setup TIM2
	// Prescaler: 16 000 000 / 16 000 = 1000 ticks per second.
	*TIM2_PSC = 16000 - 1;
	// Setting auto reload every 500ms
	*TIM2_ARR = 500 - 1;
	// UIE: Update Interrupt Enable
	*TIM2_DIER |= (1 << 0);
	// Force update PSC
	*TIM2_EGR |= (1 << 0);
	// Clear flag to avoid triggering interrup immediately
	*TIM2_SR = 0;

	// Setup TIM3
	*TIM3_PSC = 16000 - 1;
	*TIM3_ARR = 0xFFFF;
	*TIM3_EGR |= (1 << 0);
	*TIM3_CR1 |= (1 << 0);

	// Enable interrupt 28 bit (TIM2) in NVIC controller
	*NVIC_ISER0 |= (1 << 28);
	// Enable interrupt 38 bit (38 - 32 = 6) (USART2)
	*NVIC_ISER1 |= (1 << 6);

	// Enable counter (CEN counter enable)
	*TIM2_CR1 |= (1 << 0);

	// Setup Interrup for the button
	// Setup EXTI0 to Port A (bits 0-3 should be clean)
	*SYSCFG_EXTICR1 &= ~(0xF << 0);
	// Setup EXTI0 to falling edge
	*EXTI_IMR |= (1 << 0); // Allow line 0
	*EXTI_FTSR |= (1 << 0);
	// Enable interrupt EXI0 in NVIC (position 6)
	*NVIC_ISER0 |= (1 << 6);

	usart_send_str("Welcome to the STM32 World!\r\n");

	startup_led_blink();

	for (;;) {
		__asm volatile ("wfi");
	}
}

void delay(uint32_t ms) {
	while (ms--) {
		*TIM3_CNT = 0; // Reset Counter
		while (*TIM3_CNT < 1); // Wait exactly 1ms
	}
}

void led_on(void) {
	*GPIOC_BSRR = (1 << (13 + 16));
}

void led_off(void) {
	*GPIOC_BSRR = (1 << 13);
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

void TIM2_IRQHandler(void) {
	if (*TIM2_SR & (1 << 0)) {
		*TIM2_SR &= ~(1 << 0);
		// Check if button pin is output
		if (*GPIOC_MODER & (1 << 26)) {
			// XOR logic to toggle LED
			static int state = 0;
			if (state) {
				led_off();
				state = 0;
			} else {
				led_on();
				state = 1;
			}
		}
	}
}

void EXTI0_IRQHandler(void) {
	if (*EXTI_PR & (1 << 0)) { // Check if interrupt happened on line 0
		*EXTI_PR |= (1 << 0); // Rest flag 1 for reset

		static int mode = 0;
		mode++;

		if (mode > 2)
			mode = 0;

		if (mode == 0) {
			*TIM2_ARR = 500 - 1;
			usart_send_str("Mode: Normal\r\n");
		} else if (mode == 1) {
			*TIM2_ARR = 100 - 1;
			usart_send_str("Mode: Fast\r\n");
		} else {
			*TIM2_ARR = 2000 - 1;
			usart_send_str("Mode: Slow\r\n");
		}

		// Update timer event
		*TIM2_EGR |= (1 << 0);
	}
}

void USART2_IRQHandler(void) {
	// Check bit 5 (RxNE)
	if (*USART_SR & (1 << 5)) {
		char received = (char) (*USART_DR & 0xFF);

		if (received == '\r' || received == '\n') {
			return;
		}

		if (received == '0') {
			*TIM2_ARR = 500 - 1;
			usart_send_str("Mode: Normal\r\n");
		} else if (received == '1') {
			*TIM2_ARR = 100 - 1;
			usart_send_str("Mode: Fast\r\n");
		} else if (received == '2') {
			*TIM2_ARR = 2000 - 1;
			usart_send_str("Mode: Slow\r\n");
		} else {
			usart_send_str("Unknown command! Use 0, 1 or 2\r\n");
		}

		// Update timer event
		*TIM2_EGR |= (1 << 0);
	}
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
	usart_send_str("Select mode 0 (Normal) 1 (FAST) 2 (SLOW) \r\n");
}
