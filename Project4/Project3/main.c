/*
 * main.c
 *
 * Created: 2/17/2022 10:15:53 AM
 *  Author: kevin
 */ 

#include <xc.h>
#include "avr.h"
#include "lcd.h"
#include <stdio.h>



int get_key();
int is_pressed(int r, int c);
int is_number(int k);
int convert(int k);
int get_sample();

int main(void)
{	
	avr_init();
	lcd_init();
	char buf[17];
	int s,min,max,count=0;
	unsigned long sum=0;
	int pressed,stop=0;
    while(1) {
		if(!stop) {
			s = get_sample();
			if(s < min) {
				min = s;
			}
			if(s > max) {
				max = s;
			}
			//change avg
			count++;
			sum += s;
			//print stuff
			lcd_pos(0,0);
			sprintf(buf, "%1.2f", (s/1023.0) * 5);
			lcd_puts2(buf);
			//min
			lcd_pos(0,8);
			sprintf(buf, "%1.2f", (min/1023.0) * 5);
			lcd_puts2(buf);
			//max
			lcd_pos(0,0);
			sprintf(buf, "%1.2f", (max/1023.0) * 5);
			lcd_puts2(buf);
			//avg sum/(count*1.0)
			lcd_pos(0,0);
			sprintf(buf, "%1.2f", ((sum/count)/1023.0) * 5);
			lcd_puts2(buf);
			//wait for next calc
			int check = 0;
			while(check < 3) {
				pressed = get_key();
				if(pressed == 2) {
					stop = 1;
				}
				else if(pressed == 3) {
					min = 1000;
					max = 0;
					sum = 0;
					count = 0;
					s = 0;
				}
				else if(pressed == 1) {
					stop = 0;
				}
				check++;
			}
			avr_wait(20);
		}
		else {
			pressed = get_key();
			if(pressed == 1) {
				stop = 0;
			}
			if(pressed == 3) {
				min = 1000;
				max = 0;
				sum = 0;
				count = 0;
				s = 0;
			}
		}
		
	}
}

int get_key() {
	//goes in order of rows since is_pressed flips from 0 -> 4 for row numbering
	int i,j;
	for(i = 3; i >= 0; i--) {
		for(j = 0; j < 4; ++j) {
			if(is_pressed(i,j)) {
				return i*4+j+1;
			}
		}
	}
	return 0;
}

int is_pressed(int r, int c) {
	DDRC=0;
	PORTC=0; //N/c
	SET_BIT(DDRC,r);
	CLR_BIT(PORTC,r);//"0"
	SET_BIT(PORTC,c+4);//"W1"
	//wait
	avr_wait(10);
	if(!GET_BIT(PINC, c+4)) {
		return 1;
	}
	return 0;
}

int is_number(int k) {
	return(k % 4 && k != 13 && k != 15);
}

int convert(int k) {
	//convert k into actual number rather than keypad associated row column
	switch(k) {
		case 14:
		return 0;
		case 9:
		return 7;
		case 10:
		return 8;
		case 11:
		return 9;
		case 5:
		return 4;
		case 6:
		return 5;
		case 7:
		return 6;
		case 1:
		return 1;
		case 2:
		return 2;
		case 3:
		return 3;
	}
	return 10;
}

int get_sample() {
	
	//ADMUX = 010XXXXX;//
	//ADCSRA = 11XXXXXX;
	ADMUX = 0b01000000;
	ADCSRA = 0b11000000;
	SET_BIT(ADCSRA, 7);
	SET_BIT(ADCSRA,6);
	while(GET_BIT(ADCSRA, 6));
	CLR_BIT(ADCSRA,7);
	return ADC;
}