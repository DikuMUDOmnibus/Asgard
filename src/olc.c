/*************************************************************************** 
 *  File: olc.c                                                            *  
 *                                                                         *  
 *  Much time and thought has gone into this software and you are          *  
 *  benefitting.  We hope that you share your changes too.  What goes      *  
 *  around, comes around.                                                  *  
 *                                                                         *  
 *  This code was freely distributed with the The Isles 1.1 source code,   *  
 *  and has been used here for OLC - OLC would not be what it is without   *  
 *  all the previous coders who released their source code.                *  
 *                                                                         *  
 ***************************************************************************/

#if defined(macintosh)  
#include <types.h>  
#else  
#include <sys/types.h>  
#endif  
#include <ctype.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <time.h>  
#include "merc.h"  
#include "tables.h"  
#include "olc.h"  
#include "newclan.h"
#include "recycle.h"

long obj_points(OBJ_INDEX_DATA * obj);
long obj_balance(OBJ_INDEX_DATA * obj);

/*  
 * Local functions.  
 */
AREA_DATA *get_area_data( int vnum );

/* Executed from comm.c.  Minimizes compiling when changes are made. */
bool run_olc_editor(DESCRIPTOR_DATA *d)
{
	switch (d->editor)
	{
	case ED_AREA:
		aedit(d->character, d->incomm);
		break;
	case ED_ROOM:
		redit(d->character, d->incomm);
		break;
	case ED_OBJECT:
		oedit(d->character, d->incomm);
		break;
	case ED_MOBILE:
		medit(d->character, d->incomm);
		break;
	case ED_MPCODE:
		mpedit(d->character, d->incomm);
		break;
	case ED_HELP:
		hedit(d->character, d->incomm);
		break;
	case ED_CEDIT:
		cedit( d->character, d->incomm );
		break;
	case ED_MCEDIT:
		mcedit( d->character, d->incomm );
		break;
	case ED_OCEDIT:
		ocedit( d->character, d->incomm );
		break;
	case ED_RCEDIT:
		rcedit( d->character, d->incomm );
		break;
	case ED_SCEDIT:
		scedit( d->character, d->incomm );
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

char *olc_ed_name(CHAR_DATA *ch)
{
	static char buf[10];

	buf[0] = '\0';
	switch (ch->desc->editor)
	{
	case ED_AREA:
		sprintf(buf, "AEdit ");
		break;
	case ED_ROOM:
		sprintf(buf, "REdit ");
		break;
	case ED_OBJECT:
		sprintf(buf, "OEdit ");
		break;
	case ED_MOBILE:
		sprintf(buf, "MEdit ");
		break;
	case ED_MPCODE:
		sprintf(buf, "MPEdit ");
		break;
	case ED_HELP:
		sprintf(buf, "HEdit ");
		break;
	case ED_CLAN:
		sprintf(buf, "CEdit ");
		break;
	default:
		sprintf(buf, " ");
		break;
	}
	return buf;
}

char *olc_ed_vnum(CHAR_DATA *ch)
{
	AREA_DATA *pArea;
	ROOM_INDEX_DATA *pRoom;
	OBJ_INDEX_DATA *pObj;
	MOB_INDEX_DATA *pMob;
	MPROG_CODE *pMprog;
	HELP_DATA *pHelp;
	static char buf[10];

	buf[0] = '\0';
	switch (ch->desc->editor)
	{
	case ED_AREA:
		pArea = (AREA_DATA *) ch->desc->pEdit;
		sprintf(buf, "%d", pArea ? pArea->vnum : 0);
		break;
	case ED_ROOM:
		pRoom = ch->in_room;
		sprintf(buf, "%d", pRoom ? pRoom->vnum : 0);
		break;
	case ED_OBJECT:
		pObj = (OBJ_INDEX_DATA *) ch->desc->pEdit;
		sprintf(buf, "%d", pObj ? pObj->vnum : 0);
		break;
	case ED_MOBILE:
		pMob = (MOB_INDEX_DATA *) ch->desc->pEdit;
		sprintf(buf, "%d", pMob ? pMob->vnum : 0);
		break;
	case ED_MPCODE:
		pMprog = (MPROG_CODE *) ch->desc->pEdit;
		sprintf(buf, "%d", pMprog ? pMprog->vnum : 0);
		break;
	case ED_HELP:
		pHelp = (HELP_DATA *) ch->desc->pEdit;
		sprintf(buf, "%s", pHelp ? pHelp->keyword : "");
		break;
	default:
		sprintf(buf, " ");
		break;
	}

	return buf;
}

bool oedit_check(CHAR_DATA *ch)
{
	OBJ_INDEX_DATA *pObj;
	if (ch->desc->editor != ED_OBJECT)
	{
		return TRUE;
	}
	if (ch->desc->editor == ED_OBJECT)
	{
		pObj = (OBJ_INDEX_DATA *) ch->desc->pEdit;
		if (!IS_TRUSTED(ch,CREATOR) && ch->pcdata->security < 9
				&& (obj_balance(pObj) > atoi(MAX_OEDIT_BALANCE) || obj_balance(
						pObj) < atoi(MIN_OEDIT_BALANCE)))
		{
			return FALSE;
		}
		else if (!is_clan_obj_ind(pObj) && ch->pcdata->security < 5
				&& (obj_balance(pObj) > atoi(MAX_OEDIT_BALANCE) || obj_balance(
						pObj) < atoi(MIN_OEDIT_BALANCE)))
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	return FALSE;
}

/*****************************************************************************  
 Name:          show_olc_cmds  
 Purpose:       Format up the commands from given table.  
 Called by:     show_commands(olc_act.c).  
 ****************************************************************************/
void show_olc_cmds(CHAR_DATA *ch, const struct olc_cmd_type *olc_table)
{
	char buf[MAX_STRING_LENGTH];
	char buf1[MAX_STRING_LENGTH];
	int cmd;
	int col;

	buf1[0] = '\0';
	col = 0;
	for (cmd = 0; olc_table[cmd].name != NULL; cmd++)
	{
		sprintf(buf, "%-15.15s", olc_table[cmd].name);
		strcat(buf1, buf);
		if (++col % 5 == 0)
			strcat(buf1, "\n\r");
	}

	if (col % 5 != 0)
		strcat(buf1, "\n\r");

	send_to_char(buf1, ch);
	return;
}

/*****************************************************************************  
 Name:          show_commands  
 Purpose:       Display all olc commands.  
 Called by:     olc interpreters.  
 ****************************************************************************/
bool show_commands(CHAR_DATA *ch, char *argument)
{
	switch (ch->desc->editor)
	{
	case ED_AREA:
		show_olc_cmds(ch, aedit_table);
		break;
	case ED_ROOM:
		show_olc_cmds(ch, redit_table);
		break;
	case ED_OBJECT:
		show_olc_cmds(ch, oedit_table);
		break;
	case ED_MOBILE:
		show_olc_cmds(ch, medit_table);
		break;
	case ED_MPCODE:
		show_olc_cmds(ch, mpedit_table);
		break;
	case ED_HELP:
		show_olc_cmds(ch, hedit_table);
		break;
	}

	return FALSE;
}

/*****************************************************************************  
 *                           Interpreter Tables.                             *  
 *****************************************************************************/
const struct olc_cmd_type aedit_table[] =
{
/*  {   command         function        }, */

{ "age", aedit_age },
{ "builder", aedit_builder }, /* s removed -- Hugin */
{ "commands", show_commands },
{ "create", aedit_create },
{ "filename", aedit_file },
{ "name", aedit_name },
/*  {   "recall",       aedit_recall    },   ROM OLC */
{ "reset", aedit_reset },
{ "security", aedit_security },
{ "continent", aedit_continent },
{ "mobtier", aedit_mobtier },
{ "tier", aedit_mobtier },
{ "show", aedit_show },
{ "vnum", aedit_vnum },
{ "lvnum", aedit_lvnum },
{ "uvnum", aedit_uvnum },
{ "credits", aedit_credits },

{ "?", show_help },
{ "version", show_version },

{ NULL, 0, } };

const struct olc_cmd_type redit_table[] =
{
/*  {   command         function        }, */

{ "commands", show_commands },
{ "create", redit_create },
{ "desc", redit_desc },
{ "ed", redit_ed },
{ "format", redit_format },
{ "name", redit_name },
{ "show", redit_show },
{ "heal", redit_heal },
{ "mana", redit_mana },

{ "north", redit_north },
{ "south", redit_south },
{ "east", redit_east },
{ "west", redit_west },
{ "up", redit_up },
{ "down", redit_down },

/* New reset commands. */
{ "mreset", redit_mreset },
{ "oreset", redit_oreset },
{ "mshow", redit_mshow },
{ "oshow", redit_oshow },
{ "owner", redit_owner },
{ "room", redit_room },
{ "sector", redit_sector },

{ "?", show_help },
{ "version", show_version },
{ "copy", redit_copy },

{ NULL, 0, } };

const struct olc_cmd_type oedit_table[] =
{
/*  {   command         function        }, */

{ "addaffect", oedit_addaffect },
{ "addapply", oedit_addapply },
{ "commands", show_commands },
{ "cost", oedit_cost },
{ "create", oedit_create },
{ "delaffect", oedit_delaffect },
{ "ed", oedit_ed },
{ "long", oedit_long },
{ "name", oedit_name },
{ "short", oedit_short },
{ "show", oedit_show },
{ "v0", oedit_value0 },
{ "v1", oedit_value1 },
{ "v2", oedit_value2 },
{ "v3", oedit_value3 },
{ "v4", oedit_value4 }, /* ROM */
{ "weight", oedit_weight },

{ "extra", oedit_extra }, /* ROM */
{ "wear", oedit_wear }, /* ROM */
{ "type", oedit_type }, /* ROM */
{ "material", oedit_material }, /* ROM */
{ "level", oedit_level }, /* ROM */
{ "condition", oedit_condition }, /* ROM */

{ "?", show_help },
{ "version", show_version },
{ "copy", oedit_copy },
{ "delete", oedit_delete },

{ NULL, 0, } };

const struct olc_cmd_type medit_table[] =
{
/*  {   command         function        }, */

{ "alignment", medit_align },
{ "commands", show_commands },
{ "create", medit_create },
{ "desc", medit_desc },
{ "level", medit_level },
{ "long", medit_long },
{ "mobtier", medit_mobtier },
{ "tier", medit_mobtier },
{ "name", medit_name },
{ "shop", medit_shop },
{ "short", medit_short },
{ "show", medit_show },
{ "spec", medit_spec },
{ "shield", medit_shield },
{ "attacks", medit_attacks },

{ "sex", medit_sex }, /* ROM */
{ "act", medit_act }, /* ROM */
{ "affect", medit_affect }, /* ROM */
{ "armor", medit_ac }, /* ROM */
{ "form", medit_form }, /* ROM */
{ "part", medit_part }, /* ROM */
{ "imm", medit_imm }, /* ROM */
{ "res", medit_res }, /* ROM */
{ "vuln", medit_vuln }, /* ROM */
{ "material", medit_material }, /* ROM */
{ "off", medit_off }, /* ROM */
{ "size", medit_size }, /* ROM */
{ "hitdice", medit_hitdice }, /* ROM */
{ "manadice", medit_manadice }, /* ROM */
{ "damdice", medit_damdice }, /* ROM */
{ "race", medit_race }, /* ROM */
{ "position", medit_position }, /* ROM */
{ "wealth", medit_gold },
{ "hitroll", medit_hitroll }, /* ROM */
{ "damtype", medit_damtype }, /* ROM */
{ "group", medit_group }, /* ROM */
{ "addmprog", medit_addmprog }, /* ROM */
{ "delmprog", medit_delmprog }, /* ROM */
{ "seemort", medit_seemort },

{ "?", show_help },
{ "version", show_version },
{ "copy", medit_copy },
{ "delete", medit_delete },

{ NULL, 0, } };

/* Help Editor - kermit 1/98   
 const struct olc_cmd_type hedit_table[] =
 {
 {   command         function        },

 { "commands", show_commands  },
 { "desc",     hedit_desc     },
 { "keywords", hedit_keywords },
 { "level",    hedit_level    },
 { "make",     hedit_make     },
 { "show",     hedit_show     },
 { "delete",   hedit_delete   },
 {   "?",      show_help      },

 {   NULL,     0, }
 }; */

/*****************************************************************************  
 *                          End Interpreter Tables.                          *  
 *****************************************************************************/

/*****************************************************************************  
 Name:          get_area_data  
 Purpose:       Returns pointer to area with given vnum.  
 Called by:     do_aedit(olc.c).  
 ****************************************************************************/
AREA_DATA *get_area_data(int vnum)
{
	AREA_DATA *pArea;

	for (pArea = area_first; pArea; pArea = pArea->next)
	{
		if (pArea->vnum == vnum)
			return pArea;
	}

	return 0;
}

/*****************************************************************************  
 Name:          edit_done  
 Purpose:       Resets builder information on completion.  
 Called by:     aedit, redit, oedit, medit(olc.c)  
 ****************************************************************************/
bool edit_done(CHAR_DATA *ch)
{
	ch->desc->clanEdit = NULL;
	ch->desc->pEdit = NULL;
	ch->desc->editor = 0;
	return FALSE;
}

/*****************************************************************************  
 *                              Interpreters.                                *  
 *****************************************************************************/

/* Area Interpreter, called by do_aedit. */
void aedit(CHAR_DATA *ch, char *argument)
{
	AREA_DATA *pArea;
	char command[MAX_INPUT_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	int cmd;
	int value;

	EDIT_AREA(ch, pArea);
	smash_tilde(argument);
	strcpy(arg, argument);
	argument = one_argument(argument, command);

	if (!IS_BUILDER( ch, pArea ))
	{
		send_to_char("AEdit:  Insufficient security to modify area.\n\r", ch);
		edit_done(ch);
		return;
	}

	if (!str_cmp(command, "done"))
	{
		edit_done(ch);
		return;
	}

	if (command[0] == '\0')
	{
		aedit_show(ch, argument);
		return;
	}

	if ((value = flag_value(area_flags, command)) != NO_FLAG)
	{
		TOGGLE_BIT(pArea->area_flags, value);

		send_to_char("Flag toggled.\n\r", ch);
		return;
	}

	/* Search Table and Dispatch Command. */
	for (cmd = 0; aedit_table[cmd].name != NULL; cmd++)
	{
		if (!str_prefix(command, aedit_table[cmd].name))
		{
			printf_player(ch, "(AEDIT) %s", arg);
			if ((*aedit_table[cmd].olc_fun)(ch, argument))
			{
				SET_BIT( pArea->area_flags, AREA_CHANGED );
				return;
			}
			else
				return;
		}
	}

	/* Default to Standard Interpreter. */
	interpret(ch, arg);
	return;
}

/* Room Interpreter, called by do_redit. */
void redit(CHAR_DATA *ch, char *argument)
{
	AREA_DATA *pArea;
	ROOM_INDEX_DATA *pRoom;
	char arg[MAX_STRING_LENGTH];
	char command[MAX_INPUT_LENGTH];
	int cmd;

	EDIT_ROOM(ch, pRoom);
	pArea = pRoom->area;

	smash_tilde(argument);
	strcpy(arg, argument);
	argument = one_argument(argument, command);

	if (!IS_BUILDER( ch, pArea ))
	{
		send_to_char("REdit:  Insufficient security to modify room.\n\r", ch);
		edit_done(ch);
		return;
	}

	if (!str_cmp(command, "done"))
	{
		edit_done(ch);
		return;
	}

	if (command[0] == '\0')
	{
		redit_show(ch, argument);
		return;
	}

	/* Search Table and Dispatch Command. */
	for (cmd = 0; redit_table[cmd].name != NULL; cmd++)
	{
		if (!str_prefix(command, redit_table[cmd].name))
		{
			printf_player(ch, "(REDIT) %s", arg);
			if ((*redit_table[cmd].olc_fun)(ch, argument))
			{
				SET_BIT( pArea->area_flags, AREA_CHANGED );
				return;
			}
			else
				return;
		}
	}

	/* Default to Standard Interpreter. */
	interpret(ch, arg);
	return;
}

/* Object Interpreter, called by do_oedit. */
void oedit(CHAR_DATA *ch, char *argument)
{
	AREA_DATA *pArea;
	OBJ_INDEX_DATA *pObj;
	char arg[MAX_STRING_LENGTH];
	char command[MAX_INPUT_LENGTH];
	int cmd;

	smash_tilde(argument);
	strcpy(arg, argument);
	argument = one_argument(argument, command);

	EDIT_OBJ(ch, pObj);
	pArea = pObj->area;

	if (!IS_BUILDER( ch, pArea ))
	{
		send_to_char("OEdit: Insufficient security to modify area.\n\r", ch);
		edit_done(ch);
		return;
	}

	if (!str_cmp(command, "done"))
	{
		if (!oedit_check(ch))
		{
			send_to_char(
					"OEdit: You must balance this eq before you are done.\n\r",
					ch);
			//return;	// Commented out until balance code is sorted out.
		}
		edit_done(ch);
		return;
	}

	if (command[0] == '\0')
	{
		oedit_show(ch, argument);
		return;
	}

	/* Search Table and Dispatch Command. */
	for (cmd = 0; oedit_table[cmd].name != NULL; cmd++)
	{
		if (!str_prefix(command, oedit_table[cmd].name))
		{
			printf_player(ch, "(OEDIT) %s", arg);
			if ((*oedit_table[cmd].olc_fun)(ch, argument))
			{
				SET_BIT( pArea->area_flags, AREA_CHANGED );
				return;
			}
			else
				return;
		}
	}

	/* Default to Standard Interpreter. */
	interpret(ch, arg);
	return;
}

/* Mobile Interpreter, called by do_medit. */
void medit(CHAR_DATA *ch, char *argument)
{
	AREA_DATA *pArea;
	MOB_INDEX_DATA *pMob;
	char command[MAX_INPUT_LENGTH];
	char arg[MAX_STRING_LENGTH];
	int cmd;

	smash_tilde(argument);
	strcpy(arg, argument);
	argument = one_argument(argument, command);

	EDIT_MOB(ch, pMob);
	pArea = pMob->area;

	if (!IS_BUILDER( ch, pArea ))
	{
		send_to_char("MEdit: Insufficient security to modify area.\n\r", ch);
		edit_done(ch);
		return;
	}

	if (!str_cmp(command, "done"))
	{
		edit_done(ch);
		return;
	}

	if (command[0] == '\0')
	{
		medit_show(ch, argument);
		return;
	}

	/* Search Table and Dispatch Command. */
	for (cmd = 0; medit_table[cmd].name != NULL; cmd++)
	{
		if (!str_prefix(command, medit_table[cmd].name))
		{
			printf_player(ch, "(MEDIT) %s", arg);
			if ((*medit_table[cmd].olc_fun)(ch, argument))
			{
				SET_BIT( pArea->area_flags, AREA_CHANGED );
				return;
			}
			else
				return;
		}
	}

	/* Default to Standard Interpreter. */
	interpret(ch, arg);
	return;
}

const struct editor_cmd_type editor_table[] =
{
/*  {   command         function        }, */

{ "area", do_aedit },
{ "room", do_redit },
{ "object", do_oedit },
{ "mobile", do_medit },
{ "mpcode", do_mpedit },
{ "help", do_hedit },

{ NULL, 0, } };

/* Entry point for all editors. */
void do_olc(CHAR_DATA *ch, char *argument)
{
	char command[MAX_INPUT_LENGTH];
	int cmd;

	if (IS_NPC(ch))
		return;

	argument = one_argument(argument, command);

	if (command[0] == '\0')
	{
		do_help(ch, "olc");
		return;
	}

	/* Search Table and Dispatch Command. */
	for (cmd = 0; editor_table[cmd].name != NULL; cmd++)
	{
		if (!str_prefix(command, editor_table[cmd].name))
		{
			(*editor_table[cmd].do_fun)(ch, argument);
			return;
		}
	}

	/* Invalid command, send help. */
	do_help(ch, "olc");
	return;
}

/* Entry point for editing area_data. */
void do_aedit(CHAR_DATA *ch, char *argument)
{
	AREA_DATA *pArea;
	int value;
	char arg[MAX_STRING_LENGTH];

	if (IS_NPC(ch))
		return;

	pArea = ch->in_room->area;

	argument = one_argument(argument, arg);

	if (!oedit_check(ch))
	{
		send_to_char(
				"OEdit: You must balance this eq before you are done.\n\r", ch);
		//return;
	}
	if (is_number(arg))
	{
		value = atoi(arg);
		if (!(pArea = get_area_data(value)))
		{
			send_to_char("That area vnum does not exist.\n\r", ch);
			return;
		}
	}
	else if (!str_cmp(arg, "create")) //Security Create Area Make it 10 for Bree
	{
		if (ch->pcdata->security < 9)
		{
			send_to_char("AEdit : Insufficient security to create areas.\n\r",
					ch);
			return;
		}

		aedit_create(ch, "");
		ch->desc->editor = ED_AREA;
		return;
	}

	if (!IS_BUILDER(ch,pArea))
	{
		send_to_char("Insufficient security to edit areas.\n\r", ch);
		return;
	}

	ch->desc->pEdit = (void *) pArea;
	ch->desc->editor = ED_AREA;
	return;
}

/* Entry point for editing room_index_data. */
void do_redit(CHAR_DATA *ch, char *argument)
{
	ROOM_INDEX_DATA *pRoom;
	char arg1[MAX_STRING_LENGTH];

	if (IS_NPC(ch))
		return;

	if (!oedit_check(ch))
	{
		send_to_char(
				"OEdit: You must balance this eq before you are done.\n\r", ch);
		//return;
	}

	argument = one_argument(argument, arg1);

	pRoom = ch->in_room;

	if (!str_cmp(arg1, "reset")) /* redit reset */
	{
		if (!IS_BUILDER( ch, pRoom->area ))
		{
			send_to_char("Insufficient security to reset room.\n\r", ch);
			return;
		}

		reset_room(pRoom);
		send_to_char("Room reset.\n\r", ch);

		return;
	}
	else if (!str_cmp(arg1, "create")) /* redit create <vnum> */
	{
		if (argument[0] == '\0' || atoi(argument) == 0)
		{
			send_to_char("Syntax:  edit room create [vnum]\n\r", ch);
			return;
		}

		if (redit_create(ch, argument)) /* pEdit == nuevo cuarto */
		{
			ch->desc->editor = ED_ROOM;
			char_from_room(ch);
			char_to_room(ch, ch->desc->pEdit);
			SET_BIT( ((ROOM_INDEX_DATA *)ch->desc->pEdit)->area->area_flags, AREA_CHANGED );
		}

		return;
	}
	else if (!IS_NULLSTR(arg1)) /* redit <vnum> */
	{
		pRoom = get_room_index(atoi(arg1));

		if (!pRoom)
		{
			send_to_char("REdit : Room doesn't exist.\n\r", ch);
			return;
		}

		if (!IS_BUILDER(ch, pRoom->area))
		{
			send_to_char("REdit : Insufficient security to edit room.\n\r", ch);
			return;
		}

		char_from_room(ch);
		char_to_room(ch, pRoom);
	}

	if (!IS_BUILDER(ch, pRoom->area))
	{
		send_to_char("REdit : Insufficient security to edit room.\n\r", ch);
		return;
	}

	ch->desc->pEdit = (void *) pRoom;
	ch->desc->editor = ED_ROOM;

	return;
}

/* Entry point for editing obj_index_data. */
void do_oedit(CHAR_DATA *ch, char *argument)
{
	OBJ_INDEX_DATA *pObj;
	AREA_DATA *pArea;
	char arg1[MAX_STRING_LENGTH];
	int value;

	if (IS_NPC(ch))
		return;

	argument = one_argument(argument, arg1);

	if (is_number(arg1))
	{
		if (!oedit_check(ch))
		{
			send_to_char(
					"OEdit: You must balance this eq before you are done.\n\r",
					ch);
			//return;
		}
		value = atoi(arg1);
		if (!(pObj = get_obj_index(value)))
		{
			send_to_char("OEdit:  That vnum does not exist.\n\r", ch);
			return;
		}

		if (!IS_BUILDER( ch, pObj->area ))
		{
			send_to_char("Insufficient security to edit objects.\n\r", ch);
			return;
		}

		ch->desc->pEdit = (void *) pObj;
		ch->desc->editor = ED_OBJECT;
		return;
	}
	else
	{
		if (!str_cmp(arg1, "create"))
		{
			if (!oedit_check(ch))
			{
				send_to_char(
						"OEdit: You must balance this eq before you are done.\n\r",
						ch);
				//return;
			}
			value = atoi(argument);
			if (argument[0] == '\0' || value == 0)
			{
				send_to_char("Syntax:  edit object create [vnum]\n\r", ch);
				return;
			}

			pArea = get_vnum_area(value);

			if (!pArea)
			{
				send_to_char("OEdit:  That vnum is not assigned an area.\n\r",
						ch);
				return;
			}

			if (!IS_BUILDER( ch, pArea ))
			{
				send_to_char("Insufficent security to create objects.\n\r", ch);
				return;
			}

			if (oedit_create(ch, argument))
			{
				SET_BIT( pArea->area_flags, AREA_CHANGED );
				ch->desc->editor = ED_OBJECT;
			}
			return;
		}
	}

	send_to_char("OEdit:  There is no default object to edit.\n\r", ch);
	return;
}

/* Entry point for editing mob_index_data. */
void do_medit(CHAR_DATA *ch, char *argument)
{
	MOB_INDEX_DATA *pMob;
	AREA_DATA *pArea;
	int value;
	char arg1[MAX_STRING_LENGTH];

	if (!oedit_check(ch))
	{
		send_to_char(
				"OEdit: You must balance this eq before you are done.\n\r", ch);
		//return;
	}
	argument = one_argument(argument, arg1);

	if (IS_NPC(ch))
		return;

	if (is_number(arg1))
	{
		value = atoi(arg1);
		if (!(pMob = get_mob_index(value)))
		{
			send_to_char("MEdit:  That vnum does not exist.\n\r", ch);
			return;
		}

		if (!IS_BUILDER( ch, pMob->area ))
		{
			send_to_char("Insufficient security to modify mobiles.\n\r", ch);
			return;
		}

		ch->desc->pEdit = (void *) pMob;
		ch->desc->editor = ED_MOBILE;
		return;
	}
	else
	{
		if (!str_cmp(arg1, "create"))
		{
			value = atoi(argument);
			if (arg1[0] == '\0' || value == 0)
			{
				send_to_char("Syntax:  edit mobile create [vnum]\n\r", ch);
				return;
			}

			pArea = get_vnum_area(value);

			if (!pArea)
			{
				send_to_char("OEdit:  That vnum is not assigned an area.\n\r",
						ch);
				return;
			}

			if (!IS_BUILDER( ch, pArea ))
			{
				send_to_char("Insufficient security to modify mobiles.\n\r", ch);
				return;
			}

			if (medit_create(ch, argument))
			{
				SET_BIT( pArea->area_flags, AREA_CHANGED );
				ch->desc->editor = ED_MOBILE;
			}
			return;
		}
	}

	send_to_char("MEdit:  There is no default mobile to edit.\n\r", ch);
	return;
}

void display_resets(CHAR_DATA *ch)
{
	ROOM_INDEX_DATA *pRoom;
	RESET_DATA *pReset;
	MOB_INDEX_DATA *pMob = NULL;
	char buf[MAX_STRING_LENGTH];
	char fmt_temp[MAX_STRING_LENGTH];
	int iReset = 0;
	BUFFER *buffer;

	EDIT_ROOM(ch, pRoom);

	buffer = new_buf();
	add_buf(buffer,
			" No.  Loads    Description       Location         Vnum   Mx Mn Description"
				"\n\r"
				"==== ======== ============= =================== ======== ===== ==========="
				"\n\r");

	for (pReset = pRoom->reset_first; pReset; pReset = pReset->next)
	{
		OBJ_INDEX_DATA *pObj;
		MOB_INDEX_DATA *pMobIndex;
		OBJ_INDEX_DATA *pObjIndex;
		OBJ_INDEX_DATA *pObjToIndex;
		ROOM_INDEX_DATA *pRoomIndex;

		sprintf(buf, "[%2d] ", ++iReset);
		add_buf(buffer, buf);

		switch (pReset->command)
		{
		default:
			sprintf(buf, "Bad reset command: %c.", pReset->command);
			add_buf(buffer, buf);
			break;

		case 'M':
			if (!(pMobIndex = get_mob_index(pReset->arg1)))
			{
				sprintf(buf, "Load Mobile - Bad Mob %d\n\r", pReset->arg1);
				add_buf(buffer, buf);
				continue;
			}

			if (!(pRoomIndex = get_room_index(pReset->arg3)))
			{
				sprintf(buf, "Load Mobile - Bad Room %d\n\r", pReset->arg3);
				add_buf(buffer, buf);
				continue;
			}

			/*
			 * Check for pet shop.
			 * -------------------
			 */
			ROOM_INDEX_DATA *pRoomIndexPrev = get_room_index(pRoomIndex->vnum - 1);

			pMob = pMobIndex;
			sprintf(
					buf,
					"%c[%5d] %s{x in room             R[%5d] %2d-%2d %s{x\n\r",
					(pRoomIndexPrev && IS_SET(pRoomIndexPrev->room_flags, ROOM_PET_SHOP)) ? 'P' : 'M',
					pReset->arg1, end_string(pMob->short_descr, 13), pReset->arg3,
					pReset->arg2, pReset->arg4, pRoomIndex->name);
			add_buf(buffer, buf);
			break;

		case 'O':
			if (!(pObjIndex = get_obj_index(pReset->arg1)))
			{
				sprintf(buf, "Load Object - Bad Object %d\n\r", pReset->arg1);
				add_buf(buffer, buf);
				continue;
			}

			pObj = pObjIndex;

			if (!(pRoomIndex = get_room_index(pReset->arg3)))
			{
				sprintf(buf, "Load Object - Bad Room %d\n\r", pReset->arg3);
				add_buf(buffer, buf);
				continue;
			}

			sprintf(buf, "O[%5d] %s{x in room             R[%5d]       %s{x\n\r",
				pReset->arg1, end_string(pObj->short_descr, 13),
				pReset->arg3, pRoomIndex->name);
			add_buf(buffer, buf);

			break;

		case 'P':
			if (!(pObjIndex = get_obj_index(pReset->arg1)))
			{
				sprintf(buf, "Put Object - Bad Object %d\n\r", pReset->arg1);
				add_buf(buffer, buf);
				continue;
			}

			pObj = pObjIndex;

			if (!(pObjToIndex = get_obj_index(pReset->arg3)))
			{
				sprintf(buf, "Put Object - Bad To Object %d\n\r", pReset->arg3);
				add_buf(buffer, buf);
				continue;
			}

			sprintf(
					buf,
					"O[%5d] %s{x inside              O[%5d] %2d-%2d %s{x\n\r",
					pReset->arg1, end_string(pObj->short_descr, 13), pReset->arg3,
					pReset->arg2, pReset->arg4, pObjToIndex->short_descr);
			add_buf(buffer, buf);

			break;

		case 'G':
		case 'E':
			if (!(pObjIndex = get_obj_index(pReset->arg1)))
			{
				sprintf(buf, "Give/Equip Object - Bad Object %d\n\r",
						pReset->arg1);
				add_buf(buffer, buf);
				continue;
			}

			pObj = pObjIndex;

			if (!pMob)
			{
				sprintf(buf, "Give/Equip Object - No Previous Mobile\n\r");
				add_buf(buffer, buf);
				break;
			}

			if (pMob->pShop)
			{
				sprintf(
						buf,
						"O[%5d] %s{x in the inventory of S[%5d]       %s{x\n\r",
						pReset->arg1, end_string(pObj->short_descr, 13), pMob->vnum,
						pMob->short_descr);
			}
			else {
				strcpy(fmt_temp, end_string(pObj->short_descr, 13));
				sprintf(buf,
					"O[%5d] %s{x %s{x M[%5d]       %s{x\n\r",
					pReset->arg1, fmt_temp,
					end_string(pReset->command == 'G'
						? flag_string( wear_loc_strings, WEAR_NONE )
						: flag_string(wear_loc_strings, pReset->arg3), 19),
					pMob->vnum, pMob->short_descr);
			}
			add_buf(buffer, buf);
			break;

			/*
			 * Doors are set in rs_flags don't need to be displayed.
			 * If you want to display them then uncomment the new_reset
			 * line in the case 'D' in load_resets in db.c and here.
			 */
		case 'D':
			pRoomIndex = get_room_index(pReset->arg1);
			sprintf(buf, "R[%5d] %s door of %s{x reset to %s{x\n\r",
					pReset->arg1, capitalize(dir_name[pReset->arg2]),
					end_string(pRoomIndex->name, 19), flag_string(door_resets, pReset->arg3));
			add_buf(buffer, buf);

			break;
			/*
			 * End Doors Comment.
			 */
		case 'R':
			if (!(pRoomIndex = get_room_index(pReset->arg1)))
			{
				sprintf(buf, "Randomize Exits - Bad Room %d\n\r", pReset->arg1);
				add_buf(buffer, buf);
				continue;
			}

			sprintf(buf, "R[%5d] Exits are randomized in %s\n\r", pReset->arg1,
					pRoomIndex->name);
			add_buf(buffer, buf);

			break;
		}
	}
	page_to_char(buffer->string, ch);
	free_buf(buffer);

	return;
}

/*****************************************************************************  
 Name:          add_reset  
 Purpose:       Inserts a new reset in the given index slot.  
 Called by:     do_resets(olc.c).  
 ****************************************************************************/
void add_reset(ROOM_INDEX_DATA *room, RESET_DATA *pReset, int index)
{
	RESET_DATA *reset;
	int iReset = 0;

	if (!room->reset_first)
	{
		room->reset_first = pReset;
		room->reset_last = pReset;
		pReset->next = NULL;
		return;
	}

	index--;

	if (index == 0) /* First slot (1) selected. */
	{
		pReset->next = room->reset_first;
		room->reset_first = pReset;
		return;
	}

	/*
	 * If negative slot( <= 0 selected) then this will find the last.
	 */
	for (reset = room->reset_first; reset->next; reset = reset->next)
	{
		if (++iReset == index)
			break;
	}

	pReset->next = reset->next;
	reset->next = pReset;
	if (!pReset->next)
		room->reset_last = pReset;
	return;
}

void do_resets(CHAR_DATA *ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char arg4[MAX_INPUT_LENGTH];
	char arg5[MAX_INPUT_LENGTH];
	char arg6[MAX_INPUT_LENGTH];
	char arg7[MAX_INPUT_LENGTH];
	RESET_DATA *pReset = NULL;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);
	argument = one_argument(argument, arg3);
	argument = one_argument(argument, arg4);
	argument = one_argument(argument, arg5);
	argument = one_argument(argument, arg6);
	argument = one_argument(argument, arg7);

	if (!IS_BUILDER( ch, ch->in_room->area ))
	{
		send_to_char("Resets: Invalid security for editing this area.\n\r", ch);
		return;
	}

	/*
	 * Display resets in current room.
	 * -------------------------------
	 */
	if (arg1[0] == '\0')
	{
		if (ch->in_room->reset_first)
		{
			send_to_char("Resets: M = mobile, R = room, O = object, "
				"P = pet, S = shopkeeper\n\r", ch);
			display_resets(ch);
		}
		else
			send_to_char("No resets in this room.\n\r", ch);
	}

	/*
	 * Take index number and search for commands.
	 * ------------------------------------------
	 */
	if (is_number(arg1))
	{
		ROOM_INDEX_DATA *pRoom = ch->in_room;

		/*
		 * Delete a reset.
		 * ---------------
		 */
		if (!str_cmp(arg2, "delete"))
		{
			int insert_loc = atoi(arg1);

			if (!ch->in_room->reset_first)
			{
				send_to_char("No resets in this area.\n\r", ch);
				return;
			}

			if (insert_loc - 1 <= 0)
			{
				pReset = pRoom->reset_first;
				pRoom->reset_first = pRoom->reset_first->next;
				if (!pRoom->reset_first)
					pRoom->reset_last = NULL;
			}
			else
			{
				int iReset = 0;
				RESET_DATA *prev = NULL;

				for (pReset = pRoom->reset_first; pReset; pReset = pReset->next)
				{
					if (++iReset == insert_loc)
						break;
					prev = pReset;
				}

				if (!pReset)
				{
					send_to_char("Reset not found.\n\r", ch);
					return;
				}

				if (prev)
					prev->next = prev->next->next;
				else
					pRoom->reset_first = pRoom->reset_first->next;

				for (pRoom->reset_last = pRoom->reset_first; pRoom->reset_last->next; pRoom->reset_last
						= pRoom->reset_last->next)
					;
			}

			free_reset_data(pReset);
			send_to_char("Reset deleted.\n\r", ch);
		}
		else
		/*
		 * Add a reset.
		 * ------------
		 */
		if ((!str_cmp(arg2, "mob") && is_number(arg3))
				|| (!str_cmp(arg2, "obj") && is_number(arg3)))
		{
			/*
			 * Check for Mobile reset.
			 * -----------------------
			 */
			if (!str_cmp(arg2, "mob"))
			{
				if (get_mob_index(is_number(arg3) ? atoi(arg3) : 1) == NULL)
				{
					send_to_char("Mobile does not exist.\n\r", ch);
					return;
				}
				pReset = new_reset_data();
				pReset->command = 'M';
				pReset->arg1 = atoi(arg3);
				pReset->arg2 = is_number(arg4) ? atoi(arg4) : 1; /* Max # */
				pReset->arg3 = ch->in_room->vnum;
				pReset->arg4 = is_number(arg5) ? atoi(arg5) : 1; /* Min # */
			}
			else
			/*
			 * Check for Object reset.
			 * -----------------------
			 */
			if (!str_cmp(arg2, "obj"))
			{
				pReset = new_reset_data();
				pReset->arg1 = atoi(arg3);
				/*
				 * Inside another object.
				 * ----------------------
				 */
				if (!str_prefix(arg4, "inside"))
				{
					OBJ_INDEX_DATA *temp;

					temp = get_obj_index(is_number(arg5) ? atoi(arg5) : 1);
					if ((temp->item_type != ITEM_CONTAINER) && (temp->item_type
							!= ITEM_CORPSE_NPC))
					{
						send_to_char("Object 2 is not a container.\n\r", ch);
						return;
					}
					pReset->command = 'P';
					pReset->arg2 = is_number(arg6) ? atoi(arg6) : 1;
					pReset->arg3 = is_number(arg5) ? atoi(arg5) : 1;
					pReset->arg4 = is_number(arg7) ? atoi(arg7) : 1;
				}
				else
				/*
				 * Inside the room.
				 * ----------------
				 */
				if (!str_cmp(arg4, "room"))
				{
					if (get_obj_index(atoi(arg3)) == NULL)
					{
						send_to_char("That vnum does not exist.\n\r", ch);
						return;
					}
					pReset->command = 'O';
					pReset->arg2 = 0;
					pReset->arg3 = ch->in_room->vnum;
					pReset->arg4 = 0;
				}
				else
				/*
				 * Into a Mobile's inventory.
				 * --------------------------
				 */
				{
					if (flag_value(wear_loc_flags, arg4) == NO_FLAG)
					{
						send_to_char("Resets: '? wear-loc'\n\r", ch);
						return;
					}
					if (get_obj_index(atoi(arg3)) == NULL)
					{
						send_to_char("That vnum does not exist.\n\r", ch);
						return;
					}
					pReset->arg1 = atoi(arg3);
					pReset->arg3 = flag_value(wear_loc_flags, arg4);
					if (pReset->arg3 == WEAR_NONE)
						pReset->command = 'G';
					else
						pReset->command = 'E';
				}
			}
			add_reset(ch->in_room, pReset, atoi(arg1));
			SET_BIT( ch->in_room->area->area_flags, AREA_CHANGED );
			send_to_char("Reset added.\n\r", ch);
		}
		else if (!str_cmp(arg2, "random") && is_number(arg3))
		{
			if (atoi(arg3) < 1 || atoi(arg3) > 6)
			{
				send_to_char("Invalid argument.\n\r", ch);
				return;
			}
			pReset = new_reset_data();
			pReset->command = 'R';
			pReset->arg1 = ch->in_room->vnum;
			pReset->arg2 = atoi(arg3);
			add_reset(ch->in_room, pReset, atoi(arg1));
			SET_BIT( ch->in_room->area->area_flags, AREA_CHANGED );
			send_to_char("Random exits reset added.\n\r", ch);
		}
		else
		{
			send_to_char("Syntax: RESET <number> OBJ <vnum> <wear_loc>\n\r", ch);
			send_to_char(
					"        RESET <number> OBJ <vnum> inside <vnum> [limit] [count]\n\r",
					ch);
			send_to_char("        RESET <number> OBJ <vnum> room\n\r", ch);
			send_to_char(
					"        RESET <number> MOB <vnum> [max #x area] [max #x room]\n\r",
					ch);
			send_to_char("        RESET <number> DELETE\n\r", ch);
			send_to_char("        RESET <number> RANDOM [#x exits]\n\r", ch);
		}
	}

	return;
}

/*****************************************************************************  
 Name:          do_alist  
 Purpose:       Normal command to list areas and display area information.  
 Called by:     interpreter(interp.c)  
 ****************************************************************************/
void do_alist(CHAR_DATA *ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	AREA_DATA *pArea;
	BUFFER *buffer;
	int vnum_min, vnum_max, vnum_next;
	bool fFree = FALSE;

	if (area_first == NULL) {
		send_to_char("There are no areas.\r\n", ch);
		return;
	}

	buffer = new_buf();

	if (!str_cmp(argument, "free")) {
		fFree = TRUE;
		sprintf(buf, "Free slot ranges (inclusive):\r\n");
	} else
		sprintf(buf, "[%3s] %-29s (%-6s-%6s) %-12s %3s [%-10s]\n\r", "Num",
			"Area Name", "lvnum", "uvnum", "Filename", "Sec", "Builders");

	add_buf(buffer, buf);

	// First iteration - get minum vnum (done so we can have an arbitrary min-vnum.
	vnum_min = area_first->min_vnum;
	for (pArea = area_first; pArea; pArea = pArea->next)
		if (pArea->min_vnum < vnum_min)
			vnum_min = pArea->min_vnum;

	while (1) {
		vnum_next = -1;
		for (pArea = area_first; pArea; pArea = pArea->next) {
			if (pArea->min_vnum == vnum_min) {
				if (!fFree) {
					sprintf(buf, "[%3d] %-29.29s (%-6d-%6d) %-12.12s [%d] [%-10.10s]\n\r",
						pArea->vnum, pArea->name, pArea->min_vnum, pArea->max_vnum,
						pArea->file_name, pArea->security, pArea->builders);
					add_buf(buffer, buf);
				}
				// Go on the assumption there will be no overlaps.
				vnum_max = pArea->max_vnum;
			} else if (pArea->min_vnum > vnum_min
			&& (vnum_next == -1 || pArea->min_vnum < vnum_next))
				vnum_next = pArea->min_vnum;
		}
		// No area bigger than the last one.
		if (vnum_next == -1) {
			if (fFree) {
				sprintf(buf, "%d onwards\r\n", vnum_max+1);
				add_buf(buffer, buf);
			}
			break;
		}
		if (fFree && ++vnum_max < vnum_next) {
			// Theres a gap between last and next.
			sprintf(buf, "Between %-6d-%6d [%6d slots]\r\n",
				vnum_max, vnum_next-1, vnum_next-vnum_max);
			add_buf(buffer, buf);
		}
		vnum_min = vnum_next;
	}

	if (!fFree)
		add_buf(buffer, "Use \"free\" as an argument to see free slots.\r\n");
	page_to_char(buffer->string, ch);
	free_buf(buffer);

	/*

	char result[MAX_STRING_LENGTH * 4]; // May need tweaking.
	sprintf(result, "[%3s] [%-27s] (%-5s-%5s) [%-10s] %3s [%-10s]\n\r", "Num",
			"Area Name", "lvnum", "uvnum", "Filename", "Sec", "Builders");

	for (pArea = area_first; pArea; pArea = pArea->next)
	{
		sprintf(buf, "[%3d] %-29.29s (%-5d-%5d) %-12.12s [%d] [%-10.10s]\n\r",
				pArea->vnum, pArea->name, pArea->min_vnum, pArea->max_vnum,
				pArea->file_name, pArea->security, pArea->builders);
		strcat(result, buf);
	}

	send_to_char(result, ch);
	return;*/
}

void do_freevnum(CHAR_DATA *ch, char *argument)
{
	AREA_DATA *pArea, *sArea, *tArea;
	char buf[MAX_STRING_LENGTH];
	int curvnum, minvnum, maxvnum;
	char arg[MAX_STRING_LENGTH];
	bool Found = FALSE;

	if (!IS_IMMORTAL(ch) && ch->pcdata->security < 1)
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	one_argument(argument, arg);

	maxvnum = 0;

	for (pArea = area_first; pArea != NULL; pArea = pArea->next)
	{
		if (pArea->min_vnum > maxvnum)
			maxvnum = pArea->min_vnum;
	}

	minvnum = maxvnum + 1;

	for (pArea = area_first; pArea != NULL; pArea = pArea->next)
	{
		if (pArea->min_vnum < minvnum)
			minvnum = pArea->min_vnum;
	}

	while (minvnum <= maxvnum)
	{
		sArea = NULL;

		tArea = NULL;

		curvnum = minvnum;

		for (pArea = area_first; pArea != NULL; pArea = pArea->next)
		{
			if (minvnum == pArea->min_vnum)
				tArea = pArea;
		}

		minvnum = maxvnum + 1;

		for (pArea = area_first; pArea != NULL; pArea = pArea->next)
		{
			if ((pArea->min_vnum < minvnum) && (pArea->min_vnum > curvnum))
			{
				minvnum = pArea->min_vnum;
			}
		}

		for (pArea = area_first; pArea != NULL; pArea = pArea->next)
		{
			if ((minvnum == pArea->min_vnum) && (minvnum <= maxvnum))
				sArea = pArea;
		}

		if ((sArea != NULL) && (tArea != NULL) && (sArea->min_vnum
				- tArea->max_vnum > 2))
		{
			if (Found == FALSE)
			{
				send_to_char("Free Vnums:\n\r", ch);
				Found = TRUE;
			}
			sprintf(buf, "%4d - %4d\n\r", tArea->max_vnum + 1, sArea->min_vnum
					- 1);
			send_to_char(buf, ch);
		}
	}
	return;
}

typedef void (*AUD_FN)(CHAR_DATA*, char*);
#define AUD_FN_NAME(Name) audit_ ## Name

#define AUD_FN_DEF(Name) \
	void AUD_FN_NAME(Name) (CHAR_DATA *ch, char *argument)

#define AUD_FN_ENTRY(Name) \
	#Name, AUD_FN_NAME(Name)

typedef struct audit_cmd {
	char	*name;
	AUD_FN	func;
	int		min_trust;
	char	*desc;
} AUDIT_CMD;

void mprog_list_mobs(BUFFER *buffer, int mpvnum) {
	char buf[MAX_STRING_LENGTH];
	MPROG_LIST *mprog;
	MOB_INDEX_DATA *mob;
	int i;

	if (mpvnum <= 0 || buffer == NULL)
		return;

	for (i = 0; i < MAX_KEY_HASH; i++) {
		for (mob = mob_index_hash[i % MAX_KEY_HASH]; mob != NULL; mob = mob->next) {
			for (mprog = mob->mprogs; mprog != NULL; mprog = mprog->next) {
				if (mpvnum == mprog->vnum) {
					sprintf(buf, "%-15.15s: [%-6d][%s] on %-7s(%s) \r\n",
						mob->area != NULL ? mob->area->name : "null",
						mpvnum, mob->short_descr != NULL ? end_string(mob->short_descr, 20) : "null",
						mprog_type_to_name(mprog->trig_type),
						mprog->trig_phrase);
					add_buf(buffer, buf);
				}
			}
		}
	}
}

char* end_nl(char *str) {
	static char buf[MAX_STRING_LENGTH];
	int i = 0;
	while (str[i] != '\0' && str[i] != '\n') {
		buf[i] = str[i];
		++i;
	}
	buf[i] = '\0';
	return buf;
}

AUD_FN_DEF(mprog) {
	int vnum;

	if (argument[0] == '\0' || !is_number(argument)) {
		send_to_char("Please specify an MPROG vnum.\r\n", ch);
		return;
	}

	if ((vnum = atoi(argument)) <= 0) {
		send_to_char("The vnum must be greater than 0.\r\n", ch);
		return;
	}

	BUFFER *buffer = new_buf();
	mprog_list_mobs(buffer, vnum);

	if (buffer->string[0] != '\0')
		page_to_char(buffer->string, ch);
	else
		send_to_char("None found.\r\n", ch);
	free_buf(buffer);
}

AUD_FN_DEF(mpoload) {
	char buf[MAX_STRING_LENGTH];
	MPROG_CODE *mprog;
	int i, j;
	bool showMobs = FALSE;

	if (!str_cmp(argument, "usage"))
		showMobs = TRUE;

	BUFFER *buffer = new_buf();
	for (mprog = mprog_list; mprog; mprog = mprog->next) {
		char *search = mprog->code, *context = search;
		if (search == NULL)
			continue;
		for (i = 0, j = 1; j < 6 && search[i] != '\0'; ) {
			// KMP algorithm for fast string matching.
			switch (j) {
			case 0:	// Shift
				if (search[i] == '\n')
					context = &search[i+1];
				++i; ++j;
				break;
			case 1:	// o
				if (search[i] == 'o') {
					++i; ++j;
				} else
					j = 0;
				break;
			case 2:	// l
				if (search[i] == 'l') {
					++i; ++j;
				} else
					j = 1;
				break;
			case 3:	// o
				if (search[i] == 'o') {
					++i; ++j;
				} else
					j = 0;
				break;
			case 4:	// a
				if (search[i] == 'a') {
					++i; ++j;
				} else
					j = 2;
				break;
			case 5:	// d
				if (search[i] == 'd') {
					++i; ++j;
				} else
					j = 1;
				break;
			}
			if (j == 6) {
				sprintf(buf, "---> Found in %d (%s)\r\n",
					mprog->vnum, end_nl(context));
				add_buf(buffer, buf);
				if (showMobs) {
					mprog_list_mobs(buffer, mprog->vnum);
				}
				break;
			}
		}
	}

	if (buffer->string[0] != '\0') {
		if (!showMobs)
			add_buf(buffer, "Use \"usage\" as an argument to see which mobs have the mprogs.\r\n");
		page_to_char(buffer->string, ch);
	}
	free_buf(buffer);
}

AUD_FN_DEF(mpgrep) {
	send_to_char("Under construction.\r\n", ch);
	return;
}

bool isnumber(char *str) {
	int i;
	for (i = 0; str[i] != '\0'; i++)
		if (!isdigit(str[i]) && (i != 0 || (i == 0 && str[i] != '-')))
			return FALSE;

	return TRUE;
}

#define ADD_OBJ_EFF(Effect)														\
	for (effect = objIndex->affected; effect != NULL; effect = effect->next) {	\
		if (effect->location == Effect)											\
			lvalue += effect->modifier;											\
	}

extern int wear_map[];
char *wear_names[] = {
	"light",
	"finger",
	"~",
	"neck",
	"~",
	"body",
	"head",
	"legs",
	"feet",
	"hands",
	"arms",
	"shield",
	"about",
	"waist",
	"wrist",
	"~",
	"wield",
	"hold",
	"float",
	"~",
	"face",
	"ear",
	"~",
	"ankle",
	"~",
	"clan",
	"relig",
	"patch",
	"back",
	""
};

static char *ops[]={
#define OP_LT		0
	"<",
#define OP_GT		1
	">",
#define OP_EQ		2
	"=",
	""
};



// Returns NULL if successful, otherwise the reason why it failed.
char *aud_process_field_op_val(char **text, char **fields, int *field, int *op, int *value) {
	char arg[MAX_INPUT_LENGTH];

	*text = one_argument(*text, arg);

	if (arg[0] == '\0')
		return "Please specify a field.\r\n";

	for (*field = 0; fields[*field][0] != '\0'; (*field)++)
		if (!str_cmp(arg, fields[*field]))
			break;

	if (fields[*field][0] == '\0')
		return "Invalid field name.\r\n";

	*text = one_argument(*text, arg);

	if (arg[0] == '\0')
		return "Please supply an operator.\r\n";

	for (*op = 0; ops[*op][0] != '\0'; (*op)++)
		if (!str_cmp(arg, ops[*op]))
			break;

	if (ops[*op][0] == '\0')
		return "Invalid operator.\r\n";

	*text = one_argument(*text, arg);

	if (arg[0] == '\0')
		return "Please supply a value.\r\n";

	if (!isnumber(arg))
		return "The value must be a valid number.\r\n";

	*value = atoi(arg);
	return NULL;
}

bool aud_test_op(int op, int lvalue, int rvalue) {
	bool result;
	switch (op) {
	case OP_GT:
		result = (lvalue > rvalue) && lvalue != 0;
		break;
	case OP_LT:
		result = (lvalue < rvalue) && lvalue != 0;
		break;
	case OP_EQ:
		result = (lvalue == rvalue);
		break;
	default:
		printf_debug("aud_test_op: invalid operator index %d", op);
		return FALSE;
	}
	return result;
}

void display_list(BUFFER *buffer, char **list, int columns) {
	char buf[MAX_STRING_LENGTH];
	int i, cnt = 0;
	for (i = 0; list[i][0] != '\0'; i++) {
		if (list[i][0] == '~')
			continue;
		sprintf(buf, "%s[%-6s] ", (cnt++%columns == 0 ? "\r\n" : ""), list[i]);
		add_buf(buffer, buf);
	}
}

AUD_FN_DEF(objects) {
	static char *fields[]={
#define OF_ACEFF		0
		"aceff",
#define OF_DR		1
		"dr",
#define OF_HR		2
		"hr",
#define OF_HP		3
		"hp",
#define OF_MANA		4
		"mana",
#define OF_MOVE		5
		"move",
#define OF_LVL		6
		"lvl",
#define OF_AVGDAM	7
		"avgdam",
#define OF_MINDAM	8
		"mindam",
#define OF_MAXDAM	9
		"maxdam",
#define OF_SAVES		10
		"saves",
#define OF_HPREG		11
		"hpreg",
#define OF_MANREG	12
		"manreg",
#define OF_ACV0		13
		"acv0",
#define OF_ACV1		14
		"acv1",
#define OF_ACV2		15
		"acv2",
#define OF_ACV3		16
		"acv3",
#define OF_ACAVG		17
		"acavg",
		""
	};

	if (argument[0] == '\0') {
		BUFFER *buffer = new_buf();

		add_buf(buffer, "Syntax:  audit objects <area>|all <wearloc>|all <field> >|=|< <value>\r\n");
		add_buf(buffer, "Example: audit objects enoch ear dr > 300\r\n");
		add_buf(buffer, "Example: audit objects all all hp < -5");

		add_buf(buffer, "\r\n\r\nWear locations include:");
		display_list(buffer, wear_names, 6);

		add_buf(buffer, "\r\n\r\nFields include:");
		display_list(buffer, fields, 6);

		add_buf(buffer, "\r\n\r\nNote that the > and < operators do not show 0-valued objects.\r\n");

		page_to_char(buffer->string, ch);
		free_buf(buffer);
		return;
	}

	char arg[MAX_INPUT_LENGTH];
	
	AREA_DATA *area = NULL;
	argument = one_argument(argument, arg);

	if (str_cmp(arg, "all")) {
		for (area = area_first; area != NULL; area = area->next) {
			if (is_name(arg, area->name))
				break;
		}
		if (area == NULL) {
			send_to_char("Area not found.\r\n", ch);
			return;
		}
	}

	int wearloc;
	argument = one_argument(argument, arg);

	if (!str_cmp(arg, "all"))
		wearloc = -1;
	else {
	    for (wearloc = 0; wear_names[wearloc][0] != '\0'; wearloc++) {
    	    if (wear_names[wearloc][0] == '~')
        	    continue;
			if (!str_cmp(arg, wear_names[wearloc]))
				break;
		}
		if (wear_names[wearloc][0] == '\0') {
			send_to_char("Invalid wear location.\r\n", ch);
			return;
		}
	}

	int field, op, value;
	char *err_reason = aud_process_field_op_val(&argument, fields, &field, &op, &value);

	if (err_reason != NULL) {
		send_to_char(err_reason, ch);
		return;
	}

	// Ok, finally got our field, operator and value, time to do stuff.
	char buf[MAX_STRING_LENGTH];
	OBJ_INDEX_DATA *objIndex;
	AFFECT_DATA *effect;
	int lvalue, hash;
	BUFFER *buffer = new_buf();

	add_buf(buffer, "Matching objects:\r\n");

	for (hash = 0; hash < MAX_KEY_HASH; hash++) {
		for (objIndex = obj_index_hash[hash]; objIndex != NULL; objIndex = objIndex->next) {
			// Area check.
			if ((area != NULL && objIndex->area != area) || (area == NULL && is_name("Unlinked", objIndex->area->builders)))
				continue;
			// Hack for lights.
			if (wearloc == 0 && objIndex->item_type != ITEM_LIGHT)
				continue;
			if (wearloc != -1 && !IS_SET(objIndex->wear_flags, wear_map[wearloc]))
				continue;
			lvalue = 0;
			switch (field) {
			case OF_ACEFF:
				ADD_OBJ_EFF(APPLY_AC);
				if (objIndex->item_type == ITEM_ARMOR)
					lvalue += (objIndex->value[0] + objIndex->value[1] + objIndex->value[2] + objIndex->value[3])/4;
				break;
			case OF_DR:
				ADD_OBJ_EFF(APPLY_DAMROLL);
				break;
			case OF_HR:
				ADD_OBJ_EFF(APPLY_HITROLL);
				break;
			case OF_HP:
				ADD_OBJ_EFF(APPLY_HIT);
				break;
			case OF_MANA:
				ADD_OBJ_EFF(APPLY_MANA);
				break;
			case OF_MOVE:
				ADD_OBJ_EFF(APPLY_MOVE);
				break;
			case OF_LVL:
				lvalue = objIndex->level;
				break;
			case OF_AVGDAM:
				if (objIndex->item_type != ITEM_WEAPON)
					continue;
				lvalue = objIndex->value[1] * objIndex->value[2] / 2;
				break;
			case OF_MINDAM:
				if (objIndex->item_type != ITEM_WEAPON)
					continue;
				lvalue = objIndex->value[1];
				break;
			case OF_MAXDAM:
				if (objIndex->item_type != ITEM_WEAPON)
					continue;
				lvalue = objIndex->value[1] * objIndex->value[2];
				break;
			case OF_SAVES:
				ADD_OBJ_EFF(APPLY_SAVES);
				ADD_OBJ_EFF(APPLY_SAVING_ROD);
				ADD_OBJ_EFF(APPLY_SAVING_PETRI);
				ADD_OBJ_EFF(APPLY_SAVING_BREATH);
				ADD_OBJ_EFF(APPLY_SAVING_SPELL);
				break;
			case OF_HPREG:
				ADD_OBJ_EFF(APPLY_REGEN);
				break;
			case OF_MANREG:
				ADD_OBJ_EFF(APPLY_MANA_REGEN);
				break;
			case OF_ACV0:
                if (objIndex->item_type != ITEM_ARMOR)
					continue;
				lvalue = objIndex->value[0];
				break;
            case OF_ACV1:
                if (objIndex->item_type != ITEM_ARMOR)
                    continue;
                lvalue = objIndex->value[0];
                break;
            case OF_ACV2:
                if (objIndex->item_type != ITEM_ARMOR)
                    continue;
                lvalue = objIndex->value[0];
                break;
            case OF_ACV3:
                if (objIndex->item_type != ITEM_ARMOR)
                    continue;
                lvalue = objIndex->value[0];
                break;
            case OF_ACAVG:
                if (objIndex->item_type != ITEM_ARMOR)
                    continue;
                lvalue += (objIndex->value[0] + objIndex->value[1] + objIndex->value[2] + objIndex->value[3])/4;
				break;
			default:
				send_to_char("**BUG** Invalid field index.\r\n", ch);
				printf_debug("AUD_FN_ENTRY(objects): invalid field index %d", field);
				return;
			}


			if (aud_test_op(op, lvalue, value)) {
				sprintf(buf, "[%-5d in %15.15s] %s @ %-6d : %s\r\n",
					objIndex->vnum, objIndex->area->name, fields[field], lvalue, objIndex->short_descr);
				add_buf(buffer, buf);
			}
		}
	}

	page_to_char(buffer->string, ch);
	free_buf(buffer);
}

AUD_FN_DEF(mobs) {
	static char *fields[] = {
#define MF_LVL		0
		"lvl",
#define MF_TIER		1
		"tier",
		""
	};

	BUFFER *buffer;
	char buf[MAX_STRING_LENGTH], arg[MAX_INPUT_LENGTH];
	int op, field, value;

	if (argument[0] == '\0') {
		buffer = new_buf();

		add_buf(buffer,
			"Syntax:  audit mobs <area>|all <field> >|=|< <value>\r\n"
			"Example: audit mobs enoch lvl > 140\r\n");
		add_buf(buffer, "\r\nFields include:\r\n");
		display_list(buffer, fields, 6);
		add_buf(buffer, "\r\n");
		page_to_char(buffer->string, ch);
		free_buf(buffer);
		return;
	}

	AREA_DATA *area = NULL;
	argument = one_argument(argument, arg);

	if (str_cmp(arg, "all")) {
		for (area = area_first; area != NULL; area = area->next) {
			if (is_name(arg, area->name))
				break;
		}
		if (area == NULL) {
			send_to_char("Area not found.\r\n", ch);
			return;
		}
	}

	char *err_reason = aud_process_field_op_val(&argument, fields, &field, &op, &value);

	if (err_reason != NULL) {
		send_to_char(err_reason, ch);
		return;
	}

	MOB_INDEX_DATA *mobIndex;
	int lvalue, hash;

	buffer = new_buf();
	add_buf(buffer, "Matching mobs:\r\n");

	for (hash = 0; hash < MAX_KEY_HASH; hash++) {
		for (mobIndex = mob_index_hash[hash]; mobIndex != NULL; mobIndex = mobIndex->next) {
			if ((area != NULL && mobIndex->area != area) || (area == NULL && is_name("Unlinked", mobIndex->area->builders)))
				continue;

			switch (field) {
			case MF_LVL:
				lvalue = mobIndex->level;
				break;
			case MF_TIER:
				lvalue = mobIndex->mob_tier;
				break;
			default:
				send_to_char("**BUG** Invalid field index.\r\n", ch);
				printf_debug("AUD_FN_ENTRY(mobs): invalid field index %d", field);
				return;
			}

			if (aud_test_op(op, lvalue, value)) {
//				sprintf(buf, "[%-5d in %15.15s] %s @ %-6d : %s\r\n",
	//				mobIndex->vnum, mobIndex->area->name, fields[field], lvalue, mobIndex->short_descr);

	sprintf(buf, "[%-5d in %s] %s:%-3d,hp:%dd%d+%d,",
		mobIndex->vnum,mobIndex->area->name,fields[field],lvalue,
		mobIndex->hit[DICE_NUMBER],mobIndex->hit[DICE_TYPE],mobIndex->hit[DICE_BONUS]);
				add_buf(buffer,buf);
	sprintf(buf, "hitroll:%4d,ac:%4d,damdice:%dd%d+%d,%s\n\r",
		mobIndex->hitroll,mobIndex->ac[AC_PIERCE],
		mobIndex->damage[DICE_NUMBER], mobIndex->damage[DICE_TYPE],
		mobIndex->damage[DICE_BONUS],
		mobIndex->short_descr);
				add_buf(buffer, buf);
			}
		}
	}

	page_to_char(buffer->string, ch);
	free_buf(buffer);
}

AUDIT_CMD	audit_cmds[] = {
	{ AUD_FN_ENTRY(mprog),		107,	"lists the mobiles with a specified mprog"								},
	{ AUD_FN_ENTRY(mpoload),	109,	"lists the mprogs with the word \"oload\" (will be expanded to grep)"	},
	{ AUD_FN_ENTRY(mpgrep),		109,	"lists the mprogs with a specified word (UNDER CONSTRUCTION)"			},
	{ AUD_FN_ENTRY(objects),	108,	"lists the objects with the specified attribute"						},
	{ AUD_FN_ENTRY(mobs),		108,	"lists mobs with the specified attribute"								},
	{ 0, 0,						0,		""																		}
};

void do_audit(CHAR_DATA *ch, char *argument) {
	AUDIT_CMD *entry;

	if (argument[0] == '\0') {
		char buf[MAX_STRING_LENGTH];
		BUFFER *buffer = new_buf();
		int trust = get_trust(ch);

		add_buf(buffer, "Commands available:\r\n");
		for (entry = audit_cmds; entry->name; entry++) {
			sprintf(buf, "[{%c%3d{x] [%-8s] : %s\r\n",
				trust >= entry->min_trust ? 'G' : 'R',
				entry->min_trust, entry->name, entry->desc);
			add_buf(buffer, buf);
		}
		page_to_char(buffer->string, ch);
		free_buf(buffer);
		return;
	}

	char arg[MAX_INPUT_LENGTH];
	argument = one_argument(argument, arg);
	for (entry = audit_cmds; entry->name; entry++) {
		if (!str_cmp(arg, entry->name)) {
			(*entry->func)(ch, argument);
			return;
		}
	}
	send_to_char("Command not found.\r\n", ch);
}
