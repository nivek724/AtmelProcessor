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



#define A1 0
#define A1s 1
#define B1 2
#define C1 3
#define C1s 4
#define D1 5
#define D1s 6
#define E1 7
#define F1 8
#define F1s 9
#define G1 10
#define G1s 11
#define A2 12
#define A2s 13
#define B2 14
#define C2 15
#define C2s 16
#define D2 17
#define D2s 18
#define E2 19
#define F2 20
#define F2s 21 
#define G2 22
#define G2s 23

#define W 4 // 1/2^(x-1) = time played
#define H 3
#define Q 2
#define E 1


//note info
struct note {
	int freq;
	int duration;
};

struct note SONG[] = { //The lick
	{G1,E},
	{A2,E},
	{B2,E},
	{C2,E},
	{A2,Q},
	{F1,E},	
	{G1,H},		
};

struct note HOTBUNS[] = {
	{G1,Q},
	{E1,Q},
	{C1,H},
	{G1,Q},
	{E1,Q},
	{C1,H},
	{C1,E},
	{C1,E},
	{C1,E},
	{C1,E},
	{E1,E},
	{E1,E},
	{E1,E},
	{E1,E},
	{G1,Q},
	{E1,Q},
	{C1,H},
};

struct note MARY[] = {
	{D2s,Q},
	{C2s,Q},
	{B2,Q},
	{C2s,Q},
	{D2s,Q},
	{D2s,Q},
	{D2s,H},
	{C2s,Q},
	{C2s,Q},
	{C2s,H},
	{D2s,Q},
	{F2s,Q},
	{F2s,H},
	{D2s,Q},
	{C2s,Q},
	{B2,Q},
	{C2s,Q},
	{D2s,Q},
	{D2s,Q},
	{D2s,Q},
	{D2s,Q},
	{C2s,Q},
	{C2s,Q},
	{D2s,Q},
	{C2s,Q},
	{B2,W},
};

//periods rounded and divided by .00004 
//multiply by .00004 later to return to period 
static int period[] = {
	114,
	108,
	102,
	96,
	90,
	86,
	80,
	76,
	72,
	68,
	64,
	60,
	56,
	54,
	50,
	48,
	46,
	42,
	40,
	38,
	36,
	34,
	32,
	30,
};

static int th[] = {
	57,
	54,
	51,
	48,
	45,
	43,
	40,
	38,
	36,
	34,
	32,
	30,
	28,
	27,
	25,
	24,
	23,
	21,
	20,
	19,
	18,
	17,
	16,
	15,
};


int get_key();
int is_pressed(int r, int c);
int is_number(int k);
int convert(int k);
void playNote(const struct note n, int tempo, int pitch);
int playSong(const struct note *song, int N, int tempo, int pitch); // if interrupted it returns a value
//to allow for continue playing expand return value from playSong and its params to include third for starting point and return point left on

