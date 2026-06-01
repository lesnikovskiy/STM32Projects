#include "main.h"

void HAL_MspInit(void) {
	// Setup priority grouping of the ARM Cortex MX processor
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

	// Enable the required system exceptions of the ARM Cortex MX processor
	SCB->SHCSR |= (0x7 << 16); // Enable usage fault, memory fault and bus fault system exceptions

	// Configure priority for the system exceptions
	HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
	HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
	HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart) {
	GPIO_InitTypeDef gpio_uart = { 0 };

	// Enable the Clock for USART1 Peripheral
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_USART1_CLK_ENABLE();

	// Do the pin muxing configurations PA9 = TX, PA10 = RX
	gpio_uart.Pin = GPIO_PIN_9 | GPIO_PIN_10;
	gpio_uart.Mode = GPIO_MODE_AF_PP;
	gpio_uart.Pull = GPIO_PULLUP;
	 // Higher speeds match 100MHz internal rails better
	gpio_uart.Speed = GPIO_SPEED_FREQ_HIGH;
	gpio_uart.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(GPIOA, &gpio_uart);

	// Enable IRQ and set the priority (NVIC Settings)
	HAL_NVIC_SetPriority(USART1_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
}
