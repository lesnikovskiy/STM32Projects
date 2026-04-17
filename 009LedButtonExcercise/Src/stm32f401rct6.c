#include "stm32f401rct6.h"

const uint32_t DBGMCU_BASE = 0xE0042000UL;
const uint32_t RCC_BASE = 0x40023800UL;
const uint32_t GPIOA_BASE = 0x40020000UL;
const uint32_t GPIOB_BASE = 0x40020400UL;
const uint32_t GPIOC_BASE = 0x40020800UL;
const uint32_t USART2_BASE = 0x40004400UL;
const uint32_t I2C1_BASE = 0x40005400UL;
const uint32_t TIM2_BASE = 0x40000000UL;
const uint32_t TIM3_BASE = 0x40000400UL;
const uint32_t EXTI_BASE = 0x40013C00UL;
const uint32_t SYSCFG_BASE = 0x40013800UL;
const uint32_t NVIC_BASE = 0xE000E100UL;

DBGMCU_TypeDef *const DBGMCU = (DBGMCU_TypeDef*) DBGMCU_BASE;
RCC_TypeDef *const RCC = (RCC_TypeDef*) RCC_BASE;
GPIO_TypeDef *const GPIOA = (GPIO_TypeDef*) GPIOA_BASE;
GPIO_TypeDef *const GPIOB = (GPIO_TypeDef*) GPIOB_BASE;
GPIO_TypeDef *const GPIOC = (GPIO_TypeDef*) GPIOC_BASE;
USART_TypeDef *const USART = (USART_TypeDef*) USART2_BASE;
I2C_TypeDef *const I2C = (I2C_TypeDef*) I2C1_BASE;
TIM_TypeDef *const TIM2 = (TIM_TypeDef*) TIM2_BASE;
TIM_TypeDef *const TIM3 = (TIM_TypeDef*) TIM3_BASE;
EXTI_TypeDef *const EXTI = (EXTI_TypeDef*) EXTI_BASE;
SYSCFG_TypeDef *const SYSCFG = (SYSCFG_TypeDef*) SYSCFG_BASE;
NVIC_TypeDef *const NVIC = (NVIC_TypeDef*) NVIC_BASE;

