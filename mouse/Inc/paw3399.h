#pragma once

#include <delay.h>
#include <m3k_resource.h>
#include <stdint.h>
#include "stm32f7xx.h"

// Define Registers for Bank 0x00
// Registers marked with ? are not present in data sheet
#define REG_PRODUCT_ID			0x00
#define REG_REVISION_ID			0x01
#define REG_MOTION				0x02
#define REG_DELTA_X_L			0x03
#define REG_DELTA_X_H			0x04
#define REG_DELTA_Y_L			0x05
#define REG_DELTA_Y_H			0x06
#define REG_SQUAL				0x07
#define REG_RAWDATA_SUM			0x08
#define REG_MAXIMUM_RAWDATA		0x09
#define REG_MINIMUM_RAWDATA		0x0A
#define REG_SHUTTER_LOWER		0x0B
#define REG_SHUTTER_UPPER		0x0C

#define REG_OBSERVATION			0x15
#define REG_MOTION_BURST		0x16

#define REG_SROM_ENABLE         0x22 // ?
#define REG_SROM_CONFIG         0x23 // ?

#define REG_POWER_UP_RESET		0x3A
#define REG_SHUTDOWN			0x3B

#define REG_PERFORMANCE			0x40

#define REG_SET_RESOLUTION		0x47
#define REG_RESOLUTION_X_LOW	0x48
#define REG_RESOLUTION_X_HIGH	0x49
#define REG_RESOLUTION_Y_LOW	0x4A
#define REG_RESOLUTION_Y_HIGH	0x4B

#define REG_INIT_RES_X_HIGH		0x4E // ?
#define REG_INIT_RES_X_LOW		0x4F // ?

#define REG_INIT_RES_Y_HIGH		0x51 // ?
#define REG_INIT_RES_Y_LOW		0x52 // ?

#define REG_MODE_CONTROL        0x55 // ?
#define REG_ANGLE_SNAP			0x56

#define REG_RAWDATA_OUTPUT		0x58
#define REG_RAWDATA_STATUS		0x59
#define REG_RIPPLE_CONTROL		0x5A
#define REG_AXIS_CONTROL		0x5B
#define REG_MOTION_CTRL			0x5C

#define REG_INV_PRODUCT_ID		0x5F



#define REG_SROM_STATUS			0x6C

#define REG_RUN_DOWNSHIFT		0x77
#define REG_REST1_RATE			0x78
#define REG_REST1_DOWNSHIFT		0x79
#define REG_REST2_RATE			0x7A
#define REG_REST2_DOWNSHIFT		0x7B
#define REG_REST3_RATE			0x7C
#define REG_RUN_DOWNSHIFT_MULT	0x7D
#define REG_REST_DOWNSHIFT_MULT	0x7E
#define REG_BANK_SELECT			0x7F


static void spi_init()
{
	// GPIO config
	SPIx_SCK_GPIO_CLK_ENABLE();
	SPIx_MISO_GPIO_CLK_ENABLE();
	SPIx_MOSI_GPIO_CLK_ENABLE();
	SPIx_SS_GPIO_CLK_ENABLE();

	// SCK
	MODIFY_REG(SPIx_SCK_GPIO_PORT->MODER,
			0b11 << (2*SPIx_SCK_PIN_Pos),
			0b10 << (2*SPIx_SCK_PIN_Pos));
	MODIFY_REG(SPIx_SCK_GPIO_PORT->AFR[SPIx_SCK_PIN_Pos >= 8],
			0b1111 << ((4*SPIx_SCK_PIN_Pos)%32),
			SPIx_SCK_AF << ((4*SPIx_SCK_PIN_Pos)%32));
	// MISO
	MODIFY_REG(SPIx_MISO_GPIO_PORT->MODER,
			0b11 << (2*SPIx_MISO_PIN_Pos),
			0b10 << (2*SPIx_MISO_PIN_Pos));
	MODIFY_REG(SPIx_MISO_GPIO_PORT->PUPDR,
			0b11 << (2*SPIx_MISO_PIN_Pos),
			0b10 << (2*SPIx_MISO_PIN_Pos));
	MODIFY_REG(SPIx_MISO_GPIO_PORT->AFR[SPIx_MISO_PIN_Pos >= 8],
			0b1111 << ((4*SPIx_MISO_PIN_Pos)%32),
			SPIx_MISO_AF << ((4*SPIx_MISO_PIN_Pos)%32));
	// MOSI
	MODIFY_REG(SPIx_MOSI_GPIO_PORT->MODER,
			0b11 << (2*SPIx_MOSI_PIN_Pos),
			0b10 << (2*SPIx_MOSI_PIN_Pos));
	MODIFY_REG(SPIx_MOSI_GPIO_PORT->AFR[SPIx_MOSI_PIN_Pos >= 8],
			0b1111 << ((4*SPIx_MOSI_PIN_Pos)%32),
			SPIx_MOSI_AF << ((4*SPIx_MOSI_PIN_Pos)%32));
	// PB6 SS
	MODIFY_REG(SPIx_SS_PORT->MODER,
			0b11 << (2*SPIx_SS_PIN_Pos),
			0b01 << (2*SPIx_SS_PIN_Pos));

	// SPI config
	SPIx_CLK_ENABLE();
	SPIx->CR1 = SPI_CR1_SSM | SPI_CR1_SSI // software SS
			| (0b000 << SPI_CR1_BR_Pos) // assumes PCLK2 = 32MHz. divide by 4 for 8MHz
			| SPI_CR1_MSTR // master
			| SPI_CR1_CPOL // CPOL = 1
			| SPI_CR1_CPHA; // CPHA = 1
	SPIx->CR2 = SPI_CR2_FRXTH // 8-bit level for RXNE
			| (0b0111 << SPI_CR2_DS_Pos); // 8-bit data
	SPIx->CR1 |=  SPI_CR1_SPE; // enable SPI
}

