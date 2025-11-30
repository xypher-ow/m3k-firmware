#include "stm32f7xx.h"
#include "delay.h"

void delay_init(void)
{
	// see pg 132 of ref manual: TIM2CLK = PCLK1 = HCLK
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	TIM2->CR1 = TIM_CR1_CEN;
}
