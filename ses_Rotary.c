/*
 * ses_Rotary.c
 *
 * Created: 6/25/2019 11:08:06 PM
 *  Author: Amr
 */ 
#include "ses_Rotary.h"
#include "ses_common.h"

#define ROTARY_DIRECTION_1  5
#define ROTARY_DIRECTION_2  2
#define BUTTON_NUM_DEBOUNCE_CHECKS 3


typedef void (*pTypeRotaryCallback)();

int Inc_CW=0;
int Inc_CCW=0;

pTypeRotaryCallback Rotary_CounterClockWise;
pTypeRotaryCallback Rotary_ClockWise;
bool Rotary_debouncing;

void rotary_setClockwiseCallback(pTypeRotaryCallback callback){
	Rotary_ClockWise=callback;
}


void rotary_setCounterClockwiseCallback(pTypeRotaryCallback callback){
	Rotary_CounterClockWise=callback;
}
void rotary_init(bool debouncing){
	DDRB &=~(1<<ROTARY_DIRECTION_1);
	PORTB |=(1<<ROTARY_DIRECTION_1);
	DDRG &=~(1<<ROTARY_DIRECTION_2);
	PORTG |=(1<<ROTARY_DIRECTION_2);
	
	if (Rotary_debouncing)
	{
		timer1_setCallback(&Rotary_checkState);
	}
	
}


bool Rotary_pressed_CW(void){
	if (!(PINB & (1 << ROTARY_DIRECTION_1))) {
		return true;
		} else {
		return false;
	}
}


bool Rotary_pressed_CCW(void){
	if (!(PING & (1 << ROTARY_DIRECTION_2))) {
		return true;
		} else {
		return false;
	}
}
void Rotary_checkState(){
	static uint8_t state[BUTTON_NUM_DEBOUNCE_CHECKS] = { };
	static uint8_t index = 0;
	static uint8_t debouncedState = 0;
	uint8_t lastDebouncedState = debouncedState;
	state[index] = 0;
	if (Rotary_pressed_CCW()) {
		state[index] |= 1;
	}
	if (Rotary_pressed_CW()) {
		state[index] |= 2;
	}
	index++;
	if (index == BUTTON_NUM_DEBOUNCE_CHECKS) {
		index = 0;
		uint8_t j = 0xFF;
		for (uint8_t i = 0; i < BUTTON_NUM_DEBOUNCE_CHECKS; i++) {
			j = j & state[i];
		}
		debouncedState = j;
		if (((debouncedState & 1) != 0) && ((lastDebouncedState &1) ==0)){
			Rotary_CounterClockWise(); 
		}
		if (((debouncedState & 2) != 0) && ((lastDebouncedState &2) ==0)) {
			Rotary_ClockWise(); 
		}
}
}
