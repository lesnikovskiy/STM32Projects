/* LED PC13 */
/* Button PA0 */

#include <stdint.h>

typedef struct {
	volatile uint32_t IDCODE;
	volatile uint32_t CR;
	volatile uint32_t APB1_FZ;
	volatile uint32_t APB2_FZ;
} DBGMCU_TypeDef;

typedef struct {
	volatile uint32_t CR;
	volatile uint32_t PLLCFGR;
	volatile uint32_t CFGR;
	volatile uint32_t CIR;
	volatile uint32_t AHB1RSTR;
	volatile uint32_t AHB2RSTR;
	volatile uint32_t AHB3RSTR;
	uint32_t RESERVED1;
	volatile uint32_t APB1RSTR;
	volatile uint32_t APB2RSTR;
	uint32_t RESERVED2[2];
	volatile uint32_t AHB1ENR;
	volatile uint32_t AHB2ENR;
	volatile uint32_t AHB3ENR;
	uint32_t RESERVED3;
	volatile uint32_t APB1ENR;
	volatile uint32_t APB2ENR;
} RCC_TypeDef;

typedef struct {
	volatile uint32_t MODER;    // 0x00
	volatile uint32_t OTYPER;   // 0x04
	volatile uint32_t OSPEEDR; // 0x08
	volatile uint32_t PUPDR;    // 0x0C
	volatile uint32_t IDR;      // 0x10
	volatile uint32_t ODR;      // 0x14
	volatile uint32_t BSRR;     // 0x18
	volatile uint32_t LCKR;     // 0x1C
	volatile uint32_t AFRL;     // 0x20
	volatile uint32_t AFRH;     // 0x24
} GPIO_TypeDef;

typedef struct {
	volatile uint32_t SR;
	volatile uint32_t DR;
	volatile uint32_t BRR;
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t CR3;
	volatile uint32_t GTPR;
} USART_TypeDef;

typedef struct {
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t SMCR;
	volatile uint32_t DIER;
	volatile uint32_t SR;
	volatile uint32_t EGR;
	volatile uint32_t CCMR1;
	volatile uint32_t CCMR2;
	volatile uint32_t CCER;
	volatile uint32_t CNT;
	volatile uint32_t PSC;
	volatile uint32_t ARR;
	uint32_t RESERVED1;
	volatile uint32_t CCR1;
	volatile uint32_t CCR2;
	volatile uint32_t CCR3;
	volatile uint32_t CCR4;
	uint32_t RESERVED2;
	volatile uint32_t DCR;
	volatile uint32_t DMAR;
	volatile uint32_t OR;
} TIM_TypeDef;

typedef struct {
	volatile uint32_t IMR;
	volatile uint32_t EMR;
	volatile uint32_t RTSR;
	volatile uint32_t FTSR;
	volatile uint32_t SWIER;
	volatile uint32_t PR;
} EXTI_TypeDef;

typedef struct {
	volatile uint32_t MEMRMP;
	volatile uint32_t PMC;
	volatile uint32_t EXTICR[4];
	uint32_t RESERVED1[2];
	volatile uint32_t CMPCR;
} SYSCFG_TypeDef;

typedef struct {
	volatile uint32_t ISER[8]; // 0x000 - 0x01C: Interrupt Set-Enable Registers
	uint32_t RESERVED0[24];
	volatile uint32_t ICER[8]; // 0x080 - 0x09C: Interrupt Clear-Enable Registers
	uint32_t RESERVED1[24];
	volatile uint32_t ISPR[8]; // 0x100 - 0x11C: Interrupt Set-Pending Registers
	uint32_t RESERVED2[24];
	volatile uint32_t ICPR[8]; // 0x180 - 0x19C: Interrupt Clear-Pending Registers
	uint32_t RESERVED3[24];
	volatile uint32_t IABR[8]; // 0x200 - 0x21C: Interrupt Active Bit Registers
	uint32_t RESERVED4[56];
	volatile uint8_t IP[240];  // 0x400: Interrupt Priority Registers
} NVIC_TypeDef;

const uint32_t DBGMCU_BASE = 0xE0042000UL;
const uint32_t RCC_BASE = 0x40023800UL;
const uint32_t GPIOA_BASE = 0x40020000UL;
const uint32_t GPIOC_BASE = 0x40020800UL;
const uint32_t USART2_BASE = 0x40004400UL;
const uint32_t TIM2_BASE = 0x40000000UL;
const uint32_t TIM3_BASE = 0x40000400UL;
const uint32_t EXTI_BASE = 0x40013C00UL;
const uint32_t SYSCFG_BASE = 0x40013800UL;
const uint32_t NVIC_BASE = 0xE000E100UL;

