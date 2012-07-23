///////////////////////////// events.c //////////////////////////////////
//
//  
//
//
//

#include "LARD.h"

voidFuncPtr eventFunctions[EVENT_DUMMY] = {0};

const uint32 N_EVENTS = (sizeof(eventFunctions) / sizeof(eventFunctions[0]));

void attachEventHandler (uint32 event, void (*userFunc)(void)) {

	if (event >= N_EVENTS) {
		SYS_ERROR(ERR_INV_INTERRUPT);
		return;
	}

	eventFunctions[event] = userFunc;
}

void detachEventHandler(uint32 event) {

	if (event >= N_EVENTS) {
		SYS_ERROR(ERR_INV_INTERRUPT);
		return;
	}

	eventFunctions[event] = 0;

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
