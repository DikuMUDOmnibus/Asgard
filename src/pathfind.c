/*
 * pathfind.c
 *
 *  Created on: 28 Dec 2010
 *      Author: Nico
 */

#include <stdlib.h>
#include <stdio.h>
#include "merc.h"
#include "string.h"
#include "pathfind.h"

static char pf_path[PF_MAX_DEPTH+1];
char * get_path() {
	return pf_path;
}

static void compact_directions(int length) {
	int i, w, cnt, compact_length;
	char last_c;
	char compact_buf[6];
	for (i = 0, w = 0; i < length; i++) {
		for (cnt = 0, last_c = pf_path[i]; i < length && pf_path[i] == last_c; i++, cnt++);
		if (cnt > 1) {
			// Produce the compacted form, e.g. "eee" -> "3e".
			sprintf(compact_buf, "%d%c", cnt, last_c);
			// Get the length ("3e" == 2).
			compact_length = strlen(compact_buf);
			// Copy that number of characters into the pf_path starting at w.
			memcpy(pf_path+w, compact_buf, compact_length);
			// Increment by compact_length.
			w += compact_length;
		} else
			pf_path[w++] = last_c;
		// Decrement as the inner loop overshoots by one.
		i--;
	}
	pf_path[w] = '\0';
}

static int visited_counter = 0;
int bfs_pathfind(
		ROOM_INDEX_DATA *start,
		PF_END_COND *end_func,
		PF_FILT_COND *filter_func,
		int max_depth,
		bool compact) {
	static char dir_map[MAX_DIR] =
		{ 'n', 'e', 's', 'w', 'u', 'd' };

	// Sanity check.
	if (start == NULL || end_func == NULL)
		return PF_FAIL;

	// Cap the maximum depth allowed.
	max_depth = UMIN(max_depth, PF_MAX_DEPTH);

	// Increment the visited counter that tags room nodes as visited.
	// We use this so we don't need to do any cleaning up before or after.
	visited_counter++;

	ROOM_INDEX_DATA *room = start, *to_process = start, *next_room;
	int i, depth = 0;

	// Initialise the start depth.
	start->pf_depth = 0;
	while (1) {
		// Check we have more to process.
		if (room == NULL)
			return PF_NO_PATH;

		// Path is too long.
		if ((depth = room->pf_depth) > max_depth)
			return PF_TOO_LONG;

		// Check end condition.
		if ((*end_func)(room)) {
			if (depth == 0)
				return PF_HERE;

			// Backtrace and build the path.
			for (i = depth-1; i >= 0 && room != NULL; i--, room = room->pf_prev)
				pf_path[i] = room->pf_direction;
			pf_path[depth] = '\0';

			if (compact)
				compact_directions(depth);
			return PF_FOUND;
		}

		// Increment the depth for the children.
		depth++;
		// Add children of this room to the end of the processing vector.
		for (i = 0; i < MAX_DIR; i++) {
			// Check the exit exists, points to a room and hasn't already been visited.
			if (room->exit[i] == NULL
			|| (next_room = room->exit[i]->u1.to_room) == NULL
			||  next_room->pf_visited == visited_counter
			||  (filter_func != NULL && (*filter_func)(room->exit[i])))
				continue;

			// Add room to the to_process list.
			to_process->pf_next = next_room;
			// Set the parent of the child.
			next_room->pf_prev = room;
			// Shift the to_process list pointer along.
			to_process = to_process->pf_next;
			// Set the direction to the child from the current room.
			to_process->pf_direction = dir_map[i];
			// Set the current depth.
			to_process->pf_depth = depth;
			// Mark visited on this iteration.
			to_process->pf_visited = visited_counter;
		}
		// Ensure the end of the to_process list is NULL otherwise we could get into a nasty loop.
		to_process->pf_next = NULL;
		// Move onto the next room.
		room = room->pf_next;
	}

	// Shouldn't get here.
	return PF_FAIL;
}

