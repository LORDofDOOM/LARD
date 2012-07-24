///////////////////////////// timer.c //////////////////////////////////
//
//  
//
//
//

#include "LARD.h"

LPC_CTxxBx_Type * timers[] = {
		LPC_CT16B0,
		LPC_CT16B1,
		LPC_CT32B0,
		LPC_CT32B1
};

hwTimer * hwTimerCreate (uint8 tmr, uint32 reload_val, uint8 mode,
		void (*callback_func)(struct _hwTimer *)) {

	hwTimer * t;

	t = (void*)safeMalloc(sizeof (hwTimer));

	if (t == NULL) {
		SYS_ERROR (ERR_MALLOC_FAILED);
		return (hwTimer *)ERROR;
	}

	t->object_id		= OBJID_HWTIMER;
	t->not_object_id	= ~OBJID_HWTIMER;
	t->timer 			= timers[tmr];
	t->mode 			= mode;
	t->callback_func	= callback_func;

	/////////////////////////////////////////////////////////////////
	//
	// Disable and reset the timer.
	//
	t->timer->TCR = 2;

	/////////////////////////////////////////////////////////////////
	//
	// Set max value the timer can have
	//
	t->max_val = (tmr < 2) ? 0xFFFF : 0xFFFFFFFF;

	/////////////////////////////////////////////////////////////////
	//
	// Setup reload value
	//
	if (reload_val > t->max_val) {
		SYS_ERROR (ERR_HWTIMER_BAD_RELOAD_VAL);
		reload_val &= 0xFFFF0000;
	}

	/////////////////////////////////////////////////////////////////
	//
	// Enable the appropriate AHB clock, the bits in SYSAHBCLKCTRL
	// are 7, 8, 9 and 10 for the four timers, so we use the tmr var
	// added to the first bit position.
	//
	LPC_SYSCON->SYSAHBCLKCTRL |= (0x1 << (7 + tmr));

	/////////////////////////////////////////////////////////////////
	//
	// Set the interrupt enable for MRxINT.
	//
	// NOTE: In hwTimer mode we only use a single match (MR0) register
	// and interrupt.
	//
	t->timer->IR = 1;

	/////////////////////////////////////////////////////////////////
	//
	// Reset the counter and set the match register action to
	// generate an interrupt and restart or stop according to mode.
	//
	if ((mode != HWTIMER_MODE_ASTABLE) && (mode != HWTIMER_MODE_MONOSTABLE)) {
		SYS_ERROR (ERR_HWTIMER_BAD_MODE);
		mode &= HWTIMER_MODE_ASTABLE;
	}
	t->timer->MCR =	(1 << 0) | 	// generate interrupt
					(1 << 1) |	// reset counter
					mode;		// stop if 4, restart if 0

	/////////////////////////////////////////////////////////////////
	//
	// Setup the prescaler value
	//
	// NOTE: Just set to 1 for the time being. There should be another
	// parameter (resolution?) or way of determining the max time the
	// counter can run for.
	//
	t->timer->PC = 1;

	/////////////////////////////////////////////////////////////////
	//
	// Setup the prescaler value
	//
	t->reload_val 	= reload_val;
	t->timer->MR0	= reload_val;

	/////////////////////////////////////////////////////////////////
	//
	// Ensure timer mode
	//
	t->timer->CTCR 	= 0;	// timer
	t->timer->PWMC	= 0;	// no PWM ???????

	return t;

}

uint32 hwTimerAttachCallback (hwTimer * t, void (*callback_func)(struct _hwTimer *)) {

	VERIFY_STRUCTURE(t);

	t->callback_func = callback_func;

}

uint32	hwTimerSetReLoadVal (hwTimer * t, uint32 reload_val) {

	VERIFY_STRUCTURE(t);

	if (reload_val > t->max_val) {
		SYS_ERROR (ERR_HWTIMER_BAD_RELOAD_VAL);
		reload_val &= t->max_val;
	}

	t->timer->MR0	= t->reload_val;

}

uint32	hwTimerStart (hwTimer * t) {

	VERIFY_STRUCTURE(t);

	t->timer->MR0	= t->reload_val;
	t->timer->TCR 	= 1;

}

uint32	hwTimerRestart (hwTimer * t) {

	VERIFY_STRUCTURE(t);

	t->timer->TCR 	= 1;

}

uint32	hwTimerStop (hwTimer * t) {

	VERIFY_STRUCTURE(t);

	t->timer->TCR 	= 0;

}
