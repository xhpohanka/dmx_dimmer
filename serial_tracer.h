/*
 * serial_tracer.h
 *
 *  Created on: 12. 3. 2013
 *      Author: honza
 */

#ifndef SERIAL_TRACER_H_
#define SERIAL_TRACER_H_

void tracer_init(void);
void tracer_putchar(const char ch);
void tracer_puts(const char *str);

#endif /* SERIAL_TRACER_H_ */
