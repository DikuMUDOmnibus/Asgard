/*
 * global_mods.c
 *
 *  Created on: 6 May 2011
 *      Author: deryck
 */

#include <stdlib.h>
#include <stdio.h>
#include "merc.h"
#include "global_mods.h"

DECLARE_DO_FUN(do_announce);

#define GMOD_FILE				"globalmods.txt"
#define GMOD_MAX_MULT			3.0f
#define GMOD_UPDATE_INTERVAL	2*PULSE_TICK		// Roughly every minute.
#define GMOD_TICKET_DURATION	60*60				// One hour.

GMOD_TICKET *gmod_tickets;
static GMOD_TICKET *gmod_free_tickets;

static float gmod_current[GMOD_MAX];
static bool gmod_initialised = FALSE;
static int gmod_update_counter = GMOD_UPDATE_INTERVAL;	// Ensures it runs on first update.

static GMOD_TICKET *gmod_new_ticket(GMOD_TYPE type, float multiplier, time_t expiry_time) {
	GMOD_TICKET *ticket;

	if (gmod_free_tickets == NULL)
		ticket = malloc(sizeof(GMOD_TICKET));
	else {
		ticket = gmod_free_tickets;
		gmod_free_tickets = gmod_free_tickets->next;
	}

	if (ticket != NULL) {
		ticket->type = type;
		ticket->multiplier = multiplier;
		ticket->expiry_time = expiry_time;
	}

	return ticket;
}

static void gmod_free_ticket(GMOD_TICKET *ticket) {
	if (ticket != NULL) {
		ticket->next = gmod_free_tickets;
		gmod_free_tickets = ticket;
	}
}

static void gmod_save() {
	FILE *fp;

	if ((fp = get_temp_file()) == NULL) {
		printf_debug("gmod_save: could not open temporary file.");
		return;
	}

	GMOD_TICKET *ticket;
	for (ticket = gmod_tickets; ticket != NULL; ticket = ticket->next) {
		fprintf(fp, "%d %d %ld\n", ticket->type,
			(int)(ticket->multiplier*100.0f), ticket->expiry_time);
	}
	fprintf(fp, "-1\n");

	if (!close_write_file(GMOD_FILE))
		printf_debug("gmod_save: could not save file as "GMOD_FILE".");
}

static void gmod_load() {
	FILE *fp;

	gmod_initialised = TRUE;
	if ((fp = fopen(GMOD_FILE, "r")) == NULL) {
		printf_debug("gmod_load: could not open "GMOD_FILE".");
		return;
	}

	GMOD_TICKET *ticket;
	int type, time;
	float multiplier;
	while ((type = fread_number(fp)) >= 0) {
		multiplier = ((float)fread_number(fp))/100.0f;
		time = fread_number(fp);
		ticket = gmod_new_ticket(type, multiplier, time);
		if (ticket == NULL) {
			printf_debug("gmod_load: couldn't allocate memory for a ticket.");
			return;
		}
		ticket->next = gmod_tickets;
		gmod_tickets = ticket;
	}

	fclose(fp);
}

void gmod_update(bool force) {
	GMOD_TICKET *ticket, *ticket_prev = NULL, *ticket_next;

	// Update every GMOD_UPDATE_INTERVAL.
	if (++gmod_update_counter >= GMOD_UPDATE_INTERVAL || force)
		gmod_update_counter = 0;
	else
		return;

	// Load any saved values.
	if (!gmod_initialised)
		gmod_load();

	// Reset multipliers to 1x.
	int i;
	for (i = 0; i < GMOD_MAX; i++)
		gmod_current[i] = 1.0f;

	// Loop through tickets and update the modifiers.
	bool changed = FALSE;
	for (ticket = gmod_tickets; ticket != NULL; ticket = ticket_next) {
		ticket_next = ticket->next;

		if (ticket->expiry_time <= current_time || ticket->multiplier <= 0.0f
		||  ticket->type < 0 || ticket->type >= GMOD_MAX) {
			char buf[MAX_STRING_LENGTH];
			// Remove expired/invalid ticket.
			if (ticket_prev != NULL)
				ticket_prev->next = ticket_next;
			else
				gmod_tickets = ticket_next;
			sprintf(buf, "A {G+%.2f{Yx %s modifier ticket has {rexpired{Y.",
				ticket->multiplier, gmod_get_name(ticket->type));
			do_announce(NULL, buf);
			gmod_free_ticket(ticket);
			changed = TRUE;
			continue;
		}

		gmod_current[ticket->type] += ticket->multiplier;
		ticket_prev = ticket;
	}

	// Save any changes.
	if (changed || force)
		gmod_save();
}

#define CHECK_TYPE_BOUNDS(Type, RetVal)					\
	if ((Type) < 0 || (Type) >= GMOD_MAX) {				\
		printf_debug("Invalid GMOD_TYPE: %d.", (Type));	\
		return RetVal;									\
	}

float gmod_get_mult(GMOD_TYPE type) {
	CHECK_TYPE_BOUNDS(type, 1.0f);
	return gmod_current[type];
}

char * gmod_get_name(GMOD_TYPE type) {
	static char *typenames[] = {
		"experience",
		"quest rewards",
	};
	CHECK_TYPE_BOUNDS(type, "unknown");
	return typenames[type];
}

bool gmod_apply(GMOD_TYPE type, int *value) {
	float mult = gmod_get_mult(type);
	if (mult > 1.0f) {
		if (value != NULL && *value > 0)
			*value = (int) (((float)*value) * mult);
		return TRUE;
	}
	return FALSE;
}

bool gmod_add(GMOD_TYPE type, float multiplier) {
	CHECK_TYPE_BOUNDS(type, FALSE);
	float current_mult = gmod_get_mult(type);

	if (current_mult + multiplier > GMOD_MAX_MULT)
		return FALSE;

	GMOD_TICKET *ticket = gmod_new_ticket(type, multiplier, current_time+GMOD_TICKET_DURATION);
	if (ticket == NULL) {
		printf_debug("gmod_add: couldn't allocate memory for a ticket.");
		return FALSE;
	}

	// Add ticket to queue, force an update and save state.
	ticket->next = gmod_tickets;
	gmod_tickets = ticket;
	gmod_update(TRUE);
	return TRUE;
}
