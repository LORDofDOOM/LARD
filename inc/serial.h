//////////////////////////////// serial.h ////////////////////////////
//
//
//
//
//

#ifndef SERIAL_H_
#define SERIAL_H_

///////////////////////////////////////////////////////////////////////
//
typedef struct {

	uint8	object_id;

	uint8	port;
	uint32	baudrate;
	uint8	data_bits;
	uint8	parity;
	uint8	stop_bits;
	uint8	rxPin;
	uint8	txPin;
	fifo16 	* RxBuffer;
	fifo16 	* TxBuffer;
	LPC_UART0_Type 	* uart;

	uint8	not_object_id;

} serialConnection;

serialConnection * serialCreate (uint8 port, uint32 baudrate, uint8 data_bits, uint8 parity, uint8 stop_bits,
		uint8 rx_buff_size, uint8 tx_buff_size);
uint32 serialSetUartPins(serialConnection * s, uint32 location);

enum {
	SERIAL_UART0,	// Allow for up to 6 hardware UARTs
	SERIAL_UART1,
	SERIAL_UART2,
	SERIAL_UART3,
	SERIAL_UART4,
	SERIAL_UART5,
	SERIAL_SOFT0,	// Allow for 2 software serial connections
	SERIAL_SOFT1
};

enum {
	UART_DATA_BITS_5 = 5,
	UART_DATA_BITS_6 = 6,
	UART_DATA_BITS_7 = 7,
	UART_DATA_BITS_8 = 8
};

enum {
	UART_STOP_BITS_1 = (0 << 2),
	UART_STOP_BITS_2 = (1 << 2)
};

enum {
	UART_PARITY_NONE	= 0,
	UART_PARITY_ODD		= (1 << 3) | (0 << 4),	// 0x08
	UART_PARITY_EVEN	= (1 << 3) | (1 << 4),	// 0x18
	UART_PARITY_FORCE1	= (1 << 3) | (2 << 4),	// 0x28
	UART_PARITY_FORCE0	= (1 << 3) | (3 << 4),	// 0x38
};

#define MAX_SERIAL_CONNECTIONS	SERIAL_SOFT1

#define	UART0_PINS_R1_T2	0
#define	UART0_PINS_R40_T41	1
#define	UART1_PINS_R8_T9	0
#define	UART1_PINS_R50_T49	1
#define	UART1_PINS_R51_T52	2


#endif /* SERIAL_H_ */
