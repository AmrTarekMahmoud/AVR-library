#include "ses_common.h"
/*
#include <avr/io.h>
#include "ses_common.h"
#include "ses_led.h"
#include "ses_button.h"
#include "ses_timer.h"
*/
#define BUTTON_ROTARY_PIN 6
#define BUTTON_JOYSTICK_PIN 7
#define BUTTON_NUM_DEBOUNCE_CHECKS 5

void button_checkState();

typedef void (*pButtonCallback)();

//variables to assign function pointer to the corresponding button
pButtonCallback rotaryCallback;
pButtonCallback joystickCallback;

void button_setRotaryButtonCallback(pButtonCallback callback) {
	//assign function to rotary button
	rotaryCallback = callback;
}

void button_setJoystickButtonCallback(pButtonCallback callback) {
	//assign function to the joystick
	joystickCallback = callback;
}

void button_init(bool debouncing) {
	//setting register to get a response from our buttons
	DDRB &= ~((1 << BUTTON_ROTARY_PIN) | (1 << BUTTON_JOYSTICK_PIN));
	PORTB |= ((1 << BUTTON_ROTARY_PIN) | (1 << BUTTON_JOYSTICK_PIN));

	if (debouncing) {
		// with debouncing on timer_1 so every 5 milliseconds the state of the button is checked
		timer1_setCallback(button_checkState);
	} else {
		// direct interrupt, without debouncing
		//PCICR |= (1 << PCIE0);
		//PCMSK0 |= (1 << PCINT7) | (1 << PCINT6);
		//SREG  |= (1<< 7); enabled with sei()
	}
}
bool button_isJoystickPressed(void) {
	//routine to check if the joystick is pressed or not
	if (!(PINB & (1 << BUTTON_JOYSTICK_PIN))) {
		return 1;
	} else {
		return 0;
	}
}

bool button_isRotaryPressed(void) {
	//routine to check if the rotarybutton is pressed
	if (!(PINB & (1 << BUTTON_ROTARY_PIN))) {
		return 1;
	} else {
		return 0;
	}
}

void button_checkState() {
	static uint8_t state[BUTTON_NUM_DEBOUNCE_CHECKS] = { };
	static uint8_t index = 0;
	static uint8_t debouncedState = 0;
	uint8_t lastDebouncedState = debouncedState;

// each bit in every state byte represents one button
	state[index] = 0;
	if (button_isJoystickPressed()) {
		state[index] |= 1;
	}
	if (button_isRotaryPressed()) {
		state[index] |= 2;
	}
	index++;
	if (index == BUTTON_NUM_DEBOUNCE_CHECKS) {
		index = 0;
	}
// init compare value and compare with ALL reads, only if
// we read BUTTON_NUM_DEBOUNCE_CHECKS consistent "1" in the state
// array, the button at this position is considered pressed
	uint8_t j = 0xFF;
	for (uint8_t i = 0; i < BUTTON_NUM_DEBOUNCE_CHECKS; i++) {
		j = j & state[i];
	}
	debouncedState = j;
	if (((debouncedState & 1) != 0) && ((lastDebouncedState &1) ==0)) {
		joystickCallback(); //do_stuff
	}
	if (((debouncedState & 2) !=0) && ((lastDebouncedState & 2) ==0)) {
		rotaryCallback(); //do_stuff
	}
}

ISR(PCINT0_vect) {
	//interrupt service routine for the direct interrupt
	if (button_isRotaryPressed()) {
		//execute assigned function if rotary is pressed
		rotaryCallback();
	}
	if (button_isJoystickPressed()) {
		//execute assigned function if joystick is pressed
		joystickCallback();
	}
}
