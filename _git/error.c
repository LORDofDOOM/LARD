//////////////////////////// sys_error.c ////////////////////////////////
//
//  
//
//
//

#include "LARD.h"

uint32 sysErrors [10];
uint32 *sysErrorPtr = sysErrors;
uint32 nSysErrors = 0;

jmp_buf syserr_env;

void	sysErrorHandler (void) {

	if (eventFunctions[EVENT_SYS_ERROR] != NULL) {
		(eventFunctions[EVENT_SYS_ERROR]) ();
	}

}

const uint32 MAX_SYS_ERRORS = (sizeof(sysErrors) / sizeof(sysErrors[0]));

////////////////////////////////////////////////////////////////
//
// Function name:		sysErrInit
//
// Description:
//
// Parameters:
//
// Returned value:
//
// Errors raised:
//
// Example:
//
// Notes:
//
void sysErrInit () {
	sysErrorPtr = sysErrors;
	nSysErrors = 0;
}

////////////////////////////////////////////////////////////////
//
// Function name:		sysErrGetCount
//
// Description:
//
// Parameters:
//
// Returned value:
//
// Errors raised:
//
// Example:
//
// Notes:
//
uint32 sysErrGetCount () {

	return nSysErrors;

}

////////////////////////////////////////////////////////////////
//
// Function name:		sysErrPeek
//
// Description:
//
// Parameters:
//
// Returned value:
//
// Errors raised:
//
// Example:
//
// Notes:
//
uint32 sysErrPeek () {

	if (nSysErrors == 0) return (ERR_NONE);

	return (*(sysErrorPtr-1));
}

////////////////////////////////////////////////////////////////
//
// Function name:		sysErrRead
//
// Description:
//
// Parameters:
//
// Returned value:
//
// Errors raised:
//
// Example:
//
// Notes:
//
uint32 sysErrRead () {

	if (nSysErrors == 0) return (ERR_NONE);

	sysErrorPtr--;
	return (*(sysErrorPtr));
}

////////////////////////////////////////////////////////////////
//
// Function name:		sysErrRaise
//
// Description:
//
// Parameters:
//
// Returned value:
//
// Errors raised:
//
// Example:
//
// Notes:
//
void sysErrRaise (uint32 err) {

	if (nSysErrors >= MAX_SYS_ERRORS) {
//		longjmp(syserr_env, err);
//		FATAL (ERR_TOO_MANY);
		while(1)
			sysErrDump();
	}

	*sysErrorPtr++ = err;
	nSysErrors++;
	__last_syserr = err;

	sysErrorHandler();

}

////////////////////////////////////////////////////////////////
//
// Function name:		sysErrDump
//
// Description:
//
// Parameters:
//
// Returned value:
//
// Errors raised:
//
// Example:
//
// Notes:
//
void sysErrDump () {

	uint32 * errPtr = sysErrorPtr -1;

	for (int i = 0; errPtr >= sysErrors; i++) {
		debugPrintVar (*errPtr--);
	}

}
