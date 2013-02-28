/*
 * dmx512_rec.h
 *
 *  Created on: 27.1.2013
 *      Author: honza
 */

#ifndef DMX512_REC_H_
#define DMX512_REC_H_

#define NUMBER_OUTPUTS 16

enum packet_type {
	DATA_PACKET = 0,
	TEST_PACKET,
};

struct dmx512_data {
	enum packet_type type;
	int processed_flag;
	int packet_len;
	uint8_t data[NUMBER_OUTPUTS];
};

void dmx512_set_input(int i, uint8_t value);
void dmx512_clear_input(void);
int dmx512_get_start_addr(void);
void dmx512_rec_init(TIM_TypeDef *timx, USART_TypeDef *usartx);
void dmx512_rec_enable(int on);
struct dmx512_data *dmx512_get_data(void);
void dmx512_new_data(enum packet_type type, int packet_len);
void dmx512_set_startaddr(int addr);

#endif /* DMX512_REC_H_ */
