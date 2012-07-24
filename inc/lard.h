//////////////////////////////// LARD.h ////////////////////////////////
//
//
//
//
//

#ifndef BUSNET_H_
#define BUSNET_H_

#ifdef __USE_CMSIS
#include "LPC122x.h"
#endif

#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <core_cm0.h>


#define 	TRUE	1
#define 	FALSE	0
#define 	ERROR	-1
#define 	NOERROR	0

#define 	HIGH	1
#define 	LOW		0

#define		msTicks		__msTicks

typedef		uint8_t 	boolean;
typedef		uint8_t 	byte;
typedef		uint8_t 	uint8;
typedef		uint16_t 	uint16;
typedef		uint32_t	uint32;
typedef		int32_t		int32;

#define     __I     volatile const       /*!< defines 'read only' permissions                 */
#define     __O     volatile             /*!< defines 'write only' permissions                */
#define     __IO    volatile             /*!< defines 'read / write' permissions              */

#define F_CPU 35000000UL

#define 	MAX_PINS 	39

#define byte0(w) ((uint8_t) ((w) & 0xff))
#define byte1(w) ((uint8_t) ((w) >> 8) & 0xFF)
#define byte2(w) ((uint8_t) ((w) >> 16) & 0xFF)
#define byte3(w) ((uint8_t) ((w) >> 24) & 0xFF)

long map(long x, long in_min, long in_max, long out_min, long out_max);

typedef void (*voidFuncPtr)(void);

#define	CR_INTEGER_PRINTF	1		// No floating point in Redlib printf()


#include "LARD.h"
#include "interrupts.h"
#include "uart.h"
#include "ssp.h"
#include "pin.h"
#include "pingroup.h"
#include "debug.h"
#include "error.h"
#include "arduino.h"
#include "shift.h"
#include "fifo.h"
#include "serial.h"
#include "memory.h"
#include "swtimer.h"
#include "hwtimer.h"
#include "globals.h"
#include "events.h"
#include "adc.h"
#include "debounce.h"
#include "fsm.h"
#include "resources.h"

enum {
	PINS_ON_PORT0	= 32,
	PINS_ON_PORT1	= 7,
	PINS_ON_PORT2	= 0
};

enum {
	OBJID_SSP_CONNECTION,
	OBJID_SERIAL_CONNECTION,
	OBJID_HWTIMER,
	OBJID_SWTIMER,
	OBJID_PINGROUP
};


#endif /* BUSNET_H_ */
