#ifndef SES_BUTTON_H_
#define SES_BUTTON_H_

/* INCLUDES ******************************************************************/

#include "ses_common.h"

/* FUNCTION PROTOTYPES *******************************************************/
/**
 * Initializes rotary encoder and joystick button
 */
void button_init(bool debouncing);

/** 
 * Get the state of the joystick button.
 */
bool button_isJoystickPressed(void);

/** 
 * Get the state of the rotary button.
 */
bool button_isRotaryPressed(void);
 // function pointer
typedef void (*pButtonCallback)();
//
void button_setRotaryButtonCallback(pButtonCallback callback);
//
void button_setJoystickButtonCallback(pButtonCallback callback);

void button_checkState();


#endif /* SES_BUTTON_H_ */