static inline void ss_low(void)
{
	SPIx_SS_PORT->BSRR = (uint32_t)SPIx_SS_PIN << 16;
}

static inline void ss_high(void)
{
	SPIx_SS_PORT->BSRR = SPIx_SS_PIN;
}

static inline uint8_t spi_sendrecv(uint8_t b)
{
    while (!(SPIx->SR & SPI_SR_TXE));
    *(__IO uint8_t *)&SPIx->DR = b;
    while (!(SPIx->SR & SPI_SR_RXNE));
    return *(__IO uint8_t *)&SPIx->DR;
}

#define spi_recv(x) spi_sendrecv(0)
#define spi_send(x) (void)spi_sendrecv(x)

static void spi_write(const uint8_t addr, const uint8_t data) {
	spi_send(addr | 0x80);
	spi_send(data);
	delay_us(5); // maximum of t_SWW, t_SWR
}

static uint8_t spi_read(const uint8_t addr) {
    spi_send(addr);
    delay_us(2); // t_SRAD
    uint8_t rd = spi_recv();
	delay_us(2); // maximum of t_SRW, t_SRR
	return rd;
}

// -------------------------------------------------------------------------------------------------
// Power-Up Initialization (6.2.1 - 6.2.107)
// -------------------------------------------------------------------------------------------------
static void PowerUpInit(void)
{
	ss_low();

	spi_write(REG_PERFORMANCE, 0x80); // Disable Rest Mode

	spi_write(REG_BANK_SELECT, 0x0E); // Select Bank 0x0E
	spi_write(0x55, 0x0D);
	spi_write(0x56, 0x1B);
	spi_write(0x57, 0xE8);
	spi_write(0x58, 0xD5);

	spi_write(REG_BANK_SELECT, 0x14); // Select Bank 0x14
	spi_write(0x42, 0xBC);
	spi_write(0x43, 0x74);
	spi_write(0x4B, 0x20);
	spi_write(0x4D, 0x00);
	spi_write(0x53, 0x0D);

	spi_write(REG_BANK_SELECT, 0x05); // Select Bank 0x05
	spi_write(0x51, 0x40);
	spi_write(0x53, 0x40);
	spi_write(0x55, 0xCA);
	spi_write(0x61, 0x31);
	spi_write(0x62, 0x64);
	spi_write(0x6D, 0xB8);
	spi_write(0x6E, 0x0F);
	spi_write(0x70, 0x02);
	spi_write(0x4A, 0x2A);
	spi_write(0x60, 0x26);

	spi_write(REG_BANK_SELECT, 0x06); // Select Bank 0x06
	spi_write(0x6D, 0x70);
	spi_write(0x6E, 0x60);
	spi_write(0x6F, 0x04);
	spi_write(0x53, 0x02);
	spi_write(0x55, 0x11);
	spi_write(0x7D, 0x51);

	spi_write(REG_BANK_SELECT, 0x08); // Select Bank 0x08
	spi_write(0x71, 0x4F);

	spi_write(REG_BANK_SELECT, 0x09); // Select Bank 0x09
	spi_write(0x62, 0x1F);
	spi_write(0x63, 0x1F);
	spi_write(0x65, 0x03);
	spi_write(0x66, 0x03);
	spi_write(0x67, 0x1F);
	spi_write(0x68, 0x1F);
	spi_write(0x69, 0x03);
	spi_write(0x6A, 0x03);
	spi_write(0x6C, 0x1F);
	spi_write(0x6D, 0x1F);
	spi_write(0x51, 0x04);
	spi_write(0x53, 0x20);
	spi_write(0x54, 0x20);
	spi_write(0x71, 0x0F);

	spi_write(REG_BANK_SELECT, 0x0A); // Select Bank 0x0A
	spi_write(0x4A, 0x14);
	spi_write(0x4C, 0x14);
	spi_write(0x55, 0x19);

	spi_write(REG_BANK_SELECT, 0x14); // Select Bank 0x14
	spi_write(0x63, 0x16);

	spi_write(REG_BANK_SELECT, 0x0C); // Select Bank 0x0C
	spi_write(0x41, 0x30);
	spi_write(0x55, 0x14);
	spi_write(0x49, 0x0A);
	spi_write(0x42, 0x00);
	spi_write(0x44, 0x0A);
	spi_write(0x5A, 0x0A);
	spi_write(0x5F, 0x1E);
	spi_write(0x5B, 0x05);
	spi_write(0x5E, 0x0F);

	spi_write(REG_BANK_SELECT, 0x0D); // Select Bank 0x0D
	spi_write(0x48, 0xDC);
	spi_write(0x5A, 0x29);
	spi_write(0x5B, 0x47);
	spi_write(0x5C, 0x81);
	spi_write(0x5D, 0x40);
	spi_write(0x71, 0xDC);
	spi_write(0x70, 0x07);
	spi_write(0x73, 0x00);
	spi_write(0x72, 0x08);
	spi_write(0x75, 0xDC);
	spi_write(0x74, 0x07);
	spi_write(0x77, 0x00);
	spi_write(0x76, 0x08);

	spi_write(REG_BANK_SELECT, 0x10); // Select Bank 0x10
	spi_write(0x4C, 0xD0);

	spi_write(REG_BANK_SELECT, 0x00); // Select Bank 0x00
	spi_write(REG_INIT_RES_X_LOW, 0x63); // Set lower Initialization X-Resolution
	spi_write(REG_INIT_RES_X_HIGH, 0x00); // Set higher Initialization X-Resolution
	spi_write(REG_INIT_RES_Y_LOW, 0x63); // Set lower Initialization Y-Resolution
	spi_write(REG_INIT_RES_Y_HIGH, 0x00); // Set higher Initialization Y-Resolution
	spi_write(REG_RUN_DOWNSHIFT, 0x4F); // Enter Rest1 Mode after ~1s (79*256*50µs)
	spi_write(REG_SET_RESOLUTION, 0x01); // Update Resolution
	spi_write(REG_AXIS_CONTROL, 0x40);	// Invert Y-Axis
	spi_write(0x66, 0x13);
	spi_write(0x67, 0x0F);
	spi_write(REG_REST1_RATE, 0x01); // Set Rest1 Mode polling rate to 1000Hz (1ms/1)
	spi_write(REG_REST1_DOWNSHIFT, 0x9C); // Enter Rest2 Mode after ~10s (156*64*1ms)
	spi_write(REG_MODE_CONTROL, 0x02); // Enter SROM Mode
	spi_write(REG_SROM_CONFIG, 0x70); // Set SROM ID
	spi_write(REG_SROM_ENABLE, 0x01); // Enable SROM Access

	ss_high();
	int i;
	for (i = 0; i < 60; i++) {
		ss_high();
		delay_us(992);
		ss_low();
		if (spi_read(REG_SROM_STATUS) == 0x80) break;
	}
	ss_low();
	if (i == 60) {
		spi_write(REG_BANK_SELECT, 0x14);
		spi_write(0x6C, 0x00);
		spi_write(REG_BANK_SELECT, 0x00);
	}

	spi_write(REG_SROM_ENABLE, 0x00); // Disable SROM Access (?)
	spi_write(REG_MODE_CONTROL, 0x00); // Return to Normal Run Mode (?)

	spi_write(REG_BANK_SELECT, 0x00); // Select Bank 0x00
	spi_write(REG_PERFORMANCE, 0x00); // Enables Rest Mode

	(void)spi_read(REG_MOTION); // Read Motion Register
	(void)spi_read(REG_DELTA_X_L); // Read lower X-Movement
	(void)spi_read(REG_DELTA_X_H); // Read higher X-Movement
	(void)spi_read(REG_DELTA_Y_L); // Read lower Y-Movement
	(void)spi_read(REG_DELTA_Y_H); // Read higher Y-Movement

	ss_high();
}

