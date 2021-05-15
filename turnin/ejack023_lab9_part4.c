
/*	Author: ejack023
 *	 *	 *	Lab Section: 023
 *	  *	  *	Assignment: Lab #9  Exercise #4
 *	   *	   *
 *	    *	    *	I acknowledge all content contained herein, excluding template or example
 *	     *	     *	code, is my own original work.
 *	      *	      *
 *	       *	       *	Demo Link: https://www.youtube.com/watch?v=FWUYBvWbZd4&ab_channel=EthanJackson
 *	        *	        */

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

enum SM4_STATES {SM4_START, SM4_WAIT, SM4_ON, SM4_OFF} SM4_STATE;
unsigned char btn;
unsigned char playSound = 0;
void Tick_PlaySoundSM() {
	btn = (~PINA) & 0x04;
	switch (SM4_STATE) {
		case SM4_START:
		if (btn) SM4_STATE = SM4_ON;
		else SM4_STATE = SM4_WAIT;
		break;
		case SM4_WAIT:
		if (btn) SM4_STATE = SM4_ON;
		break;
		case SM4_ON:
		if (!btn) SM4_STATE = SM4_WAIT;
		else SM4_STATE = SM4_OFF;
		break;
		case SM4_OFF:
		if (!btn) SM4_STATE = SM4_WAIT;
		else SM4_STATE = SM4_ON;
		break;
		default:
		SM4_STATE = SM4_WAIT;
		break;
	}
	switch (SM4_STATE) {
		case SM4_START:
			playSound = 0x00;
			break;
		case SM4_WAIT:
			playSound = 0x00;
			break;
		case SM4_ON:
			playSound = 0x01;
			break;
		case SM4_OFF:
			playSound = 0x00;
			break;
		default:
			playSound = 0x00;
			break;
	}
};

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
			PORTB = (playSound << 4) | (blinkingLED << 3) | threeLEDs;
			break;
		default:
			break;
	}
}

unsigned char soundPeriod = 2;
enum SM5_STATES {SM5_START, SM5_WAIT, SM5_INC_PER, SM5_WAIT_INC_RELEASE, SM5_DEC_PER, SM5_WAIT_DEC_RELEASE} SM5_STATE;
void Tick_AdjustFrequencySM() {
	btn = ~PINA;
	switch (SM5_STATE) {
		case SM5_START:
			SM5_STATE = SM5_WAIT;
			break;
		case SM5_WAIT:
			if (btn & 0x01) {
				SM5_STATE = SM5_DEC_PER;
			}
			else if (btn & 0x02) {
				SM5_STATE = SM5_INC_PER;
			}
			break;
		case SM5_INC_PER:
			SM5_STATE = SM5_WAIT_INC_RELEASE;
			break;
		case SM5_WAIT_INC_RELEASE:
			if (!(btn & 0x02)) {
				SM5_STATE = SM5_WAIT;
			}
			break;
		case SM5_DEC_PER:
			SM5_STATE = SM5_WAIT_DEC_RELEASE;
			break;
		case SM5_WAIT_DEC_RELEASE:
			if (!(btn & 0x01)) {
				SM5_STATE = SM5_WAIT;
			}
			break;
		default:
			SM5_STATE = SM5_WAIT;
			break;
	}
	switch (SM5_STATE) {
		case SM5_START:
			break;
		case SM5_WAIT:
			break;
		case SM5_INC_PER:
			++soundPeriod;
			break;
		case SM5_WAIT_INC_RELEASE:
			break;
		case SM5_DEC_PER:
			if (soundPeriod > 1) --soundPeriod;		
			break;
		case SM5_WAIT_DEC_RELEASE:
			break;
		default:
			break;
	}
};

int main(void)
{
	unsigned long SM1_elapsedTime = 300;
	unsigned long SM2_elapsedTime = 1000;
	unsigned long SM4_elapsedTime = soundPeriod;
	const unsigned long timerPeriod = 1;
	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0X00; PORTA = 0XFF;
	TimerSet(timerPeriod);
	TimerOn();
	SM1_STATE = SM1_START;
	SM2_STATE = SM2_START;
	SM3_STATE = SM3_START;
	SM4_STATE = SM4_START;
	SM5_STATE = SM5_START;
	while(1) {
		Tick_AdjustFrequencySM();
		if (SM1_elapsedTime >= 300) {
			Tick_ThreeLEDsSM();
			SM1_elapsedTime = 0;
		}
		if (SM2_elapsedTime >= 1000) {
			Tick_BlinkingLEDSM();
			SM2_elapsedTime = 0;
		}
		if (SM4_elapsedTime >= soundPeriod) {
			Tick_PlaySoundSM();
			SM4_elapsedTime = 0;
		}
		Tick_CombineLEDsSM();
		while(!TimerFlag);
		TimerFlag = 0;
		SM1_elapsedTime += timerPeriod;
		SM2_elapsedTime += timerPeriod;
		SM4_elapsedTime += timerPeriod;
	}
}

