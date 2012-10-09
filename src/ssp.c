////////////////////////////// ssp.c //////////////////////////////////
//
// This is a simple implementation of the SSP object, at present it
// does not use interrupts, can only transfer 8 items at a time and
// can only be a master.
//
//
//

#include "LARD.h"

sspConnection * sspCreate (uint8 data_bits, uint8 frame_format, uint8 mode, uint32 bit_rate) {

	sspConnection * s;
	LPC_SSP_Type * port;

	/////////////////////////////////////////////////////////////////
	//
	// Create the SSP structure
	//
	s = (void*)safeMalloc(sizeof (sspConnection));

	if (s == NULL) {
		SYS_ERROR (ERR_SSP_INIT_FAILED | 1);
		return (sspConnection *)ERROR;
	}

	port 		= LPC_SSP;				// Only a single SSP on the LPC1227
	port->CR1  |= (1 << SSP_CR1_MS);	// Set master mode, no slave for the time being
	s->port		= port;

	/////////////////////////////////////////////////////////////////
	//
	// Set the structure's guard bytes
	//
	s->object_id 	 = OBJID_SSP_CONNECTION;
	s->not_object_id = ~OBJID_SSP_CONNECTION;

	/////////////////////////////////////////////////////////////////
	//
	// Enable the port's AHB clock
	//
	LPC_SYSCON->SYSAHBCLKCTRL |= (0x1 << 11);

	/////////////////////////////////////////////////////////////////
	//
	// Setup data bits
	//
	if (data_bits < 4 || data_bits > 16) {
		SYS_ERROR (ERR_SSP_BAD_DATA_BITS | data_bits);
		data_bits = 8;
	}
	s->data_bits 	= data_bits;
	port->CR0		= data_bits;

	/////////////////////////////////////////////////////////////////
	//
	// Setup frame format
	//
	if (frame_format > 3) {
		SYS_ERROR (ERR_SSP_BAD_FRAME_FORMAT | frame_format);
		frame_format = SSP_FRAME_FORMAT_SPI;
	}
	s->frame_format	 = frame_format;
	port->CR0		|= (frame_format << SSP_CR0_FRF);

	/////////////////////////////////////////////////////////////////
	//
	// Setup mode
	//
	if (mode > 3) {
		SYS_ERROR (ERR_SSP_BAD_FRAME_FORMAT | mode);
		mode = SSP_MODE_0;
	}
	s->mode		 = mode;
	port->CR0	|= (mode << SSP_CR0_CPOL);


	s->bit_rate	 = bit_rate;
	port->CR0	|= (bit_rate << SSP_CR0_SCR);  // NOTE: Not right, value needs to be calculated

	/////////////////////////////////////////////////////////////////
	//
	// Make sure the port is disabled
	//
	s->enabled = FALSE;
	s->port->CR1 &= ~(1 << SSP_CR1_SSE);

	return s;

}

void sspFlush (sspConnection * s) {

	VERIFY_OBJECT(s, OBJID_SSP_CONNECTION)

	uint8 x;

	sspLoopbackOn(s);

	for (int i = 0; i < 8; i++)
		s->port->DR = 0;

	for (int i = 0; i < 8; i++)
		x = s->port->DR;

	sspLoopbackOff(s);
}

uint32 sspWrite (sspConnection * s, uint16 * buf, uint8 nVals) {

	VERIFY_OBJECT(s, OBJID_SSP_CONNECTION)

	for (; nVals; nVals--)
		s->port->DR = *buf++;

	return NOERROR;
}

uint32 sspExchange (sspConnection * s) {

	VERIFY_OBJECT(s, OBJID_SSP_CONNECTION)

	s->port->CR1 |= (1 << SSP_CR1_SSE);

	////////////////////////////////////////////////////////////
	// Wait for FIFO to empty
	while (!(s->port->SR & (1 << SSP_SR_TFE)));

	s->port->CR1 &= ~(1 << SSP_CR1_SSE);

	return NOERROR;
}

uint32 sspRead (sspConnection * s, uint16 * buf) {

	VERIFY_OBJECT(s, OBJID_SSP_CONNECTION)

	////////////////////////////////////////////////////////////
	// Read values until FIFO is empty
	while (s->port->SR & (1 << SSP_SR_RNE))
		*buf++ = s->port->DR;

	return NOERROR;
}

uint32 sspEnable (sspConnection * s) {

	VERIFY_OBJECT(s, OBJID_SSP_CONNECTION)

	s->enabled = TRUE;
	s->port->CR1 |= (1 << SSP_CR1_SSE);

	return NOERROR;
}

uint32 sspDisable (sspConnection * s) {

	VERIFY_OBJECT(s, OBJID_SSP_CONNECTION)

	s->enabled = FALSE;
	s->port->CR1 &= ~(1 << SSP_CR1_SSE);

	return NOERROR;
}

uint32 sspLoopbackOn (sspConnection * s) {


	VERIFY_OBJECT(s, OBJID_SSP_CONNECTION)

	s->port->CR1 |= (1 << SSP_CR1_LBM);

	return NOERROR;
}

uint32 sspLoopbackOff (sspConnection * s) {

	VERIFY_OBJECT(s, OBJID_SSP_CONNECTION)

	s->port->CR1 &= ~(1 << SSP_CR1_LBM);

	return NOERROR;
}

uint32 sspMisoDisable (sspConnection * s) {

	VERIFY_OBJECT(s, OBJID_SSP_CONNECTION)

	s->port->CR1 |= (1 << SSP_CR1_SOD);

	return NOERROR;
}

uint32 sspMisoEnable (sspConnection * s) {

	VERIFY_OBJECT(s, OBJID_SSP_CONNECTION)

	s->port->CR1 &= ~(1 << SSP_CR1_SOD);

	return NOERROR;
}
