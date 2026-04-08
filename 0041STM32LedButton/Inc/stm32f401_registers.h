#ifndef STM32F401_REGISTERS_H_
#define STM32F401_REGISTERS_H_

#include <stdint.h>

// --- Clock Control Registers (RCC) ---
#define RCC_BASE      0x40023800UL

#define GPIOA_BASE    0x40020000UL
#define GPIOC_BASE    0x40020800UL

// Specific registers
#define RCC_AHB1ENR   *(volatile uint32_t*)(RCC_BASE + 0x30) // Enable GPIO clock
#define RCC_APB1ENR   *(volatile uint32_t*)(RCC_BASE + 0x40) // Enable UART2 clock

#define GPIOA_MODER   *(volatile uint32_t*)(GPIOA_BASE + 0x00)
#define GPIOA_PUPDR   *(volatile uint32_t*)(GPIOA_BASE + 0x0C)
#define GPIOA_IDR     *(volatile uint32_t*)(GPIOA_BASE + 0x10) // Input Data Register
#define GPIOA_AFRL    *(volatile uint32_t*)(GPIOA_BASE + 0x20)

#define GPIOC_MODER   *(volatile uint32_t*)(GPIOC_BASE + 0x00)
#define GPIOC_ODR     *(volatile uint32_t*)(GPIOC_BASE + 0x14)

#define GPIOC_BSRR    *(volatile uint32_t*)(GPIOC_BASE + 0x18)

// USART2 Registers

#define USART2_BASE   0x40004400UL
#define USART2_SR     *(volatile uint32_t*)(USART2_BASE + 0x00) // Status
#define USART2_DR     *(volatile uint32_t*)(USART2_BASE + 0x04) // Data
#define USART2_BRR    *(volatile uint32_t*)(USART2_BASE + 0x08) // Baudrate
#define USART2_CR1    *(volatile uint32_t*)(USART2_BASE + 0x0C) // Control 1

#endif /* STM32F401_REGISTERS_H_ */
