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

static volatile int dmx_error = 1;
static volatile int byte_count = 0;
static volatile int start_flag = 0;
static enum packet_type packet_type;

void TIM1_CC_IRQHandler(void)
{
	uint32_t break_time, mab_time;

	TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);

	break_time = TIM_GetCapture2(TIM1);
	mab_time = TIM_GetCapture1(TIM1) - break_time;

	/* use some tolerance for times */
	if (break_time > 80 && break_time < 10e3 && mab_time > 8)
		start_flag = 1;
	else
		start_flag = 0;
}

void USART2_IRQHandler(void)
{
	uint8_t rx_byte;
	int16_t fe_flag;
	int start_addr = dmx512_get_start_addr();

	USART_ClearITPendingBit(USART2, USART_IT_RXNE);

	fe_flag = USART_GetFlagStatus(USART2, USART_FLAG_FE);
	rx_byte = USART_ReceiveData(USART2); /* also clears FE flag */

	if (start_flag) {
		dmx512_new_data(packet_type, byte_count - 1);
		byte_count = 0;
	}

	if (fe_flag && !start_flag)
		return;

	/* first byte determines packet type */
	if (byte_count == 0) {
		dmx_error = 0;
		switch (rx_byte) {
		case 0x0:
			packet_type = DATA_PACKET;
			break;
		case 0x17:
			packet_type = TEST_PACKET;
			break;
		default:
			dmx512_clear_input();
			dmx_error = 1;
			return;
		}
	}

	if (byte_count >= start_addr && byte_count < start_addr + NUMBER_OUTPUTS)
		dmx512_set_input(byte_count - start_addr, rx_byte);

	byte_count++;
}


/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1) {}
}



/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */
void SysTick_Handler(void)
{
}



