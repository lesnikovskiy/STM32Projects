#include "main.h"

void SystemClock_Config(void);
void HAL_GPIO_MspInit(void);
void UART1_Init(void);
void Error_Handler(void);

UART_HandleTypeDef huart1;

char *greeting_message = "The application is running on STM32F411CEU6\r\n";

int main(void) {
	HAL_Init();

	SystemClock_Config();

	HAL_GPIO_MspInit();

	UART1_Init();
	HAL_UART_Transmit(&huart1, (uint8_t*) greeting_message, strlen(greeting_message), HAL_MAX_DELAY);

	char msg[100];

	snprintf(msg, sizeof(msg), "SYSCLK : %luHz\r\n", HAL_RCC_GetSysClockFreq());
	HAL_UART_Transmit(&huart1, (uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);

	snprintf(msg, sizeof(msg), "HLCLK : %luHz\r\n", HAL_RCC_GetHCLKFreq());
	HAL_UART_Transmit(&huart1, (uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);

	snprintf(msg, sizeof(msg), "PCLK1 : %luHz\r\n", HAL_RCC_GetPCLK1Freq());
	HAL_UART_Transmit(&huart1, (uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);

	snprintf(msg, sizeof(msg), "PCLK2 : %luHz\r\n", HAL_RCC_GetPCLK2Freq());
	HAL_UART_Transmit(&huart1, (uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);

	while (1) {
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		HAL_Delay(500);
	}

	return 0;
}

void SystemClock_Config(void) {
	RCC_OscInitTypeDef osc_init = { 0 };
	RCC_ClkInitTypeDef clk_init = { 0 };

	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	osc_init.HSEState = RCC_HSE_ON;
	osc_init.PLL.PLLState = RCC_PLL_ON;
	osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	osc_init.PLL.PLLM = 25;
	osc_init.PLL.PLLN = 200;
	osc_init.PLL.PLLP = RCC_PLLP_DIV2;
	osc_init.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&osc_init) != HAL_OK) {
		Error_Handler();
	}

	clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1; // HCLK = 100MHz
	clk_init.APB1CLKDivider = RCC_HCLK_DIV2;  // PCLK1 = 50MHz
	clk_init.APB2CLKDivider = RCC_HCLK_DIV1;  // PLCK2 = 100MHz
	if (HAL_RCC_ClockConfig(&clk_init, FLASH_LATENCY_3) != HAL_OK) {
		Error_Handler();
	}

	__HAL_RCC_HSI_DISABLE();

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
}

void UART1_Init() {
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;

	if (HAL_UART_Init(&huart1) != HAL_OK) {
		Error_Handler();
	}
}

void Error_Handler(void) {
	__disable_irq();

	while (1);
}
