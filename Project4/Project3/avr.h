/*
 * avr.h
 *
 * Created: 2/9/2022 5:10:40 PM
 *  Author: kevin
 */ 


#ifndef AVR_H_
#define AVR_H_
/*
#ifndef __avr__
#define __avr__
*/

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/io.h>

#define XTAL_FRQ 8000000lu

#define SET_BIT(p,i) ((p) |=  (1 << (i)))
#define CLR_BIT(p,i) ((p) &= ~(1 << (i)))
#define GET_BIT(p,i) ((p) &   (1 << (i)))

#define WDR() asm volatile("wdr"::)
#define NOP() asm volatile("nop"::)
#define RST() for(;;);

void ini_avr(void);
void wait_avr(unsigned short msec);


#endif /* AVR_H_ */