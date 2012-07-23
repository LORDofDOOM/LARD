//////////////////////////////// fsm.h ////////////////////////////
//
//
//
//
//

#ifndef FSM_H_
#define FSM_H_

void fsmHandleEvent (byte event);

enum {
	STATE_0,
	STATE_1,
	STATE_2
};

enum {
	EVENT_DN,
	EVENT_UP
};

enum {
	ACTION_DN1,
	ACTION_UP1,
	ACTION_DN2,
	ACTION_UP2,
	ACTION_WRAP
};

#endif /* FSM_H_ */
