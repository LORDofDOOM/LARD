/////////////////////////////// events.c //////////////////////////////////
//
//  
//
//
//

#include "LARD.h"

voidFuncPtr eventFunctions[EVENT_DUMMY] = {0};

const uint32 N_EVENTS = (sizeof(eventFunctions) / sizeof(eventFunctions[0]));

uint32 attachEventHandler (uint32 event, void (*userFunc)(void)) {

	if (event >= N_EVENTS) {
		SYS_ERROR(ERR_INV_INTERRUPT);
		return ERROR;
	}

	eventFunctions[event] = userFunc;
	return NOERROR;
}

uint32 detachEventHandler(uint32 event) {

	if (event >= N_EVENTS) {
		SYS_ERROR(ERR_INV_INTERRUPT);
		return ERROR;
	}

	eventFunctions[event] = NULL;
	return NOERROR;

}

//////////////////////////////////////////////////////////////////////
//
// This is the default handler for the SysTick timer, it overrides the
// weak definition provided by the LPC startup code.
//
void SysTick_Handler () {

	msTicks++;

	if (eventFunctions[EVENT_SYS_TICK] != 0)
		(eventFunctions[EVENT_SYS_TICK]) ();

}
