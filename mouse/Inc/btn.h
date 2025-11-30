#pragma once

#include <m3k_resource.h>
#include "stm32f7xx.h"

static void btn_init(void)
{
	LMB_NO_CLK_ENABLE();
	LMB_NC_CLK_ENABLE();
	RMB_NO_CLK_ENABLE();
	RMB_NC_CLK_ENABLE();

	MODIFY_REG(LMB_NO_PORT->PUPDR,
			0b11 << (2*LMB_NO_PIN_Pos),
			0b01 << (2*LMB_NO_PIN_Pos));
	MODIFY_REG(LMB_NC_PORT->PUPDR,
			0b11 << (2*LMB_NC_PIN_Pos),
			0b01 << (2*LMB_NC_PIN_Pos));
	MODIFY_REG(RMB_NO_PORT->PUPDR,
			0b11 << (2*RMB_NO_PIN_Pos),
			0b01 << (2*RMB_NO_PIN_Pos));
	MODIFY_REG(RMB_NC_PORT->PUPDR,
			0b11 << (2*RMB_NC_PIN_Pos),
			0b01 << (2*RMB_NC_PIN_Pos));
}
