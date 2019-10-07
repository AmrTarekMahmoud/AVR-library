/*INCLUDES ************************************************************/
/*#include "ses_timer.h"
#include "ses_scheduler.h"
#include "ses_lcd.h"
#include "util/atomic.h"
#include "ses_uart.h"
#include <util/delay.h>*/
#include "ses_common.h"

/* PRIVATE VARIABLES **************************************************/
/** list of scheduled tasks */
static taskDescriptor* taskList = NULL;
static systemTime_t System_time_milli_seconds=0;

/*FUNCTION DEFINITION *************************************************/
void scheduler_update(void) {
	//incrementing system time every millisecond
	System_time_milli_seconds++;
	//creating variable to work on the task list
	struct taskDescriptor_s *pointer = taskList;
	//looping through the list until we found the next pointer which is NULL
	//this is the case if the list is empty or the end of the list is reached
	while (pointer != NULL) {
		//check for the expire time of the task. if the task hasn't been expired yet, the expire parameter
		// is decreased by 1
		if (pointer->expire != 0) {
			pointer->expire--;
		}
		if (pointer->expire == 0) {
			// if expire is at zero, then we set the expire time to the period time
			// this will reset the execution back to its initial value
			pointer->expire = pointer->period;
			// mark list element for execution
			pointer->execute = true;
		}
		//set the address of the pointer to point to the next list element
		pointer = pointer->next;
	}
}

void scheduler_init() {
	//assigning the scheduler update to type PTimerCallback and setting its value to the address of scheduler_update
	pTimerCallback scheduler_update_2 = &scheduler_update;
	//send timer2_setCallback a parameter of address to update function every milli second
	timer2_setCallback(scheduler_update_2);
	//initialize of timer 2 to trigger ISR
	timer2_start();
}

void scheduler_run() {
	//creating variable to work on the task list
	//pointer will be used to walk through the hole list
	struct taskDescriptor_s *pointer = taskList;
	while (1) {
		while (pointer != NULL) {
			//check if the current task is to be executed once or periodically

			//first check if the task itself has a flag to be executed AND it has a period over 0 for execution
			//which are both the characteristics of the periodically tasks
			if (pointer->execute == true && pointer->period > 0) {
				// pass the parameters to the pointer function task to be executed
				//fprintf(lcdout, "V to par = %d\n", pointer->param);
				pointer->task(pointer->param);
				//clear the execution flag after it has been passed to task
				pointer->execute = 0;
			}
			// for a non-periodic functions
			if (pointer->period == 0 && pointer->execute == true) {
				// pass the parameters to the pointer function task to be executed
				pointer->task(pointer->param);
				//remove the current task from the schedule of execution
				scheduler_remove(pointer);
			}
			//set the address of the pointer to point to the next list element
			pointer = pointer->next;
		}
		pointer = taskList;
	}
}

bool scheduler_add(taskDescriptor * toAdd) {
	//stop interrupts while a task is added to the list to prevent unexpected behaviour
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		//declaring 2 pointers of type taskDescriptor to be used in adding new list elements
		taskDescriptor* currentNode = taskList;
		taskDescriptor* previousNode = taskList;
		// in case the tasks list is empty then the toAdd is assigned to be task list which acts as the head in the list
		//adding a new node at the beginning of the list (in case the list is empty)
		if (taskList == NULL) {
			// assign the value of toAdd to taskList
			taskList = toAdd;
			// because taskList is the first node in the list so the next node is assigned to NULL
			taskList->next = NULL;
		}
		// now checking if there is an existing node in the list and we want to add another list element.
		// we check if the current list element is empty so we can assign it to values later on
		else {
			while (currentNode != NULL) {
				//set the values of previousNode to currentNode
				previousNode = currentNode;
				//once we copied the value of currentNode to previousNode, now we can assign the currentNode to another address
				//which is by default is equal to NULL, waiting for another node to be added
				currentNode = currentNode->next;
			}
			//now for the now that we want to add, all we need to do is set the previousNode->next node address to the toAdd node, thus
			//completing a chain of connecting nodes
			previousNode->next = toAdd;
			//previousNode = previousNode->next;
			previousNode->next->next = NULL;
			return true;
		}
	}
	//return success
	return true;
}

void scheduler_remove(taskDescriptor * toRemove) {
	//pointer variable of type taskDescriptor to work on the task list
	struct taskDescriptor_s *current_pointer = taskList;
	struct taskDescriptor_s *prev_pointer = taskList;

	//while loop to go through the list element until the proper element is found
	while (current_pointer != NULL) {
		// for the function to remove, we have to think about 2 scenarios
		//1. Task to be removed is the first element in the list
		if ((current_pointer == toRemove) && (current_pointer == taskList)) {
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
			{
				//assigning next element as beginning to tasklisf
				//if it is the only element in list, NULL will be assigned
				//if the list has more then one element, then the next will be the new beginning
				taskList = current_pointer->next;
			}
			//leave function, because everything we came for is done honey
			return;
		}

		//2.the taskList has few nodes inside and we need to search for a specific node

		else if (current_pointer == toRemove && current_pointer != taskList) {
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
			{
				//linking the previous node to the next node, so the searched node will be out of the list
				prev_pointer->next = current_pointer->next;
			}
			//leave function, because everything we came for is done honey
			return;
		} else if (current_pointer == toRemove && current_pointer->next == NULL) {
			prev_pointer->next = NULL;
		} else {
			//fprintf(uartout, "Task not found, what the hack went wrong\n");
		}
		//shifting both pointer by one through the list, so we always know our previous list element
		//and we can search for the proper element to remove from the task list
		prev_pointer = current_pointer;
		current_pointer = current_pointer->next;
	}
}

void scheduler_setTime(systemTime_t time){
	System_time_milli_seconds=time;
}

systemTime_t scheduler_getTime(){
	return System_time_milli_seconds;
}
