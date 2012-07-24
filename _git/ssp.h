//////////////////////////////// ssp.h ////////////////////////////
//
//
//
//
//

#ifndef SSP_H_
#define SSP_H_

typedef struct {

	uint8	object_id;

	uint8	data_bits;		// 4-16, number of bits to transmit/receive
	uint8	frame_format;	// 0, SPI frame format
							// 1, Texas Instruments synchronous serial frame format
							// 2, Microwire frame format

	uint8	mode;			// 0, CPOL=0, CPHA=0
							// 1, CPOL=0, CPHA=1
							// 2, CPOL=1, CPHA=0
							// 3, CPOL=1, CPHA=1
	LPC_SSP_Type * port;
	uint32	bit_rate;
	boolean	enabled;
	boolean master;
	boolean	loopback;

	uint8	not_object_id;

} sspConnection;

enum {
	SSP_FRAME_FORMAT_SPI,
	SSP_FRAME_FORMAT_TI,
	SSP_FRAME_FORMAT_MW
};

////////////////////////////////////////////////////////////////////
//
// Bit offsets in the SSP control register 0
//
enum {
	SSP_CR0_DSS,
	SSP_CR0_FRF 	= 4,
	SSP_CR0_CPOL 	= 6,
	SSP_CR0_CPHA,
	SSP_CR0_SCR
};

////////////////////////////////////////////////////////////////////
//
// Bit offsets in the SSP control register 1
//
enum {
	SSP_CR1_LBM,
	SSP_CR1_SSE,
	SSP_CR1_MS,
	SSP_CR1_SOD
};

////////////////////////////////////////////////////////////////////
//
// Bit offsets in the SSP status register
//
enum {
	SSP_SR_TFE,
	SSP_SR_TNF,
	SSP_SR_RNE,
	SSP_SR_RFF,
	SSP_SR_BSY
};

enum {
	SSP_MODE_0,
	SSP_MODE_1,
	SSP_MODE_2,
	SSP_MODE_3
};

sspConnection * sspCreate (uint8 data_bits, uint8 frame_format, uint8 mode, uint32 bit_rate);
uint32 sspWrite 		(sspConnection * s, uint16 * buf, uint8 nVals);
uint32 sspExchange		(sspConnection * s);
uint32 sspRead 			(sspConnection * s, uint16 * buf);
uint32 sspEnable		(sspConnection * s);
uint32 sspDisable		(sspConnection * s);
uint32 sspLoopbackOn	(sspConnection * s);
uint32 sspLoopbackOff	(sspConnection * s);
uint32 sspMisoDisable	(sspConnection * s);
uint32 sspMisoEnable	(sspConnection * s);

#endif /* SSP_H_ */
