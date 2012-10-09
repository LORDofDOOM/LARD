//////////////////////////////// buffer.h ////////////////////////////

#ifndef BUFFER_H_
#define BUFFER_H_

//////////////////////////////////////////////////////////////////////
//
// Name:				fifo8/16/32
//
// Description:			A FIFO or circular buffer.
//
// Notes:				Read and write pointers are post-inc/decremented,
//						ie at any given time they point to the next vacant
//						location (wr) or the current oldest byte (rd).
//
typedef struct  {

	uint8	object_id;

	uint8 * start;		// Pointer to the first data item in buffer
	uint8 * end;		// Pointer to the last data item in buffer +1
	uint8 * rd_ptr;		// Read pointer
	uint8 * wr_ptr;		// Write pointer
	uint32 maxItems;	// Max number of data items in buffer
	uint32 nItems;		// Current number of data items in buffer

	uint8	not_object_id;

} fifo8;

typedef struct  {

	uint8	object_id;

	uint16 * start;		// Pointer to the first data item in buffer
	uint16 * end;		// Pointer to the last data item in buffer +1
	uint16 * rd_ptr;	// Read pointer
	uint16 * wr_ptr;	// Write pointer
	uint32 maxItems;	// Max number of data items in buffer
	uint32 nItems;		// Current number of data items in buffer

	uint8	not_object_id;

} fifo16;

typedef struct  {

	uint8	object_id;

	uint32 * start;		// Pointer to the first data item in buffer
	uint32 * end;		// Pointer to the last data item in buffer +1
	uint32 * rd_ptr;	// Read pointer
	uint32 * wr_ptr;	// Write pointer
	uint32 maxItems;	// Max number of data items in buffer
	uint32 nItems;		// Current number of data items in buffer

	uint8	not_object_id;

} fifo32;


fifo8			*fifo8Create 	(uint32 nItems);
fifo16			*fifo16Create 	(uint32 nItems);
fifo32			*fifo32Create 	(uint32 nItems);

uint8 			fifo8Read 		(fifo8 * buf, uint8 * result);
uint16 			fifo16Read 		(fifo16 * buf, uint8 * result);
uint32 			fifo32Read 		(fifo32 * buf, uint8 * result);

uint32 			fifo8Write 		(fifo8 * buf, uint8 val);
uint32 			fifo16Write		(fifo16 * buf, uint16 val);
uint32 			fifo32Write 	(fifo32 * buf, uint32 val);


#endif /* BUFFER_H_ */
