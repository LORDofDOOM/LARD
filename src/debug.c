////////////////////////// debug.c //////////////////////////////////
//
//  
//
//
//

#include "LARD.h"

////////////////////////////////////////////////////////////////
//
// Function name:		swTimerScan
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
void debugPrintVar (uint32 var) {

	shiftOut32 (DBG_DATA_PIN, DBG_CLK_PIN, LSBFIRST, var);

}
