/*
 * newolc_area.c
 *
 *  Created on: 27 May 2011
 *      Author: Nico
 *  Area schema and custom functions for OLC.
 */

#include <stdlib.h>
#include <stdio.h>
#include "merc.h"
#include "newolc.h"
#include "tables.h"

/* struct area_data
{
	AREA_DATA * next;
	RESET_DATA * reset_first;
	RESET_DATA * reset_last;
	char * file_name;
	char * name;
	char * credits;
	sh_int age;
	sh_int nplayer;
	sh_int low_range;
	sh_int high_range;
	sh_int min_vnum;
	sh_int max_vnum;
	bool empty;
	char * builders; // OLC  Listing of
	int vnum; // OLC Area Vnum
	int area_flags; // OLC
	int security; // OLC - Value 1 - 9
	int continent; // OLC - Defined Values
	HELP_AREA * helps; // Hedit
};
 */
OLC_SCHEMA_START(AREA_DATA)
	OLC_RO_INT_FIELD(vnum)
	OLC_STRING_FIELD(name)
	OLC_STRING_FIELD(file_name)
	OLC_INT_FIELD(min_vnum)
	OLC_INT_FIELD(max_vnum)
	OLC_INT_FIELD(security)
	OLC_INT_FIELD(continent)
	OLC_INT_FIELD(low_range)
	OLC_INT_FIELD(high_range)
	OLC_STRING_FIELD(credits)
	OLC_FLAG_FIELD(area_flags, area_flags)
OLC_SCHEMA_END


////////////////////////////// Get/set functions //////////////////////////////
