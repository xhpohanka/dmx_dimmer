/*
 * dmx512_rec.c
 *
 *  Created on: 27.1.2013
 *      Author: honza
 */

#include <string.h>
#include <stdint.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_usart.h>
#include "dmx512_rec.h"

struct dmx512_config {
	TIM_TypeDef *tim;
	USART_TypeDef *usart;
	int start_addr;
};

static uint8_t input_data[NUMBER_OUTPUTS];
static struct dmx512_config dmx512_config;
static struct dmx512_data data;


struct dmx512_data *dmx512_get_data(void)
{
	return &data;
}

void dmx512_set_input(int i, uint8_t value)
{
	input_data[i] = value;
}

void dmx512_new_data(enum packet_type type, int packet_len)
{
	data.processed_flag = 0;
	data.type = type;
	data.packet_len = packet_len;
	memcpy(data.data, input_data, NUMBER_OUTPUTS);
}

void dmx512_clear_input(void)
{
	memset(input_data, 0, NUMBER_OUTPUTS);
}

int dmx512_get_start_addr(void)
{
	return dmx512_config.start_addr;
}

/*
 * configures timer for detecting BREAK and MAB
 * UART for dmx512 data
 */
void dmx512_rec_init(TIM_TypeDef *timx, USART_TypeDef *usartx)
{
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	USART_InitTypeDef USART_InitStructure;

	dmx512_config.tim = timx;
	dmx512_config.usart = usartx;
	dmx512_config.start_addr = 0;

	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
	TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock/1e6 - 1; /* timer tics are in us */
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(timx, &TIM_TimeBaseStructure);

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0x0;
	TIM_PWMIConfig(timx, &TIM_ICInitStructure);

	TIM_SelectInputTrigger(timx, TIM_TS_TI1FP1);
	TIM_SelectSlaveMode(timx, TIM_SlaveMode_Reset);
	TIM_SelectMasterSlaveMode(timx, TIM_MasterSlaveMode_Enable);

	TIM_Cmd(timx, ENABLE);

	USART_InitStructure.USART_BaudRate = 250000;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_2;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx;

	dmx512_clear_input();
	memset(&data, 0, sizeof(struct dmx512_data));

	USART_Init(usartx, &USART_InitStructure);
	USART_Cmd(usartx, ENABLE);
}

void dmx512_set_startaddr(int addr)
{
	dmx512_config.start_addr = addr;
}

void dmx512_rec_enable(int on)
{
	if (on) {
		USART_ITConfig(dmx512_config.usart, USART_IT_RXNE, ENABLE);
		TIM_ITConfig(dmx512_config.tim, TIM_IT_CC1, ENABLE);
	} else {
		USART_ITConfig(dmx512_config.usart, USART_IT_RXNE, DISABLE);
		TIM_ITConfig(dmx512_config.tim, TIM_IT_CC1, DISABLE);
	}
}
