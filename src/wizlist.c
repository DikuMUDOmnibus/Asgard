/***************************************************************************   
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
 *	ROM 2.4 is copyright 1993-1995 Russ Taylor			   *
 *	ROM has been brought to you by the ROM consortium		   *
 *	    Russ Taylor (rtaylor@pacinfo.com)				   *
 *	    Gabrielle Taylor (gtaylor@pacinfo.com)			   *
 *	    Brian Moore (rom@rom.efn.org)				   *
 *	By using this code, you have agreed to follow the terms of the	   *
 *	ROM license, in the file Rom24/doc/rom.license			   *
 ***************************************************************************/

/*************************************************************************** 
 *       ROT 1.4 is copyright 1996-1997 by Russ Walsh                       *
 *       By using this code, you have agreed to follow the terms of the     *
 *       ROT license, in the file doc/rot.license                           *
 ***************************************************************************/

#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "recycle.h"
#include "interp.h"

extern char boot_buf[MAX_STRING_LENGTH];
WIZ_DATA *wiz_list;

#define WIZRANK_MAX             9
char * wizranks[WIZRANK_MAX + 1] =
{ "{wT{Dr{wa{Win{we{De{ws",
		"{WJu{wni{Do{wr {WBu{wi{Dld{wer{Ws",
		"{DQu{we{Wst{wo{Drs",
		"{wB{Du{wi{Wld{we{Dr{ws",
		// "{WH{we{Dad{w B{Wui{wl{Dde{wr{Ws",
		"{WSe{wni{Do{wr {WBu{wi{Dld{wer{Ws",
		//"{WH{we{Dad{w Q{Wue{ws{Dto{wr{Ws",
		"{WE{wn{Df{wo{Wr{wc{De{wr{Ws", "{WH{we{Dad{w B{Wui{wl{Dde{wr",
		"{DC{wo-{WI{wmp{Ds", "{WI{wm{Dpl{we{Wme{wn{Dto{wr{Ws",
		"{rT{Rh{re {rP{Ri{Ym{Rp{rs" };

void do_wizhelp(CHAR_DATA *ch, char *argument)
{
	BUFFER *output;
	char buf[MAX_STRING_LENGTH];
	int i, trust = get_trust(ch), index, len;
	int lvl_cnt[WIZRANK_MAX + 1];
	char lvl_buf[WIZRANK_MAX + 1][MAX_STRING_LENGTH];

	sprintf(lvl_buf[WIZRANK_MAX], "{D- {rA{Rb{Yo{Rv{re {Y%d {D- %s {D",
			(LEVEL_IMMORTAL + WIZRANK_MAX - 1), wizranks[WIZRANK_MAX]);
	for (i = 0; i < WIZRANK_MAX + 1; i++)
	{
		if (i < WIZRANK_MAX)
			sprintf(lvl_buf[i], "{D- {DL{we{Wv{we{Dl {C%d {D- %s {D", i
					+LEVEL_IMMORTAL, wizranks[i]);
		for (len = strlen_color(lvl_buf[i]); len <= 13 * 6; len++)
			strcat(lvl_buf[i], "-");
		strcat(lvl_buf[i], "{w");
		lvl_cnt[i] = 0;
	}

	for (i = 0; cmd_table[i].name[0] != '\0'; i++)
	{
		if (!cmd_table[i].show || cmd_table[i].level < LEVEL_IMMORTAL
				|| !is_granted(ch, cmd_table[i].do_fun))
			continue;

		sprintf(buf, "%s%-12s%s ", (cmd_table[i].log == LOG_ALWAYS && trust
				>= MAX_LEVEL) ? "{W" : "", cmd_table[i].name, (cmd_table[i].log
				== LOG_ALWAYS && trust >= MAX_LEVEL) ? "{w" : "");

		index = UMIN( cmd_table[i].level-LEVEL_IMMORTAL, WIZRANK_MAX );
		if (lvl_cnt[index] % 6 == 0)
			strcat(lvl_buf[index], "\n\r");

		strcat(lvl_buf[index], buf);
		lvl_cnt[index]++;
	}

	output = new_buf();

	for (i = WIZRANK_MAX; i >= 0; i--)
	{
		if (lvl_cnt[i] <= 0)
			continue;
		strcat(lvl_buf[i], "\n\r");
		add_buf(output, lvl_buf[i]);
	}

	add_buf(output, "{x");
	page_to_char(output->string, ch);
	free_buf(output);
}

