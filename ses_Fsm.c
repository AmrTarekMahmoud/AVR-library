#include "ses_common.h"
#include "ses_Fsm.h"

#define TRANSITION(newState) (fsm->state = newState, RET_TRANSITION)

#define HOURCONVERSION 3600000
#define MINUTECONVERSION 60000
#define SECONDCONVERSION 1000

struct time_t Clock_Time;
struct time_t Alarm_Time;

static systemTime_t systemtime_milli = 0;

//task for the scheduler to handle events on the alarm clock
struct taskDescriptor_s Alarm_On_Off;
struct taskDescriptor_s Blink_red_led;
struct taskDescriptor_s run_clock_scheuler;
struct taskDescriptor_s update_display_scheduler;
struct taskDescriptor_s remove_alarm;

//Flags which are used to deal in some conditional switch cases
Fsm* fsm_intern;
bool Buttons_disable_alarm_F = false;

//translating human time from HH:MM:SS to milliseconds and return the value for further calculation
systemTime_t human_time_to_milli_seconds(struct time_t time) {
	systemTime_t time_milli = time.hour * HOURCONVERSION
			+ time.minute * MINUTECONVERSION + time.second * SECONDCONVERSION;
	return time_milli;
}

//function to print actual time the display, parm "bool seconds" is used to decide if seconds should be printed too or not
void print_time(struct time_t time, bool seconds) {
	//clear old values
	lcd_clear();
	lcd_setCursor(0, 0);
	//to keep the display consistent, print additional zero if a value is a single digit number
	//effect of parm seconds is considered
	if (seconds == true) {
		if (time.hour < 10) {
			fprintf(lcdout, "0%d:", time.hour);
		} else {
			fprintf(lcdout, "%d:", time.hour);
		}
		if (time.minute < 10) {
			fprintf(lcdout, "0%d:", time.minute);
		} else {
			fprintf(lcdout, "%d:", time.minute);
		}
		if (time.second < 10) {
			fprintf(lcdout, "0%d", time.second);
		} else {
			fprintf(lcdout, "%d", time.second);
		}
	} else {
		if (time.hour < 10) {
			fprintf(lcdout, "0%d:", time.hour);
		} else {
			fprintf(lcdout, "%d:", time.hour);
		}
		if (time.minute < 10) {
			fprintf(lcdout, "0%d", time.minute);
		} else {
			fprintf(lcdout, "%d", time.minute);
		}
	}
}

//converting the system time from milliseconds to human friendly time every second causes a lot of computation
//due to the effect that devision and or modulo operants are needed
//so we simple increment the Clock_Time every second with our scheduler
void update_time_to_display() {
	//increment seconds
	Clock_Time.second++;
	//check if we hit 60 seconds or not to know, when a minute is over
	if (Clock_Time.second < 60) {
	} else {
		//if the count for seconds reaches 60, set seconds back to zero and increment minutes
		Clock_Time.second = 0;
		//check if minutes reach the value of 60
		Clock_Time.minute++;
		if (Clock_Time.minute < 60) {
		}
		//if minutes reaches 60, increment hours
		else {
			Clock_Time.minute = 0;
			Clock_Time.hour++;
			//if hours reaches 24, set it back to zero
			if (Clock_Time.hour < 24) {
			} else {
				Clock_Time.hour = 0;
			}
		}
	}
}

//task for the scheduler which will be added, if the alarm event is triggerd
void trigger_alarm() {
	//set flag for the alarm event to handle if condition in the "Start_Clock" state
	Buttons_disable_alarm_F = true;
	Blink_red_led.execute = false;
	Blink_red_led.expire = 1;
	Blink_red_led.period = 125;
	Blink_red_led.task = &led_toggle;
	scheduler_add(&Blink_red_led);
}
void led_toggle(void* parm){
	led_redToggle();
}
//task to remove the alarm after 5 seconds
void remove_alarm_5_seconds(void* parm) {
	scheduler_remove(&Blink_red_led);
	fsm_intern->isAlarmEnabled = false;
	led_yellowOff();
}

//function to update the display with the given time
//this task is added to the scheduler by a second interval
void scheduler_update_display() {
	//toggle green led every second
	led_greenToggle();
	lcd_clear();
	lcd_setCursor(0, 0);
	print_time(Clock_Time, true);
	if (fsm_intern->isAlarmEnabled == false) {
		fprintf(lcdout, "\nALARM DISABLED");
	} else {
		fprintf(lcdout, "\nALARM ENABLED");
	}
}

