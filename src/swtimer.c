////////////////////////////// swtimer.c ////////////////////////////
//
// A collection of functions that implement software timers with a 1mS
// tick resolution.
//
//
//
//
//

#include "LARD.h"


uint32 timer_flags = 0;
swTimer * swTimers [32];

/////////////////////////////////////////////////////////////////////
//
// Function name:		swTimerScan
//
// Description:			Scan all timers decrementing their counter,
//						If the counter reaches 0 reload the counter if
//						the timer is an astable type.
//
// Parameters:			none
//
// Returned value:		> 0 if any timers timed out, 0 if not
//
// Errors raised:		none
//
// Example:				if (swTimerScan())
//							swTimerExecuteCallbacks();
//
// Notes:				Sets a bit in timer_flags for each
//						timer that times out.
//
uint32 swTimerScan (void) {

	uint32	flag_mask = 1;
	swTimer	* t;
	boolean	flag = FALSE;

	timer_flags = 0;

	for (int i = 0; swTimers[i] != NULL; i++) {
		t = swTimers[i];
		if (t->enabled) {
			t->count--;
			if (t->count == 0) {
				if (t->type == SWTIMER_TYPE_ASTABLE)
					t->count = t->reload_value;
				else
					t->enabled = FALSE;
				flag = TRUE;
				timer_flags |= flag_mask;
			}
		}
		flag_mask <<= 1;
	}
	return (flag);
}

/////////////////////////////////////////////////////////////////////
//
// Function name:		swTimerExecuteCallbacks
//
// Description:			Scan the timer_flags variable looking for
//						set bits. When found call the appropriate
//						timer's callback function.
//
// Parameters:			none
//
// Returned value:		none
//
// Errors raised:		none
//
// Example:				if (swTimerScan())
//							swTimerExecuteCallbacks();
//
// Notes:				timer_flags cleared by this function.
//						swTimerExecuteCallbacks called automatically every mS
//							by the main() function.
//
void swTimerExecuteCallbacks (void) {

	uint32 flags = timer_flags;
	swTimer	* t;

	if (flags == 0)
		return;

	for (int i = 0; swTimers[i] != NULL; i++) {
		t = swTimers[i];
		if ((flags & 1) && ((t->callback_func) != 0) ) {
			(t->callback_func)(t);
		}
		flags >>= 1;
	}
	timer_flags = 0;

}

/////////////////////////////////////////////////////////////////////
//
// Function name:		swTimerAttachCallback
//
// Description:			Set the callback function for a timer.
//
// Parameters:			swTimer * t, pointer to the timer number
//						void (*callback_func)(void), pointer to the timer's
//							callback function
//
// Returned value:		Hardcoded to NOERROR at present
//
// Errors raised:		none
//
// Example:				myFunc () {
// 							// do this when timer 1 times out
//						}
//						...
//						swTimerAttachCallback (1, myFunc);
//
// Notes:
//
uint32 swTimerAttachCallback (swTimer * t, void (*callback_func)(swTimer *)) {

	t->callback_func = callback_func;

	return NOERROR;
}

/////////////////////////////////////////////////////////////////////
//
// Function name:		swTimerSetReLoadVal
//
// Description:			Load a timer's reload value
//
// Parameters:			swTimer * t, pointer to the timer number
//						uint32 reload_val, the timer's reload_value in mS
//
// Returned value:		Hardcoded to NOERROR at present
//
// Errors raised:		none
//
// Example:
//
// Notes:				This will have no effect on the timer's
//						current count. The loaded value will only
//						come into play when the timer next times out.
//
uint32 swTimerSetReLoadVal (swTimer * t, uint32 reload_val) {

	t->reload_value = reload_val;

	return NOERROR;
}

/////////////////////////////////////////////////////////////////////
//
// Function name:		swTimerReload
//
// Description:			Forces the timer's counter to the reload value.
//
// Parameters:			swTimer * t, pointer to the timer number
//
// Returned value:		Hardcoded to NOERROR at present
//
// Errors raised:		none
//
// Example:
//
// Notes:				If the timer is enabled the next scan will
//						count from the reload value. The timer's
//						enabled status is not modified.
//
uint32 swTimerReload (swTimer * t) {

	t->count = t->reload_value;

	return NOERROR;
}

/////////////////////////////////////////////////////////////////////
//
// Function name:		swTimerStart
//
// Description:			Enables a timer, counting will resume
//						from the reload value.
//
// Parameters:			swTimer * t, pointer to the timer number
//
// Returned value:		Hardcoded to NOERROR at present
//
// Errors raised:		none
//
// Example:
//
// Notes:
//
uint32 swTimerStart (swTimer * t) {

	t->count = t->reload_value;
	t->enabled = true;

	return NOERROR;
}

/////////////////////////////////////////////////////////////////////
//
// Function name:		swTimerRestart
//
// Description:			Enables a timer, counting will resume
//						from the current count value.
//
// Parameters:			swTimer * t, pointer to the timer number
//
// Returned value:		Hardcoded to NOERROR at present
//
// Errors raised:		none
//
// Example:
//
// Notes:
//
uint32 swTimerRestart (swTimer * t) {

	t->enabled = true;

	return NOERROR;
}

/////////////////////////////////////////////////////////////////////
//
// Function name:		swTimerStop
//
// Description:			Disables a timer so it's counter will not
//						be decremented.
//
// Parameters:			swTimer * t, pointer to the timer number
//
// Returned value:		Hardcoded to NOERROR at present
//
// Errors raised:		none
//
// Example:
//
// Notes:				No timer values are modified, the timer
//						can be restarted and continue from where it was
//						stopped.
//
uint32 swTimerStop (swTimer * t) {

	t->enabled = false;

	return NOERROR;
}

/////////////////////////////////////////////////////////////////////
//
// Function name:		swTimerCreate
//
// Description:			Create and populate a new swTimer.
//
// Parameters:			uint32 reload_val, the timers count value in mS
//						uint32 type,
//							SWTIMER_TYPE_MONOSTABLE if the timer should stop
//								on timeout
//							SWTIMER_TYPE_ASTABLE if the timer should be
//								reloaded and continue counting on timeout
//						void (*callback_func)(void), pointer to the callback function
//
// Returned value:		ERROR if an error was encountered,
//						Pointer to the new swTimer if not
//
// Errors raised:		ERR_MALLOC_FAILED if the safeMalloc failed.
//
// Example:				void myTimerFunc (swTimer * t) {
//							// do something when timer expires
//						}
//
//						swTimer * myTimer;
//						myTimer = swTimerCreate (1000, SWTIMER_TYPE_ASTABLE, myTimerFunc);
//
//						myTimer will run every second with no further intervention
//
// Notes:				Can only be called from within the setup() function.
//						At any other time safeMalloc will fail and raise an error
//
swTimer * swTimerCreate (uint32 reload_val, uint32 type, void (*callback_func)(swTimer *)) {

	swTimer * t;

	t = (void*)safeMalloc(sizeof (swTimer));

	if (t == NULL) {
		SYS_ERROR (ERR_MALLOC_FAILED);
		return (swTimer *)ERROR;
	}

	t->reload_value  = reload_val;
	t->count 		 = reload_val;
	t->type 		 = type;
	t->callback_func = callback_func;
	t->enabled 		 = FALSE;

	__n_swTimers++;

	return t;
}