void save_wizlist(void)
{
	WIZ_DATA *pwiz;
	FILE *fp;
	bool found = FALSE;

	if ((fp = get_temp_file()) == NULL)
	{
		perror(WIZ_FILE);
		return;
	}

	for (pwiz = wiz_list; pwiz != NULL; pwiz = pwiz->next)
	{
		found = TRUE;
		fprintf(fp, "%s %d %s~\n", pwiz->name, pwiz->level, pwiz->desc);
	}

	close_write_file(WIZ_FILE);
	if (!found)
		unlink(WIZ_FILE);
}

void load_wizlist(void)
{
	FILE *fp;
	WIZ_DATA *pwiz;

	strcat(boot_buf, "sson to all .");
	if ((fp = fopen(WIZ_FILE, "r")) == NULL)
	{
		strcat(boot_buf, "....\n\r\n\r                    ");
		return;
	}

	strcat(boot_buf, "....\n\r\n\r                    ");
	for (;;)
	{
		if (feof(fp))
			break;

		pwiz = new_wiz();
		free_string(pwiz->name);
		pwiz->name = str_dup(fread_word(fp));
		pwiz->level = fread_number(fp);
		free_string(pwiz->desc);
		pwiz->desc = fread_string(fp);
		//pwiz->desc = str_dup( "none" );
		fread_to_eol(fp);
		pwiz->next = wiz_list;
		wiz_list = pwiz;
	}
	fclose(fp);
}

WIZ_DATA * get_wiz_data(char *name)
{
	WIZ_DATA *pwiz;
	for (pwiz = wiz_list; pwiz != NULL; pwiz = pwiz->next)
		if (!str_cmp(name, pwiz->name))
			return pwiz;
	return NULL;
}

bool delete_wiz_data(char *name)
{
	WIZ_DATA *pwiz, *pwiz_prev, *pwiz_next;
	bool found = FALSE;

	for (pwiz = wiz_list, pwiz_prev = NULL; pwiz != NULL; pwiz_prev = pwiz, pwiz
			= pwiz_next)
	{
		pwiz_next = pwiz->next;
		if (str_cmp(name, pwiz->name))
			continue;

		if (pwiz_prev == NULL)
			wiz_list = pwiz->next;
		else
			pwiz_prev->next = pwiz->next;

		free_wiz(pwiz);
		found = TRUE;
	}
	if (found)
		save_wizlist();
	return found;
}

void update_wizlist(CHAR_DATA *ch, int level)
{
	WIZ_DATA *pwiz;

	if (IS_NPC( ch ))
		return;

	if (level < LEVEL_IMMORTAL)
	{
		delete_wiz_data(ch->name);
		return;
	}
	if ((pwiz = get_wiz_data(ch->name)) == NULL)
	{
		pwiz = new_wiz();
		pwiz->next = wiz_list;
		wiz_list = pwiz;
	}

	free_string(pwiz->name);
	pwiz->name = str_dup(ch->name);
	free_string(pwiz->desc);
	pwiz->desc = str_dup("");
	pwiz->level = level;
}