// -------------------------------------------------------------------------------------------------
// Disable Lift Cut Off Calibration (7.5.3.1 - 7.5.3.20)
// -------------------------------------------------------------------------------------------------
static void DisableLiftOff() {
	ss_low();

	spi_write(REG_BANK_SELECT, 0x0C); // Select Bank 0x0C
	spi_write(0x41, 0x30);
	spi_write(0x43, 0x20);
	spi_write(0x44, 0x0D);
	spi_write(0x4A, 0x12);
	spi_write(0x4B, 0x09);
	spi_write(0x4C, 0x30);
	spi_write(0x4E, 0x08);
	spi_write(0x53, 0x16);
	spi_write(0x55, 0x14);
	spi_write(0x5A, 0x0D);
	spi_write(0x5B, 0x05);
	spi_write(0x5F, 0x1E);
	spi_write(0x66, 0x30);

	spi_write(REG_BANK_SELECT, 0x05); // Select Bank 0x05
	spi_write(0x6E, 0x0F);

	spi_write(REG_BANK_SELECT, 0x09); // Select Bank 0x09
	spi_write(0x71, 0x0F);
	spi_write(0x72, 0x0A);

	spi_write(REG_BANK_SELECT, 0x00); // Select Bank 0x00

	ss_high();
}

// -------------------------------------------------------------------------------------------------
// Enable Corded Gaming Mode (7.3)
// -------------------------------------------------------------------------------------------------
static void EnableCordedGaming() {
	ss_low();

	spi_write(REG_BANK_SELECT, 0x05); // Select Bank 0x05
	spi_write(0x51, 0x40);
	spi_write(0x53, 0x40);
	spi_write(0x61, 0x31);
	spi_write(0x6E, 0x0F);

	spi_write(REG_BANK_SELECT, 0x07); // Select Bank 0x07
	spi_write(0x42, 0x2F);
	spi_write(0x43, 0x00);

	spi_write(REG_BANK_SELECT, 0x0D); // Select Bank 0x0D
	spi_write(0x51, 0x12);
	spi_write(0x52, 0xDB);
	spi_write(0x53, 0x12);
	spi_write(0x54, 0xDC);
	spi_write(0x55, 0x12);
	spi_write(0x56, 0xE4);
	spi_write(0x57, 0x15);
	spi_write(0x58, 0x2D);

	spi_write(REG_BANK_SELECT, 0x14); // Select Bank 0x14
	spi_write(0x63, 0x1E);

	spi_write(REG_BANK_SELECT, 0x00); // Select Bank 0x00
	spi_write(0x54, 0x55);
	spi_write(REG_PERFORMANCE, 0x83); // Disable Rest Mode and set Corded Gaming Mode (?)

	ss_high();
}