DBGMCU_TypeDef *const DBGMCU = (DBGMCU_TypeDef*) DBGMCU_BASE;
RCC_TypeDef *const RCC = (RCC_TypeDef*) RCC_BASE;
GPIO_TypeDef *const GPIOA = (GPIO_TypeDef*) GPIOA_BASE;
GPIO_TypeDef *const GPIOC = (GPIO_TypeDef*) GPIOC_BASE;
USART_TypeDef *const USART = (USART_TypeDef*) USART2_BASE;
TIM_TypeDef *const TIM2 = (TIM_TypeDef*) TIM2_BASE;
TIM_TypeDef *const TIM3 = (TIM_TypeDef*) TIM3_BASE;
EXTI_TypeDef *const EXTI = (EXTI_TypeDef*) EXTI_BASE;
SYSCFG_TypeDef *const SYSCFG = (SYSCFG_TypeDef*) SYSCFG_BASE;
NVIC_TypeDef *const NVIC = (NVIC_TypeDef*) NVIC_BASE;

volatile int global_mode = 0;
volatile uint32_t duty_cycle = 50;

void delay(uint32_t ms);
void led_on(void);
void led_off(void);
void usart_send_char(const char ch);
void usart_send_str(const char *str);
void update_mode_settings(void);
void TIM2_IRQHandler(void);
void EXTI0_IRQHandler(void);
void USART2_IRQHandler(void);
void startup_led_blink(void);

int main(void) {
	// Enable Debugger in sleep/stop/standby modes not to brick the board
	DBGMCU->CR |= (1 << 0) | (1 << 1) | (1 << 2);

	// Enable AHB1 for Port A
	RCC->AHB1ENR |= (1 << 0);

	// Enable AHB1 (Advanced High-performance bus) Port C
	RCC->AHB1ENR |= (1 << 2);

	// Enable APB1 (Advanced Peripheral Bus) USART2
	RCC->APB1ENR |= (1 << 17);

	// Enable TIM2 on APB1
	RCC->APB1ENR |= (1 << 0);

	// Enable TIM3 on APB1
	RCC->APB1ENR |= (1 << 1);

	// Enable SYSCFG
	RCC->APB2ENR |= (1 << 14);

	// SET PA0 Mode input (default 00) just clear bits to make sure
	GPIOA->MODER &= ~(3 << 0);
	// Clear first 2 bits
	GPIOA->PUPDR &= ~(3 << 0);
	// IMPORTANT: If the button shorts to ground, we need a pull-up to 3.3V
	GPIOA->PUPDR |= (1 << 0); // 1(01) pull up, 2(10) pull down

	// Set AF Mode (10) for pin PA2 (TX)
	GPIOA->MODER &= ~(3 << 4);
	GPIOA->MODER |= (2 << 4);
	// Set AF Mode (10) for PA3 (RX)
	GPIOA->MODER &= ~(3 << 6);
	GPIOA->MODER |= (2 << 6);

	// SET AF7 mode for USART2 PA2 pin which belongs to AFRL controlled by pins 8-15
	GPIOA->AFRL &= ~(0xF << 8); // Clear bits 8-15
	GPIOA->AFRL |= (7 << 8); // Set bits to 0111
	// Set AF7 mode for PA3
	GPIOA->AFRL &= ~(0xF << 12);
	GPIOA->AFRL |= (7 << 12);

	// Set PC13 (LED) Mode Output
	GPIOC->MODER &= ~(3 << 26); // clear bits 26-27
	GPIOC->MODER |= (1 << 26); // set output mode to bit 26 (01)

	// SETUP USART2
	// For 16 MHz and 115200: 16000000 / (16 * 115200) = 8.68
	// Mantissa 8 (0x8), Fraction 0.68 * 16 = 11 (0xB). Result: 0x8B.
	USART->BRR = 0x008B;

	// USART Enable (UE - 13)
	USART->CR1 |= (1 << 13);
	// Transmitter Enable (TE - 3)
	USART->CR1 |= (1 << 3);
	// Receiver Enable (RE - 2)
	USART->CR1 |= (1 << 2);
	// Enable RxNEIE (Receive not empty interrupt enable)
	USART->CR1 |= (1 << 5);

	// Setup TIM2
	update_mode_settings();
	// UIE: Update Interrupt Enable
	TIM2->DIER |= (1 << 0);
	// Clear flag to avoid triggering interrupt immediately
	TIM2->SR = 0;

	// Setup TIM3
	TIM3->PSC = 16000 - 1;
	TIM3->ARR = 0xFFFF;
	TIM3->EGR |= (1 << 0);
	TIM3->CR1 |= (1 << 0);

	// Enable interrupt 28 bit (TIM2) in NVIC controller
	NVIC->ISER[0] |= (1 << 28);
	// Enable interrupt 38 bit (38 - 32 = 6) (USART2)
	NVIC->ISER[1] |= (1 << 6);

	// Enable counter (CEN counter enable)
	TIM2->CR1 |= (1 << 0);

	// Setup Interrupt for the button
	// Setup EXTI0 to Port A (bits 0-3 should be clean)
	SYSCFG->EXTICR[0] &= ~(0xF << 0);
	// Setup EXTI0 to falling edge
	EXTI->IMR |= (1 << 0); // Allow line 0
	EXTI->FTSR |= (1 << 0);
	// Enable interrupt EXI0 in NVIC (position 6)
	NVIC->ISER[0] |= (1 << 6);

	usart_send_str("Welcome to the STM32 World!\r\n");

	startup_led_blink();

	for (;;) {
		if (global_mode == 3) {
			static int8_t fade_dir = 1;

			duty_cycle += fade_dir;
			if (duty_cycle >= 100)
				fade_dir = -1;

			if (duty_cycle <= 0)
				fade_dir = 1;

			delay(20);
		} else {
			__asm volatile ("wfi");
		}
	}
}

