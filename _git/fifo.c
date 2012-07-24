///////////////////////////// buffer.c //////////////////////////////////
//
// A set of functions to implement a FIFO (or circular) buffer.
//
//

#include "LARD.h"

fifo8 * _fifoCreate (uint32 nItems, uint32 itemSize) {

	fifo8 * buffer;

	/////////////////////////////////////////////////////////
	//
	// Test for bad buffer size request
	//
	if (nItems > 256 || nItems == 0) {
		SYS_ERROR (ERR_BUFFER_BAD_SIZE | nItems);
		return 0;
	}

	/////////////////////////////////////////////////////////
	//
	// Allocate RAM for the buffer's control block
	//
	buffer = (void*)safeMalloc(sizeof (fifo8));

	if (buffer == 0) {
		SYS_ERROR (ERR_MALLOC_FAILED);
		return 0;
	}

	/////////////////////////////////////////////////////////
	//
	// Allocate RAM for the buffer's data
	//
	buffer->start = (void*)safeMalloc(nItems * itemSize);

	if (buffer->start  == 0) {
		free (buffer);
		SYS_ERROR (ERR_MALLOC_FAILED);
		return 0;
	}

	/////////////////////////////////////////////////////////
	//
	// Load all variables with start values
	//
	buffer->rd_ptr	= buffer->start;
	buffer->wr_ptr	= buffer->start;
	buffer->end		= buffer->start + (nItems * itemSize);	// points to EOB + 1
	buffer->nItems	= 0;
	buffer->maxItems= nItems;

	return buffer;
}

////////////////////////////////////////////////////////////////
//
// Function name:		fifo8/16/32Create
//
// Description:			Functions to create FIFO buffers with 8, 16 or 32 bit
//						items. Allocates RAM as appropriate.
//
// Parameters:			uint32 size: The number if data bytes required valid values 1-256
//						uint32 itemSize, the size of each item in bytes, 1, 2 and 4 are valid.
//
// Returned value:		TRUE if no errors
//						FALSE if there are errors
//
// Errors raised:		ERR_BUFFER_BAD_SIZE if the buffer size request is invalid
//						ERR_BUFFER_MALLOC_FAILED if either malloc failed
//
// Example:				fifo8 myBuffer;
//						myBuffer = fifo8Create(100);
//						if (myBuffer == 0) myErrorFunc();
//
// Notes:				These functions call safeMalloc, therefore they can only be used
//						when the system_initialising flag is TRUE, ie during the setup()
//						function.
//
fifo8 * fifo8Create (uint32 nItems) {
	return (fifo8 *) _fifoCreate (nItems, 1);
}
fifo16 * fifo16Create (uint32 nItems) {
	return (fifo16 *) _fifoCreate (nItems, 2);
}
fifo32 * fifo32Create (uint32 nItems) {
	return (fifo32 *) _fifoCreate (nItems, 4);
}

////////////////////////////////////////////////////////////////
//
// Function name:		fifo8/16/32Read
//
// Description:			Read a single item from a fifo.
//
// Parameters:			none
//
// Returned value:		uint32, 0-256 if byte available, -1 if not
//
// Errors raised:		none
//
// Example:				uint32 val;
//						val = fifo8Read ();
//						if (val != -1)
//							// val is a char from the buffer
//
// Notes:
//
uint8 fifo8Read (fifo8 * buf) {

	uint32 val;

	if (buf->nItems == 0) {
		SYS_ERROR (ERR_BUFFER_EMPTY);
		return ERROR;
	}

	val = * buf->rd_ptr;
	buf->rd_ptr++;
	buf->nItems--;

	if (buf->rd_ptr == buf->end)
		buf->rd_ptr = buf->start;

	return (val);
}

uint16 fifo16Read (fifo16 * buf) {

	uint32 val;

	if (buf->nItems == 0) {
		SYS_ERROR (ERR_BUFFER_EMPTY);
		return ERROR;
	}

	val = * buf->rd_ptr;
	buf->rd_ptr++;
	buf->nItems--;

	if (buf->rd_ptr == buf->end)
		buf->rd_ptr = buf->start;

	return (val);
}

uint32 fifo32Read (fifo32 * buf) {

	uint32 val;

	if (buf->nItems == 0) {
		SYS_ERROR (ERR_BUFFER_EMPTY);
		return ERROR;
	}

	val = * buf->rd_ptr;
	buf->rd_ptr++;
	buf->nItems--;

	if (buf->rd_ptr == buf->end)
		buf->rd_ptr = buf->start;

	return (val);
}

////////////////////////////////////////////////////////////////
//
// Function name:		fifo8/16/32Write
//
// Description:			Write a single item to a fifo.
//
// Parameters:			fifoN * buf, the buffer to write into
//						uintN, the item to be written
//
// Returned value:		TRUE if the write succeeded
//						FALSE if not
//
// Errors raised:		none
//
// Example:				uint8 val = 'A';
//						if (fifo8Write(val) == FALSE)
//							// write failed
//						else
//							// write OK
//
// Notes:
//
uint32 fifo8Write (fifo8 * buf, uint8 val) {

	if (buf->nItems >= buf->maxItems) {
		SYS_ERROR (ERR_BUFFER_FULL);
		return ERROR;
	}

	*buf->wr_ptr = val;
	buf->wr_ptr++;
	buf->nItems++;

	if (buf->wr_ptr == buf->end)
		buf->wr_ptr = buf->start;

	return NOERROR;
}

uint32 fifo16Write (fifo16 * buf, uint16 val) {

	if (buf->nItems >= buf->maxItems) {
		SYS_ERROR (ERR_BUFFER_FULL);
		return ERROR;
	}

	*buf->wr_ptr = val;
	buf->wr_ptr++;
	buf->nItems++;

	if (buf->wr_ptr == buf->end)
		buf->wr_ptr = buf->start;

	return NOERROR;
}

uint32 fifo32Write (fifo32 * buf, uint32 val) {

	if (buf->nItems >= buf->maxItems) {
		SYS_ERROR (ERR_BUFFER_FULL);
		return ERROR;
	}

	*buf->wr_ptr = val;
	buf->wr_ptr++;
	buf->nItems++;

	if (buf->wr_ptr == buf->end)
		buf->wr_ptr = buf->start;

	return NOERROR;
}
