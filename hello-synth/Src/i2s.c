#include "stm32f411xe.h"
#include "stm32f4xx.h"
#include "system_stm32f4xx.h"

// DS STM32F411:
//
// I2S1:
//   SD - PA1
//   WS - PA4
//   CK - PA5

// 00 - Input
// 01 - Output
// 10 - AF
// 11 - Analog
#define MODER_AF 0x1UL

// I2S mode (see DS page 48)
#define AF_MODE_5 0x05UL

#define OUTPUT_HIGH_SPEED 0x3UL

void i2s1_gpioa_init() {
  // Enable GPIOA RCC
  SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);

  // Set push-pull for PA1, PA4, PA5
  CLEAR_BIT(GPIOA->OTYPER,
            (GPIO_OTYPER_OT1 | GPIO_OTYPER_OT4 | GPIO_OTYPER_OT5));

  // Set no pull-up / no pull-down
  CLEAR_BIT(GPIOA->PUPDR,
            GPIO_PUPDR_PUPD1_Msk | GPIO_PUPDR_PUPD4_Msk | GPIO_PUPDR_PUPD5_Msk);

  // Set high speed for PA1, PA4, PA5
  MODIFY_REG(GPIOA->OSPEEDR,
             (GPIO_OSPEEDR_OSPEED1_Msk | GPIO_OSPEEDR_OSPEED4_Msk |
              GPIO_OSPEEDR_OSPEED5_Msk),
             (OUTPUT_HIGH_SPEED << GPIO_OSPEEDR_OSPEED1_Pos) |
                 (OUTPUT_HIGH_SPEED << GPIO_OSPEEDR_OSPEED4_Pos) |
                 (OUTPUT_HIGH_SPEED << GPIO_OSPEEDR_OSPEED5_Pos));

  // Set MODER to AF for PA1, PA4, PA5
  MODIFY_REG(GPIOA->MODER,
             GPIO_MODER_MODER1_Msk | GPIO_MODER_MODER4_Msk |
                 GPIO_MODER_MODER5_Msk,
             (MODER_AF << GPIO_MODER_MODER1_Pos) |
                 (MODER_AF << GPIO_MODER_MODER4_Pos) |
                 (MODER_AF << GPIO_MODER_MODER5_Pos));

  // Set AFRL to AF5 for PA1, PA4, PA5
  MODIFY_REG(GPIOA->AFR[0],
             GPIO_AFRL_AFSEL1_Msk | GPIO_AFRL_AFSEL4_Msk | GPIO_AFRL_AFSEL5_Msk,
             (AF_MODE_5 << GPIO_AFRL_AFSEL1_Pos) |
                 (AF_MODE_5 << GPIO_AFRL_AFSEL4_Pos) |
                 (AF_MODE_5 << GPIO_AFRL_AFSEL5_Pos));
}

void i2s1_init() {
  i2s1_gpioa_init();
  // NVIC_EnableIRQ(SysTick_IRQn);
}
