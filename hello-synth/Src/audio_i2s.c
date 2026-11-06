#include "stm32f411xe.h"
#include "stm32f4xx.h"
#include "system_stm32f4xx.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
  Audio16Bit, // 16-bit frame, 48 kHz stereo: 48000 * 2 * 16 = 1.536 MHz
  Audio24Bit  // 24-bit frame, 48kHz stereo: 48000 * 2 * 32 = 3.072 MHz (32-bit
              // padded frame used for 24-bit audio)
} AudioPacketLength;

typedef struct {
  AudioPacketLength audio_type;
  uint8_t packet_length;

} AudioHandle_t;

// Datasheet For STM32F411:
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

// How fast edges rise/fall
// 00 = low speed ~4-8MHz
// 01 = medium speed ~25MHz
// 10 = fast speed ~50MHz
// 11 = high speed ~100MHz
#define OUTPUT_SLOW_SPEED 0x0UL
#define OUTPUT_MEDIUM_SPEED 0x1UL
#define OUTPUT_FAST_SPEED 0x2UL
#define OUTPUT_HIGH_SPEED 0x3UL

/// @brief  Enable GPIOA RCC Clock on AHB1
void i2s1_rcc_enable(AudioHandle_t *a) {
  // Enable GPIOA RCC
  SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);

  // MODIFY_REG(RCC->PLLI2SCFGR, RCC_PLLI2SCF, SETMASK)

  // MODIFY_REG(RCC->PLLI2SCFGR, RCC_PLL);

  // Do not use external I2S RCC
  CLEAR_BIT(RCC->CFGR, RCC_CFGR_I2SSRC_Pos);

  // Enable SPI1 RCC
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI1EN);
}

/// @brief Initializes GPIO for I2S on PA7, PA4, PA5
/// @details Set no pull-up / no pull-down
///          Set high speed for I2S
///          Set MODER=AF (Alternative Function) to connect this GPIO to some
///          peripheral Set Alterative Function to 5 (because it's SPI/I2S1
///          peripheral)
///          PA7 -> SD
///          PA5 -> CK
///          PA4 -> WS
void i2s1_gpioa_configure() {

  // Set push-pull for PA7, PA4, PA5
  CLEAR_BIT(GPIOA->OTYPER,
            (GPIO_OTYPER_OT7 | GPIO_OTYPER_OT4 | GPIO_OTYPER_OT5));

  // Set no pull-up / no pull-down
  CLEAR_BIT(GPIOA->PUPDR,
            GPIO_PUPDR_PUPD7_Msk | GPIO_PUPDR_PUPD4_Msk | GPIO_PUPDR_PUPD5_Msk);

  // Low speed for WS
  CLEAR_BIT(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED4_Pos);

  // Low speed for SD
  CLEAR_BIT(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED7_Pos);

  // Medium speed for CK (just in case)
  MODIFY_REG(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED5_Msk,
             OUTPUT_MEDIUM_SPEED << GPIO_OSPEEDR_OSPEED5_Pos);

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

void i2s1_peripheral_init() {
  
}

bool i2s1_init(AudioHandle_t *a) {
  if (a == NULL) {
    return false;
  }

  i2s1_rcc_enable(a);
  i2s1_gpioa_configure();
  i2s1_peripheral_init();
  // NVIC_EnableIRQ(SysTick_IRQn);

  return true;
}
