/*
 * newolc_room.c
 *
 *  Created on: 27 May 2011
 *      Author: Nico
 *  Room schema and custom functions for OLC.
 */

#include <stdlib.h>
#include <stdio.h>
#include "merc.h"
#include "newolc.h"
#include "tables.h"

//////////////////////////// Function Prototypes ////////////////////////////
OLC_GET_FUNC_DEF(olc_room_north_get);
OLC_SET_FUNC_DEF(olc_room_north_set);
OLC_GET_FUNC_DEF(olc_room_east_get);
OLC_SET_FUNC_DEF(olc_room_east_set);
OLC_GET_FUNC_DEF(olc_room_south_get);
OLC_SET_FUNC_DEF(olc_room_south_set);
OLC_GET_FUNC_DEF(olc_room_west_get);
OLC_SET_FUNC_DEF(olc_room_west_set);
OLC_GET_FUNC_DEF(olc_room_up_get);
OLC_SET_FUNC_DEF(olc_room_up_set);
OLC_GET_FUNC_DEF(olc_room_down_get);
OLC_SET_FUNC_DEF(olc_room_down_set);

/* struct room_index_data
{
	sh_int affect_room_type;
	ROOM_INDEX_DATA * next;
	CHAR_DATA * people;
	OBJ_DATA * contents;
	EXTRA_DESCR_DATA * extra_descr;
	AREA_DATA * area;
	EXIT_DATA * exit[12];
	EXIT_DATA * old_exit[12];
	RESET_DATA * reset_first;
	RESET_DATA * reset_last;
	char * name;
	char * description;
	char * owner;
	sh_int vnum;
	int room_flags;
	sh_int light;
	sh_int sector_type;
	sh_int heal_rate;
	sh_int mana_rate;
	int tele_dest;
	AFFECT_DATA * affected;
	int affected_by;

	// Pathfinding stuff.
	ROOM_INDEX_DATA * pf_next;
	ROOM_INDEX_DATA * pf_prev;
	long pf_visited;
	int pf_depth;
	char pf_direction;
};
 */
OLC_SCHEMA_START(ROOM_INDEX_DATA)
	OLC_EDIT_FIELD(area, olc_area_get, NULL, 0, NULL)
	OLC_RO_INT_FIELD(vnum)
	OLC_STRING_FIELD(name)
	OLC_INT_FIELD(heal_rate)
	OLC_INT_FIELD(mana_rate)
	OLC_FLAG_FIELD(room_flags, room_flags)
	OLC_ESTRING_FIELD(description)
	OLC_SYM_FIELD(north, olc_room_north_get, olc_room_north_set)
	OLC_SYM_FIELD(east, olc_room_east_get, olc_room_east_set)
	OLC_SYM_FIELD(south, olc_room_south_get, olc_room_south_set)
	OLC_SYM_FIELD(west, olc_room_west_get, olc_room_west_set)
	OLC_SYM_FIELD(up, olc_room_up_get, olc_room_up_set)
	OLC_SYM_FIELD(down, olc_room_down_get, olc_room_down_set)
OLC_SCHEMA_END


////////////////////////////// Get/set functions //////////////////////////////
OLC_GET_FUNC_DEF(olc_room_north_get) {
	buf[0] = '\0';	// TODO: implement.
}
OLC_SET_FUNC_DEF(olc_room_north_set) {
	return TRUE;
}

OLC_GET_FUNC_DEF(olc_room_east_get) {
	buf[0] = '\0';	// TODO: implement.
}
OLC_SET_FUNC_DEF(olc_room_east_set) {
	return TRUE;
}

OLC_GET_FUNC_DEF(olc_room_south_get) {
	buf[0] = '\0';	// TODO: implement.
}
OLC_SET_FUNC_DEF(olc_room_south_set) {
	return TRUE;
}

OLC_GET_FUNC_DEF(olc_room_west_get) {
	buf[0] = '\0';	// TODO: implement.
}
OLC_SET_FUNC_DEF(olc_room_west_set) {
	return TRUE;
}

OLC_GET_FUNC_DEF(olc_room_up_get) {
	buf[0] = '\0';	// TODO: implement.
}
OLC_SET_FUNC_DEF(olc_room_up_set) {
	return TRUE;
}

OLC_GET_FUNC_DEF(olc_room_down_get) {
	buf[0] = '\0';	// TODO: implement.
}
OLC_SET_FUNC_DEF(olc_room_down_set) {
	return TRUE;
}
