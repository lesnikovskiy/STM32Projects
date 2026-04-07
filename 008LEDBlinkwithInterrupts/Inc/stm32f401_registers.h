#ifndef STM32F401_REGISTERS_H
#define STM32F401_REGISTERS_H

#include <stdint.h>

/* ============================================
   BASE ADDRESSES
   ============================================ */

#define RCC_BASE        0x40023800UL
#define GPIOA_BASE      0x40020000UL
#define GPIOC_BASE      0x40020800UL
#define SYSCFG_BASE     0x40013800UL
#define EXTI_BASE       0x40013C00UL
#define NVIC_BASE       0xE000E100UL

/* ============================================
   RCC - RESET AND CLOCK CONTROL
   ============================================ */

#define RCC_AHB1ENR     (*(volatile uint32_t*)(RCC_BASE + 0x30))
#define RCC_APB2ENR     (*(volatile uint32_t*)(RCC_BASE + 0x44))

/* RCC_AHB1ENR bits */
#define RCC_AHB1ENR_GPIOAEN   (1 << 0)
#define RCC_AHB1ENR_GPIOCEN   (1 << 2)

/* RCC_APB2ENR bits */
#define RCC_APB2ENR_SYSCFGEN  (1 << 14)

/* ============================================
   GPIO - GENERAL PURPOSE INPUT/OUTPUT
   ============================================ */

#define GPIOA_MODER     (*(volatile uint32_t*)(GPIOA_BASE + 0x00))
#define GPIOA_PUPDR     (*(volatile uint32_t*)(GPIOA_BASE + 0x0C))
#define GPIOA_IDR       (*(volatile uint32_t*)(GPIOA_BASE + 0x10))

#define GPIOC_MODER     (*(volatile uint32_t*)(GPIOC_BASE + 0x00))
#define GPIOC_BSRR      (*(volatile uint32_t*)(GPIOC_BASE + 0x18))
#define GPIOC_ODR       (*(volatile uint32_t*)(GPIOC_BASE + 0x14))

/* GPIO MODER bit masks */
#define GPIO_MODER_MASK   (3U)  /* 2 bits per pin */
#define GPIO_MODE_INPUT   (0U)
#define GPIO_MODE_OUTPUT  (1U)
#define GPIO_MODE_AF      (2U)
#define GPIO_MODE_ANALOG  (3U)

/* GPIO PUPDR bit masks */
#define GPIO_PUPDR_MASK   (3U)
#define GPIO_PUPDR_NONE   (0U)
#define GPIO_PUPDR_PULLUP (1U)
#define GPIO_PUPDR_PULLDN (2U)

/* PC13 (LED) definitions */
#define LED_PIN           13
#define LED_PIN_MASK      (GPIO_MODER_MASK << (LED_PIN * 2))
#define LED_BSRR_SET      (1 << LED_PIN)
#define LED_BSRR_RESET    (1 << (LED_PIN + 16))

/* PA0 (Button) definitions */
#define BUTTON_PIN        0
#define BUTTON_PIN_MASK   (GPIO_MODER_MASK << (BUTTON_PIN * 2))
#define BUTTON_PULL_MASK  (GPIO_PUPDR_MASK << (BUTTON_PIN * 2))

/* ============================================
   SYSCFG - SYSTEM CONFIGURATION
   ============================================ */

#define SYSCFG_EXTICR1   (*(volatile uint32_t*)(SYSCFG_BASE + 0x08))

/* EXTI line configuration masks */
#define EXTI0_CONFIG_MASK  (0xFU << 0)
#define EXTI0_PORT_A       (0x0U << 0)

/* ============================================
   EXTI - EXTERNAL INTERRUPTS
   ============================================ */

#define EXTI_IMR        (*(volatile uint32_t*)(EXTI_BASE + 0x00))
#define EXTI_FTSR       (*(volatile uint32_t*)(EXTI_BASE + 0x0C))
#define EXTI_PR         (*(volatile uint32_t*)(EXTI_BASE + 0x14))

/* EXTI line 0 definitions */
#define EXTI_LINE0      (1 << 0)

/* ============================================
   NVIC - NESTED VECTORED INTERRUPT CONTROLLER
   ============================================ */

#define NVIC_ISER0      (*(volatile uint32_t*)(NVIC_BASE + 0x00))

/* IRQ numbers for STM32F4 */
#define EXTI0_IRQn      6
#define EXTI0_IRQ_BIT   (1 << EXTI0_IRQn)

#endif /* STM32F401_REGISTERS_H */
