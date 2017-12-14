#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include "merc.h"
#include "newclan.h"
#include "recycle.h"
#include "global_mods.h"

bool load_ch_obj_nodesc(CHAR_DATA *ch, char *name);

// Effectively the normal game function pointer, but defined here
// in case we want to change it in the future.
typedef void (*TEST_FUN)(CHAR_DATA*, char*);
#define TEST_FUN_NAME(Name) testfunc_ ## Name

#define TEST_FUN_DEF(Name) \
	void TEST_FUN_NAME(Name) (CHAR_DATA *ch, char *argument)

#define TEST_FUN_ENTRY(Name) \
	#Name, TEST_FUN_NAME(Name)

#define MAX_NAME_LENGTH		128

typedef struct test_func_entry
{
	const char *name; // Command name
	TEST_FUN func; // Function pointer
	int min_trust; // Min trust to see the command
	const char *desc; // Description of the command
	char *restrict_people[MAX_NAME_LENGTH]; // If { NULL }, anyone with > min_trust can run this, otherwise a list of the only people who can run this
} TEST_TAB_ENTRY;

// Define functions here in the same way as TEST_FUN_DEF(crash).

TEST_FUN_DEF(crash)
{
	send_to_char("Crashing the mud!\r\n", ch);
	// Since the character isn't an NPC, this should crash it.
	int vnum;
	vnum = ch->pIndexData->vnum;
}

// Defining this here instead of just inserting make_rosters into table.
TEST_FUN_DEF(make_rost)
{
	send_to_char("Making rosters from rosterlist.txt", ch);
	make_rosters(ch, argument);
}

TEST_FUN_DEF(fix_noob)
{
	ROSTER_DATA *roster;
	FILE *fp;
	char name[MAX_STRING_LENGTH];

	if ((fp = fopen("../../rostlist.txt", "r")) == NULL)
	{
		send_to_char("No rostlist.\n\r", ch);
		return;
	}

	while (!feof(fp))
	{
		strcpy(name, fread_word(fp));

		if (name[0] == '#')
			break;

		if ((roster = get_roster_by_name(name, FALSE)) == NULL)
		{
			send_to_char("No roster for ", ch);
			send_to_char(name, ch);
			send_to_char("\r\n", ch);
		}

		fread_number(fp);
		fread_number(fp);

		if (fread_number(fp) > 0)
		{
			SET_BIT( roster->flags, ROST_NEWBIE );

			send_to_char("Newbied ", ch);
			send_to_char(name, ch);
			send_to_char("\r\n", ch);
		}

		fread_number(fp);
	}
	update_all_rosters();
	fclose(fp);
}

TEST_FUN_DEF(ref_pfiles) {
	CHAR_DATA *vch;
	DIR *plrdir;
	struct dirent *entry;
	char dirname[512], letter, buf[MAX_STRING_LENGTH];

	for (letter = 'a'; letter <= 'z'; letter++) {
		sprintf(dirname, "../player/%c/", letter);
		if ((plrdir = opendir(dirname)) != NULL) {
			while ((entry = readdir(plrdir)) != NULL) {
				if (entry->d_name && isupper(entry->d_name[0])) {
					for (vch = char_list; vch != NULL; vch = vch->next)
						if (!str_cmp(entry->d_name, vch->name))
							break;
					if (vch != NULL) {
						sprintf(buf, "Skipping %s\r\n", vch->name);
						send_to_char(buf, ch);
						continue;
					}
					sprintf(buf, "Refreshing %s\r\n", entry->d_name);
					send_to_char(buf, ch);
					vch = new_char();
					vch->pcdata = new_pcdata();
					if (load_ch_obj_nodesc(vch, entry->d_name))
						save_char_obj(vch);
					extract_char(vch, TRUE);
				}
			}
			closedir(plrdir);
		}
	}
}

TEST_FUN_DEF(inc_xpmod) {
	send_to_char("Increasing xpmod.\r\n", ch);
	printf_debug("XPMOD increased: %s.", gmod_add(GMOD_XP, 0.25f) ? "success" : "fail");
}

