///////////////////////////// resources.c /////////////////////////////////
//
//  
//
//
//

#include "LARD.h"

byte resources[(N_RESOURCES / 16) + 1];

uint32 _getResourceVal (uint32 r);
uint32 _setResourceVal (uint32 r, uint32 val);

uint32 resourceMask (uint32 r) {

	if (r >= N_RESOURCES) {
		SYS_ERROR(ERR_BAD_RESOURCE | r);
		return ERROR;
	}

	if (_getResourceVal(r) != RESOURCE_USED) {
		SYS_ERROR(ERR_RESOURCE_CLASH | r);
		return ERROR;
	}

	return _setResourceVal (r, RESOURCE_MASKED);

}

uint32 resourceUnmask (uint32 r) {

	if (r >= N_RESOURCES) {
		SYS_ERROR(ERR_BAD_RESOURCE | r);
		return ERROR;
	}

#if 0
	if (_getResourceVal(r) != RESOURCE_MASKED) {
		SYS_ERROR(ERR_RESOURCE_CLASH | r);
		return ERROR;
	}
#endif

	return _setResourceVal (r, RESOURCE_USED);

}

#if 1
uint32 resourceRecordUsage (uint32 r) {

	uint32 current_resource_val;

	if (r >= N_RESOURCES) {
		ASSERT (r >= N_RESOURCES, ERR_BAD_RESOURCE | r);
		SYS_ERROR(ERR_BAD_RESOURCE | r);
		return ERROR;
	}

//	_getResourceVal (32);
//	_setResourceVal (32, RESOURCE_MASKED);
//	_incResourceVal (32);

	current_resource_val = _getResourceVal (r);

	///////////////////////////////////////////////////////////////
	//
	// If resource has been masked take no further action
	//
	if (current_resource_val == RESOURCE_MASKED) {
		return NOERROR;
	}

	///////////////////////////////////////////////////////////////
	//
	// If resource is already in use, raise an error
	// and mark the resource as having a clash.
	//
	if (current_resource_val == RESOURCE_USED) {
		ASSERT (FORCE, ERR_RESOURCE_CLASH | r);
		_setResourceVal (r, RESOURCE_CLASH);
//		SYS_ERROR(ERR_RESOURCE_CLASH | r);
		return ERROR;
	}

	///////////////////////////////////////////////////////////////
	//
	// Set the usage value to indicate the resource is used
	//
	_setResourceVal (r, RESOURCE_USED);

	return NOERROR;
}
#else
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
	ATOMIC (
		if (resources[r] < 0x7F) {
			resources[r]++;
			resources[r] &= 0x7F;
		}
	)
	return retval;
}
#endif
uint32 resourceCheckForClashes (void) {

	for (int i = 0; i < N_RESOURCES -1; i++) {
		if (_getResourceVal (i) == 2) {
			SYS_ERROR(ERR_RESOURCE_CLASH | i);
			return ERROR;
		}
	}
	return NOERROR;

}

uint32 _getResourceVal (uint32 r) {

		uint32 word = r / 16;
		uint32 offset = (r % 16) << 1;

		return (resources[word] >> offset) & 3;
}

uint32 _setResourceVal (uint32 r, uint32 val) {

		uint32 word 	= r / 16;
		uint32 offset 	= (r % 16) << 1;
		uint32 mask 	= 3 << offset;

		resources[word] &= ~mask;
		resources[word] |= (val & 3) << offset;

		return NOERROR;
}

#if 0
uint32 _incResourceVal (uint32 r) {

	uint32 current_val = _getResourceVal(r);

	if (current_val != RESOURCE_UNUSED) {
		SYS_ERROR(ERR_RESOURCE_CLASH | r);
		_setResourceVal(r, RESOURCE_CLASH);
		return ERROR;
	}

	return (_setResourceVal(r, _getResourceVal(r)+1));

}
#endif
