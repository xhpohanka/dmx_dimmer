/**
 ******************************************************************************
 * @file    GPIO/IOToggle/stm32f10x_it.c
 * @author  MCD Application Team
 * @version V3.5.0
 * @date    08-April-2011
 * @brief   Main Interrupt Service Routines.
 *          This file provides template for all exceptions handler and peripherals
 *          interrupt service routine.
 ******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
 ******************************************************************************
 */

#include <string.h>
#include "stm32f10x_it.h"
#include "dmx512_rec.h"


static int packet_count = 0;
static int start_flag = 0;


void TIM1_CC_IRQHandler(void)
{
	uint32_t break_time, mab_time;

	TIM_ClearITPendingBit(TIM1, TIM_IT_CC2);

	break_time = TIM_GetCapture1(TIM1)/SystemCoreClock;
	mab_time = TIM_GetCapture2(TIM1);

	/* times in microseconds */
	break_time = break_time / (SystemCoreClock/1000);
	mab_time = mab_time / (SystemCoreClock/1000);

	/* use some tolerance for times */
	if (break_time > 85 && break_time < 178 && mab_time > 8) {
		packet_count = 0;
		start_flag = 1;
		dmx512_set_output();
	}
}

void USART2_IRQHandler(void)
{
	uint8_t rx_byte;
	int start_addr = dmx512_get_start_addr();

	USART_ClearITPendingBit(USART2, USART_IT_RXNE);

	if (USART_GetFlagStatus(USART2, USART_FLAG_FE) && !start_flag)
		return;

	rx_byte = USART_ReceiveData(USART2);

	if (packet_count == 0 && rx_byte != 0)
		return;

	if (packet_count >= start_addr && packet_count < start_addr + NUMBER_OUTPUTS)
		dmx512_set_input(packet_count - start_addr, rx_byte);

	packet_count++;
}


/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void)
{
    volatile int i = 0;
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
        i++;
    }
}



/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */
void SysTick_Handler(void)
{
}



