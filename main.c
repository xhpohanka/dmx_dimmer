#include <stdint.h>
#include "stm32f10x.h"
#include "dmx512_rec.h"

void delay(volatile uint32_t x)
{
	for (; x > 0; x--) ;
}

static void pwm_out_init(TIM_TypeDef *timx, int channels)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	uint16_t prescalerValue = 0;

	prescalerValue = (uint16_t) (SystemCoreClock / 24e6) - 1;
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 512 -1; //~46kHz  (24e6/36e3) - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = prescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(timx, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;

	if (channels & (1 << 0)) {
		TIM_OC1Init(timx, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(timx, TIM_OCPreload_Enable);
	}

	if (channels & (1 << 1)) {
		TIM_OC2Init(timx, &TIM_OCInitStructure);
		TIM_OC2PreloadConfig(timx, TIM_OCPreload_Enable);
	}

	if (channels & (1 << 2)) {
		TIM_OC3Init(timx, &TIM_OCInitStructure);
		TIM_OC3PreloadConfig(timx, TIM_OCPreload_Enable);
	}

	if (channels & (1 << 3)) {
		TIM_OC4Init(timx, &TIM_OCInitStructure);
		TIM_OC4PreloadConfig(timx, TIM_OCPreload_Enable);
	}

	TIM_ARRPreloadConfig(timx, ENABLE);

	TIM_Cmd(timx, ENABLE);
}

static void pwm_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

#if !defined DISCOVERY_KIT
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
	        RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 |
	        RCC_APB1Periph_TIM4 | RCC_APB1Periph_TIM5, ENABLE);
#ifndef DEBUG
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
#endif

	/* PA: TIM5 CH1-4 (0-3) no remap,
	 *     TIM3 CH1-2 (6,7) no remap,
	 *     TIM2 CH1 (15j)	full remap */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
	        GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* PB: TIM3 CH3-4 (0,1) no remap,
	 *     TIM2 CH2-4 (3j,10,11) full remap */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 |
	        GPIO_Pin_3 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* PD: TIM4 CH1-4 (12-15) remap */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, DISABLE);
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_TIM4, ENABLE);

	pwm_out_init(TIM2, 0x0f);
	pwm_out_init(TIM3, 0x0f);
	pwm_out_init(TIM4, 0x0f);
	pwm_out_init(TIM5, 0x0f);

#else
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 |
	        RCC_APB1Periph_TIM4 | RCC_APB1Periph_TIM5, ENABLE);

	/* PC: TIM3 CH3-4 (8,8) full remap */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);
	pwm_out_init(TIM3, (1 << 2) | (1 << 3));
#endif

}

void pwm_set_output(uint8_t *data)
{
#if !defined DISCOVERY_KIT
	TIM2->CCR1 = data[0];
	TIM2->CCR2 = data[1];
	TIM2->CCR3 = data[2];
	TIM2->CCR4 = data[3];
	TIM3->CCR1 = data[4];
	TIM3->CCR2 = data[5];
	TIM3->CCR3 = data[6];
	TIM3->CCR4 = data[7];
	TIM4->CCR1 = data[8];
	TIM4->CCR2 = data[9];
	TIM4->CCR3 = data[10];
	TIM4->CCR4 = data[11];
	TIM5->CCR1 = data[12];
	TIM5->CCR2 = data[13];
	TIM5->CCR3 = data[14];
	TIM5->CCR4 = data[15];
#else
	TIM3->CCR3 = data[0];
	TIM3->CCR4 = data[1];
#endif
}

static void dmx512_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

#if !defined DISCOVERY_KIT
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM1, ENABLE);
#else
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinRemapConfig(GPIO_Remap_USART2, DISABLE);
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM1, DISABLE);
#endif

	dmx512_rec_init(TIM1, USART2);
}

static void NVIC_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure the NVIC Preemption Priority Bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

int main(void)
{
	struct dmx512_data *dmx512_data;

	NVIC_init();
	pwm_init();
	dmx512_init();
	dmx512_set_startaddr(49);
	dmx512_rec_enable(1);

	while (1)
	{
		dmx512_data = dmx512_get_data();
		if (!dmx512_data->processed_flag) {
			pwm_set_output(dmx512_data->data);
			dmx512_data->processed_flag = 1;
		}
	}

}
