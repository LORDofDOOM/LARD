/////////////////////////////// pingroup.c //////////////////////////////////
//
//  
//
//
//

#include "LARD.h"

/////////////////////////////////////////////////////////////////////
//
// Function name:		pinGroupCreate
//
// Description:			Create and populate a new pinGroup structure.
//
// Parameters:			uint32 in_out, INPUT or OUTPUT
//						uint32 pin_lo, the first pin in the group
//						uint32 pin_hi, the last pin in the group
//
// Returned value:		ERROR if any of the safeMalloc calls failed.
//						Pointer to the new pinGroup if not.
//
// Errors raised:		ERR_INV_PIN if the pin number was > the max pins on the CPU
//							or pin_lo == pin_hi (pingroup cannot be a single pin).
//						ERR_DIFF_PINPORT if pinlo and pin_hi are on different physical
//							ports.
//						ERR_MALLOC_FAILED if the safeMalloc call failed.
//
// Notes:				pin_lo and pin_hi can be in any order, the function tests
//						their relative values and swaps them if necessary.
//
//						Can only be called from setup() because safeMalloc() is used.
//
// Example:				pinGroup * pg;
//						pg = pinGroupCreate (OUTPUT, 5, 10);
//
//						Defines the 6 pins (5 thru 10 inclusive) as a pinGroup
//
pinGroup * pinGroupCreate(uint32 in_out, uint32 pin_lo, uint32 pin_hi) {

	if (pin_hi == pin_lo) {
		SYS_ERROR (ERR_INV_PIN);
		return ERROR;
	}

	if (pin_hi < pin_lo) {
		uint32 x = pin_lo;
		pin_lo = pin_hi;
		pin_hi = x;
	}

	if (pin_hi >= nPins()) {
		SYS_ERROR (ERR_INV_PIN);
		return ERROR;
	}

	if (pinPort (pin_lo) != pinPort (pin_hi)) {
		SYS_ERROR (ERR_DIFF_PINPORT);
		return ERROR;
	}

	pinGroup * pg;

	pg = (void*)safeMalloc(sizeof (pinGroup));

	if (pg == 0) {
		SYS_ERROR (ERR_MALLOC_FAILED);
		return ERROR;
	}

	pg->pin_lo = pin_lo;
	pg->pin_hi = pin_hi;
	pg->port = pinPort(pin_lo);
	pg->port_address = (pinPort(pin_lo) == 0) ? LPC_GPIO0 : LPC_GPIO1;
	pg->nBits = pin_hi - pin_lo + 1;
	pg->shift_bits = pin_lo - (32 / pg->port);

	/////////////////////////////////////////////////////////////////
	// Create various masks for the pinGroup.
	// Comments below using the 5-10 example.
	//
	pg->set_mask = 0;
	for (int i = 0 ; i < pg->nBits; i++) {
		pg->set_mask <<= 1;
		pg->set_mask |= 1;
	}
	// set_mask now == 	00000000 00000000 00000000 00111111

	pg->max_val = pg->set_mask;
	// max_val now  == 	00000000 00000000 00000000 00111111

	pg->msb_mask = (pg->set_mask + 1) >> 1;
	// msb_mask now == 	00000000 00000000 00000000 00100000

	pg->set_mask <<= pg->shift_bits;
	// set_mask now == 	00000000 00000000 00000111 11100000

	pg->clear_mask = ~(pg->set_mask);
	// clear_mask now ==11111111 11111111 11111000 00111111

	pg->val = 0;

	for (int i = pin_lo ; i <= pin_hi; i++) {
		(in_out == INPUT) ? pinSetAsDigitalInput(i) : pinSetAsDigitalOutput(i);
	}

	return pg;
}

/////////////////////////////////////////////////////////////////////
//
// Function name:		pinGroupWrite
//
// Description:			Write a value to a pinGroup.
//
// Parameters:			pinGroup * pg, pointer to the pinGroup
//						uint32 val, the value to write
//
// Returned value:		NOERROR
//
// Errors raised:		none
//
// Notes:
//
// Example:
//
uint32 pinGroupWrite(pinGroup * pg, uint32 val) {

	val &= pg->max_val;

	ATOMIC_START
		pg->val = val & pg->max_val;
		uint32 mask = pg->port_address->MASK;
		pg->port_address->MASK = pg->clear_mask;
		pg->port_address->OUT = val << pg->shift_bits;
		pg->port_address->MASK = mask;
	ATOMIC_END

	return NOERROR;
}

uint32 pinGroupRead(pinGroup * pg) {

	uint32 val;

	val = pg->port_address->PIN;
	val &= pg->set_mask;
	val >>= pg->shift_bits;
	pg->val = val;

	return val;
}

uint32 pinGroupInc(pinGroup * pg) {

	uint32 val = pg->val;
	val++;
	pinGroupWrite (pg, val);

	return NOERROR;
}

uint32 pinGroupDec(pinGroup * pg) {

	uint32 val = pg->val;
	val--;
	pinGroupWrite (pg, val);

	return NOERROR;
}

uint32 pinGroupClear(pinGroup * pg) {

	uint32 val = pg->val;
	val &= pg->clear_mask;
	pinGroupWrite (pg, val);

	return NOERROR;
}

uint32 pinGroupSet(pinGroup * pg) {

	uint32 val = pg->val;
	val &= pg->set_mask;
	pinGroupWrite (pg, val);

	return NOERROR;
}

uint32 pinGroupShiftLeft(pinGroup * pg, uint32 bits) {

	uint32 val = pg->val;
	val <<= bits;
	pinGroupWrite (pg, val);

	return NOERROR;
}

uint32 pinGroupShiftRight(pinGroup * pg, uint32 bits) {

	uint32 val = pg->val;
	val >>= bits;
	pinGroupWrite (pg, val);

	return NOERROR;
}

uint32 pinGroupRotateLeft(pinGroup * pg) {

	uint32 val = pg->val;

	uint8 carry = (val & pg->msb_mask) >> (pg->nBits -1);
	val <<= 1;

	pinGroupWrite (pg, val | carry);

	return NOERROR;
}

uint32 pinGroupRotateRight(pinGroup * pg) {

	uint32 val = pg->val;
	uint8 carry = val & 1;
	val >>= 1;

	pinGroupWrite (pg, val | (carry << (pg->nBits -1)));

	return NOERROR;
}


uint32 pinGroupInvert	(pinGroup * pg) {

	uint32 val = pg->val;
	val = ~val;
	pinGroupWrite (pg, val);

	return NOERROR;
}

uint32 pinGroupOR(pinGroup * pg, uint32 or_val) {

	uint32 val = pg->val;
	val |= or_val;
	pinGroupWrite (pg, val);

	return NOERROR;
}

uint32 pinGroupAND(pinGroup * pg, uint32 and_val) {

	uint32 val = pg->val;
	val |= and_val;
	pinGroupWrite (pg, val);

	return NOERROR;
}

uint32 pinGroupXOR(pinGroup * pg, uint32 xor_val) {

	uint32 val = pg->val;
	val ^= xor_val;
	pinGroupWrite (pg, val);

	return NOERROR;
}

