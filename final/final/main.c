/*
 * main.c
 *
 * Created: 3/18/2022 6:19:12 AM
 *  Author: kevin
 */ 

#include <xc.h>
#include "avr.h"
#include "lcd.h"
#include <stdio.h>
#include <stdlib.h>



#define A1 1
/*#define A1s 1*/
#define B1 2
#define C1 3
/*#define C1s 4*/
#define D1 4
/*#define D1s 6*/
#define E1 5
#define F1 6
/*#define F1s 9*/
#define G1 7
/*#define G1s 11*/
#define A2 8
/*#define A2s 13*/
#define B2 9
#define C2 10
/*#define C2s 16*/
#define D2 11
/*#define D2s 18*/
#define E2 12
/*#define F2 12*/
/*#define F2s 21 */
/*#define G2 22*/
/*#define G2s 23*/

#define W 4 // 1/2^(x-1) = time played
#define H 3
#define Q 2
#define E 1


//note info
struct note {
	int freq;
	int duration;
};

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

struct note game[20];
struct note player[20];

int get_key();
int is_pressed(int r, int c);
int is_number(int k);
int convert(int k);
void playNote(const struct note n, int tempo, int pitch);
int playSong(const struct note *song, int N, int tempo, int pitch);
void addNote(struct note *song, int pos, int pitch);
void clear(struct note *song);

