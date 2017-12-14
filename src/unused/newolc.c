/*
 * newolc.c
 *
 *  Created on: 26 May 2011
 *      Author: Nico
 *  New generic OLC system - shared code.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"
#include "newolc.h"
#include "newclan.h"

////////////////////////////// External imports //////////////////////////////
char *flag_string(const struct flag_type *flag_table, int bits);
int flag_lookup(const char *name, const struct flag_type *flag_table);

////////////////// Generic set/get functions for OLC fields //////////////////
OLC_GET_FUNC_DEF(olc_generic_int_get) {
	snprintf(buf, MAX_STRING_LENGTH, "%d", *(int *)field);
}
OLC_SET_FUNC_DEF(olc_generic_int_set) {
	*(int *)field = *(int *)value;
	return TRUE;
}

OLC_GET_FUNC_DEF(olc_generic_estring_get) {
	snprintf(buf, MAX_STRING_LENGTH, "%s%s%s",
		"{D<see below>\r\n{B------------------------------------------------------------------------{x\r\n",
		*(char **)field,
		"{B------------------------------------------------------------------------");
}
OLC_GET_FUNC_DEF(olc_generic_string_get) {
	strncpy(buf, *(char **)field, MAX_STRING_LENGTH);
}
OLC_SET_FUNC_DEF(olc_generic_string_set) {
	char *str = *(char **)field;
	free_string(str);
	str = str_dup(*(char **)value);
	return TRUE;
}

OLC_GET_FUNC_DEF(olc_generic_flag_get) {
	strncpy(buf, flag_string(schema->flag_table, *(int *)field), MAX_STRING_LENGTH);
}
#define DO_TOG	0
#define DO_SET	1
#define DO_CLR	2
OLC_SET_FUNC_DEF(olc_generic_flag_set) {
	char buf[MAX_STRING_LENGTH];
	char *name, *argument = *(char**)value;
	int flag, action = DO_TOG, *flagfield = (int *)field;

	while (1) {
		argument = one_argument(argument, name);

		// Decide what to do.
		switch (name[0]) {
		case '\0':
			return TRUE;
		case '=':
			name++;
			action = DO_SET;
			break;
		case '-':
			name++;
			action = DO_CLR;
			break;
		default:
			action = DO_TOG;
			break;
		}

		// Find the flag.
		if ((flag = flag_lookup(name, schema->flag_table)) == 0) {
			sprintf(buf, "The flag '%s' does not exist. Skipping.\r\n", name);
			send_to_char(buf, ch);
			continue;
		}

		// Set the flag.
		switch (action) {
		case DO_SET:
			SET_BIT(*flagfield, flag);
			break;
		case DO_CLR:
			REMOVE_BIT(*flagfield, flag);
			break;
		case DO_TOG:
			TOGGLE_BIT(*flagfield, flag);
			break;
		}
	}
	return TRUE;
}

OLC_GET_FUNC_DEF(olc_area_get) {
	AREA_DATA *area = *(AREA_DATA **)field;
	snprintf(buf, MAX_STRING_LENGTH, "%s (vnum: %d)", area->name, area->vnum);
}

// TODO DELETE
#define ED_NONE         0
#define ED_AREA         1
#define ED_ROOM         2
#define ED_OBJECT       3
#define ED_MOBILE       4
#define ED_MPCODE       5
#define ED_HELP         6
#define ED_CLAN         7
// TODO DELETE

/////////////////////////// The meat of the code ///////////////////////////
static OLC_SCHEMA get_editing_schema(CHAR_DATA *ch) {
	if (ch == NULL || ch->desc == NULL)
		return NULL;
	switch (ch->desc->editor) {
	case ED_AREA:
		return OLC_SCHEMA_NAME(AREA_DATA);
	case ED_ROOM:
		return OLC_SCHEMA_NAME(ROOM_INDEX_DATA);
	case ED_OBJECT:
		return OLC_SCHEMA_NAME(OBJ_INDEX_DATA);
//	case ED_MOBILE:
//		break;
//	case ED_MPCODE:
//		break;
//	case ED_HELP:
//		break;
	}
	return NULL;
}

void olc_edit_show(CHAR_DATA *ch, OLC_SCHEMA schema, void *pEdit) {
	char buf[MAX_STRING_LENGTH*2], field_buf[MAX_STRING_LENGTH];
	BUFFER *buffer;
	int i;

	buffer = new_buf();
	sprintf(buf, "{G%-15s {B: {G%s\r\n", "Field", "Value");
	add_buf(buffer, buf);
	for (i = 0; schema[i].name != NULL; i++) {
		if (schema[i].get_func != NULL)
			(*schema[i].get_func)(ch, &schema[i], pEdit + schema[i].offset, field_buf);
		else
			strcpy(field_buf, "{R?? UNKNOWN ??");
		sprintf(buf, "{W%-15s {B: {x%s\r\n",
			capitalize(schema[i].name),
			field_buf);
		add_buf(buffer, buf);
	}

	add_buf(buffer, "{x");
	page_to_char(buffer->string, ch);
	free_buf(buffer);
}


void olc_edit(CHAR_DATA *ch, char *argument) {
	char arg[MAX_STRING_LENGTH], whole_argument[MAX_STRING_LENGTH];
	OLC_SCHEMA schema;
	void *pEdit;

	if ((schema = get_editing_schema(ch)) == NULL
	||  (pEdit = ch->desc->pEdit) == NULL) {
		printf_debug("olc_edit: entered into function with invalid edit pointer/type.\r\n");
		send_to_char("Something has gone wrong. Booting you out of OLC.\r\n", ch);
		ch->desc->pEdit = NULL;
		ch->desc->editor = 0;
		return;
	}

	smash_tilde(argument);
	strcpy(whole_argument, argument);
	argument = one_argument(argument, arg);

	if (arg[0] == '\0') {
		olc_edit_show(ch, schema, pEdit);
		return;
	}

	if (!str_cmp(arg, "done")) {
		ch->desc->pEdit = NULL;
		ch->desc->editor = 0;
		return;
	}

	interpret(ch, whole_argument);
}