// -------------------------------------------------------------------------------------------------
// Additional Setup
// -------------------------------------------------------------------------------------------------
static void AdditionalSetup() {
	ss_low();

	spi_write(REG_AXIS_CONTROL, 0x20); //invert x-axis to compensate for chip orientation

	ss_high();
}

// -------------------------------------------------------------------------------------------------
// Custom Config
// -------------------------------------------------------------------------------------------------
static void CustomConfig() {
	ss_low();

	spi_write(REG_BANK_SELECT, 0x0C); // Select Bank 0x0C
	spi_write(0x4E, 0x00); // Set Lift Off Distance to 1mm

	spi_write(REG_BANK_SELECT, 0x00); // Select Bank 0x00
	spi_write(0x48, 0x7F); //
	spi_write(0x49, 0x00); //
	spi_write(0x4A, 0x7F); //
	spi_write(0x4B, 0x00); //
	spi_write(0x47, 0x01); // Set Resolution
	spi_write(REG_ANGLE_SNAP, 0x00); // Disable Angle Snap

	ss_high();
}

// -------------------------------------------------------------------------------------------------
// Tweaks
// -------------------------------------------------------------------------------------------------
static void Tweaks() {
	ss_low();

	spi_write(REG_RIPPLE_CONTROL, 0x00); // Disable Ripple Control

	spi_write(0x7f, 0x0D);
	spi_write(0x58, 0x00); // is lower on corded then on standard, so set to 0
	spi_write(0x7f, 0x00);

	ss_high();
}

static void paw3399_init()
{
	NRESET_GPIO_CLK_ENABLE();
	__NOP();__NOP();__NOP();__NOP(); // probably unnecessary
	MODIFY_REG(NRESET_PORT->MODER,
			0b11 << (2*NRESET_PIN_Pos),
			0b01 << (2*NRESET_PIN_Pos));
	delay_ms(10);
	NRESET_PORT->ODR |= NRESET_PIN; // drive NRESET high
	delay_ms(10);
	ss_low();
	delay_ms(1);
	NRESET_PORT->ODR &= ~NRESET_PIN; // drive NRESET low
	delay_ms(1);
	NRESET_PORT->ODR |= NRESET_PIN; // drive NRESET high
	delay_ms(1);
	spi_write(0x3A, 0x5A); // 6.1.4
	ss_high();
	delay_ms(10); // 6.1.5

	PowerUpInit();
	DisableLiftOff();
	EnableCordedGaming();
	AdditionalSetup();
	CustomConfig();
	Tweaks();
}
