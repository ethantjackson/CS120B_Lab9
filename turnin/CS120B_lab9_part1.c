/*	Author: ejack023
 *	 *	Lab Section: 023
 *	  *	Assignment: Lab #9  Exercise #1
 *	   *
 *	    *	I acknowledge all content contained herein, excluding template or example
 *	     *	code, is my own original work.
 *	      *
 *	       *	Demo Link: https://www.youtube.com/watch?v=w9vqF_ChS3U&ab_channel=EthanJackson
 *	        */

#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B=0x0B;
	
	OCR1A = 125;
	
	TIMSK1 = 0x02;
	
	TCNT1 = 0;
	
	_avr_timer_cntcurr = _avr_timer_M;
	
	SREG |= 0x80;
}
void TimerOff() {
	TCCR1B = 0x00;
}
void TimerISR() {
	TimerFlag=1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

unsigned char threeLEDs = 0;
enum SM1_STATES {SM1_START, SM1_FIRST, SM1_SECOND, SM1_THIRD} SM1_STATE;
void Tick_ThreeLEDsSM() {
	switch(SM1_STATE) {
		case SM1_START:
			SM1_STATE = SM1_FIRST;
			break;
		case SM1_FIRST:
			SM1_STATE = SM1_SECOND;
			break;
		case SM1_SECOND:
			SM1_STATE = SM1_THIRD;
			break;
		case SM1_THIRD:
			SM1_STATE= SM1_FIRST;
			break;
		default:
			SM1_STATE= SM1_FIRST;
			break;
	}
	switch(SM1_STATE) {
		case SM1_START:
		    threeLEDs = 0x00;
			break;
		case SM1_FIRST:
			threeLEDs = 0x01;
			break;
		case SM1_SECOND:
			threeLEDs = 0x02;
			break;
		case SM1_THIRD:
			threeLEDs = 0x04;
			break;
		default:
			threeLEDs = 0x00;
			break;
	}
}


unsigned char blinkingLED = 0;
enum SM2_STATES {SM2_START, SM2_TOGGLE} SM2_STATE;
void Tick_BlinkingLEDSM() {
	switch (SM2_STATE) {
		case SM2_START:
			SM2_STATE = SM2_TOGGLE;
			break;
		case SM2_TOGGLE:
			break;
		default:
			SM2_STATE = SM2_TOGGLE;
			break;
	}
	switch (SM2_STATE) {
		case SM2_START:
			blinkingLED = 0;
			break;
		case SM2_TOGGLE:
			if (!blinkingLED) blinkingLED = 1;
			else blinkingLED = 0;
			break;
		default:
			blinkingLED = 0;
			break;
	}
}

enum SM3_STATES {SM3_START, SM3_COMBINE} SM3_STATE;
void Tick_CombineLEDsSM() {
	switch (SM3_STATE) {
		case SM3_START:
			SM3_STATE = SM3_COMBINE;
			break;
		case SM3_COMBINE:
			break;
		default:
			SM3_STATE = SM3_COMBINE;
			break;
	}
	switch (SM3_STATE) {
		case SM3_START:
			break;
		case SM3_COMBINE:
			PORTB = (blinkingLED << 3) | threeLEDs;
			break;
		default:
			break;
	}
}

int main(void)
{
	DDRB = 0xFF; PORTB = 0x00;
	TimerSet(1000);
	TimerOn();
	SM1_STATE = SM1_START;
	SM2_STATE = SM2_START;
	SM3_STATE = SM3_START;
	while(1) {
		Tick_ThreeLEDsSM();
		Tick_BlinkingLEDSM();
		Tick_CombineLEDsSM();
		while(!TimerFlag);
		TimerFlag = 0;
	}
}