//starting point for the fsm
fsmReturnStatus Init_clock(Fsm* fsm, const Event *e) {
	//every fsm starts with a switch case to handle the proper event e.g. transition to the state or button press
	//every case handels the functionality of the state and return value
	switch (e->signal) {
	//first case, setting all the values needed for further calculation
	case ENTRY:
		//set clock values to zero, to have a proper starting point of entering system time
		Clock_Time.hour = 0;
		Clock_Time.minute = 0;
		Clock_Time.second = 0;
		Clock_Time.milli = 0;
		//show some text on the lcd for the user
		lcd_clear();
		lcd_setCursor(0, 0);
		fprintf(lcdout, "HH:MM\n");
		fprintf(lcdout, "please enter time");
		return RET_HANDLED;
		break;
		//second case: transition into set hour function
	case ROTARY_PRESSED:
		return TRANSITION(&Set_hour);
		break;
	default:
		return RET_IGNORED;
	}
	return 0;
}

fsmReturnStatus Clock_default(Fsm* fsm, const Event *e) {
	switch (e->signal) {
	case ROTARY_PRESSED:
		return TRANSITION(Set_hour);
		break;
		// should add here another event to check on whether the time has been changed from 0 or not (maybe!?)
	default:
		return RET_IGNORED;
	}
}

//state to enter system time hour
fsmReturnStatus Set_hour(Fsm* fsm, const Event *e) {
	switch (e->signal) {
	//entry case, clearing display and tell the user what to do
	case ENTRY:
		//call print function without seconds
		print_time(Clock_Time, false);
		fprintf(lcdout, "\nenter hours\npush rotary");
		return RET_HANDLED;
		break;
		//case 2: transition to entry the state to set minutes
	case JOYSTICK_PRESSED:
		return TRANSITION(Set_minute);
		break;
		//case 3: incrementing the hours and display everything on the lcd
	case ROTARY_PRESSED:
		Clock_Time.hour++;
		//if we hit 24 hours, we go back to zero
		if (Clock_Time.hour < 24) {
			print_time(Clock_Time, false);
			fprintf(lcdout, "\nenter hours\npush rotary");
		} else {
			Clock_Time.hour = 0;
			print_time(Clock_Time, false);
			fprintf(lcdout, "\nenter hours\npush rotary");
		}
		return RET_HANDLED;
		break;
		//using rotary clock wise to increment hours
	case ROTARY_CW:
		Clock_Time.hour++;
		if (Clock_Time.hour < 24) {
			print_time(Clock_Time, false);
			fprintf(lcdout, "\nenter hours\npush rotary");
		} else {
			Clock_Time.hour = 0;
			print_time(Clock_Time, false);
			fprintf(lcdout, "\nenter hours\npush rotary");
		}
		return RET_HANDLED;
		break;
		//using rotary counter clock wise to decrement hours
	case ROTARY_CCW:
		Clock_Time.hour--;
		//if we want to go lower then 0 hours, time goes to 23
		if (Clock_Time.hour > 0 && Clock_Time.hour < 24) {
			print_time(Clock_Time, false);
			fprintf(lcdout, "\nenter hours\npush rotary");
		} else {
			Clock_Time.hour = 23;
			print_time(Clock_Time, false);
			fprintf(lcdout, "\nenter hours\npush rotary");
		}
		return RET_HANDLED;
		break;
	default:
		return RET_IGNORED;
	}

}
//function to set the minutes for the clock
fsmReturnStatus Set_minute(Fsm* fsm, const Event *e) {
	switch (e->signal) {
	//entry case: clearing display and tell the user what to do
	case ENTRY:
		print_time(Clock_Time, false);
		fprintf(lcdout, "\nenter minutes\npush rotary");
		return RET_HANDLED;
		break;
		//case 2: Transition into the next function, where the clock shall start
	case JOYSTICK_PRESSED:
		return TRANSITION(Start_clock);
		break;
		//case 3: incrementing the minutes and show all values on the display
	case ROTARY_PRESSED:
		Clock_Time.minute++;
		//if counter hits 60 minutes, go back to zero
		if (Clock_Time.minute < 60) {
			print_time(Clock_Time, false);
			fprintf(lcdout, "\nenter minutes\npush rotary");
		} else {
			Clock_Time.minute = 0;
			print_time(Clock_Time, false);
			fprintf(lcdout, "\nenter minutes\npush rotary");
		}
		return RET_HANDLED;
		break;
		//using rotary clock wise to increment minutes
	case ROTARY_CW:
		Clock_Time.minute++;
		//minute counter hits 60, go back to zero
		if (Clock_Time.minute < 60) {
			print_time(Clock_Time, false);
			fprintf(lcdout, "\nenter minute\npush rotary");
		} else {
			Clock_Time.minute = 0;
			print_time(Clock_Time, false);
			fprintf(lcdout, "\nenter minute\npush rotary");
		}
		return RET_HANDLED;
		break;
		//using rotary counter clock wise to decrement minutes
	case ROTARY_CCW:
		Clock_Time.minute--;
		//if minutes counter goes lower then 0, set it to 59
		if (Clock_Time.minute > 0 && Clock_Time.minute < 60) {
			print_time(Clock_Time, false);
			fprintf(lcdout, "\nenter minute\npush rotary");
		} else {
			Clock_Time.minute = 59;
			print_time(Clock_Time, false);
			fprintf(lcdout, "\nenter minute\npush rotary");
		}
		return RET_HANDLED;
		break;
	default:
		return RET_IGNORED;
	}

}

