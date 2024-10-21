/*
 * avr.c
 *
 * Created: 2/9/2022 5:11:56 PM
 *  Author: kevin
 */ 

#include "avr.h"

void
avr_init(void)
{
	WDTCR = 15;
}

void
avr_wait(unsigned short msec)
{
	//TCCR0 = 3;
	TCCR0 = 2;
	while (msec--) {// divide 64 for 3 .001
		TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 8) * .00004); 
		SET_BIT(TIFR, TOV0);
		WDR();
		while (!GET_BIT(TIFR, TOV0));
	}
	TCCR0 = 0;
}
