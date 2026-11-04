#include "stm32f411xe.h"
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
#define MODER_AF (1U << 1)
#define MODER_AF_P(n) (2 * n)

void i2s1_gpio_init() {
  GPIOA->MODER |= MODER_AF << MODER_AF_P(1);
  GPIOA->MODER |= MODER_AF << MODER_AF_P(4);
  GPIOA->MODER |= MODER_AF << MODER_AF_P(5);

  // AFRL for 0..7
  // GPIOA->AFR[0] |= ;
}

void i2s1_init() {
  i2s1_gpio_init();
  // NVIC_EnableIRQ(SysTick_IRQn);
}
