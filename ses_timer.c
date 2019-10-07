/* INCLUDES ******************************************************************/
#include "ses_timer.h"
#include "ses_uart.h"
#include "ses_common.h"

/* DEFINES & MACROS **********************************************************/
#define TIMER1_CYC_FOR_5MILLISEC    0x4E1
#define TIMER2_CYC_FOR_1MILLISEC	0xF9

/*FUNCTION DEFINITION ********************************************************/

//typedef void (*pTimerCallback)();
pTimerCallback timer_5;
pTimerCallback timer_2;
pTimerCallback timer_1;

void timer2_setCallback(pTimerCallback callback) {
	//assign function pointer to timer_2 for execution in the interrupt service routine
	timer_2 = callback;
}

void timer2_start() {
	//initialization of timer_2 to trigger every milli second

	PRR0 &= ~(1 << PRTIM2);
	TCCR2A |= (1 << WGM21); //Use Clear Timer on Compare Match (CTC) mode operation
	TCCR2B |= (1 << CS22);  //Select a prescalar of 64
	TIMSK2 |= (1 << OCIE2A);  //Set interrupt mask register for Compare A
	TIFR2 |= (1 << OCF2A); //Clear the interrupt flag by setting a 1 in flag register for Compare A
	OCR2A = 0xFA;//set a value in register OCR2A in order to generate an interrupt every 1 ms
}

void timer2_stop() {
	PRR0 |= (1 << PRTIM2);
	TCCR2B &= ~(1 << CS22);//reset prescalar for timer 2
}

void timer1_setCallback(pTimerCallback callback) {
	//assign function pointer to timer_1 for execution in the interrupt service routine
	timer_1 = callback;
}

void timer1_start() {
	//initialization of timer_1 to trigger every 5 milli seconds
	// set power on for timer 1
	PRR0 &= ~(1 << PRTIM1);
	//Use Clear Timer on Compare Match (CTC) mode operation
	TCCR1B |= (1 << WGM12) | (1 << CS11) | (1 << CS10);
	//set a value in register OCR2A in order to generate an interrupt every 5 ms
	OCR1A |= 0x4E2;
	//enable interrupt
	TIMSK1 |= (1 << OCIE1A);
	TIFR1 |= (1 << OCF1A);
}

void timer1_stop() {
	//Stop timer_1
	PRR0 |= (1 << PRTIM1);
	TCCR1B &= ~(1 << CS11) | (1 << CS10);
}

void timer5_setCallback(pTimerCallback callback) {
	//assign function pointer to timer_2 for execution in the interrupt service routine
	timer_5 = callback;
}

void timer5_start(){
	PRR1 &=~(1<<PRTIM5);
	TCCR5B |= (1 << WGM52) |(1<<CS51)| (1 << CS50);
	OCR5A  |=0xFFFF;
	TIMSK5 |= (1<<OCIE5A);
	TIFR5  |=(1<<OCF5A);
}

void timer5_stop(){
	PRR1 |= (1<<PRTIM5);
}

ISR(TIMER1_COMPA_vect) {
	//execute assigned function in the interrupt service routine
	timer_1();
}

ISR(TIMER2_COMPA_vect) {
	//execute assigned function in the interrupt service routine
	timer_2();
}

ISR(TIMER5_COMPA_vect) {
	timer_5();
}
