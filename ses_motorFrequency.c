/*
 * ses_motorFrequency.c
 *
 * Created: 6/1/2019 5:15:25 PM
 *  Author: Amr
 */
#include <avr/io.h>
#include "ses_common.h"
#include "ses_led.h"

// Defines to calculate the actual motor frequencies
#define TIME_PREIOD 0.1
#define SPIKES 6
#define ARRAYSIZE 30
#define TICK_TIME 0.000004

static bool motorState = true;
static uint16_t Counter_freq;
static uint16_t motor_freq;
static uint8_t Spike_counter = 0;
static uint16_t Median_array[ARRAYSIZE];
static uint16_t Median_array_coppy[ARRAYSIZE];
static uint8_t Array_counter = 0;
static uint16_t temp = 0;
static uint16_t Median = 0;

void motorFrequency_init() {
	//The rising edge of INT0 generate an interrupt request.
	EICRA |= (1 << ISC00) | (1 << ISC01);
	//A edge or logic change on INT0 occurred and triggered an interrupt request.
	EIFR |= (1 << INTF0);
	//external pin at pin 0 is enabled
	EIMSK |= (1 << INT0);
	//enable global interrupt
	sei();
	//set CTC mode , prescaler 64, 20ms
	//timer5_start();
}
//set the current state of the motor, true for running, false for standing still
void motorCurrentState(bool state) {
	motorState = state;
}
//function to get the actual frequency of the motor in Hertz.
uint_fast16_t motorFrequency_getRecent() {
	float time_count;
	//of Motor is standing still, return 0 Hertz
	if (motorState == false) {
		return 0;
	} else {
		//calculating time for clock cycles needed for one turn of the motor
		time_count = Counter_freq * TICK_TIME;
		//divide 1 second by the calculated time for one turn to get frequency in Hertz
		motor_freq = (1 / time_count);
		return motor_freq;
	}
}

uint16_t motorFrequency_getMedian() {
	if (motorState == false) {
		// zero if motor is not running
		return 0;
	} else {
		for (int k = 0; k < ARRAYSIZE; k++) {
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
			{
				//copy Array in an atomic block, to have no new values in the array, while sorting it
				Median_array_coppy[k] = Median_array[k];
			}
		}
		//sorting the copied array from small numbers to big ones
		for (int i = 0; i < ARRAYSIZE; i++) {
			for (int j = 0; j < (ARRAYSIZE - 1); j++) {
				if (Median_array_coppy[j] > Median_array_coppy[j + 1]) {
						temp = Median_array_coppy[j];
						Median_array_coppy[j] = Median_array_coppy[j + 1];
						Median_array_coppy[j + 1] = temp;
						j = 0;
				}
			}
		}
		//select the median as return value
		Median = Median_array[ARRAYSIZE / 2];
	}
	return Median;
}
//check the state of the motor either running or stopped
void set_zero() {
	//setting the motor state to false by default 
	motorState = false;
}

ISR(INT0_vect) {
	//counting spikes from the motor to know, when one turn is over
	Spike_counter++;
	//if the spike counter is 6, we know that the motor has finished one full turn
	if (Spike_counter == SPIKES)  // what shall be included to this if statement
	{
		//start timer 5 again, if the motor was stopped externally and starts spinning again
		timer5_start();
		//turn green led off, while motor is spinning
		led_greenOff();
		//toggle yellow led for every full round
		led_yellowToggle();
		//get the value from timer 5 which has the number of clock cycles made during one turn 
		Counter_freq = TCNT5;
		//reset timer 5 to zero, to count for a new turn
		TCNT5 = 0;
		//reset spike counter to know, when it hits 6 spikes
		Spike_counter = 0;
		//if this part of the ISR is triggered, we really know, that the motor is spinning, so we set the state to true
		//normally this is done by our button press, but if we stop the motor by hand, and our ISR for timer 5 overflows
		//we set the motor state to false.
		//this part of the code is only to debug unexpected behavior
		motorState = true;
	}
	//write the actual value of the motor speed to an array, for calculating the median value
	if (Array_counter >= ARRAYSIZE) {
		Array_counter = 0;
	} else if (Array_counter < ARRAYSIZE) {
		Median_array[Array_counter] = motorFrequency_getRecent();
		// increment counter value to write in the next array index
		Array_counter++;
	}
}
