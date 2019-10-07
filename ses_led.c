/* INCLUDES ******************************************************************/

#include "ses_common.h"
#include "ses_led.h"

/* DEFINES & MACROS **********************************************************/

// LED wiring on SES board
#define LED_RED_PORT       	PORTG
#define LED_RED_PIN         	1

#define LED_YELLOW_PORT 	PORTF
#define LED_YELLOW_PIN      	7

#define LED_GREEN_PORT 		PORTF
#define LED_GREEN_PIN       	6

/* FUNCTION DEFINITION *******************************************************/

void led_redInit(void) {
	//setting Register for the LED to output
	DDR_REGISTER(LED_RED_PORT) |= (1 << LED_RED_PIN);
}

void led_redToggle(void) {
	//switching the pit state to toggle LED
	LED_RED_PORT ^= (1 << LED_RED_PIN);
}

void led_redOn(void) {
	//Setting bit to high, to turn LED on
	LED_RED_PORT &= ~(1 << LED_RED_PIN);
}

void led_redOff(void) {
	//Setting bit to low, to turn LED off
	LED_RED_PORT |= (1 << LED_RED_PIN);
}

void led_yellowInit(void) {
	//setting Register for the LED to output
	DDR_REGISTER(LED_YELLOW_PORT) |= (1<< LED_YELLOW_PIN);
}

void led_yellowToggle(void) {
	//switching the pit state to toggle LED
	LED_YELLOW_PORT ^= (1 << LED_YELLOW_PIN);
}

void led_yellowOn(void) {
	//Setting bit to high, to turn LED on
	LED_YELLOW_PORT &= ~(1 << LED_YELLOW_PIN);
}

void led_yellowOff(void) {
	//Setting bit to low, to turn LED off
	LED_YELLOW_PORT |= (1 << LED_YELLOW_PIN);
}

void led_greenInit(void) {
	//setting Register for the LED to output
	DDR_REGISTER(LED_GREEN_PORT) |= (1 << LED_GREEN_PIN);
}

void led_greenToggle(void) {
	//switching the pit state to toggle LED
	LED_GREEN_PORT ^= (1 << LED_GREEN_PIN);
}

void led_greenOn(void) {
	//Setting bit to high, to turn LED on
	LED_GREEN_PORT &= ~(1 << LED_GREEN_PIN);
}

void led_greenOff(void) {
	//Setting bit to low, to turn LED off
	LED_GREEN_PORT |= (1 << LED_GREEN_PIN);
}

// Funktion to initialize alle three LEDs on the board and turn them off by default
void init_led(){
	OCR2A = 0xFF;
	led_redInit();
	led_yellowInit();
	led_greenInit();
	led_greenOff();
	led_yellowOff();
	led_redOff();
}
