//////////////////////////////// events.h ////////////////////////////
//
//
//
//
//

#ifndef EVENTS_H_
#define EVENTS_H_

extern voidFuncPtr 		eventFunctions[];

void SysTick_Handler		(void);
void attachEventHandler 	(uint32 interrupt, void (*userFunc)(void));
void detachEventHandler		(uint32 interrupt);

typedef enum {
	EVENT_SYS_TICK,

	EVENT_UART0_RX_OVERRUN_ERR,		// OE:	Rx FIFO is full and new byte received in SR
	EVENT_UART0_RX_PARITY_ERR,		// PE
	EVENT_UART0_RX_FRAMING_ERR,		// FE
	EVENT_UART0_RX_BREAK_DETECT,	// BI
	EVENT_UART0_RX_FIFO_TRIGLVL,	// RDA:	Rx FIFO has reached trigger level
	EVENT_UART0_RX_CHAR_AVAILABLE,	// RDR:	There is at least one char in Rx FIFO
	EVENT_UART0_RX_CHAR_TIMOUT,		// CTI
	EVENT_UART0_TX_FIFO_EMPTY,		// THRE:Tx FIFO is empty (still one byte in SR)
	EVENT_UART0_TX_EMPTY,			// TEMT:Tx Shift register is empty

	EVENT_UART1_RX_OVERRUN_ERR,		// OE:	Rx FIFO is full and new byte received in SR
	EVENT_UART1_RX_PARITY_ERR,		// PE
	EVENT_UART1_RX_FRAMING_ERR,		// FE
	EVENT_UART1_RX_BREAK_DETECT,	// BI
	EVENT_UART1_RX_FIFO_TRIGLVL,	// RDA:	Rx FIFO has reached trigger level
	EVENT_UART1_RX_CHAR_AVAILABLE,	// RDR:	There is at least one char in Rx FIFO
	EVENT_UART1_RX_CHAR_TIMOUT,		// CTI
	EVENT_UART1_TX_FIFO_EMPTY,		// THRE:Tx FIFO is empty (still one byte in SR)
	EVENT_UART1_TX_EMPTY,			// TEMT:Tx Shift register is empty

	EVENT_PININT_0,					// Interrupt on pin0,
	EVENT_PININT_1,
	EVENT_PININT_2,
	EVENT_PININT_3,
	EVENT_PININT_4,
	EVENT_PININT_5,
	EVENT_PININT_6,
	EVENT_PININT_7,
	EVENT_PININT_8,
	EVENT_PININT_9,
	EVENT_PININT_10,
	EVENT_PININT_11,
	EVENT_PININT_12,
	EVENT_PININT_13,
	EVENT_PININT_14,
	EVENT_PININT_15,
	EVENT_PININT_16,
	EVENT_PININT_17,
	EVENT_PININT_18,
	EVENT_PININT_19,
	EVENT_PININT_20,
	EVENT_PININT_21,
	EVENT_PININT_22,
	EVENT_PININT_23,
	EVENT_PININT_24,
	EVENT_PININT_25,
	EVENT_PININT_26,
	EVENT_PININT_27,
	EVENT_PININT_28,
	EVENT_PININT_29,
	EVENT_PININT_30,
	EVENT_PININT_31,
	EVENT_PININT_32,
	EVENT_PININT_33,
	EVENT_PININT_34,
	EVENT_PININT_35,
	EVENT_PININT_36,
	EVENT_PININT_37,
	EVENT_PININT_38,

	EVENT_SYS_ERROR,				// Handle a system error

	EVENT_DUMMY

} system_events;


#endif /* EVENTS_H_ */
