///////////////////////////// arduino.c /////////////////////////////
//
//  
//
//
//

#include "LARD.h"

void attachInterrupt (uint8_t pin, void (*func)(void), int mode) {

	switch (mode) {

		case LOW:
			pinSetInterruptMode(pin, PININT_LOW_LEVEL);
			break;

		case CHANGE:
			pinSetInterruptMode(pin, PININT_BOTH_EDGES);
			break;

		case FALLING:
			pinSetInterruptMode(pin, PININT_FALLING_EDGE);
			break;

		case RISING:
			pinSetInterruptMode(pin, PININT_RISING_EDGE);
			break;

	}

	attachEventHandler(EVENT_PININT_0 + pin, func);
	pinEnableInterrupt(pin);

}

void 	detachInterrupt	(uint8_t pin) {

	pinDisableInterrupt(pin);
	detachEventHandler(EVENT_PININT_0 + pin);

}

void pinMode(uint8_t pin, uint8_t mode) {

	pinConfig ((uint32) pin, (uint32) mode);

}

void Serial_begin (uint8 uart, long baudrate) {
	serialCreate(uart, (uint32)baudrate, UART_DATA_BITS_8, UART_PARITY_NONE, UART_STOP_BITS_1, 10, 10);
}

uint32 Serial_available () {
	return 1;

}

uint32 Serial_flush () {
	return 1;

}

uint32 Serial_peek () {
	return 1;

}

uint32 Serial_read () {
	return 1;
}

uint32 Serial_print () {
	return 1;

}

uint32 Serial_println () {
	return 1;
}

uint16 analogRead(uint8 pin) {
	return adcRead((uint8_t)pin);
}

uint32 millis(void) {
	return msTicks;
}

uint32 micros (void){
	return SysTick->VAL;	// not right, needs to be looked into
}

void delayMillseconds(uint32 delay) {
	uint32 stop = msTicks + delay;

	while (msTicks <= stop);
}


void delay(uint32 ms) {
	uint32 start = millis();

	while (ms > 0) {
		if ((micros() - start) >= 1000) {
			ms--;
			start += 1000;
		}
	}
}
