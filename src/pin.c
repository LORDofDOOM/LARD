//////////////////////////////// pins.c /////////////////////////////
//
// A set of functions to implement th ehamdling of logical pin on
// the LPC1227 (48-pin version).
//
// Only the 48-pin version of the chip is supported at this time,
// however support for the 64-pin version would be easy to add.
//
// Logical pins the allocated in pin andport order, ie pin0 is
// PO:O, pin20 is P0:20, pin30 is P1:9. There is no provision
// for quasi-random pin allocations as per the Arduino.

#include "LARD.h"

/////////////////////////////////////////////////////////////////////
//
// Addresses of all 39 IOCON registers, one for each pin
//
__IO uint32_t * pin_regs [] = {
		// Port 0
		&LPC_IOCON->PIO0_0,			// pin 0
		&LPC_IOCON->PIO0_1,			// pin 1
		&LPC_IOCON->PIO0_2,			// pin 2
		&LPC_IOCON->PIO0_3,			// pin 3
		&LPC_IOCON->PIO0_4,			// pin 4
		&LPC_IOCON->PIO0_5,			// pin 5
		&LPC_IOCON->PIO0_6,			// pin 6
		&LPC_IOCON->PIO0_7,			// pin 7
		&LPC_IOCON->PIO0_8,			// pin 8
		&LPC_IOCON->PIO0_9,			// pin 9
		&LPC_IOCON->PIO0_10,		// pin 10
		&LPC_IOCON->PIO0_11,		// pin 11
		&LPC_IOCON->PIO0_12,		// pin 12
		&LPC_IOCON->RESET_PIO0_13,	// pin 13
		&LPC_IOCON->PIO0_14,		// pin 14
		&LPC_IOCON->PIO0_15,		// pin 15
		&LPC_IOCON->PIO0_16,		// pin 16
		&LPC_IOCON->PIO0_17,		// pin 17
		&LPC_IOCON->PIO0_18,		// pin 18
		&LPC_IOCON->PIO0_19,		// pin 19
		&LPC_IOCON->PIO0_20,		// pin 20
		&LPC_IOCON->PIO0_21,		// pin 21
		&LPC_IOCON->PIO0_22,		// pin 22
		&LPC_IOCON->PIO0_23,		// pin 23
		&LPC_IOCON->PIO0_24,		// pin 24
		&LPC_IOCON->SWDIO_PIO0_25,	// pin 25
		&LPC_IOCON->SWCLK_PIO0_26,	// pin 26
		&LPC_IOCON->PIO0_27,		// pin 27
		&LPC_IOCON->PIO0_28,		// pin 28
		&LPC_IOCON->PIO0_29,		// pin 29
		&LPC_IOCON->R_PIO0_30,		// pin 30
		&LPC_IOCON->R_PIO0_31,		// pin 31

		// Port 1
		&LPC_IOCON->R_PIO1_0,		// pin 32
		&LPC_IOCON->R_PIO1_1,		// pin 33
		&LPC_IOCON->PIO1_2,			// pin 34
		&LPC_IOCON->PIO1_3,			// pin 35
		&LPC_IOCON->PIO1_4,			// pin 36
		&LPC_IOCON->PIO1_5,			// pin 37
		&LPC_IOCON->PIO1_6			// pin 38
};

/////////////////////////////////////////////////////////////////////
//
// Array of pointers to the 14 special GPIO registers.
//
// Most of the regs are read/write but PIN, RIS and MIS are read only
// and defined as __I so they have been cast to __IO to keep the
// compiler quite.
//
__IO uint32_t * gpio_regs [14][2] = {
		{&LPC_GPIO0->MASK,	&LPC_GPIO1->MASK},
		{(__IO uint32_t *)	&LPC_GPIO0->PIN,
		 (__IO uint32_t *)	&LPC_GPIO1->PIN},
		{&LPC_GPIO0->OUT,	&LPC_GPIO1->OUT},
		{&LPC_GPIO0->SET,	&LPC_GPIO1->SET},
		{&LPC_GPIO0->CLR,	&LPC_GPIO1->CLR},
		{&LPC_GPIO0->NOT,	&LPC_GPIO1->NOT},
		{&LPC_GPIO0->DIR,	&LPC_GPIO1->DIR},
		{&LPC_GPIO0->IS, 	&LPC_GPIO1->IS},
		{&LPC_GPIO0->IBE,	&LPC_GPIO1->IBE},
		{&LPC_GPIO0->IEV,	&LPC_GPIO1->IEV},
		{&LPC_GPIO0->IE, 	&LPC_GPIO1->IE},
		{(__IO uint32_t *)	&LPC_GPIO0->RIS,
		 (__IO uint32_t *)	&LPC_GPIO1->RIS},
		{(__IO uint32_t *)	&LPC_GPIO0->MIS,
		 (__IO uint32_t *)	&LPC_GPIO1->MIS},
		{&LPC_GPIO0->IC,	&LPC_GPIO1->IC}
};

