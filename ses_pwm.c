/*INCLUDES *******************************************************************/
#include "ses_common.h"
#include "ses_timer.h"
#include "ses_scheduler.h"
#include "ses_lcd.h"
//#include "util/atomic.h"
#include "ses_uart.h"
//#include <util/delay.h>

void pwm_init(void){
	DDRG |= (1<< PORTG5);
	//timer 0 aktivstion
	PRR0 &= ~(1<<PRTIM0);
	//setting the wave generation to fast-WPM
	TCCR0A |= (1<< WGM00) | (1<< WGM01);
	TCCR0B |= (1<< WGM02);
	//disable prescalar
	TCCR0B &= ~(1<< CS02) | (1<< CS01);
	TCCR0B |= (1<< CS00);
	// inverted waveform
	TCCR0A |= (1<< COM0B0) | (1<<COM0B1);
	//this bit must be set to zero when TCCR0B is written to PWM
	TCCR0B &= ~(1<< FOC0A) | (1 << FOC0B);
	//TCCR0A |= (1<< COM0B1);
	OCR0A =0xFF;
	sei();
	//OCR0B =0xFF;
}
void pwm_setDutyCycle(uint8_t dutyCycle){
	OCR0B = dutyCycle;
}