//state to enable the alarm
fsmReturnStatus Enable_alarm(Fsm* fsm, const Event *e) {
	switch (e->signal) {
	case ENTRY:
		//set alarm flags to true
		fsm_intern->isAlarmEnabled = true;
		fsm->isAlarmEnabled = true;
		//yellow led indicates an enabled alarm
		led_yellowOn();
		//adding alarm event of blinking red led
		Alarm_On_Off.execute = false;
		Alarm_On_Off.period = 0;
		//calculating the time in milliseconds, when the alarm event shall take place
		Alarm_On_Off.expire = human_time_to_milli_seconds(Alarm_Time)
				- scheduler_getTime();
		Alarm_On_Off.task = &trigger_alarm;
		scheduler_add(&Alarm_On_Off);
		//adding additional event to stop the alarm after 5 seconds
		remove_alarm.execute = false;
		remove_alarm.expire = 5000 + human_time_to_milli_seconds(Alarm_Time)
				- scheduler_getTime();
		remove_alarm.period = 0;
		remove_alarm.task = &remove_alarm_5_seconds;
		scheduler_add(&remove_alarm);
		//go back to start clockevent
		return TRANSITION(&Start_clock);
		break;
	default:
		return RET_IGNORED;
	}

}

//state to disable alarm
fsmReturnStatus Disable_alarm(Fsm* fsm, const Event *e) {
	switch (e->signal) {
	case ENTRY:
		//remove alarm flags
		fsm_intern->isAlarmEnabled = false;
		fsm->isAlarmEnabled = false;
		//turn led definitly off
		led_yellowOff();
		led_redOff();
		//remove all scheduler task related to the alarm
		scheduler_remove(&Alarm_On_Off);
		scheduler_remove(&remove_alarm);
		scheduler_remove(&Blink_red_led);
		Buttons_disable_alarm_F = false;
		return TRANSITION(Start_clock);
		break;
	default:
		return RET_IGNORED;
	}
}
//main control state after system time is set, we always return to this state to handle all events
fsmReturnStatus Start_clock(Fsm* fsm, const Event *e) {

	switch (e->signal) {
	//case to display the time entered by the user
	case ENTRY:
		//translate the time, which was given by the user to milliseconds
		systemtime_milli = human_time_to_milli_seconds(Clock_Time);
		//give the calculated system time in milli seconds over to the scheduler to increment it
		scheduler_setTime(systemtime_milli);
		//updateing the time which will be displayed on the lcd, by using the scheduler
		run_clock_scheuler.execute = false;
		run_clock_scheuler.expire = 1000;
		run_clock_scheuler.period = 1000;
		run_clock_scheuler.task = &update_time_to_display;
		scheduler_add(&run_clock_scheuler);
		//task to update the actual time on the lcd
		//to do it on the scheduler, we have the freedom to stop this function while we
		//are entering the alarm time without any new values on the display
		update_display_scheduler.execute = false;
		update_display_scheduler.expire = 1;
		update_display_scheduler.period = 1000;
		update_display_scheduler.task = &scheduler_update_display;
		scheduler_add(&update_display_scheduler);
		return RET_HANDLED;
		break;
		//case to set the alarm to enable or disable
	case ROTARY_PRESSED:
		//checking if alarm is enabled or not and a alarm is actually triggered
		if (Buttons_disable_alarm_F == true) {
			return TRANSITION(Disable_alarm);
			break;
		}
		if (fsm->isAlarmEnabled == false) {
			return TRANSITION(Enable_alarm);
			break;
		} else {
			return TRANSITION(Disable_alarm);
			break;
		}
		//setting alarm time or disable an active alarm
	case JOYSTICK_PRESSED:
		//if state is fullfilled if an alarm is actually going (blinking red led)
		//in this case any button has to disable the alarm
		if (Buttons_disable_alarm_F == true) {
			return TRANSITION(Disable_alarm);
			break;
		}
		//normal function of the joystick, where the state of enteryng alarm time is enabled
		return TRANSITION(Set_alarm);
	default:
		return scheduler_getTime();
	}
}

