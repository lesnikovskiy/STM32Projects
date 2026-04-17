/* LED PC13 */
/* Button PA0 */

#include "stm32f401rct6.h"
#include "app.h"
#include "delay.h"
#include "led.h"
#include "tim2.h"
#include "usart.h"
#include "i2c.h"
#include "bmp280.h"

void TIM2_IRQHandler(void);
void EXTI0_IRQHandler(void);

int main(void) {
	// Enable Debugger in sleep/stop/standby modes not to brick the board
	DBGMCU->CR |= (1 << 0) | (1 << 1) | (1 << 2);

	// Enable AHB1 for Port A
	RCC->AHB1ENR |= (1 << 0);

	// Enable AHB1 for Port B
	RCC->AHB1ENR |= (1 << 1);

	// Enable AHB1 (Advanced High-performance bus) Port C
	RCC->AHB1ENR |= (1 << 2);

	// Enable APB1 (Advanced Peripheral Bus) USART2
	RCC->APB1ENR |= (1 << 17);

	// Enable I2C1 ON APB1
	RCC->APB1ENR |= (1 << 21);

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

	// Setup I2C1 (Pins B7, B6)
	// Setup mode AF=10
	GPIOB->MODER &= ~((3 << 12) | (3 << 14));
	GPIOB->MODER |= ((2 << 12) | (2 << 14));
	// Open-Drain = 1 Critical for I2C
	GPIOB->OTYPER |= (1 << 6) | (1 << 7);
	// Pull-up (01)
	GPIOB->PUPDR &= ~((3 << 12) | (3 << 14));
	GPIOB->PUPDR |= ((1 << 12) | (1 << 14));
	// AF4 in AFRL (pins 6-7 where pin is 4 bits)
	GPIOB->AFRL &= ~((0xF << 24) | (0xF << 28));
	GPIOB->AFRL |= ((4 << 24) | (4 << 28));

	i2c_init();

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

	uint8_t id = bmp280_read_id();
	if (id == 0x58) {
		usart_send_str("BMP280 Detected!\r\n");
		usart_send_str("BMP280 id = ");
		usart_send_int(id);
		usart_send_str("\r\n");

		bmp280_init_sensor();
		bmp280_read_calibration();

		int32_t rawTemp = bmp280_read_raw_temp();
		usart_send_str("Raw temp = ");
		usart_send_int(rawTemp);
		usart_send_str("\r\n");

		int32_t realTemp = bmp280_compensate_T(rawTemp);
		usart_send_str("Compensated temp = ");
		usart_send_temp(realTemp);
		usart_send_str("\r\n");
	} else {
		usart_send_str("BMP280 Connection Error\r\n");
	}

	startup_led_blink();

	uint32_t last_temp_update = 0;

	for (;;) {
		if (global_mode == 3) {
			static int8_t fade_dir = 1;
			static uint32_t last_pwm_update = 0;

			if ((TIM3->CNT - last_pwm_update) >= 15) {
				last_pwm_update = TIM3->CNT;
				duty_cycle += fade_dir;
				if (duty_cycle >= 100 || duty_cycle <= 0) {
					fade_dir = -fade_dir;
				}
			}
		}

		// TIM3 is setup 1 tick 1 ms so delay 5000 ms
		if ((TIM3->CNT - last_temp_update) >= 5000) {
			last_temp_update = TIM3->CNT;

			int32_t raw = bmp280_read_raw_temp();
			int32_t realTemp = bmp280_compensate_T(raw);

			usart_send_str("Temp: ");
			usart_send_temp(realTemp);
		}

		if (global_mode != 3) {
			__asm volatile ("wfi");
		}
	}
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

