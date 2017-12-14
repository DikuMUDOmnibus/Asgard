/*
 * newolc_obj.c
 *
 *  Created on: 27 May 2011
 *      Author: Nico
 *  Object schema and custom functions for OLC.
 */

#include <stdlib.h>
#include <stdio.h>
#include "merc.h"
#include "newolc.h"
#include "tables.h"

/* struct obj_index_data
{
	OBJ_INDEX_DATA * next;
	EXTRA_DESCR_DATA * extra_descr;
	AFFECT_DATA * affected;
	AREA_DATA * area; // OLC
	bool new_format;
	char * name;
	char * short_descr;
	char * description;
	sh_int vnum;
	sh_int reset_num;
	char * material;
	sh_int item_type;
	int extra_flags;
	int wear_flags;
	sh_int level;
	sh_int condition;
	sh_int count;
	sh_int weight;
	int cost;
	int value[5];
	sh_int class;
	sh_int timer;
	int class_restrict_flags;
};
*/
OLC_SCHEMA_START(OBJ_INDEX_DATA)
	OLC_EDIT_FIELD(area, olc_area_get, NULL, 0, NULL)
	OLC_RO_INT_FIELD(vnum)
	OLC_STRING_FIELD(name)
	OLC_FLAG_FIELD(item_type, type_flags)					// TODO: fix so only one type allowed.
	OLC_INT_FIELD(level)
	OLC_INT_FIELD(condition)
	OLC_INT_FIELD(weight)
	OLC_INT_FIELD(cost)
	OLC_STRING_FIELD(material)
	OLC_STRING_FIELD(short_descr)
	OLC_STRING_FIELD(description)
	OLC_FLAG_FIELD(extra_flags, extra_flags)
	OLC_FLAG_FIELD(wear_flags, wear_flags)
OLC_SCHEMA_END



////////////////////////////// Get/set functions //////////////////////////////
