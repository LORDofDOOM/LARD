///////////////////////////////// serial.c ///////////////////////////////
//
//  
//
//
//

#include "LARD.h"

#if 0
void uartinit(uint32_t brate)
{
    uint32_t regVal, Fdiv;

    NVIC_DisableIRQ(UART0_IRQn);

    //Enable Pins 0_1 and 0_2 for UART0
    LPC_IOCON->PIO0_1 &= ~0x07;    /* UART0 I/O config */
    LPC_IOCON->PIO0_1 |= 0x02;     /* UART0 RXD LOC0 */
    LPC_IOCON->PIO0_2 &= ~0x07;
    LPC_IOCON->PIO0_2 |= 0x02;     /* UART0 TXD LOC0 */

    /* Enable UART 0 clock */
    LPC_SYSCON->PRESETCTRL |= (0x1<<2);
    LPC_SYSCON->SYSAHBCLKCTRL |= (0x1<<12);
    LPC_SYSCON->UART0CLKDIV = 0x1;     /* divided by 1 */

    LPC_UART0->LCR = 0x83;             /* 8 bits, no Parity, 1 Stop bit */
    regVal = LPC_SYSCON->UART0CLKDIV;
    Fdiv = ((SystemCoreClock/regVal)/16)/brate ;    /*baud rate */

    LPC_UART0->DLM = Fdiv / 256;
    LPC_UART0->DLL = Fdiv % 256;
    LPC_UART0->LCR = 0x03;        /* DLAB = 0 */
    LPC_UART0->FDR = 0x10;        /* set to default value: 0x10 */
    LPC_UART0->FCR = 0x07;        /* Enable and reset TX and RX FIFO. */

    /* Read to clear the line status. */
    regVal = LPC_UART0->LSR;

    /* Ensure a clean start, no data in either TX or RX FIFO. */
    while ( (LPC_UART0->LSR & (LSR_THRE|LSR_TEMT)) != (LSR_THRE|LSR_TEMT) );

    NVIC_EnableIRQ(UART0_IRQn); //Re-enable interrupt after changes

    //LPC_UART0->IER = IER_RBR | IER_THRE | IER_RLS;    /* Enable UART interrupt */
}
#endif

/////////////////////////////////////////////////////////////////
// Array of pointers to all created serial structures
//
// The first 6 entries are reserved for hardware UARTs, the next 4
// may be used for software serial
//
serialConnection * serialConnections [10];

/////////////////////////////////////////////////////////////////
//
// Array of logical pin numbers usable by the two UARTs
//
// UART0 can use 1/2 or 40/41 for RxD/Txd
// UART1 can use 8/9, 50/49 or 51/52 for RxD/Txd
//
uint8 __uart_logical_pins[2][6] = {
/*   				Rxd		Txd		Rxd		Txd		Rxd		Txd */
/* UART0 */		   {1,		2,		40,		41,		ERROR,	ERROR},
/* UART1 */		   {8,		9,		50, 	49,		51,		52}
};

uint32 serialSetUartPins(serialConnection * s, uint32 location) {

	uint8 txPin;
	uint8 rxPin;

	VERIFY_OBJECT (s, ERR_BAD_OBJECT);

	ASSERT_RETERR (location > 3, ERR_SERIAL_BAD_PORT);

	location <<= 1;
	rxPin = __uart_logical_pins[s->port][location];
	txPin = __uart_logical_pins[s->port][location+1];


	ASSERT_RETERR ((rxPin == ERROR) || (txPin == ERROR), ERR_SERIAL_BAD_LOCATION);

	TRY
		pinFunc (rxPin, (s->port == 0) ? FUNC_RXD0 : FUNC_RXD1);
		pinFunc (txPin, (s->port == 0) ? FUNC_TXD0 : FUNC_TXD1);
	CATCH_RETERR

	s->txPin = txPin;
	s->rxPin = rxPin;

	return NOERROR;
}


