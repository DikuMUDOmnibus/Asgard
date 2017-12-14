/*
 * global_mods.h
 *
 *  Created on: 6 May 2011
 *      Author: deryck
 */

#ifndef GLOBAL_MODS_H_
#define GLOBAL_MODS_H_

typedef enum gmod_type {
	GMOD_FIRST = 0,
	GMOD_XP = 0,
	GMOD_QP,
	GMOD_MAX,
} GMOD_TYPE;

typedef struct gmod_ticket GMOD_TICKET;
struct gmod_ticket {
	GMOD_TICKET *next;
	GMOD_TYPE type;
	float multiplier;
	time_t expiry_time;
};

void gmod_update(bool force);					// Update procedure for recalculating global modifiers.
float gmod_get_mult(GMOD_TYPE type);			// Get the current multiplier for modifier type.
char * gmod_get_name(GMOD_TYPE type);			// Get the name of the multiplier type.
bool gmod_apply(GMOD_TYPE type, int *value);	// Returns true if there is an active modifier for the type specified.
bool gmod_add(GMOD_TYPE type, float multiplier);// Add a new multiplier to the current multiplier.

extern GMOD_TICKET *gmod_tickets;

#endif /* GLOBAL_MODS_H_ */
