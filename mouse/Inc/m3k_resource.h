#pragma once

#define M3K_USB_MFG "Zaunkoenig"
#define M3K_USB_NAME "M3K"
#define M3K_FW_VERSION " firmware v1.21"
#define M3K_USB_PID 0xA462
#define M3K_USB_VID 0x0483

// SPI periph pin clocks
#define SPIx                             SPI3
#define SPIx_CLK_ENABLE()                do {RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;} while(0)
#define SPIx_SCK_GPIO_CLK_ENABLE()       do {RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;} while(0)
#define SPIx_MISO_GPIO_CLK_ENABLE()      do {RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;} while(0)
#define SPIx_MOSI_GPIO_CLK_ENABLE()      do {RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;} while(0)
#define SPIx_SS_GPIO_CLK_ENABLE()	     do {RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;} while(0)

// SPI
#define SPIx_SCK_GPIO_PORT  GPIOC
#define SPIx_SCK_PIN_Pos    10
#define SPIx_SCK_PIN        (1 << SPIx_SCK_PIN_Pos)
#define SPIx_SCK_AF         6 //GPIO_AF6_SPI3

#define SPIx_MISO_GPIO_PORT GPIOB
#define SPIx_MISO_PIN_Pos   4
#define SPIx_MISO_PIN       (1 << SPIx_MISO_PIN_Pos)
#define SPIx_MISO_AF        6 //GPIO_AF6_SPI3

#define SPIx_MOSI_GPIO_PORT GPIOB
#define SPIx_MOSI_PIN_Pos   2
#define SPIx_MOSI_PIN       (1 << SPIx_MOSI_PIN_Pos)
#define SPIx_MOSI_AF        7 //GPIO_AF6_SPI3

#define SPIx_SS_PORT        GPIOC
#define SPIx_SS_PIN_Pos     9
#define SPIx_SS_PIN         (1 << SPIx_SS_PIN_Pos)

// 3399 NRESET
#define NRESET_GPIO_CLK_ENABLE() do {RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;} while(0)
#define NRESET_PORT    GPIOA
#define NRESET_PIN_Pos 8
#define NRESET_PIN     (1 << NRESET_PIN_Pos)

// lmb
#define LMB_NO_CLK_ENABLE() do {RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;} while(0)
#define LMB_NO_PORT         GPIOE
#define LMB_NO_PIN_Pos      9
#define LMB_NO_PIN          (1 << LMB_NO_PIN_Pos)
#define LMB_NO_EXTICFG      SYSCFG_EXTICR3_EXTI9_PE

#define LMB_NC_CLK_ENABLE() do {RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;} while(0)
#define LMB_NC_PORT         GPIOC
#define LMB_NC_PIN_Pos      2
#define LMB_NC_PIN          (1 << LMB_NC_PIN_Pos)
#define LMB_NC_EXTICFG      SYSCFG_EXTICR1_EXTI2_PC

// rmb
#define RMB_NO_CLK_ENABLE() do {RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;} while(0)
#define RMB_NO_PORT         GPIOE
#define RMB_NO_PIN_Pos      3
#define RMB_NO_PIN          (1 << RMB_NO_PIN_Pos)
#define RMB_NO_EXTICFG      SYSCFG_EXTICR1_EXTI3_PE

#define RMB_NC_CLK_ENABLE() do {RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;} while(0)
#define RMB_NC_PORT         GPIOE
#define RMB_NC_PIN_Pos      6
#define RMB_NC_PIN          (1 << RMB_NC_PIN_Pos)
#define RMB_NC_EXTICFG      SYSCFG_EXTICR2_EXTI6_PE
