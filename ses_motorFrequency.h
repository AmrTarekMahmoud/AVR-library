#ifndef SES_MOTORFREQUENCY_H_
#define SES_MOTORFREQUENCY_H_


/*INCLUDES *******************************************************************/
#include "ses_common.h"


void motorFrequency_init();
uint16_t motorFrequency_getRecent();
uint16_t motorFrequency_getMedian();
void motorCurrentState(bool state);
int Spike_counter_funktion();
void set_zero();

#endif /* SES_MOTORFREQUENCY_H_ */