serialConnection * serialCreate (uint8 port, uint32 baudrate, uint8 data_bits, uint8 parity, uint8 stop_bits,
		uint8 rx_buff_size, uint8 tx_buff_size) {

	LPC_UART0_Type * uart;
	uint32 clkDiv;
	serialConnection * s;

	if (port > MAX_SERIAL_CONNECTIONS) {
		SYS_ERROR (ERR_SERIAL_BAD_PORT | 1);
		SYS_ERROR (ERR_SERIAL_INIT_FAILED);
		return (serialConnection *)ERROR;
	}

	/////////////////////////////////////////////////////////////////
	// Create the serial structure
	s = (void*)safeMalloc(sizeof (serialConnection));

	if (s == NULL) {
		SYS_ERROR (ERR_SERIAL_INIT_FAILED | 2);
		return (serialConnection *)ERROR;
	}

	serialConnections [port] = s;

	s->object_id 	 = OBJID_SERIAL_CONNECTION;
	s->not_object_id = ~OBJID_SERIAL_CONNECTION;

	s->port = port;

	/////////////////////////////////////////////////////////////////
	//
	switch (port) {
		case SERIAL_UART0:

			/////////////////////////////////////////////////////////////////
			// Setup default Rx and Tx pins
			serialSetUartPins (s, UART0_PINS_R1_T2);

			/////////////////////////////////////////////////////////////////
			// Disable this UART's interrupt for the duration
			NVIC_DisableIRQ(UART0_IRQn);

			/////////////////////////////////////////////////////////////////
			// Set a pointer to the UART structure. Note that UART0 and UART1 have different
			// structs but for the purposes of this function they are the same because
			// we are not using the RS-485 features of UART0
			uart = LPC_UART0;
			s->uart = uart;

			/////////////////////////////////////////////////////////////////
			// Create Rx and Tx buffers
			s->RxBuffer = fifo16Create(rx_buff_size);
			if (s->RxBuffer == NULL) {
				free (s);
				SYS_ERROR (ERR_SERIAL_INIT_FAILED | 3);
				return (serialConnection *)ERROR;
			};

			s->TxBuffer = fifo16Create(tx_buff_size);
			if (s->TxBuffer == NULL) {
				free (s);
				free (s->RxBuffer);
				SYS_ERROR (ERR_SERIAL_INIT_FAILED | 4);
				return (serialConnection *)ERROR;
			}

			/////////////////////////////////////////////////////////////////
			// Release the UART's reset
			LPC_SYSCON->PRESETCTRL |= (0x1 << 2);		// Set UART0_RST_N bit

			/////////////////////////////////////////////////////////////////
			// Enable the UART's AHB clock
			LPC_SYSCON->SYSAHBCLKCTRL |= (0x1 << 12);	// Set UART0 bit

			/////////////////////////////////////////////////////////////////
			// Set the UART's clock divider
			LPC_SYSCON->UART0CLKDIV = 0x1;  			// divide by 1, may be different later

			/////////////////////////////////////////////////////////////////
			// Get the clock divider for later use in baudrate calcs
			// Could use the constant 1 but maybe this will change later
			// so we just re-read the register
			clkDiv = LPC_SYSCON->UART0CLKDIV;
			break;

		case SERIAL_UART1:
			// NOTE: Comments as per the above UART0 code

			serialSetUartPins (s, UART1_PINS_R8_T9);

			NVIC_DisableIRQ(UART1_IRQn);
			uart = (LPC_UART0_Type*) LPC_UART1;
			s->uart = uart;
			s->RxBuffer = fifo16Create(rx_buff_size);
			if (s->RxBuffer == NULL) {
				free (s);
				SYS_ERROR (ERR_SERIAL_INIT_FAILED | 5);
			}

			s->TxBuffer = fifo16Create(tx_buff_size);
			if (s->TxBuffer == NULL) {
				free (s);
				free (s->RxBuffer);
				SYS_ERROR (ERR_SERIAL_INIT_FAILED | 6);
			}

			LPC_SYSCON->PRESETCTRL |= (0x1 << 3);		// Set UART1_RST_N bit

			LPC_SYSCON->SYSAHBCLKCTRL |= (0x1 << 13);	// Set UART1 bit

			LPC_SYSCON->UART1CLKDIV = 0x1; 				// divide by 1, may be different later

			clkDiv = LPC_SYSCON->UART1CLKDIV;

			break;

		default:
			free (s);
			SYS_ERROR (ERR_SERIAL_INIT_FAILED | (port << 8) | 7);
			return (serialConnection *)ERROR;

	}

	/////////////////////////////////////////////////////////////////
	//
	// Start with a blank slate in the Line Control Register
	//
	uart->LCR = 0;

	/////////////////////////////////////////////////////////////////
	//
	// Setup data bits
	//
	if (data_bits < 5 || data_bits > 8) {
		SYS_ERROR (ERR_SERIAL_BAD_DATA_BITS | data_bits);
		data_bits = UART_DATA_BITS_8;
	}
	uart->LCR |= data_bits - 5;
	s->data_bits = data_bits;

	/////////////////////////////////////////////////////////////////
	//
	// Setup stop bits
	//
	if ((stop_bits != UART_STOP_BITS_1) && (stop_bits != UART_STOP_BITS_2)) {
		SYS_ERROR (ERR_SERIAL_BAD_STOP_BITS | stop_bits);
		stop_bits = UART_STOP_BITS_2;
	}
	uart->LCR |= stop_bits;
	s->stop_bits = stop_bits;

	//////////////////////////////////////////////////////////////////
	//
	// Setup parity
	//
	if ((parity != UART_PARITY_NONE) &&
		(parity != UART_PARITY_ODD) &&
		(parity != UART_PARITY_EVEN) &&
		(parity != UART_PARITY_FORCE1) &&
		(parity != UART_PARITY_FORCE0)) {

		SYS_ERROR (ERR_SERIAL_BAD_PARITY);
		parity = UART_PARITY_NONE;
	}
	uart->LCR |= parity;
	s->parity = parity;

	//////////////////////////////////////////////////////////////////
	//
	// Setup baudrate
	//
	uint32_t Fdiv;

	Fdiv = ((SystemCoreClock / clkDiv) / 16) / baudrate;

	uart->LCR |= 0x80;  		// DLAB = 1
	uart->DLM = Fdiv / 256;
	uart->DLL = Fdiv % 256;
	uart->LCR &= ~0x80; 		// DLAB = 0

	s->baudrate = baudrate;

	//////////////////////////////////////////////////////////////////
	//
	// Setup Fractional Divide Register
	//
	uart->FDR = 0x10; // set to default value, does nothing

	//////////////////////////////////////////////////////////////////
	//
	// Setup FIFOs
	//
	uart->FCR = (1 << 0) |		// Enable both FIFOs
			    (1 << 1) |		// Reset Rx FIFO
			    (1 << 2) |		// Reset Tx FIFO
			    (3 << 7);		// RX FIFO trigger level = 14 chars

	//////////////////////////////////////////////////////////////////
	//
	//	 Ensure a clean start, no data in either Tx or Rx FIFO.
	//
	while ((uart->LSR & (LSR_THRE | LSR_TEMT)) != (LSR_THRE | LSR_TEMT)) ;
	while (uart->LSR & LSR_RDR)	clkDiv = uart->RBR; // Dump data from Rx FIFO

	//////////////////////////////////////////////////////////////////
	//
	// Clear any line status bits
	//
	clkDiv = uart->LSR;

	//////////////////////////////////////////////////////////////////
	//
	// Setup and enable UART interrupts
	//
	uart->IER = IER_RBR | IER_THRE | IER_RX;

	if (port == 0)
		NVIC_EnableIRQ(UART0_IRQn);
	else
		NVIC_EnableIRQ(UART1_IRQn);

	return s;

}