int main(void)
{
	SET_BIT(DDRB, 3);
	avr_init();
	lcd_init();
	int song = 2;
	int prev = 0;
	int ret = 0;
	int tempo = 3;
	int pitch = 0; // default 0 = normal 1 = up whole step 2 = down whole step
	int pressed;
	char buf[17];
    while(1) {
		if(!song) {
			//play default song
			lcd_clr();
			lcd_pos(0,0);
			sprintf(buf,"C major Scale");
			lcd_puts2(buf);
			int ret = playSong(SONG,7, tempo, pitch); // reassign song to new value
			if(ret){
				// if song doesn't finish normally change song to be new code
				prev = song;
				song = ret;
			}
		}
		else if(song == 4) { // copy this 1
			lcd_clr();
			lcd_pos(0,0);
			sprintf(buf,"Hot cross Buns");
			lcd_puts2(buf);
			int ret = playSong(HOTBUNS,17, tempo, pitch);
			if(ret){
				// if song doesn't finish normally change song to be new code
				prev = song;
				song = ret;
			}
		}
		else if(song == 8) {
			lcd_clr();
			lcd_pos(0,0);
			sprintf(buf,"Mary little lamb");
			lcd_puts2(buf);
			int ret = playSong(MARY, 26, tempo, pitch);
			if(ret){
				// if song doesn't finish normally change song to be new code
				prev = song;
				song = ret;
			}
		}
		
		//check for button press
		if(ret) {
			pressed = ret;
		}
		else {
			pressed = get_key();
		}
		if(pressed) {
			if(pressed == 1){
				song = prev;
				lcd_clr();
				lcd_pos(0,0);
				sprintf(buf,"Starting song %d", song);
				lcd_puts2(buf);
				avr_wait(200);
			}
			else if(pressed == 3) {
				song = 0;
				lcd_clr();
				lcd_pos(0,0);
				sprintf(buf,"Starting song %d", song);
				lcd_puts2(buf);
				avr_wait(200);
			}
			else if(pressed == 4) { //copy this 2
				song = 4;
				lcd_clr();
				lcd_pos(0,0);
				sprintf(buf,"Starting song %d", song);
				lcd_puts2(buf);
				avr_wait(200);
			}
			else if(pressed == 8) {
				song = 8;
				lcd_clr();
				lcd_pos(0,0);
				sprintf(buf,"Starting song %d", song);
				lcd_puts2(buf);
				avr_wait(200);
			}
			
			//tempo controls
			else if(pressed == 5) {
				lcd_clr();
				lcd_pos(0,0);
				sprintf(buf,"Slowing down");
				lcd_puts2(buf);
				avr_wait(200);
				if(tempo-1)
					tempo--;
				song = prev;
			}
			else if(pressed == 6) {
				lcd_clr();
				lcd_pos(0,0);
				sprintf(buf,"Default tempo");
				lcd_puts2(buf);
				avr_wait(200);
				tempo = 3;
				song = prev;
			}
			else if(pressed == 7) {
				lcd_clr();
				lcd_pos(0,0);
				sprintf(buf,"speeding up");
				lcd_puts2(buf);
				avr_wait(200);
				if(tempo < 30)
					tempo++;
				song = prev;
			}
			
			//pitch controls
			else if(pressed == 9 && pitch != 2) {
				lcd_clr();
				lcd_pos(0,0);
				sprintf(buf,"Lowered pitch");
				lcd_puts2(buf);
				avr_wait(200);
				pitch = 2;
				song = prev;
			}
			else if(pressed == 10 && pitch != 0) {
				lcd_clr();
				lcd_pos(0,0);
				sprintf(buf,"Default pitch");
				lcd_puts2(buf);
				avr_wait(200);
				pitch = 0;
				song = prev;
			}
			else if(pressed == 11 && pitch != 1) {
				lcd_clr();
				lcd_pos(0,0);
				sprintf(buf,"Raised pitch");
				lcd_puts2(buf);
				avr_wait(200);
				pitch = 1;
				song = prev;
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

void playNote(const struct note n, int tempo, int pitch) {
	int fr;
	if(pitch == 2) {	
		fr = n.freq-2;
		if(fr < 0)
			fr = 0;
	}
	else  if(pitch == 1){
		fr = (n.freq+2) % 24;
	}
	else{
		fr = n.freq;
	}
	int i, k = (n.duration/(period[fr]*.00004))/tempo;
	for(i = 0; i < k; ++i) {
		SET_BIT(PORTB,3);
		avr_wait(th[fr]);
		CLR_BIT(PORTB,3);
		avr_wait(th[fr]);
	}
}

int playSong(const struct note *song, int N, int tempo, int pitch) {
	int i;
	for(i = 0; i < N; ++i) {
		playNote(song[i], tempo, pitch);
		//check for input
		int pressed = get_key();
		if(pressed) {
			if(pressed == 2) {
				// stop song
				lcd_clr();
				lcd_pos(0,0);
				char buf[20];
				sprintf(buf, "Stopping song");
				lcd_puts2(buf);
				avr_wait(200);
				return 2; 
			}
		}
	}
	return 0;
}

void switch_song(int n) {
	
}