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

static uint8_t input_data[NUMBER_OUTPUTS];
static int start_addr = 1;

void dmx512_set_input(int i, uint8_t value)
{
	input_data[i] = value;
}

void dmx512_set_output(void)
{
	TIM2->CCR1 = input_data[0];
	TIM2->CCR2 = input_data[1];
	TIM2->CCR3 = input_data[2];
	TIM2->CCR4 = input_data[3];
	TIM3->CCR1 = input_data[4];
	TIM3->CCR2 = input_data[5];
	TIM3->CCR3 = input_data[6];
	TIM3->CCR4 = input_data[7];
	TIM4->CCR1 = input_data[8];
	TIM4->CCR2 = input_data[9];
	TIM4->CCR3 = input_data[10];
	TIM4->CCR4 = input_data[11];
	TIM5->CCR1 = input_data[12];
	TIM5->CCR2 = input_data[13];
	TIM5->CCR3 = input_data[14];
	TIM5->CCR4 = input_data[15];
}

void dmx512_clear_input(void)
{
	memset(input_data, 0, NUMBER_OUTPUTS);
}

int dmx512_get_start_addr(void)
{
	return start_addr;
}

/*
 * configures timer for detecting BREAK and MAB
 * UART for dmx512 data
 */
void dmx512_rec_init(TIM_TypeDef *timx, USART_TypeDef *usartx)
{
	TIM_ICInitTypeDef TIM_ICInitStructure;
	USART_InitTypeDef USART_InitStructure;

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0x0;

	TIM_PWMIConfig(timx, &TIM_ICInitStructure);
	TIM_SelectInputTrigger(timx, TIM_TS_TI1FP1);
	TIM_SelectSlaveMode(timx, TIM_SlaveMode_Reset);
	TIM_SelectMasterSlaveMode(timx, TIM_MasterSlaveMode_Enable);
	TIM_Cmd(timx, ENABLE);
	TIM_ITConfig(timx, TIM_IT_CC2, ENABLE);

	USART_InitStructure.USART_BaudRate = 250000;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_2;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx;

	USART_Init(usartx, &USART_InitStructure);
	USART_ITConfig(usartx, USART_IT_RXNE, ENABLE);
	USART_Cmd(usartx, ENABLE);
}