void do_lookup(CHAR_DATA *ch, char *argument) {
	PF_END_COND *end_func;
	char buf[MAX_STRING_LENGTH];

	if (argument[0] == '\0')
	{
		send_to_char("Syntax: lookup <area>\n\r", ch);
		send_to_char("        lookup practice\n\r", ch);
		send_to_char("        lookup train\n\r", ch);
		send_to_char("        lookup gain\n\r", ch);
		send_to_char("        lookup bank\r\n", ch);
		if (!IS_NPC(ch))
			send_to_char("        lookup corpse\n\r", ch);
		if (IS_IMMORTAL( ch ))
			send_to_char("        lookup <vnum>\n\r", ch);
		return;
	}

	if (ch->in_room == NULL)
		return;

	if (!IS_NPC(ch) && !str_cmp(argument, "corpse")) {
		// Lookup corpse - could find the corpse then search for the room its in,
		// but this finds the closest (accessible) corpse to the player.
		// Maybe we could look up whether the person has a corpse first? Or not bother?
		end_func = pf_end_at_corpse(ch->name);
		send_to_char("Directions to your corpse: ", ch);
	} else if (!str_cmp(argument, "practice")) {
		send_to_char("Directions to where you can practice: ", ch);
		end_func = pf_end_at_mobact(ACT_PRACTICE);
	} else if (!str_cmp(argument, "train")) {
		send_to_char("Directions to where you can train: ", ch);
		end_func = pf_end_at_mobact(ACT_TRAIN);
	} else if (!str_cmp(argument, "gain")) {
		send_to_char("Directions to where you can gain skills: ", ch);
		end_func = pf_end_at_mobact(ACT_GAIN);
	} else if (!str_cmp(argument, "bank")) {
		send_to_char("Directions to the nearest bank: ", ch);
		end_func = pf_end_at_mobact(ACT_IS_BANKER);
	} else if (IS_IMMORTAL( ch ) && is_number(argument)) {
		// Lookup room vnum.
		ROOM_INDEX_DATA *room;
		int vnum = atoi(argument);

		if ((room = get_room_index(vnum)) == NULL)
		{
			send_to_char("The room vnum does not exist.\n\r", ch);
			return;
		}

		end_func = pf_end_at_room(room);
		sprintf(buf, "Directions to %d: ", vnum);
		send_to_char(buf, ch);
	}
	else {
		// Area lookup.
		AREA_DATA *area;
		for (area = area_first; area != NULL; area = area->next) {
			// Skip areas that are unlinked/marked as non-lookup areas.
			if (!IS_IMMORTAL(ch)
			&& (strstr(area->builders, "Unlinked") || strstr(area->builders, "Nolookup")))
				continue;
			if (area->name != NULL && is_name(argument, area->name))
				break;
		}

		if (area == NULL) {
			send_to_char("Area not found.\n\r", ch);
			return;
		}

		end_func = pf_end_at_area(area);
		sprintf(buf, "Directions to %s{x: ", area->name);
		send_to_char(buf, ch);
	}

	switch (bfs_pathfind(ch->in_room, end_func, pf_filt_normal(ch), PF_MAX_DEPTH, TRUE)) {
	default:
	case PF_FAIL:
		printf_debug("do_lookup: pathfind returned PF_FAIL (ch: %s, argument: %s).\r\n", ch->name, argument);
		send_to_char("Something unexpected happened. Please tell an immortal.\r\n", ch);
		break;
	case PF_TOO_LONG:
		send_to_char("Its too far away!\r\n", ch);
		break;
	case PF_NO_PATH:
		send_to_char("Cannot find a path.\r\n", ch);
		break;
	case PF_HERE:
		send_to_char("You're already here!\r\n", ch);
		break;
	case PF_FOUND:
		send_to_char(get_path(), ch);
		send_to_char("\r\n", ch);
		break;
	}
}

// Pathfinding termination/filter conditions and their initialisation functions.
CHAR_DATA *pathfind_character;
PF_FILT_COND_DEF(cond_filt_normal) {
	return !can_enter_room(pathfind_character, exit->u1.to_room, TRUE);
}
PF_FILT_COND * pf_filt_normal(CHAR_DATA *ch) {
	pathfind_character = ch;
	return &cond_filt_normal;
}

ROOM_INDEX_DATA *pathfind_room;
PF_END_COND_DEF(cond_end_at_room) { return room == pathfind_room; }
PF_END_COND * pf_end_at_room(ROOM_INDEX_DATA *target)
{
	pathfind_room = target;
	return &cond_end_at_room;
}

AREA_DATA *pathfind_area;
PF_END_COND_DEF(cond_end_at_area) { return room->area != NULL && room->area == pathfind_area; }
PF_END_COND * pf_end_at_area(AREA_DATA *target)
{
	pathfind_area = target;
	return &cond_end_at_area;
}

char *pathfind_corpsename;
PF_END_COND_DEF(cond_end_at_corpse)
{
	OBJ_DATA *obj;
	for (obj = room->contents; obj != NULL; obj = obj->next_content)
		if (obj->item_type == ITEM_CORPSE_PC
		&& strstr(obj->short_descr, pathfind_corpsename))
			return TRUE;
	return FALSE;
}
PF_END_COND * pf_end_at_corpse(char *target)
{
	pathfind_corpsename = target;
	return &cond_end_at_corpse;
}

long pathfind_mobact_type;
PF_END_COND_DEF(cond_end_at_mobact)
{
	CHAR_DATA *mob;
	for (mob = room->people; mob != NULL; mob = mob->next_in_room)
		if (IS_NPC(mob) && IS_SET(mob->act, pathfind_mobact_type))
			return TRUE;
	return FALSE;
}
PF_END_COND * pf_end_at_mobact(long act_flag)
{
	pathfind_mobact_type = act_flag;
	return &cond_end_at_mobact;
}

