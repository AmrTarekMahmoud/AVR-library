#ifndef SES_TIMER_H_
#define SES_TIMER_H_

/*INCLUDES *******************************************************************/

#include "ses_common.h"


/*PROTOTYPES *****************************************************************/


/**type of function pointer used as timer callback
 */
typedef void (*pTimerCallback)(void);


/**
 * Sets a function to be called when the timer fires. If NULL is
 * passed, no callback is executed when the timer fires.
 *
 * @param cb  valid pointer to callback function
 */
void timer2_setCallback(pTimerCallback callback);

/**
 * Starts hardware timer 2 of MCU with a period
 * of 1 ms.
 */
void timer2_start();


/**
 * Stops timer 2.
 */
void timer2_stop();


/**
 * Sets a function to be called when the timer fires.
 *
 * @param cb  pointer to the callback function; if NULL, no callback
 *            will be executed.
 */
void timer1_setCallback(pTimerCallback callback);


/**
 * Start timer 1 of MCU to trigger on compare match every 5ms.
 */
void timer1_start();

/**
 * Stops timer 1 of the MCU if it is no longer needed.
 */
void timer1_stop();
void timer5_setCallback();
void timer5_start();
void timer5_stop();

#endif /* SES_TIMER_H_ */
