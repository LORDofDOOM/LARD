//////////////////////////////// resources.h ////////////////////////////
//
//
//
//
//

#ifndef RESOURCES_H_
#define RESOURCES_H_

enum {
	RESOURCE_PIN0,
	RESOURCE_PIN1,
	RESOURCE_PIN2,
	RESOURCE_PIN3,
	RESOURCE_PIN4,
	RESOURCE_PIN5,
	RESOURCE_PIN6,
	RESOURCE_PIN7,
	RESOURCE_PIN8,
	RESOURCE_PIN9,
	RESOURCE_PIN10,
	RESOURCE_PIN11,
	RESOURCE_PIN12,
	RESOURCE_PIN13,
	RESOURCE_PIN14,
	RESOURCE_PIN15,
	RESOURCE_PIN16,
	RESOURCE_PIN17,
	RESOURCE_PIN18,
	RESOURCE_PIN19,
	RESOURCE_PIN20,
	RESOURCE_PIN21,
	RESOURCE_PIN22,
	RESOURCE_PIN23,
	RESOURCE_PIN24,
	RESOURCE_PIN25,
	RESOURCE_PIN26,
	RESOURCE_PIN27,
	RESOURCE_PIN28,
	RESOURCE_PIN29,
	RESOURCE_PIN30,
	RESOURCE_PIN31,
	RESOURCE_PIN32,
	RESOURCE_PIN33,
	RESOURCE_PIN34,
	RESOURCE_PIN35,
	RESOURCE_PIN36,
	RESOURCE_PIN37,
	RESOURCE_PIN38,
	RESOURCE_TMR0,
	RESOURCE_TMR1,
	RESOURCE_TMR2,
	RESOURCE_TMR3,

	N_RESOURCES	// used to get array size

} resource_types;

typedef enum {
	RESOURCE_UNUSED,
	RESOURCE_USED,
	RESOURCE_CLASH,
	RESOURCE_MASKED
} resource_states;

uint32 resourceRecordUsage 			(uint32 r);
uint32 resourceCheckForClashes 		(void);
uint32 resourceMask 				(uint32 r);
uint32 resourceUnmask 				(uint32 r);


#endif /* RESOURCES_H_ */
