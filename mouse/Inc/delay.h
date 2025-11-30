#pragma once

#include "stm32f7xx.h"

void delay_init(void);

static inline void delay_us(const uint32_t us)
{
	TIM2->CNT = 0;
	while (TIM2->CNT < 32*us); // assumes TIM2CLK = 32MHz
}

#define delay_ms(x) delay_us(1000*(x))