void delay(uint32_t ms) {
	while (ms--) {
		TIM3->CNT = 0; // Reset Counter
		while (TIM3->CNT < 1); // Wait exactly 1ms
	}
}

void led_on(void) {
	GPIOC->BSRR = (1 << (13 + 16));
}

void led_off(void) {
	GPIOC->BSRR = (1 << 13);
}

void usart_send_char(const char ch) {
	// TXE (Transmitter Empty bit 7)
	while (!(USART->SR & (1 << 7)));
	USART->DR = ch;
}

void usart_send_str(const char *str) {
	while (*str) {
		usart_send_char(*str++);
	}
}

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

void TIM2_IRQHandler(void) {
	if (TIM2->SR & (1 << 0)) {
		TIM2->SR &= ~(1 << 0);

		// Check if button pin is output
		if (GPIOC->MODER & (1 << 26)) {
			static uint32_t pwm_count = 0;

			if (global_mode == 3) {
				if (++pwm_count >= 100) {
					pwm_count = 0;
				}
				if (pwm_count < duty_cycle)
					led_on();
				else
					led_off();
			} else {
				// XOR logic to toggle LED
				static int state = 0;
				if (state) {
					led_off();
				} else {
					led_on();
				}

				state = !state;
			}
		}
	}
}

void EXTI0_IRQHandler(void) {
	if (EXTI->PR & (1 << 0)) { // Check if interrupt happened on line 0
		EXTI->PR |= (1 << 0); // Rest flag 1 for reset

		for (volatile int i = 0; i < 20000; i++);

		global_mode = (global_mode + 1) % 4;
		update_mode_settings();
		usart_send_str("Button Mode Change\r\n");
	}
}

void USART2_IRQHandler(void) {
	// Check bit 5 (RxNE)
	if (USART->SR & (1 << 5)) {
		char received = (char) (USART->DR & 0xFF);

		if (received == '\r' || received == '\n') {
			return;
		}

		if (received == '0') {
			global_mode = 0;
			update_mode_settings();
			usart_send_str("Mode: Normal\r\n");
		} else if (received == '1') {
			global_mode = 1;
			update_mode_settings();
			usart_send_str("Mode: Fast\r\n");
		} else if (received == '2') {
			global_mode = 2;
			update_mode_settings();
			usart_send_str("Mode: Slow\r\n");
		} else if (received == '3') {
			global_mode = 3;
			update_mode_settings();
			usart_send_str("Mode: Shim\r\n");
		} else {
			usart_send_str("Unknown command! Use 0, 1, 2 or 3\r\n");
		}

		// Update timer event
		TIM2->EGR |= (1 << 0);
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
	usart_send_str("Select mode 0 (Normal) 1 (FAST) 2 (SLOW) 3 (SHIM)\r\n");
}
