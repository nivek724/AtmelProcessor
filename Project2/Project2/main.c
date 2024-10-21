/*
 * main.c
 *
 * Created: 2/9/2022 5:08:24 PM
 *  Author: kevin
 */ 

#include <xc.h>
#include "avr.h"
#include "lcd.h"
#include <stdio.h>

struct dt {
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
	int type;// 0 default time 1 military
	int aorp;// 0 am 1 pm
};


int get_key();
int is_pressed(int r, int c);
int is_number(int k);
int convert(int k);
void init_dt(struct dt *mydt);
void advance_dt(struct dt *mydt);
int month_days(const struct dt *dt);
void recalc_time(struct dt *dt);
void print_dt(const struct dt *dt);
void print_time(const struct dt *dt);
int calc_mili_time(const struct dt *dt);
void switch_time(struct dt *dt);
void switch_ampm(struct dt *dt);
void test_newyears(struct dt *dt);
void test_leapyear(struct dt *dt);
void set_ampm(struct dt *dt);
void set_time(struct dt *dt); 
void set_date(struct dt *dt);


int main(void)
{
	
	avr_init();
	lcd_init();
	struct dt myDT;
	init_dt(&myDT);
	print_dt(&myDT);
	int test_ny = 1, test_leap = 0;
	//testing function change to 1 to test/*
	if(test_ny) {
		test_newyears(&myDT);
	}
	if(test_leap) {
		test_leapyear(&myDT);
	}
    while(1)
    {
        //TODO:: Please write your application code 
		/*
		Functionality:
			Displays default time of 1/1/2022 1:00:00 AM
			A will start setting functionality to change the time press keys of desired day to complete the day press A
			B will toggle between military and normal time
			C will switch am pm 
			D will reset to default time and date
			
		Setting functionality:
			Pressing A will insert whatever value is currently pressed into the dt structure going month day year hour minute second AM/PM each needing an A press and ampm needing 1 or 2 selected
			Pressing A with no other buttons having been pressed will keep previous value 
			Otherwise press numbers on keypad slowly till all numbers have been inputted.
		*/
		lcd_clr();
		print_dt(&myDT);
		print_time(&myDT);
		int press = get_key();
		if(press) { // key pressed
			if(press == 4) { // set time/date operation
				set_date(&myDT);
				set_time(&myDT);
				set_ampm(&myDT);
			}
			if(press == 8) { // switch between military and am/pm
				switch_time(&myDT);
			}
			if(press == 12) {
				switch_ampm(&myDT);
			}
			if(press == 16) {
				init_dt(&myDT);
			}
		}
		avr_wait(840);
		advance_dt(&myDT);
    }
	
}

/*
key press is bottom to top left to right
every four keys is the letter so key 4,8,12,16 are a b c d respectively
1 2 3 a
4 5 6 b
7 8 9 c
* 0 # d
*/

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

void init_dt(struct dt *mydt) {
	mydt -> year = 2022;
	mydt -> month = 1;
	mydt -> day = 1;
	mydt -> hour = 1;
	mydt -> minute = 0;
	mydt -> second = 0;
	mydt -> type = 0;
	mydt -> aorp = 0; // am
}

void advance_dt(struct dt *mydt) { 
	//doesn't calc military do math for it somewhere else
	++mydt->second;
	if(mydt->second == 60) {
		++mydt->minute;
		mydt->second = 0;
		if(mydt->minute == 60) {
			++mydt->hour;
			mydt->minute = 0;
			if(mydt->hour == 12) {
				if(mydt->aorp) {
					//change day here
					++mydt->day;
					if(mydt->day == (month_days(mydt) + 1)) {
						++mydt->month;
						mydt->day = 1;
						if(mydt->month == 13){
							mydt->month = 1;
							++mydt->year;
						}
					}
				}
				mydt->aorp = 1 - mydt->aorp; // at 12 flip am to pm and visa versa 
			}
			if(mydt->hour == 13) {
				mydt->hour = 1;
			}
		}
	}
}

void set_date(struct dt *dt) {
	//change date structure
	char buf[20];
	int press, con, value=-1;
	avr_wait(500); // wait
	while((press = get_key()) != 4){ //wait for A press
		if(press) { // some key is pressed check is press is number and then convert
			if(is_number(press)) {
				con = convert(press);
				if(value == -1) {
					value = con;
				}
				else {
					value = value * 10 + con;
				}
				sprintf(buf,"Month: %02d", value);
				lcd_clr();
				lcd_pos(0,0);
				lcd_puts2(buf);
			}
			avr_wait(1000); // avoid double pressing same button for entering value can also add screen flash here
		}
	}
	// verify dt for incorrect value insertion
	// different method for military time insertion
	dt->month = value % 12;
	//day
	value = -1;
	avr_wait(500);
	while((press = get_key()) != 4){
		if(press) { // some key is pressed
			if(is_number(press)) {
				con = convert(press);
				if(value == -1) {
					value = con;
				}
				else {
					value = value * 10 + con;
				}
				sprintf(buf,"Day: %02d", value);
				lcd_clr();
				lcd_pos(0,0);
				lcd_puts2(buf);
			}
			avr_wait(1000); // avoid double pressing same button for entering value can also add screen flash here
		}
	}
	dt->day = value; // day can never be wrong and will be recalc'd based on excess time
	//year
	value = -1;
	avr_wait(500);
	while((press = get_key()) != 4){
		if(press) { // some key is pressed
			if(is_number(press)) {
				con = convert(press);
				if(value == -1) {
					value = con;
				}
				else {
					value = value * 10 + con;
				}
				sprintf(buf,"Year: %04d", value);
				lcd_clr();
				lcd_pos(0,0);
				lcd_puts2(buf);
			}
			avr_wait(1000); // avoid double pressing same button for entering value can also add screen flash here
		}
	}
	dt->year = value;
	//recheck days based on year
	dt->day = dt->day % month_days(dt);
}

