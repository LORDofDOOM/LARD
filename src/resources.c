///////////////////////////// resources.c /////////////////////////////////
//
//  
//
//
//

#include "LARD.h"

byte resources[N_RESOURCES];

uint32 resourceMask (uint32 r) {

	uint32 retval = NOERROR;

	if (r >= N_RESOURCES) {
		SYS_ERROR(ERR_BAD_RESOURCE | r);
		retval = ERROR;
	}
	resources[r] |= 0x80;

	return retval;
}

uint32 resourceUnmask (uint32 r) {

	uint32 retval = NOERROR;

	if (r >= N_RESOURCES) {
		SYS_ERROR(ERR_BAD_RESOURCE | r);
		retval = ERROR;
	}
	resources[r] &= 0x7F;

	return retval;
}

uint32 resourceRecordUsage (uint32 r) {

	uint32 retval = NOERROR;

	if (r >= N_RESOURCES) {
		SYS_ERROR(ERR_BAD_RESOURCE | r);
		retval = ERROR;
	}

	///////////////////////////////////////////////////////////////
	//
	// If resource has been masked take no further action
	//
	if (resources[r] & 0x80) {
		return retval;
	}

	///////////////////////////////////////////////////////////////
	//
	// If resource has already been used raise an error
	//
	if (resources[r]) {
		SYS_ERROR(ERR_RESOURCE_CLASH | r);
	}

	///////////////////////////////////////////////////////////////
	//
	// Increment the usage counter if not already maxed out
	//
	if (resources[r] < 0x7F) {
		resources[r]++;
		resources[r] &= 0x7F;
	}

	return retval;
}

uint32 resourceCheckForClashes (void) {

	uint32 retval = NOERROR;

	for (int i = 0; i < N_RESOURCES -1; i++) {
		if (resources[i] > 2) {
			SYS_ERROR(ERR_RESOURCE_CLASH | i);
			retval = ERROR;
		}
	}
	return retval;

}