uint32	nPins(void) {
	return MAX_PINS;
}

/////////////////////////////////////////////////////////////////////
//
// Non-exported function to set any GPIO register bit
//
uint32 _pinSetGpioBit(uint32 pin, uint32 gpio_reg) {

	if (pin >= MAX_PINS) {
		SYS_ERROR (ERR_INV_PIN);
		return ERROR;
	}

	uint32 port;

	switch (gpio_reg) {

		case GPIOREG_PIN:	// Read only registers
		case GPIOREG_MIS:
		case GPIOREG_RIS:
			return ERROR;

		default:
			port = pinPort(pin);
			*gpio_regs[gpio_reg][port] |= word_bits[pin - (32 * port)];
			return NOERROR;
	}
}

/////////////////////////////////////////////////////////////////////
//
// Non-exported function to clear any GPIO register bit
//
boolean _pinClearGpioBit(uint32 pin, uint32 gpio_reg) {

	if (pin >= MAX_PINS) {
		SYS_ERROR (ERR_INV_PIN);
		return ERROR;
	}

	uint32 port;

	switch (gpio_reg) {

		case GPIOREG_PIN:	// Read only registers
		case GPIOREG_MIS:
		case GPIOREG_RIS:
			return ERROR;

		default:
			port = pinPort(pin);
			*gpio_regs[gpio_reg][port] &= ~(word_bits[pin - (32 * port)]);
			return NOERROR;
	}
}

/////////////////////////////////////////////////////////////////////
//
// Function name:		pinXxx
//
// Description:			A series of functions to set, clear, mask etc a pin.
//
// Parameters:			uint32 pin, the logical pin number: valid values 0-38
//
// Returned value:		TRUE if no errors
//						FALSE if there are errors
//						Except pinRead which returns the logic state of the pin
///
// Errors raised:		none
//
//
uint32 pinHigh(uint32 pin) {

	return _pinSetGpioBit (pin, GPIOREG_SET);

}

uint32 pinLow(uint32 pin) {

	return _pinSetGpioBit (pin, GPIOREG_CLR);
}

uint32 pinWrite(uint32 pin, uint32 val) {
	// NOTE: Any non-zero value sets the pin
	if (val)
		return _pinSetGpioBit (pin, GPIOREG_SET);
	else
		return _pinSetGpioBit (pin, GPIOREG_CLR);

}

uint32 pinRead(uint32 pin) {

	uint32 val, port;

	port = pinPort(pin);
	val = *gpio_regs[GPIOREG_PIN][port];	// Get all 32 bits from the port
	val >>= (pin % 32);						// Shift our bit to the LS position
	val &= 1;								// Ditch any other bits

	return val;

}

uint32 pinToggle(uint32 pin) {

	return _pinSetGpioBit (pin, GPIOREG_NOT);

}

uint32 pinMask(uint32 pin) {

	return _pinSetGpioBit (pin, GPIOREG_MASK);

}

uint32 pinSetAsDigitalInput(uint32 pin) {

	uint32 retval = 0;

	retval |= _pinClearGpioBit (pin, GPIOREG_DIR);
	retval |= pinFunc (pin, FUNC_GPIO);
	retval |= pinConfig (pin, PIN_ADMODE_DISABLED);

	return (retval);

}

uint32 pinSetAsDigitalOutput(uint32 pin) {

	uint32 retval = 0;

	retval |= _pinSetGpioBit (pin, GPIOREG_DIR);
	retval |= pinFunc (pin, FUNC_GPIO);
	retval |= pinConfig (pin, PIN_ADMODE_DISABLED);

	return (retval);

}

uint32 pinSetAsAnalogInput(uint32 pin) {

	uint32 retval = 0;

	retval |= _pinClearGpioBit (pin, GPIOREG_DIR);
	retval |= pinFunc (pin, FUNC_ADC);
	retval |= pinConfig (pin, PIN_ADMODE_ENABLED | PIN_MODE_NOPULLUP);

	return (retval);

}

uint32 pinSetInterruptMode (uint32 pin, uint8 mode) {

	uint32 retval = 0;

	retval |= _pinClearGpioBit (pin, GPIOREG_IS);
	retval |= _pinClearGpioBit (pin, GPIOREG_IEV);
	retval |= _pinClearGpioBit (pin, GPIOREG_IBE);

	switch (mode) {
		case PININT_HIGH_LEVEL:
			retval |= _pinSetGpioBit (pin, GPIOREG_IS);
			retval |= _pinSetGpioBit (pin, GPIOREG_IEV);
			break;

		case PININT_LOW_LEVEL:
			retval |= _pinSetGpioBit (pin, GPIOREG_IS);
			break;

		case PININT_RISING_EDGE:
			retval |= _pinSetGpioBit (pin, GPIOREG_IEV);
			break;

		case PININT_FALLING_EDGE:
			break;

		case PININT_BOTH_EDGES:
			retval |= _pinSetGpioBit (pin, GPIOREG_IBE);
			break;
	}

	return (retval);

}

