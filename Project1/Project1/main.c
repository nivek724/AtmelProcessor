/*
 * main.c
 *
 * Created: 1/24/2022 1:19:44 AM
 *  Author: kevin
 */ 

#include <xc.h>
#include "avr.h"

int main(void)
{
	DDRA = 0;
	DDRB = 1;
	PORTB = 0;
	volatile unsigned short wait_time = 500;
    while(1)
    {
        //TODO:: Please write your application code 
		if(!PINA&1) {
			PORTB = 1;
			avr_wait(wait_time);
			PORTB = 0;
			avr_wait(wait_time);
		}
    }
}