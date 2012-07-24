//////////////////////////////// timer.h ////////////////////////////
//
//
//
//
//

#ifndef TIMER_H_
#define TIMER_H_

typedef struct _hwTimer hwTimer;

struct _hwTimer {

	uint8			object_id;

	LPC_CTxxBx_Type * timer;
	uint32			reload_val;
	uint32			max_val;
	uint8			mode;
	void 			(*callback_func)(struct _hwTimer *);

	uint8			not_object_id;

};

typedef enum {
	HWTIMER_MODE_ASTABLE,		// Timer is reloaded on timeout
	HWTIMER_MODE_MONOSTABLE = 4	// Timer is not reloaded on timeout
} hwTimer_types;


hwTimer * hwTimerCreate 		(uint8 tmr, uint32 reload_val, uint8 mode, void (*expire_func)(struct _hwTimer *));
uint32 	hwTimerAttachCallback 	(hwTimer * t, void (*callback_func)(struct _hwTimer *));
uint32	hwTimerSetReLoadVal 	(hwTimer * t, uint32 reload_val);
uint32	hwTimerStart 			(hwTimer * t);
uint32	hwTimerRestart 			(hwTimer * t);
uint32	hwTimerStop 			(hwTimer * t);

#endif /* TIMER_H_ */
