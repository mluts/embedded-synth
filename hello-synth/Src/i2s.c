#include "stm32f411xe.h"
#include "stm32f4xx.h"
#include "system_stm32f4xx.h"

typedef enum {
  Audio16Bit, // 16-bit frame, 48 kHz stereo: 48000 * 2 * 16 = 1.536 MHz
  Audio24Bit  // 24-bit frame, 48kHz stereo: 48000 * 2 * 32 = 3.072 MHz (32-bit
              // padded frame used for 24-bit audio)
} AudioType;

// DS STM32F411:
//
// I2S1:
//   SD - PA7
//   WS - PA4
//   CK - PA5

// 00 - Input
// 01 - Output
// 10 - AF
// 11 - Analog
#define MODER_AF 0x1UL

// I2S mode (see DS page 48)
#define AF_MODE_5 0x05UL

// 00 = low speed
// 01 = medium speed
// 10 = fast speed
// 11 = high speed
#define OUTPUT_HIGH_SPEED 0x3UL

/// @brief  Enable GPIOA RCC Clock on AHB1
void i2s1_rcc_enable(AudioType at) {
  // Enable GPIOA RCC
  SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);
  // Enable SPI1 RCC
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI1EN);

  // Do not use external I2S RCC
  CLEAR_BIT(RCC->CFGR, RCC_CFGR_I2SSRC_Pos);

  // MODIFY_REG(RCC->PLLI2SCFGR, RCC_PLLI2SCF, SETMASK)

  // MODIFY_REG(RCC->PLLI2SCFGR, RCC_PLL);
}

/// @brief Initializes GPIO for I2S on PA7, PA4, PA5
/// @details Set no pull-up / no pull-down
///          Set high speed for I2S
///          Set MODER=AF (Alternative Function) to connect this GPIO to some
///          peripheral Set Alterative Function to 5 (because it's SPI/I2S1
///          peripheral)
void i2s1_gpioa_configure() {

  // Set push-pull for PA7, PA4, PA5
  CLEAR_BIT(GPIOA->OTYPER,
            (GPIO_OTYPER_OT7 | GPIO_OTYPER_OT4 | GPIO_OTYPER_OT5));

  // Set no pull-up / no pull-down
  CLEAR_BIT(GPIOA->PUPDR,
            GPIO_PUPDR_PUPD7_Msk | GPIO_PUPDR_PUPD4_Msk | GPIO_PUPDR_PUPD5_Msk);

  // Set high speed for PA7, PA4, PA5
  MODIFY_REG(GPIOA->OSPEEDR,
             (GPIO_OSPEEDR_OSPEED7_Msk | GPIO_OSPEEDR_OSPEED4_Msk |
              GPIO_OSPEEDR_OSPEED5_Msk),
             (OUTPUT_HIGH_SPEED << GPIO_OSPEEDR_OSPEED7_Pos) |
                 (OUTPUT_HIGH_SPEED << GPIO_OSPEEDR_OSPEED4_Pos) |
                 (OUTPUT_HIGH_SPEED << GPIO_OSPEEDR_OSPEED5_Pos));

  // Set MODER to AF for PA7, PA4, PA5
  MODIFY_REG(GPIOA->MODER,
             GPIO_MODER_MODER7_Msk | GPIO_MODER_MODER4_Msk |
                 GPIO_MODER_MODER5_Msk,
             (MODER_AF << GPIO_MODER_MODER7_Pos) |
                 (MODER_AF << GPIO_MODER_MODER4_Pos) |
                 (MODER_AF << GPIO_MODER_MODER5_Pos));

  // Set AFRL to AF5 for PA7, PA4, PA5
  MODIFY_REG(GPIOA->AFR[0],
             GPIO_AFRL_AFSEL7_Msk | GPIO_AFRL_AFSEL4_Msk | GPIO_AFRL_AFSEL5_Msk,
             (AF_MODE_5 << GPIO_AFRL_AFSEL7_Pos) |
                 (AF_MODE_5 << GPIO_AFRL_AFSEL4_Pos) |
                 (AF_MODE_5 << GPIO_AFRL_AFSEL5_Pos));
}

void i2s1_peripheral_init() {}

void i2s1_init(AudioType at) {
  i2s1_rcc_enable(at);
  i2s1_gpioa_configure();
  i2s1_peripheral_init();
  // NVIC_EnableIRQ(SysTick_IRQn);
}