/////////////////////////////////////////////////////////////////////
//
// There is at least one char in Rx FIFO
//
void serial0_rlsFuncRDR (void) {
	uint32 LSRval;

	if (eventFunctions[EVENT_UART0_RX_CHAR_AVAILABLE] != NULL) {
		(eventFunctions[EVENT_UART0_RX_CHAR_AVAILABLE]) ();
	} else {
		do  {
			LSRval = LPC_UART0->LSR;
			fifo16Write(serialConnections[0]->RxBuffer, (LSRval << 8) | LPC_UART0->RBR);
		} while (LSRval & LSR_RDR);
	}
}

/////////////////////////////////////////////////////////////////////
//
// Rx FIFO has reached trigger level
//
void serial0_rlsFuncRDA (void) {
	uint32 LSRval;

	if (eventFunctions[EVENT_UART0_RX_FIFO_TRIGLVL] != NULL) {
		(eventFunctions[EVENT_UART0_RX_FIFO_TRIGLVL]) ();
	} else {
		do  {
			LSRval = LPC_UART0->LSR;
			fifo16Write(serialConnections[0]->RxBuffer, (LSRval << 8) | LPC_UART0->RBR);
		} while (LSRval & LSR_RDR);
	}
}

/////////////////////////////////////////////////////////////////////
//
// Overrun error, there's a new character in the Rx shift register
// and the FIFO is full.
//
void serial0_rlsFuncOE (void) {

	if (eventFunctions[EVENT_UART0_RX_OVERRUN_ERR] != NULL)
		(eventFunctions[EVENT_UART0_RX_OVERRUN_ERR]) ();
	else {
//		UARTStatus0 = LSRval; 					// Read LSR will clear interrupt
//		serial0enqueueByte (LPC_UART0->RBR); 	// Save character in buffer
	}
}

/////////////////////////////////////////////////////////////////////
//
// Parity error, the character at the top of the FIFO has a parity error.
//
void serial0_rlsFuncPE (void) {

}

