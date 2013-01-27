/*
 * dmx512_rec.h
 *
 *  Created on: 27.1.2013
 *      Author: honza
 */

#ifndef DMX512_REC_H_
#define DMX512_REC_H_

#define NUMBER_OUTPUTS 16

void dmx512_set_input(int i, uint8_t value);
void dmx512_set_output(void);
void dmx512_clear_input(void);
int dmx512_get_start_addr(void);
void dmx512_rec_init(TIM_TypeDef *timx, USART_TypeDef *usartx);

#endif /* DMX512_REC_H_ */