void set_time(struct dt *dt) {
	//change time structure
	char buf[20];
	int press, con, value=-1;
	avr_wait(500); // wait
	while((press = get_key()) != 4){ //wait for A press
		if(press) { // some key is pressed check is press is number and then convert
			if(is_number(press)) {
				con = convert(press);
				if(value == -1) {
					value = con;
				}
				else {
					value = value * 10 + con;
				}
				sprintf(buf,"Hour: %d", value);
				lcd_clr();
				lcd_pos(0,0);
				lcd_puts2(buf);
			}
			avr_wait(1000); // avoid double pressing same button for entering value can also add screen flash here
		}
	}
	// verify dt for incorrect value insertion
	// different method for military time insertion
	if(value != -1)
		dt->hour = value;
	value = -1;
	avr_wait(500);
	while((press = get_key()) != 4){
		if(press) { // some key is pressed
			if(is_number(press)) {
				con = convert(press);
				if(value == -1) {
					value = con;
				}
				else {
					value = value * 10 + con;
				}
				sprintf(buf,"Minute: %d", value);
				lcd_clr();
				lcd_pos(0,0);
				lcd_puts2(buf);
			}
			avr_wait(1000); // avoid double pressing same button for entering value can also add screen flash here
		}
	}
	if(value != -1)
		dt->minute = value;
	avr_wait(500);
	value = -1;
	while((press = get_key()) != 4){
		if(press) { // some key is pressed
			if(is_number(press)) {
				con = convert(press);
				if(value == -1) {
					value = con;
				}
				else {
					value = value * 10 + con;
				}
				sprintf(buf,"Second: %d", value);
				lcd_clr();
				lcd_pos(0,0);
				lcd_puts2(buf);
			}
			avr_wait(1000); // avoid double pressing same button for entering value can also add screen flash here
		}
	}
	if(value != -1)
		dt->second = value;
	// normal time should be edited
	// calc for invalid time roll over
	recalc_time(dt);
}

void recalc_time(struct dt *dt) { // sets time to roll over extra alloted time during setting but doesn't change date
	if(dt->second == 60) {
		++dt->minute;
		dt->second = 0;
	}
	if(dt->minute == 60) {
		++dt->hour;
		dt->minute = 0;
	}
	if(dt->hour == 13) {
		dt->hour = 1;
	}
}

void set_ampm(struct dt *dt) {
	int press;
	char buf[20];
	avr_wait(500); // wait
	sprintf(buf,"AM = 1 PM = 2");
	lcd_clr();
	lcd_pos(0,0);
	lcd_puts2(buf);
	while((press = get_key()) > 2); //wait for 1 or 2 press
	dt->aorp = press-1;
}

int month_days(const struct dt *dt) {
	switch(dt->month) {
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			return 31;
		case 2:
			if(!dt->year % 4)
				return 29;
			return 28;
		case 4:
		case 6:
		case 9:
		case 11:
			return 30;
		default: 
			return 0;
	}
}

void print_dt(const struct dt *dt) {
	char buf[17];
	lcd_pos(0,0);
	sprintf(buf,"%02d/%02d/%04d ",dt->month,dt->day,dt->year);
	lcd_puts2(buf);
}

void print_time(const struct dt *dt) {
	char buf[17];
	lcd_pos(1,0);
	if(!dt->type) {
		if(!dt->aorp)
			sprintf(buf, "%02d:%02d:%02d AM", dt->hour, dt->minute, dt->second);
		else
			sprintf(buf, "%02d:%02d:%02d PM", dt->hour, dt->minute, dt->second);
	}
	else{
		int military = calc_mili_time(dt);
		sprintf(buf, "%04d : %02d ", military, dt->second);
	}
	lcd_puts2(buf);
}

int calc_mili_time(const struct dt *dt) {
	int temp;
	if(dt->aorp)
		temp = 12;
	else
		temp = 0;
	return ((dt->hour + temp) * 100 + dt->minute);
}

void switch_time(struct dt *dt) {
	dt->type = 1 - dt->type;
}

void switch_ampm(struct dt *dt) {
	dt->aorp = 1 - dt->aorp;
}

void test_newyears(struct dt *dt) {
	dt->month = 12;
	dt->day = 31;
	dt->year = 2021;
	dt->hour = 11;
	dt->minute = 59;
	dt->second = 55;
	dt->type = 0;
	dt->aorp = 1;
}

void test_leapyear(struct dt *dt){
	dt->month = 2;
	dt->day = 28;
	dt->year = 2016;
	dt->hour = 11;
	dt->minute = 59;
	dt->second = 55;
	dt->type = 0;
	dt->aorp = 1;
}