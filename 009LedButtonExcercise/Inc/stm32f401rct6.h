#ifndef STM32F401RCT6_H_
#define STM32F401RCT6_H_

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
	volatile uint32_t OAR1;
	volatile uint32_t OAR2;
	volatile uint32_t DR;
	volatile uint32_t SR1;
	volatile uint32_t SR2;
	volatile uint32_t CCR;
	volatile uint32_t TRISE;
	volatile uint32_t FLTR;
} I2C_TypeDef;

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

extern DBGMCU_TypeDef *const DBGMCU;
extern RCC_TypeDef *const RCC;
extern GPIO_TypeDef *const GPIOA;
extern GPIO_TypeDef *const GPIOB;
extern GPIO_TypeDef *const GPIOC;
extern USART_TypeDef *const USART;
extern I2C_TypeDef *const I2C;
extern TIM_TypeDef *const TIM2;
extern TIM_TypeDef *const TIM3;
extern TIM_TypeDef *const TIM4;
extern EXTI_TypeDef *const EXTI;
extern SYSCFG_TypeDef *const SYSCFG;
extern NVIC_TypeDef *const NVIC;

#endif /* STM32F401RCT6_H_ */