/////////////////////////////////////////////////////////////////////
//
// Framing error, the character at the top of the FIFO has a framing
// error, IE the stop bit was LOW.
//
void serial0_rlsFuncFE (void) {

}

/////////////////////////////////////////////////////////////////////
//
//
void serial0_rlsFuncBI (void) {

}

/////////////////////////////////////////////////////////////////////
//
//
void serial0_rlsFuncTHRE (void) {

}

/////////////////////////////////////////////////////////////////////
//
//
void serial0_rlsFuncTEMT (void) {

}

const voidFuncPtr serial0_rlsFunctions[32] = {
	//						BI	FE	PE	OE	RDR
	0,					//	0	0	0	0	0
	serial0_rlsFuncRDR,	//	0	0	0	0	1
	serial0_rlsFuncOE,	//	0	0	0	1	0
	serial0_rlsFuncOE,	//	0	0	0	1	1
	serial0_rlsFuncRDR,	//	0	0	1	0	0
	serial0_rlsFuncRDR,	//	0	0	1	0	1
	serial0_rlsFuncOE,	//	0	0	1	1	0
	serial0_rlsFuncOE,	//	0	0	1	1	1
	serial0_rlsFuncFE,	//	0	1	0	0	0
	serial0_rlsFuncFE,	//	0	1	0	0	1
	serial0_rlsFuncOE,	//	0	1	0	1	0
	serial0_rlsFuncOE,	//	0	1	0	1	1
	serial0_rlsFuncFE,	//	0	1	1	0	0
	serial0_rlsFuncFE,	//	0	1	1	0	1
	serial0_rlsFuncOE,	//	0	1	1	1	0
	serial0_rlsFuncOE,	//	0	1	1	1	1
	serial0_rlsFuncBI,	//	1	0	0	0	0
	serial0_rlsFuncBI,	//	1	0	0	0	1
	serial0_rlsFuncBI,	//	1	0	0	1	0
	serial0_rlsFuncBI,	//	1	0	0	1	1
	serial0_rlsFuncBI,	//	1	0	1	0	0
	serial0_rlsFuncBI,	//	1	0	1	0	1
	serial0_rlsFuncBI,	//	1	0	1	1	0
	serial0_rlsFuncBI,	//	1	0	1	1	1
	serial0_rlsFuncBI,	//	1	1	0	0	0
	serial0_rlsFuncBI,	//	1	1	0	0	1
	serial0_rlsFuncBI,	//	1	1	0	1	0
	serial0_rlsFuncBI,	//	1	1	0	1	1
	serial0_rlsFuncBI,	//	1	1	1	0	0
	serial0_rlsFuncBI,	//	1	1	1	0	1
	serial0_rlsFuncBI,	//	1	1	1	1	0
	serial0_rlsFuncBI	//	1	1	1	1	1

};

void UART1_IRQHandler(void) {}          // UART1

void UART0_IRQHandler(void) {
	uint8_t IIRValue, LSRval;
	uint8_t Dummy = Dummy;

	//	__disable_irq();

	IIRValue = LPC_UART0->IIR;

	IIRValue >>= 1; 		// skip pending bit in IIR
	IIRValue &= 0x07; 		// check bit 1~3, interrupt identification

	switch (IIRValue) {
		case IIR_RLS:
			LSRval = LPC_UART0->LSR;
			(serial0_rlsFunctions[LSRval & 0b11111]) ();
			break;

#if 0
			if (LSRval & (LSR_OE | LSR_PE | LSR_FE | LSR_BI)) {
				UARTStatus0 = LSRval; // Read LSR will clear the interrupt
				Dummy = LPC_UART0->RBR; // Dummy read on RX to clear interrupt, then bail out
				return;
			}
			if (LSRval & LSR_RDR) { /* Receive Data Ready */
				// No errors save into the data buffer.

				UARTBuffer0[UARTCount0++] = LPC_UART0->RBR; // Note: read RBR will clear the interrupt
				if (UARTCount0 == BUFSIZE) {
					UARTCount0 = 0; /* buffer overflow */
				}
			}
#endif
			break;

		case IIR_RDA:		// Receive Data Available
			serial0_rlsFuncRDA();
			break;

		case IIR_CTI:		// Character timeout indicator
			break;

		case IIR_THRE:		// FIFO is empty (still one byte in SR though)
			LSRval = LPC_UART0->LSR;

			// Check status in the LSR to see if valid data in U0THR or not
			if (LSRval & LSR_THRE) {
//				UARTTxEmpty0 = 1;
			} else {
//				UARTTxEmpty0 = 0;
			}
			break;

	}

	//	__enable_irq();

}