//entry state of setting the alarm
fsmReturnStatus Set_alarm(Fsm* fsm, const Event *e) {
	switch (e->signal) {
	//case to display the the time entered by the user
	case ENTRY:
		//initialisation of the alarm values
		Alarm_Time.hour = 0;
		Alarm_Time.minute = 0;
		Alarm_Time.second = 0;
		Alarm_Time.milli = 0;
		//remove the display update for the time from the scheduler
		//to enter the alarm clock
		scheduler_remove(&update_display_scheduler);
		//print alarm time without seconds
		print_time(Alarm_Time, false);
		fprintf(lcdout, "\nSet Alarm time");
		return RET_HANDLED;
		break;
		//go over to set alarm hour
	case ROTARY_PRESSED:
		return TRANSITION(Set_hour_Alarm);
		break;
	default:
		return RET_IGNORED;
		break;
		return 0;
	}
}

//state to enter alarm hour
fsmReturnStatus Set_hour_Alarm(Fsm* fsm, const Event *e) {

	switch (e->signal) {
	case ENTRY:
		//entry state to display current Alarm time to the user
		print_time(Alarm_Time, false);
		fprintf(lcdout, "\nenter hours\npush rotary");
		return RET_HANDLED;
		break;
	case JOYSTICK_PRESSED:
		//if joystick is pressed go to the next state, to set the minutes for the alarm
		return TRANSITION(Set_minute_Alarm);
		break;
	case ROTARY_PRESSED:
		//in this case the hours of the alarm get incremented
		//and will be printed out to the display
		Alarm_Time.hour++;
		if (Alarm_Time.hour < 24) {
			print_time(Alarm_Time, false);
			fprintf(lcdout, "\nenter hours\npush rotary");
		} else {
			Alarm_Time.hour = 0;
			print_time(Alarm_Time, false);
			fprintf(lcdout, "\nenter hours\npush rotary");
			return RET_HANDLED;
			break;
			//use rotary clockwise to increment hours
			case ROTARY_CW:
			Alarm_Time.hour++;
			if (Alarm_Time.hour < 24) {
				print_time(Alarm_Time, false);
				fprintf(lcdout, "\nenter hours\npush rotary");
			} else {
				Alarm_Time.hour = 0;
				print_time(Alarm_Time, false);
				fprintf(lcdout, "\nenter hours\npush rotary");
			}
			return RET_HANDLED;
			break;
			//use rotary counter clock wise to decrement hour
			case ROTARY_CCW:
			Alarm_Time.hour--;
			if (Alarm_Time.hour > 0 && Alarm_Time.hour < 24) {
				print_time(Alarm_Time, false);
				fprintf(lcdout, "\nenter hours\npush rotary");
			} else {
				Alarm_Time.hour = 23;
				print_time(Alarm_Time, false);
				fprintf(lcdout, "\nenter hours\npush rotary");
			}
			return RET_HANDLED;
			break;
			default:
			return RET_IGNORED;
		}

	}
	return RET_IGNORED;
}

//state to set alarm minute
fsmReturnStatus Set_minute_Alarm(Fsm* fsm, const Event *e) {

	switch (e->signal) {
	case ENTRY:
		print_time(Alarm_Time, false);
		fprintf(lcdout, "\nenter minutes\npush rotary");
		return RET_HANDLED;
		break;
	case JOYSTICK_PRESSED:
		return TRANSITION(Start_clock);
		break;
		//increment minutes
	case ROTARY_PRESSED:
		Alarm_Time.minute++;
		if (Alarm_Time.minute < 60) {
			print_time(Alarm_Time, false);
			fprintf(lcdout, "\nenter minutes\npush rotary");
		} else {
			Alarm_Time.minute = 0;
			print_time(Alarm_Time, false);
			fprintf(lcdout, "\nenter minutes\npush rotary");
		}
		return RET_HANDLED;
		break;
		//increment minutes
	case ROTARY_CW:
		Alarm_Time.minute++;
		if (Alarm_Time.minute < 60) {
			print_time(Alarm_Time, false);
			fprintf(lcdout, "\nenter minute\npush rotary");
		} else {
			Alarm_Time.minute = 0;
			print_time(Alarm_Time, false);
			fprintf(lcdout, "\nenter minute\npush rotary");
		}
		return RET_HANDLED;
		break;
		//decrement minutes
	case ROTARY_CCW:
		Alarm_Time.minute--;
		if (Alarm_Time.minute > 0 && Alarm_Time.minute < 60) {
			print_time(Alarm_Time, false);
			fprintf(lcdout, "\nenter minute\npush rotary");
		} else {
			Alarm_Time.minute = 59;
			print_time(Alarm_Time, false);
			fprintf(lcdout, "\nenter minute\npush rotary");
		}
		return RET_HANDLED;
		break;
	default:
		return RET_IGNORED;
	}
}
