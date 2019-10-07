/*
 * Fsm.h
 *
 * Created: 6/23/2019 11:37:11 PM
 *  Author: Amr
 */


#ifndef SES_FSM_H_
#define SES_FSM_H_


#include "ses_common.h"

typedef struct fsm_s Fsm;

typedef struct event_s {
	uint8_t signal; //< identifies the type of event
} Event;
typedef struct event_s Event;

typedef uint32_t systemTime_t;

void update_time_to_display();
void trigger_alarm();
void remove_alarm_5_seconds(void* parm);


struct time_t {
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint16_t milli;
};


enum {
	RET_HANDLED,
	RET_IGNORED,
	RET_TRANSITION
};

enum {

	ENTRY,
	EXIT,
	JOYSTICK_PRESSED,
	ROTARY_PRESSED,
	ANYKEY_PRESSED,
	ALARM_MATCH_CLOCK,
	FIVE_SECONDS_PASSED,
	ROTARY_CW,
	ROTARY_CCW

};
typedef uint8_t fsmReturnStatus;
typedef fsmReturnStatus (*State)(Fsm *, const Event*);




struct fsm_s {
	State state; //< current state, pointer to event handler
	bool isAlarmEnabled; //< flag for the alarm status
	struct time_t timeSet; //< multi-purpose var for system time and alarm time
};






typedef struct{
	Fsm super;
	}Fsm_AlarmClock;

typedef struct{
	Event super;
	}Event_AlarmClock;

inline static void fsm_dispatch(Fsm* fsm, const Event* event) {
	static Event entryEvent = { .signal = ENTRY };
	static Event exitEvent = { .signal = EXIT };
	State s = fsm->state;
	fsmReturnStatus r = fsm->state(fsm, event);
	if (r == RET_TRANSITION) {
		s(fsm, &exitEvent); //< call exit action of last state
		fsm->state(fsm, &entryEvent); //< call entry action of new state
	}
}

/* sets and calls initial state of state machine */
inline static void fsm_init(Fsm* fsm, State init) {

	Event entryEvent = { .signal = ENTRY };
	fsm->state = init;
	fsm->state(fsm, &entryEvent);
}

//function and states for the fsm, for further explanation read in the fsm.c file
fsmReturnStatus Init_clock(Fsm* fsm, const Event *e);

fsmReturnStatus Clock_default(Fsm* fsm, const Event *e);

fsmReturnStatus Set_hour(Fsm* fsm, const Event *e);

fsmReturnStatus Set_minute(Fsm* fsm, const Event *e);

fsmReturnStatus Enable_alarm(Fsm* fsm, const Event *e);

fsmReturnStatus Disable_alarm(Fsm* fsm, const Event *e);

fsmReturnStatus Start_clock(Fsm* fsm, const Event *e);

fsmReturnStatus Set_alarm(Fsm* fsm, const Event *e);

fsmReturnStatus Set_hour_Alarm(Fsm* fsm, const Event *e);

fsmReturnStatus Set_minute_Alarm(Fsm* fsm, const Event *e);

fsmReturnStatus Stop_alarm(Fsm* fsm, const Event *e);

systemTime_t human_time_to_milli_seconds(struct time_t time);

void print_time(struct time_t time, bool seconds);

void led_toggle(void* parm);
#endif /* SES_FSM_H_ */
