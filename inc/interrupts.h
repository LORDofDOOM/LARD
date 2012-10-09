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
extern uint8	__interruptLevel;

void forceFullInterruptScan (boolean x);

#define 	disableInterrupts()		ATOMIC_START
#define 	restoreInterrupts()		ATOMIC_END
#define		ATOMIC_START			__disable_irq();__interruptLevel++;
#define		ATOMIC_END				if ((__interruptLevel) && !(--__interruptLevel))__enable_irq();
#define		ATOMIC(x)							\
						ATOMIC_START			\
						x;						\
						ATOMIC_END

#endif /* INTERRUPTS_H_ */