uint32 pinEnableInterrupt (uint32 pin) {

	uint32 retval = 0;

	retval |= _pinSetGpioBit (pin, GPIOREG_IC); // clear any current interrupt
	retval |= _pinSetGpioBit (pin, GPIOREG_IE); // then enable

	return (retval);

}

uint32 pinDisableInterrupt (uint32 pin) {

	uint32 retval = 0;

	retval |= _pinClearGpioBit (pin, GPIOREG_IE); // disable interrupt
	retval |= _pinSetGpioBit (pin, GPIOREG_IC);   // clear any pending interrupts

	return (retval);

}

uint32 pinClearInterrupt (uint32 pin) {

	return _pinSetGpioBit (pin, GPIOREG_IC);

}

/////////////////////////////////////////////////////////////////////
//
// Function name:		pinConfig
//
// Description:			Set the mode of a pin, the "mode" determine features such
//						as pullup resistors, polarity, open drain etc.
//
// Parameters:			uint32 pin: The logical pin number, valid values are 0-38
//						uint32 mode: A bit pattern appropriate for setting the
//							bits in the IOCONFIG register, NOT including the FUNC
//							field.
//
// Returned value:		ERROR if an error was encountered, NOERROR if not.
//
// Errors raised:		ERR_INV_PIN if the pin number was invalid.
//
// Notes:				The bit pattern is constructed from the defined values in
//						pins.h. These values are OR'd together to make a single
//						value that is placed in the upper 29 bits of the pin's
//						IOCONFIG register
//
//						The three Arduino pinMode constants are trapped and converted
//						to appropriate formats.
//
// Example:				pinConfig (1, PIN_OD_ENABLED | PIN_INV_YES);
//						Set pin to be open drain and inverted.
//
uint32 pinConfig(uint32 pin, uint32 mode) {

	if (pin >= MAX_PINS) {
		SYS_ERROR (ERR_INV_PIN);
		return ERROR;
	}

	////////////////////////////////////////////////////////////
	//
	// Handle the three standard Arduino arguments for pinMode
	//
	// Note that the calls to pinSetAsDigitalInput/Output cause
	// a recursive call to this function.
	//
	switch (mode) {

		case INPUT:
			pinSetAsDigitalInput(pin);
			return NOERROR;

		case OUTPUT:
			pinSetAsDigitalOutput(pin);
			return NOERROR;

		case INPUT_PULLUP:
			pinSetAsDigitalInput(pin);
			mode = PIN_MODE_PULLUP | PIN_ADMODE_DISABLED;
			break;
	}

	////////////////////////////////////////////////////////////
	//
	// Set appropriate bits in the pin's IOCON register
	//
	*pin_regs[pin] &= 3;  	// clear all but the FUNC field
	*pin_regs[pin] |= mode; // write the mode bits

	return NOERROR;
}

/////////////////////////////////////////////////////////////////////
//
// Function name:		pinFunc
//
// Description:			Set the function of a pin. The function sets the pin's MUX
//						and selects one of the possible functions the pin can perform,
//						eg GPIO, UART RX, TIMER IO. These are different for every pin.
//
// Parameters:			uint32 pin: The logical pin number, valid values are 0-38
//						uint32 func: A value from the gpio_func_types enum.
//
// Returned value:		NOERROR if no errors
//						ERROR if there are errors, eg you asked for a function the
//							is not implemented for the pin in question.
//
// Errors raised:		ERR_INV_PIN if the pin number was invalid.
//						(ERR_BAD_PIN_FUNC | func) if the function requested is not valid
//							for the pin.
//
// Notes:				The func field is the lowest 3 bits of the pin's IOCONFIG
//						register. This function scans the 8 values in the pins entry
//						in the pin_funcs array looking for a match with the supplied
//						function. If one is found it's index value is written
//						into the lower 3 bits. If no match is found no action is taken.
//
// Example:				pinFunc (1, FUNC_CT32B0_MAT0);
//						Set pin 1's function to the match output, channel 0 for 32-bit timer 0.
//
uint32 pinFunc(uint32 pin, uint32 func) {

	if (pin >= MAX_PINS) {
		SYS_ERROR (ERR_INV_PIN);
		return ERROR;
	}

	for (int i = 0; i < 8; i++) {
		if (pin_funcs[pin][i] == func) {
			*pin_regs[pin] &= ~3;  		// clear the func field
			*pin_regs[pin] |= i;		// write into the func field
			resourceRecordUsage (pin);	// record that this pin has been configured
			return NOERROR;
		}
	}

	SYS_ERROR (ERR_BAD_PIN_FUNC | func);
	return ERROR;
}

