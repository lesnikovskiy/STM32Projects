#include "main.h"

void SystemClock_Config(void);
void UART2_Init(void);
void Error_Handler(void);

UART_HandleTypeDef huart1;

char *greeting_message = "The application is running on STM32F411CEU6\r\n";

int main(void) {
	HAL_Init();
	SystemClock_Config();
	UART2_Init();
}

void SystemClock_Config(void) {
	RCC_OscInitTypeDef osc_init;
	RCC_ClckInitTypeDef clk_init;


}

void UART2_Init() {
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
}

void Error_Handler(void) {
	__disable_irq();

	char *panic_msg = "\r\n!!! CRITICAL ERROR !!!\r\n";
	HAL_UART_Transmit(&huart1, (uint8_t*) panic_msg, strlen(panic_msg), HAL_MAX_DELAY);

	while (1);
}
