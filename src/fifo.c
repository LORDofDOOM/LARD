///////////////////////////// buffer.c //////////////////////////////////
//
// A set of functions to implement a FIFO (or circular) buffer.
//
//

// TODO: isFull, isEmpty, nItems

#include "LARD.h"

fifo8 * _fifoCreate (uint32 nItems, uint32 itemSize) {

	fifo8 * buffer;

	/////////////////////////////////////////////////////////
	//
	// Test for bad buffer size request
	//
	if ((nItems * itemSize)  > 1024 || nItems == 0) {
		SYS_ERROR (ERR_BUFFER_BAD_SIZE | nItems * itemSize);
		return 0;
	}

	/////////////////////////////////////////////////////////
	//
	// Allocate RAM for the buffer's control block
	//
	buffer = (void*)safeMalloc(sizeof (fifo8));

	if (buffer == NULL) {
		SYS_ERROR (ERR_MALLOC_FAILED);
		return NULL;
	}

	/////////////////////////////////////////////////////////
	//
	// Allocate RAM for the buffer's data
	//
	buffer->start = (void*)safeMalloc(nItems * itemSize);

	if (buffer->start  == NULL) {
		free (buffer);
		SYS_ERROR (ERR_MALLOC_FAILED);
		return NULL;
	}

	/////////////////////////////////////////////////////////
	//
	// Load all variables with start values
	//
	buffer->object_id = OBJID_FIFO;
	buffer->not_object_id = ~OBJID_FIFO;

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
//
// Returned value:		Pointer to the buffer if no errors
//						NULL if there are errors
//
// Errors raised:		ERR_BUFFER_BAD_SIZE if the buffer size request is 0 or
//							> 1024 bytes, 512 half-words, or 256 words. In other
//							word the buffer can't be more than 1024 bytes in size.
//						ERR_MALLOC_FAILED if safeMalloc calls failed
//
// Example:				fifo8 * myBuffer;
//						myBuffer = fifo8Create(100);
//						if (myBuffer == NULL) myErrorFunc();
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
// Description:			Read a single item from a FIFO buffer.
//
// Parameters:			fifoX * buf, pointer to a FIFO buffer
//						uint8 * result, pointer to a results value. If you
//							are not interested in using the result variable
//							use NULL for this parameter.
//
// Returned value:		Item from the buffer and result = 0 if item available,
//						ERROR and result = RSLT_BUFFER_EMPTY if not
//
// Errors raised:		none
//
// Example:				val = fifo8Read(buf, NULL);
//						if (val != ERROR)
//							// val is an item from the buffer
//
//						The above example is OK if the buffer does not hold a
//						full range of binary values, say it only holds ASCII data
//						or you know for a fact that the value -1 will never appear
//						in the buffer.
//
//						However special consideration has to be given when the
//						value -1 (ERROR) is a valid item to be returned from the
//						buffer. In this case you can't test the returned value
//						and the supplied result variable must be used.
//
//						uint8 result;
// 	 					val = fifo8Read(buf, &result);
// 						if (result != RSLT_BUFFER_EMPTY)
//							// val is an item from the buffer
//
//  Notes:
//
uint8 fifo8Read (fifo8 * buf, uint8 * result) {

	uint32 val;

	VERIFY_OBJECT (buf, OBJID_FIFO)

	CLEAR_RESULT;

	if (buf->nItems == 0) {
		SET_RESULT(RSLT_BUFFER_EMPTY);
		return ERROR;
	}

	val = * buf->rd_ptr;
	buf->rd_ptr++;
	ATOMIC(buf->nItems--);

	if (buf->rd_ptr == buf->end)
		buf->rd_ptr = buf->start;
//   _rx_buffer->tail = (unsigned int)(_rx_buffer->tail + 1) % SERIAL_BUFFER_SIZE;

	return (val);
}

uint16 fifo16Read (fifo16 * buf, uint8 * result) {

	uint32 val;

	VERIFY_OBJECT (buf, OBJID_FIFO)

	CLEAR_RESULT;

	if (buf->nItems == 0) {
		SET_RESULT (RSLT_BUFFER_EMPTY);
		return ERROR;
	}

	val = * buf->rd_ptr;
	buf->rd_ptr++;
	ATOMIC(buf->nItems--);

	if (buf->rd_ptr == buf->end)
		buf->rd_ptr = buf->start;
//   _rx_buffer->tail = (unsigned int)(_rx_buffer->tail + 1) % SERIAL_BUFFER_SIZE;

	return val;
}

uint32 fifo32Read (fifo32 * buf, uint8 * result) {

	uint32 val;

	VERIFY_OBJECT (buf, OBJID_FIFO)

	CLEAR_RESULT;

	if (buf->nItems == 0) {
		SET_RESULT (RSLT_BUFFER_EMPTY);
		return ERROR;
	}

	val = * buf->rd_ptr;
	buf->rd_ptr++;
	ATOMIC(buf->nItems--);

	if (buf->rd_ptr == buf->end)
		buf->rd_ptr = buf->start;

	return val;
}

////////////////////////////////////////////////////////////////
//
// Function name:		fifo8/16/32Write
//
// Description:			Write a single item to a FIFO buffer.
//
// Parameters:			fifoX * buf, the buffer to write into
//						uintX, the item to be written
//
// Returned value:		NOERROR if the write succeeded
//						ERROR if not
//
// Errors raised:		ERR_BUFFER_FULL if the buffer is already full.
//						ERR_BAD_OBJECT if buf is corrupted.
//
// Example:				uint8 val = 'A';
//						if (fifo8Write(val) == ERROR)
//							// write failed
//						else
//							// write OK
//
// Notes:
//
uint32 fifo8Write (fifo8 * buf, uint8 val) {

	VERIFY_OBJECT (buf, OBJID_FIFO)

	if (buf->nItems >= buf->maxItems) {
		SYS_ERROR (ERR_BUFFER_FULL);
		return ERROR;
	}

	*buf->wr_ptr = val;
	buf->wr_ptr++;
	ATOMIC(buf->nItems++);

	if (buf->wr_ptr == buf->end)
		buf->wr_ptr = buf->start;

	return NOERROR;
}

uint32 fifo16Write (fifo16 * buf, uint16 val) {

	VERIFY_OBJECT (buf, OBJID_FIFO)

	if (buf->nItems >= buf->maxItems) {
		SYS_ERROR (ERR_BUFFER_FULL);
		return ERROR;
	}

	*buf->wr_ptr = val;
	buf->wr_ptr++;
	ATOMIC(buf->nItems++);

	if (buf->wr_ptr == buf->end)
		buf->wr_ptr = buf->start;

	return NOERROR;
}

uint32 fifo32Write (fifo32 * buf, uint32 val) {

	VERIFY_OBJECT (buf, OBJID_FIFO)

	if (buf->nItems >= buf->maxItems) {
		SYS_ERROR (ERR_BUFFER_FULL);
		return ERROR;
	}

	*buf->wr_ptr = val;
	buf->wr_ptr++;
	ATOMIC(buf->nItems++);

	if (buf->wr_ptr == buf->end)
		buf->wr_ptr = buf->start;

	return NOERROR;
}
