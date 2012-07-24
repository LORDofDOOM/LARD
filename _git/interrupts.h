//////////////////////////////// interrupts.h ////////////////////////////
//
//
//
//
//

#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

//extern voidFuncPtr 		intFunctions[];
extern uint32	intStatus;

void forceFullInterruptScan (boolean x);

#define 	disableInterrupts()	__disable_irq();
#define 	enableInterrupts()	__enable_irq();

#define		ATOMIC_START		\
		intStatus <<= 1;		\
		intStatus |= 1;			\
		__disable_irq();


#define		ATOMIC_END			\
		intStatus >>= 1;		\
		(intStatus & 1) ?  __disable_irq() : __enable_irq();


#endif /* INTERRUPTS_H_ */