int main(void)
{
	time_t t;
	
	srand((unsigned) time(&t));//initialize rand seed based on time
	avr_init();
	lcd_init();
	char buf[17];
	int pressed;
	//game control vars win is how many correct guesses speed in tempo 
	//notes are played life is if player guesses wrong life == 0
	int win = 0, speed = 3, life = 0, round=0;
	
    while(1)
    {
		if(!win) { // game setup
			//ask difficulty easy 10 med 15 hard 20
			//ask speed
			//print NEW GAME PRESS 1 to start
			lcd_clr();
			lcd_pos(0,3);
			sprintf(buf,"New Game")
			lcd_puts2(buf);
			lcd_pos(1,0);
			sprintf(buf,"Press 1 to start");
			lcd_puts2(buf);
			while((pressed = get_key()) != 1);
			avr_wait(500);
			//difficulty
			lcd_clr();
			lcd_pos(0,0);
			sprintf(buf,"Easy 1 Medium 2");
			lcd_puts2(buf);
			lcd_pos(1,0);
			sprintf(buf,"Hard 3");
			lcd_puts2(buf);
			
			while(1){
				pressed = get_key();
				if(pressed == 1) {
					win = 10;
					avr_wait(250);
					lcd_clr();
					lcd_pos(0,0);
					sprintf(buf,"Easy Selected");
					lcd_puts2(buf);
					break;
				}	
				else if(pressed == 2) {
					win = 15;
					avr_wait(250);
					lcd_clr();
					lcd_pos(0,0);
					sprintf(buf,"Medium Selected");
					lcd_puts2(buf);
					break;
				}
				else if(pressed == 3) {
					win = 20;
					avr_wait(250);
					lcd_clr();
					lcd_pos(0,0);
					sprintf(buf,"Hard Selected");
					lcd_puts2(buf);
					break;
				}
			}
			avr_wait(500);
			
			//speed selection
			lcd_clr();
			lcd_pos(0,0);
			sprintf(buf,"Normal 1 Fast 2");
			lcd_puts2(buf);
			lcd_pos(1,0);
			sprintf(buf,"Slow 3");
			lcd_puts2(buf);
			
			while(1){
				pressed = get_key();
				if(pressed == 1) {
					speed = 5;
					avr_wait(250);
					lcd_clr();
					lcd_pos(0,0);
					sprintf(buf,"Normal Speed");
					lcd_puts2(buf);
					break;
				}
				else if(pressed == 2) {
					speed = 10;
					avr_wait(250);
					lcd_clr();
					lcd_pos(0,0);
					sprintf(buf,"Fast Speed");
					lcd_puts2(buf);
					break;
				}
				else if(pressed == 3) {
					speed = 1;
					avr_wait(250);
					lcd_clr();
					lcd_pos(0,0);
					sprintf(buf,"Slow Speed");
					lcd_puts2(buf);
					break;
				}
			}
			avr_wait(500);
			round = 1; // game start
			life = 1;
			clear(game);
			clear(player);
		}   
		else if(life) { //play game
			//play game sound up to round number
			//take in player input
			//add input into the array 
			//display rolling input on screen so player can see notes pressed
			//compare to game and print lose/life = 0 or increase round and print correct next round
			//if round > win then print winner and make win = 0;
			
			//select rand game sound if retry the note lost on will be changed
			int next = (rand() % 12) + 1; // 1-12
			addNote(game, round-1, next);
			
			//play games sound
			lcd_clr();
			lcd_pos(0,0);
			sprintf(buf, "Round %d loading", round);
			lcd_puts2(buf);
			avr_wait(200);
			
			lcd_clr();
			lcd_pos(0,0);
			sprintf(buf, "Memorize");
			lcd_puts2(buf);
			avr_wait(150);
			//play computer song
			playSong(game,round,speed);
			
			//listen for player input
			lcd_clr();
			lcd_pos(0,0);
			sprintf(buf,"Play notes now");
			lcd_puts2(buf);
			avr_wait(200);
			
			int j = 0;
			clear(player);
			while(j < round) {
				while(!(pressed = get_key()));
				//key press isn't note
				if(pressed > 12) {
					if(pressed == 13) { // * restart game
						win = 0;
						break;
					}
				}
				// play sound for key pressed
				else {
					lcd_clr();
					lcd_pos(0,0);
					sprintf(buf, "Note %d", pressed);
					lcd_puts2(buf);
					player[j].freq = pressed;
					player[j].duration = 2;
					playNote(player[j],speed);
					if(pressed != game[j].freq) {
						//wrong note
						life = 0;
						break;
					}
					//correct note
					avr_wait(100);
					j++;
				}
			}
			if(win) {
				if(life) { //correct input
					round++;
					if(round > win) {
						//game cleared
						win = 0;
						lcd_clr();
						lcd_pos(0,0);
						sprintf(buf,"Congratulation");
						lcd_puts2();
						lcd_pos(1,0);
						sprintf(buf,"You Won!!");
						lcd_puts2(buf);
						avr_wait(300);
					}
					else {
						lcd_clr();
						lcd_pos(0,0);
						sprintf(buf, "Round %d done", round);
						lcd_puts2(buf);
						avr_wait(150);
						lcd_clr();
						lcd_pos(0,0);
						sprintf(buf, "Next round");
						lcd_puts2(buf);
					}
				}
				else {
					//wrong note selected show lose screen
					lcd_clr();
					lcd_pos(0,0);
					sprintf(buf, "Better Luck");
					lcd_puts2(buf);
					lcd_pos(1,0);
					sprintf(buf, "Next Time");
					lcd_puts2(buf);
					avr_wait(300);
				}
			}
			//game restart pressed if !win
		} 
		else if(!life) { // lost game give retry menu
			avr_wait(200);
			lcd_clr();
			lcd_pos(0,0);
			sprintf(buf,"Try again 1");
			lcd_puts2(buf);
			lcd_pos(1,0);
			sprintf(buf,"New Game 2");
			lcd_puts2(buf);
			while(1) {
				while(!(pressed = get_key()));
				if(pressed == 1) { //try same level again
					life = 1;
					lcd_clr();
					lcd_pos(0,0);
					sprintf(buf,"retry level");
					lcd_puts2(buf);
					break;
				} 
				else if(pressed == 2) { //new game
					win = 0;
					break;
				}
			}
			avr_wait(200);
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

void playNote(const struct note n, int tempo) {
	int fr = n.freq;
	int i, k = (n.duration/(period[fr]*.00004))/tempo;
	for(i = 0; i < k; ++i) {
		SET_BIT(PORTB,3);
		avr_wait(th[fr]);
		CLR_BIT(PORTB,3);
		avr_wait(th[fr]);
	}
}

int playSong(const struct note *song, int N, int tempo) {
	int i;
	char buf[17];
	for(i = 0; i < N; ++i) {
		lcd_clr();
		lcd_pos(0,0);
		sprintf(buf, "Note %d", i);
		lcd_puts2(buf);
		lcd_pos(1,3);
		sprintf(buf,"%d", song[i].freq);
		lcd_puts2(buf);
		
		playNote(song[i], tempo);
		avr_wait(250/tempo); //scaling wait based on speed 
// 		check for input
// 				int pressed = get_key();
// 				if(pressed) {
// 					if(pressed == 2) {
// 						// stop song
// 						lcd_clr();
// 						lcd_pos(0,0);
// 						char buf[20];
// 						sprintf(buf, "Stopping song");
// 						lcd_puts2(buf);
// 						avr_wait(200);
// 						return 2;
// 					}
// 				}
	}
	return 0;
}

void addNote(struct note *song, int pos, int pitch) {
	song[pos].duration = 2;
	song[pos].freq = pitch;
}

void clear(struct note *song) {
	for(int i = 0; i < 20; ++i) {
		struct note temp = {0,0};
		song[i] = temp;
	}
}