///////////////////////////// memory.c ///////////////////////////////
//
//  
//
//
//

#include "LARD.h"
extern boolean system_initialising;

uint32 __allocated_ram = 0;


////////////////////////////////////////////////////////////////
//
// Function name:		safeMalloc
//
// Description:			Allocate memory but only if called from
//						within the setup() function.
//
// Parameters:			none
//
// Returned value:		NULL or a pointer to the allocated memory
//
// Errors raised:		ERR_BAD_MALLOC | 1, if not called from within setup()
//						ERR_BAD_MALLOC | 2, if the malloc() call failed
//
// Example:
//
// Notes:				This function needs to use malloc() but nobody
//						else should so we undef it for a moment then
//						redef it to produce an obvious error at compile time
//
void * safeMalloc (size_t size) {

	void * x = 0;

#undef malloc

	if (__system_initialising) {
		x = malloc (size);
		__allocated_ram += size;
	} else
		SYS_ERROR(ERR_BAD_MALLOC | 1);

#define malloc  malloc_not_allowed_use_safemalloc

	if (x == NULL)
		SYS_ERROR(ERR_BAD_MALLOC | 2);
	return x;
}