void do_wizlist(CHAR_DATA *ch, char *argument)
{
	BUFFER *output;
	WIZ_DATA *pwiz;
	char buf[MAX_STRING_LENGTH], lvl_buf[WIZRANK_MAX][MAX_STRING_LENGTH];
	int lvl_cnt[WIZRANK_MAX], level;

	if (argument[0] != '\0')
	{
		char arg[MAX_INPUT_LENGTH];

		argument = one_argument(argument, arg);

		if (get_trust(ch) >= MAX_LEVEL)
		{
			if (!str_cmp(arg, "add"))
			{
				argument = one_argument(argument, arg);
				if (arg[0] == '\0')
				{
					send_to_char("Please specify who you want to add.\n\r", ch);
					return;
				}

				if (!is_number(argument))
				{
					send_to_char("Please specify a level.\n\r", ch);
					return;
				}
				if ((level = atoi(argument)) > MAX_LEVEL || level
						< LEVEL_IMMORTAL)
				{
					sprintf(buf, "The valid level range is %d to %d.\n\r",
							LEVEL_IMMORTAL, MAX_LEVEL );
					send_to_char(buf, ch);
					return;
				}
				if ((pwiz = get_wiz_data(arg)) == NULL)
					pwiz = new_wiz();

				free_string(pwiz->name);
				pwiz->name = str_dup(capitalize(arg));
				pwiz->level = level;
				free_string(pwiz->desc);
				pwiz->desc = str_dup("");
				pwiz->next = wiz_list;
				wiz_list = pwiz;
				sprintf(buf, "%s added to the wizlist at level %d.\n\r",
						pwiz->name, level);
				send_to_char(buf, ch);
				save_wizlist();
			}
			else if (!str_cmp(arg, "delete"))
			{
				if (argument[0] == '\0')
					send_to_char("Please specify a name to delete.\n\r", ch);
				else if (delete_wiz_data(argument))
					send_to_char("Name deleted.\n\r", ch);
				else
					send_to_char("Name not found.\n\r", ch);
			}
			else if (!str_cmp(arg, "desc"))
			{
				argument = one_argument(argument, arg);
				if (arg[0] == '\0')
				{
					send_to_char(
							"Please specify whos description you want to edit.\n\r",
							ch);
					return;
				}
				if ((pwiz = get_wiz_data(arg)) == NULL)
				{
					send_to_char("Name not found.\n\r", ch);
					return;
				}
				free_string(pwiz->desc);
				pwiz->desc = str_dup(argument);
				sprintf(buf, "%s's wizlist description is now: %s\n\r",
						pwiz->name, pwiz->desc);
				send_to_char(buf, ch);
				save_wizlist();
			}
			else
				send_to_char(
						"Syntax: wizlist                       - show the wizlist\n\r"
							"        wizlist add <name> <level>    - add an entry\n\r"
							"        wizlist delete <name> <level> - delete an entry\n\r"
							"        wizlist desc <name> <desc>    - edit a desc\n\r",
						ch);
		}
		else
		{
			if (!str_cmp(arg, "desc"))
			{
				if ((pwiz = get_wiz_data(ch->name)) == NULL)
				{
					send_to_char("You're not on the list!\n\r", ch);
					return;
				}
				free_string(pwiz->desc);
				pwiz->desc = str_dup(argument);
				sprintf(buf, "Your wizlist description is now: %s\n\r",
						pwiz->desc);
				send_to_char(buf, ch);
				save_wizlist();
			}
			else
				send_to_char(
						"Syntax: wizlist desc <desc> - edit your desc\n\r", ch);
		}
		return;
	}

	for (level = 0; level < WIZRANK_MAX; level++)
	{
		sprintf(lvl_buf[level], "{B-- %s {B- {DL{we{Wv{we{Dl {R%3d {B--\n\r",
				end_string(wizranks[level], 23), level + LEVEL_IMMORTAL );

		lvl_cnt[level] = 0;
	}
	for (pwiz = wiz_list; pwiz != NULL; pwiz = pwiz->next)
	{
		level = UMAX( UMIN(pwiz->level, MAX_LEVEL), LEVEL_IMMORTAL );
		level -= LEVEL_IMMORTAL;

		sprintf(buf, "{W%s %s", pwiz->name, pwiz->desc);
		strcat(lvl_buf[level], "{B-   ");
		strcat(lvl_buf[level], end_string(buf, 34));
		strcat(lvl_buf[level], "  {B-\n\r");
		lvl_cnt[level]++;
	}

	output = new_buf();
	add_buf(output, "{B-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\r");
	add_buf(
			output,
			"{B- {WThe Immortals of  "GAME_COL_NAME"{B -\n\r");
	add_buf(output, "{B-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\r");

	for (level = WIZRANK_MAX - 1; level >= 0; level--)
	{
		if (lvl_cnt[level] <= 0)
			continue;
		strcat(lvl_buf[level], "-                                       -\n\r");
		add_buf(output, lvl_buf[level]);
	}

	if (IS_IMMORTAL( ch ))
	{
		add_buf(output, "{B-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\r");
		add_buf(output, "{B-- {WAvailable arguments:                {B--\n\r");
		if (get_trust(ch) >= MAX_LEVEL)
		{
			add_buf(output,
					"{B-   {Wadd <name> <lvl>   {B-{w add an entry   {B-\n\r");
			add_buf(output,
					"{B-   {Wdelete <name>      {B-{w clear an entry {B-\n\r");
			add_buf(output,
					"{B-   {Wdesc <name> <desc> {B-{w edit a desc    {B-\n\r");
		}
		else
			add_buf(output,
					"{B-   {Wdesc <desc> {B- {wedit your description {B-\n\r");
	}
	add_buf(output, "{B-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-{x\n\r");
	page_to_char(output->string, ch);
	free_buf(output);
}

