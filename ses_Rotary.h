#include "ses_common.h"
/*
 * ses_Rotary.h
 *
 * Created: 6/25/2019 11:08:29 PM
 *  Author: Amr
 */ 


#ifndef SES_ROTARY_H_
typedef void (*pTypeRotaryCallback)();
#define SES_ROTARY_H_




void rotary_init(bool debouncing);


void rotary_setClockwiseCallback(pTypeRotaryCallback callback);


void rotary_setCounterClockwiseCallback(pTypeRotaryCallback callback);

bool Rotary_pressed_CW(void);


bool Rotary_pressed_Ccw(void);

void Rotary_checkState();

#endif /* SES_ROTARY_H_ */