TEST_FUN_DEF(inc_qpmod) {
	send_to_char("Increasing qpmod.\r\n", ch);
	printf_debug("QPMOD increased: %s.", gmod_add(GMOD_QP, 0.25f) ? "success" : "fail");
}

void clanskill_check();

TEST_FUN_DEF(reset_cskills) {
	CLAN_DATA *clan;
	char buf[MAX_STRING_LENGTH];
	int i;

	for (clan = clan_list; clan != NULL; clan = clan->next) {
		for (i = 0; i < 2; i++) {
			if (clan->skills[i] != -1) {
				sprintf(buf, "Resetting %s{x on %s{x.\r\n",
					clan_skill_table[clan->skills[i]].name, clan->c_name);
				send_to_char(buf, ch);
				clan->platinum += clan_skill_table[clan->skills[i]].platinum;
				clan->qps += clan_skill_table[clan->skills[i]].qps;
				clan->skills[i] = -1;
			}
		}
	}

	CHAR_DATA *vch;
	for (vch = char_list; vch != NULL; vch = vch->next)
		if (!IS_NPC(vch))
			clanskill_check();
	save_all_clans();
}

void populate_items();

// Add entries so your functions are exposed to the wide world.

TEST_TAB_ENTRY test_func_table[] =
{
/* { TEST_FUN_ENTRY(your_func_name),
 Trust to see command,
 Description of the command,
 {NULL} if not restricted, otherwise { "Name", "Name", NULL } for a whitelist of people who can use the command }
 */
{ TEST_FUN_ENTRY(crash), 110, "Crash the mud", { "Nico", NULL}},
{ TEST_FUN_ENTRY(make_rost), 110, "Make rosters", { "Nico", NULL}},
{ TEST_FUN_ENTRY(fix_noob), 110, "Fix newbie status", { "Nico", NULL }},
{ TEST_FUN_ENTRY(ref_pfiles), 110, "Refresh Pfiles", {NULL}},
{ TEST_FUN_ENTRY(inc_xpmod), 110, "Increment expmod", {NULL}},
{ TEST_FUN_ENTRY(inc_qpmod), 110, "Increment qpmod", {NULL}},
{ TEST_FUN_ENTRY(reset_cskills), 110, "Reset all the clanskills", {NULL}},

// DO NOT REMOVE THE FOLLOWING LINE OR PUT ENTRIES AFTER IT.

		{	NULL, NULL, 0, NULL,
			{	NULL}}
	};

// Housekeeping code. Shouldn't need to change this stuff.

bool is_in_list(char *elem, char **list)
{
	if (list[0] == NULL)
		return TRUE; // Short circuit if there are no names.
	while (*list != NULL)
		if (!str_cmp(elem, *(list++)))
			return TRUE;
	return FALSE;
}

void do_test(CHAR_DATA *ch, char *argument)
{
	if (IS_NPC(ch))
	{
		send_to_char("NPCs can't test things.\r\n", ch);
		return;
	}

	TEST_TAB_ENTRY *entry;
	int trust = get_trust(ch);

	if (argument[0] == '\0')
	{
		char buf[MAX_STRING_LENGTH];
		bool found = FALSE;
		send_to_char("Available choices:\r\n", ch);
		for (entry = test_func_table; entry->name != NULL; entry++)
		{
			if (entry->min_trust > trust)
				continue;
			sprintf(buf, " [%-10s] - %s%s\r\n", entry->name, entry->desc,
					(is_in_list(ch->name, entry->restrict_people) ? ""
							: " ({RRestricted{x)"));
			send_to_char(buf, ch);
			found = TRUE;
		}
		if (!found)
			send_to_char(" NOTHING!\r\n", ch);
		return;
	}

	char arg[MAX_INPUT_LENGTH];
	argument = one_argument(argument, arg);

	for (entry = test_func_table; entry->name != NULL; entry++)
	{
		if (entry->min_trust > trust)
			continue;
		if (!str_cmp(arg, entry->name))
		{
			if (!is_in_list(ch->name, entry->restrict_people))
				send_to_char(
						"This command is restricted to a list of people.\r\n",
						ch);
			else
				(*entry->func)(ch, argument);
			return;
		}
	}

	send_to_char("Test command not found.\r\n", ch);
	do_test(ch, "");
}
