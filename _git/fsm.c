///////////////////////////////// fsm.c /////////////////////////////////
//
//  
//
//
//

#include "LARD.h"

byte state_table [3][2] = {
//			EVENT_DN		EVENT_UP
/*STATE_0*/	{ACTION_DN1, 	ACTION_UP1},
/*STATE_1*/	{ACTION_DN2, 	ACTION_UP2},
/*STATE_2*/	{ACTION_WRAP, 	ACTION_WRAP}

};

byte	fsm_state = STATE_0;
byte 	fsm_counter = 0;

void action_dn1 (void) {
	fsm_state = STATE_1;
}

void action_up1 (void) {
	fsm_state = STATE_1;
}

void action_dn2 (void) {
	fsm_counter--;
	fsm_state = STATE_2;
}

void action_up2 (void) {
	fsm_counter++;
	fsm_state = STATE_2;
}

void action_wrap (void) {
	fsm_counter = 0x55;
	fsm_state = STATE_0;
}

const voidFuncPtr actions[6] = {
		action_dn1,
		action_up1,
		action_dn2,
		action_up2,
		action_wrap
};

void fsmHandleEvent (byte event) {
	(actions[state_table[fsm_state][event]]) ();
}
