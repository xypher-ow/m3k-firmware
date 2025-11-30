#include <assert.h>
#include <btn.h>
#include <m3k_resource.h>
#include <paw3399.h>
#include <stdint.h>
#include "stm32f7xx.h"
#include "usbd_hid.h"
#include "usb.h"
#include "clock.h"
#include "delay.h"

typedef union {
	struct __PACKED { // use the order in the report descriptor
		uint8_t btn;
		int8_t whl; // not used
		int16_t x, y;
		uint16_t _pad; // zero pad to 8 bytes total
	};
	uint8_t u8[8]; // btn, 0, xlo, xhi, ylo, yhi, 0, 0
	uint32_t u32[2];
} Usb_packet;

int main(void) {
	extern uint32_t _sflash;
	SCB->VTOR = (uint32_t) (&_sflash);

	SCB_EnableICache();
	SCB_EnableDCache();

	clk_init();
	delay_init();
	btn_init();
	usb_init(1);
	usb_wait_configured();
	spi_init();
	paw3399_init();

	const uint32_t USBx_BASE = (uint32_t) USB_OTG_HS; // used in macros USBx_*
	// fifo space when empty, should equal 0x174, from init_usb
	const uint32_t fifo_space = (USBx_INEP(1)->DTXFSTS
	& USB_OTG_DTXFSTS_INEPTFSAV);

	USB_OTG_HS->GINTMSK |= USB_OTG_GINTMSK_SOFM; // enable SOF interrupt

	Usb_packet new = { 0 }; // what's new this loop
	Usb_packet send = { 0 }; // what's transmitted
	uint8_t btn_prev = 0;

	while (1) {
		// wait for SOF to sync to usb frames
		USB_OTG_HS->GINTSTS |= USB_OTG_GINTSTS_SOF;
		__WFI();

		delay_us(107); // delay to write most recent data at 8000hz


		//---------------------------------------------------------------------------------
		// Read Sensor and Buttons
		//---------------------------------------------------------------------------------

		SPIx_SS_PORT->BSRR = (uint32_t)SPIx_SS_PIN << 16; // SS Low

		// Request Motion Burst
		while (!(SPIx->SR & SPI_SR_TXE));
		*(__IO uint8_t *)&SPIx->DR = REG_MOTION_BURST;
		while (!(SPIx->SR & SPI_SR_RXNE));
		(void)*(__IO uint8_t *)&SPIx->DR;

		// Normally need to wait 2µs for t_SRAD
		// Instead of waiting, we are reading Buttons
		// After that we read Motion and Observation which act as a buffer
		// they are irrelevant, so their values can be corrupt

		// Read GPIO Ports
	    uint32_t port_c = GPIOC->IDR;
	    uint32_t port_e = GPIOE->IDR;

	    // Read Buttons
		const uint8_t btn_NO = (((port_e >> 9) & 0b01) | ((port_e >> 2) & 0b10));
		const uint8_t btn_NC = (((port_c >> 2) & 0b01) | ((port_e >> 5) & 0b10));
		btn_prev = new.btn;
		new.btn = (~btn_NO & 0b011) | (btn_NC & btn_prev); // ignores scroll wheel

		// Read Sensor
		// Motion (not used)
		while (!(SPIx->SR & SPI_SR_TXE));
		*(__IO uint8_t *)&SPIx->DR = 0;
		while (!(SPIx->SR & SPI_SR_RXNE));
		(void)*(__IO uint8_t *)&SPIx->DR;

		// Observation (not used)
		while (!(SPIx->SR & SPI_SR_TXE));
		*(__IO uint8_t *)&SPIx->DR = 0;
		while (!(SPIx->SR & SPI_SR_RXNE));
		(void)*(__IO uint8_t *)&SPIx->DR;

		// x lower bits
		while (!(SPIx->SR & SPI_SR_TXE));
		*(__IO uint8_t *)&SPIx->DR = 0;
		while (!(SPIx->SR & SPI_SR_RXNE));
		new.u8[2] = *(__IO uint8_t *)&SPIx->DR;

		// x upper bits
		while (!(SPIx->SR & SPI_SR_TXE));
		*(__IO uint8_t *)&SPIx->DR = 0;
		while (!(SPIx->SR & SPI_SR_RXNE));
		new.u8[3] = *(__IO uint8_t *)&SPIx->DR;

		// y lower bits
		while (!(SPIx->SR & SPI_SR_TXE));
		*(__IO uint8_t *)&SPIx->DR = 0;
		while (!(SPIx->SR & SPI_SR_RXNE));
		new.u8[4] = *(__IO uint8_t *)&SPIx->DR;

		// y upper bits
		while (!(SPIx->SR & SPI_SR_TXE));
		*(__IO uint8_t *)&SPIx->DR = 0;
		while (!(SPIx->SR & SPI_SR_RXNE));
		new.u8[5] = *(__IO uint8_t *)&SPIx->DR;

		SPIx_SS_PORT->BSRR = SPIx_SS_PIN; // SS High

		// if last packet still sitting in fifo
		if ((USBx_INEP(1)->DTXFSTS & USB_OTG_DTXFSTS_INEPTFSAV) < fifo_space) {
			// flush fifo
			USB_OTG_HS->GRSTCTL = _VAL2FLD(USB_OTG_GRSTCTL_TXFNUM,
					1) | USB_OTG_GRSTCTL_TXFFLSH;
			while ((USB_OTG_HS->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH) != 0);
			send.x += new.x;
			send.y += new.y;
			send.btn = new.btn;

			// A simple busy loop to tell when fifo isnt empty
			// only useful for testing
//			volatile int count = 0;
//			while(count < 640000) {
//				count++;
//				__NOP(); // Prevent compiler from deleting this loop
//			}
		}
		else {
			send.x = new.x;
			send.y = new.y;
			send.btn = new.btn;
		}

		// disable transfer complete interrupts, in case enabled by OTG_HS_IRQHandler
		USBx_DEVICE->DIEPMSK &= ~USB_OTG_DIEPMSK_XFRCM;
		// set up transfer size
		MODIFY_REG(USBx_INEP(1)->DIEPTSIZ,
				USB_OTG_DIEPTSIZ_PKTCNT | USB_OTG_DIEPTSIZ_XFRSIZ,
				_VAL2FLD(USB_OTG_DIEPTSIZ_PKTCNT, 1) | _VAL2FLD(USB_OTG_DIEPTSIZ_XFRSIZ, HID_EPIN_SIZE));
		// enable endpoint
		USBx_INEP(1)->DIEPCTL |= USB_OTG_DIEPCTL_CNAK
				| USB_OTG_DIEPCTL_EPENA;
		// write to fifo
		USBx_DFIFO(1) = send.u32[0];
		USBx_DFIFO(1) = send.u32[1];
	}
	return 0;
}
