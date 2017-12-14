/* Required headers... */

/**************************************************************************
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

/* CLAN CODE by Nico/Zarko(Deryck Arnold) - if it breaks, you know who to blame!
 * Written exclusively for Asgardian Nightmares - point system idea by Solphor.
 * newclan.h contains all the header information.
 */

#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <sys/timeb.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <malloc.h>
#include <ctype.h>
#include "merc.h"
#include "recycle.h"
#include "interp.h"
#include "tables.h"
#include "olc.h"
#include "magic.h"
#include "newclan.h"
#include "pathfind.h"

//void update_power(CHAR_DATA *barbarian);

#if !defined(IS_VALID)
#define IS_VALID( data )	( ( data ) != NULL && ( data )->valid )
#endif
#if !defined(VALIDATE)
#define VALIDATE( data )	( ( data )->valid = TRUE )
#endif
#if !defined(INVALIDATE)
#define INVALIDATE( data )	( ( data )->valid = FALSE )
#endif

#define is_main_server()	TRUE

/************************************** VARIABLES *************************************/

PKHIST_DATA * pkhist_table; // Spamkill data table
int max_pkhist_entries; // Number of entries in the spamkill table

PKILL_DATA * pkill_data_free; // Pointer to PKILL_DATA free list

CLAN_DATA * clan_list; // Pointer to clan list
CLAN_DATA * clan_free; // Pointer to CLAN_DATA free list
int max_clan; // Number of present clans

ROSTER_DATA * roster_list; // Pointer to roster list
ROSTER_DATA * roster_free; // Pointer to ROSTER_DATA free list

time_t roster_next_update; // Time of next update to make sure rosters update every so often
time_t roster_next_penalty; // Time of next penalty check to make sure people are penalised for stuff every so often (heh)


/********************************* EXTERNAL FUNCTIONS *********************************/

char * length_argument( char *argument, char *arg_first, int length );
void free_auction( AUCTION_DATA *auction );
void save_area( AREA_DATA *pArea );
bool is_safe_quiet( CHAR_DATA * ch, CHAR_DATA * victim );

/*************************************** TABLES ***************************************/

/* Central location for clan log entry type strings */
const struct log_type log_entry_string[] =
{
{ "deposit", "{GDEPOSIT" }, // ENTRY_DEPOSIT
		{ "", "{RDELETED" }, // ENTRY_DELETE
		{ "upgrade", "{CUPGRADE" }, // ENTRY_UPGRADE
		{ "", "{GCREATED" }, // ENTRY_CREATE
		{ "member", "{MNEWMEMB" }, // ENTRY_ACCEPT
		{ "guilds", "{CGUILDED" }, // ENTRY_GUILD
		{ "trust", "{WTRUSTED" }, // ENTRY_TRUST
		{ "exile", "{REXILED " }, // ENTRY_EXILE
		{ "war", "{R--{YWAR{R--" }, // ENTRY_WAR
		{ "changed", "{cCHANGED" }, // ENTRY_CHANGED
		{ "", "", }, };

const struct clan_ranks clan_rank_table[TOTAL_CLAN_RANK] =
{
{ "{MM{member{x", "{MM{mem{x", RANK_MEMBER },
{ "{RR{recruit{x", "{RR{rec{x", RANK_RECRUIT },
{ "{GS{goldier{x", "{GS{gdr{x", RANK_SOLDIER },
{ "{YL{yieutenant{x", "{YL{yie{x", RANK_LIEUTENANT },
{ "{WC{wommander{x", "{WC{wdr{x", RANK_COMMANDER },
{ "{CE{clite {CC{captain{x", "{CE{clt{x", RANK_ECAPTAIN } };

const struct vnum_table cmob_table[CMOB_MAX + 1] =
{
{ "guard", VNUM_STOCK_MOB_GUARD }, // CMOB_GUARD
		{ "mage", VNUM_STOCK_MOB_MAGE }, // CMOB_MAGE
		{ "mason", VNUM_STOCK_MOB_MASON }, // CMOB_MASON
		{ "trainer", VNUM_STOCK_MOB_TRAINER }, // CMOB_TRAINER
		{ "blacksmith", VNUM_STOCK_MOB_SMITH }, // CMOB_SMITH
		{ "brewer", VNUM_STOCK_MOB_BREWER }, // CMOB_BREWER
		{ "baker", VNUM_STOCK_MOB_BAKER }, // CMOB_BAKER
		{ "good_priest", VNUM_STOCK_MOB_GPRIEST }, // CMOB_GPRIEST
		{ "evil_priest", VNUM_STOCK_MOB_EPRIEST }, // CMOB_EPRIEST
		{ "healer", VNUM_STOCK_MOB_HEALER }, // CMOB_HEALER
		{ "shopkeep", VNUM_STOCK_MOB_SHOPKEEP }, // CMOB_SHOPKEEP
		{ "pet", VNUM_STOCK_MOB_PET }, // CMOB_PET
		{ "tanner", VNUM_STOCK_MOB_TANNER }, // CMOB_TANNER
		{ "questor", VNUM_STOCK_MOB_QUESTOR }, // CMOB_QUESTOR
		{ "", -1 } };

const struct vnum_table croom_table[CROOM_MAX + 1] =
{
{ "recall", VNUM_STOCK_ROOM_RECALL }, // CROOM_RECALL
		{ "smithy", VNUM_STOCK_ROOM_SMITHY }, // CROOM_SMITHY
		{ "workshop", VNUM_STOCK_ROOM_WORKSHOP }, // CROOM_WORKSHOP
		{ "good_shrine", VNUM_STOCK_ROOM_GSHRINE }, // CROOM_GSHRINE
		{ "evil_shrine", VNUM_STOCK_ROOM_ESHRINE }, // CROOM_ESHRINE
		{ "shop", VNUM_STOCK_ROOM_SHOP }, // CROOM_SHOP
		{ "troom", VNUM_STOCK_ROOM_TROOM }, // CROOM_TROOM
		{ "stable", VNUM_STOCK_ROOM_STABLE }, // CROOM_STABLE
		{ "bakery", VNUM_STOCK_ROOM_BAKERY }, // CROOM_BAKERY
		{ "brewery", VNUM_STOCK_ROOM_BREWERY }, // CROOM_BREWERY
		{ "tannery", VNUM_STOCK_ROOM_TANNERY }, // CROOM_TANNERY
		{ "", -1 } };

const struct vnum_table portloc_table[] =
{
{ "midgaard", 3014 },
{ "hopecity", 43009 },
{ "ghostship", 8100 },
{ "box", 45051 },
{ "blackwater", 42803 },
{ "empar", 37001 },
{ "kingdoms", 17536 },
{ "plords", 14269 },
{ "dragontower", 2201 },
{ "hell", 10406 },
{ "plordsiceberg", 14277 },
{ "plordsflyingmountain", 14258 },
{ "", -1 } };

const char * process_name_word[] =
{ " a ", " an ", " and ", " at ", " her ", " his ", " in ", " is ", " it ",
		" of ", " on ", " the ", " to ", " with ", " where ", "" };

const struct cedit_cmd_type main_cedit_table[] =
{
{ "create", cedit_create, IMPLEMENTOR, TRUST_NONE, "create a new clan." },
{ "delete", cedit_delete, IMPLEMENTOR, TRUST_NONE,
		"delete a clan and ALL it's assets." },
{ "deposit", cedit_deposit, IMPLEMENTOR, TRUST_MEMBER,
		"deposit either platinum or questpoints into the clan banks." },
{ "description", cedit_desc, SUPREME, TRUST_LDR,
		"change the description of the clan." },
{ "guild", cedit_guild, SUPREME, TRUST_NONE, "guild someone into THIS clan." },
{ "independent", cedit_indep, IMPLEMENTOR, TRUST_NONE,
		"make this clan independent or not." },
{ "log", cedit_log, SUPREME, TRUST_VLR, "show the clan log." },
{ "member", cedit_member, SUPREME, TRUST_LDR,
		"view, trust or exile clan members." },
		{ "mob", cedit_mob, SUPREME, TRUST_VLR,
				"view, purchase or edit clan mobiles." },
		{ "name", cedit_name, SUPREME, TRUST_LDR, "change the clan name." },
		{ "obj", cedit_obj, SUPREME, TRUST_VLR,
				"view, purchase or edit clan equipment." },
		{ "petition", cedit_petition, IMPLEMENTOR, TRUST_VLR,
				"view, accept or reject membership petitions." },
		{ "pkill", cedit_pkill, SUPREME, TRUST_NONE,
				"make this clan a player-killing clan or not." },
		{ "rank", cedit_rank, SUPREME, TRUST_LDR,
				"edit the clan rank who and full names." },
		{ "room", cedit_room, SUPREME, TRUST_VLR,
				"view, purchase or edit clan rooms." },
		{ "show", cedit_show, SUPREME, TRUST_MEMBER,
				"show the cedit screen again." },
		{ "skills", cedit_skill, SUPREME, TRUST_MEMBER,
				"change to scedit to view, buy or sell clan skills." },
		{ "time", cedit_time, IMPLEMENTOR, TRUST_LDR,
				"buy more time to edit basic clan data." },
		{ "war", cedit_war, IMPLEMENTOR, TRUST_LDR,
				"declare or accept petitions of war." },
		{ "", NULL, TRUST_NONE, TRUST_NONE, "" } };

const struct cedit_cmd_type mob_cedit_table[] =
{
{ "buy", mcedit_buy, SUPREME, TRUST_VLR,
		"buy or view upgrades for the current mobile." },
{ "desc", mcedit_desc, SUPREME, TRUST_VLR,
		"edit the detailed description of the mobile." },
{ "long", mcedit_long, SUPREME, TRUST_VLR,
		"edit the long description(in room) of the mobile." },
{ "short", mcedit_short, SUPREME, TRUST_VLR,
		"edit the short description(interaction) of the mobile." },
{ "show", mcedit_show, SUPREME, TRUST_VLR, "show the mcedit screen again." },
{ "", NULL, TRUST_NONE, TRUST_NONE, "" } };

const struct cedit_cmd_type obj_cedit_table[] =
{
{ "buy", ocedit_buy, SUPREME, TRUST_VLR,
		"buy or view upgrades for the current object." },
{ "long", ocedit_long, SUPREME, TRUST_VLR,
		"edit the long description(in room) of the object." },
		{ "junk", ocedit_sell, SUPREME, TRUST_LDR,
				"junk the object - no money back." },
		{ "short", ocedit_short, SUPREME, TRUST_VLR,
				"edit the short description(interaction) of the object." },
		{ "show", ocedit_show, SUPREME, TRUST_VLR,
				"show the ocedit screen again." },
		{ "", NULL, TRUST_NONE, TRUST_NONE, "" } };

const struct cedit_cmd_type room_cedit_table[] =
{
{ "buy", rcedit_buy, SUPREME, TRUST_VLR,
		"buy or view upgrades for the current room." },
{ "desc", rcedit_desc, SUPREME, TRUST_VLR, "edit the room description." },
{ "exdesc", rcedit_exdesc, SUPREME, TRUST_VLR,
		"edit the extra descriptions and their keywords." },
{ "name", rcedit_name, SUPREME, TRUST_VLR, "edit the name of the room." },
{ "show", rcedit_show, SUPREME, TRUST_VLR, "show the rcedit screen again." },
{ "", NULL, TRUST_NONE, TRUST_NONE, "" } };

const struct cedit_cmd_type skill_cedit_table[] =
{
{ "buy", scedit_buy, SUPREME, TRUST_LDR, "buy a clan skill." },
{ "list", scedit_show, SUPREME, TRUST_MEMBER, "list the clan skills." },
{ "sell", scedit_sell, SUPREME, TRUST_LDR, "sell a clan skill." },
{ "show", scedit_show, SUPREME, TRUST_MEMBER,
		"same as list - list the clan skills." },
{ "", NULL, TRUST_NONE, TRUST_NONE, "" } };

struct clan_skill_cmds
		clan_skill_table[] =
		{
				{ "adrenaline", "Adrenaline",
						"Damroll bonus equal to 60%% of your level.",
						RANK_RECRUIT, PRICE_SKILL_MINOR_QP,
						PRICE_SKILL_MINOR_PLAT, &gsn_adrenaline, FALSE,
						POS_STANDING, do_adrenaline },
				{
						"bfury",
						"Battle Fury",
						"Damroll, hitroll and armour bonuses equal to a 7th of your level. Additional steel skin shield.",
						RANK_RECRUIT, PRICE_SKILL_MINOR_QP,
						PRICE_SKILL_MINOR_PLAT, &gsn_battle_fury, FALSE,
						POS_STANDING, do_battle_fury },
				{
						"blust",
						"Blood Lust",
						"Damroll, hitroll and armour bonuses equal to a 6th of your level.",
						RANK_RECRUIT, PRICE_SKILL_MINOR_QP,
						PRICE_SKILL_MINOR_PLAT, &gsn_bloodlust, FALSE,
						POS_STANDING, do_bloodlust },
				{
						"conceal",
						"Conceal",
						"Damroll and hitroll bonuses equal to an 8th of your level. Morphs you into \"A dark assassin\" and adds sneak.",
						RANK_RECRUIT, PRICE_SKILL_MINOR_QP,
						PRICE_SKILL_MINOR_PLAT, &gsn_concealClan1, FALSE,
						POS_STANDING, do_conceal },
//				{
//						"cripple",
//						"Cripple",
//						"Combat skill resulting in damage and possible blindness, possible disarmament or possible reduction in moves of the victim.",
//						RANK_LIEUTENANT, PRICE_SKILL_MAJOR_QP,
//						PRICE_SKILL_MAJOR_PLAT, NULL, TRUE, POS_FIGHTING, NULL,
//						do_cripple },
				{
						"divide",
						"The Divide",
						"Damroll, hitroll and armour bonuses equal to a 7th of your level. Additional stone skin shield.",
						RANK_RECRUIT, PRICE_SKILL_MINOR_QP,
						PRICE_SKILL_MINOR_PLAT, &gsn_divide, FALSE,
						POS_STANDING, do_divide },
				{
						"faith",
						"Blind Faith",
						"A skill causing damage and possible blindness of the victim.",
						RANK_LIEUTENANT, PRICE_SKILL_MAJOR_QP,
						PRICE_SKILL_MAJOR_PLAT, NULL, TRUE, POS_FIGHTING,
						do_faith },
				{
						"modi",
						"Power of Modi",
						"A two-part skill - either a damroll bonus or hitroll bonus equal to 40% of your level.",
						RANK_RECRUIT, PRICE_SKILL_MINOR_QP,
						PRICE_SKILL_MINOR_PLAT, NULL, FALSE, POS_STANDING,
						do_modi },
				{ "sform", "Shadow Form",
						"Armour bonus equal to 60% of your level.",
						RANK_RECRUIT, PRICE_SKILL_MINOR_QP,
						PRICE_SKILL_MINOR_PLAT, &gsn_shadow_form, FALSE,
						POS_STANDING, do_shadow_form },
//				{
//						"solar",
//						"Solar Flare",
//						"Room-effect blindness skill. NOTE: There is a 1% chance that you can blind yourself when using it.",
//						RANK_LIEUTENANT, PRICE_SKILL_MAJOR_QP,
//						PRICE_SKILL_MAJOR_PLAT, NULL, TRUE, POS_STANDING, NULL,
//						do_solar_flare },
				{
						"vmight",
						"Might of the Vanir",
						"Damroll and armour bonuses equal to a 5th of your level. Additional stone skin shield.",
						RANK_RECRUIT, PRICE_SKILL_MINOR_QP,
						PRICE_SKILL_MINOR_PLAT, &gsn_vmight, FALSE,
						POS_STANDING, do_vmight },
				{
						"wform",
						"Wolf Form",
						"Damroll and hitroll bonuses equal to an 8th of your level. Morphs you into \"A wolfish beast\" and adds sneak.",
						RANK_RECRUIT, PRICE_SKILL_MINOR_QP,
						PRICE_SKILL_MINOR_PLAT, &gsn_concealClan2, FALSE,
						POS_STANDING, do_wform },
				{ "", "", "", 0, 0, 0, NULL, FALSE, POS_DEAD, NULL } };

COST_DATA mob_prices[] =
{
{ .name = "guard", .func = mob_normal, .descr = "A guard to protect your hall",
		.curr_value = cval_mob_normal, .flags = UPGD_NO_BULK, .quantity = 1,
		.max = 1, .specific[0] = CMOB_GUARD, .specific[1] = CROOM_RECALL,
		.max_func = NULL, .qps = 300, .platinum = 10000,
		.cRoom[CROOM_RECALL] = TRUE },

{ .name = "mage", .func = mob_normal, .descr = "Your friendly in-house mage",
		.curr_value = cval_mob_normal, .flags = UPGD_NO_BULK, .quantity = 1,
		.max = 1, .specific[0] = CMOB_MAGE, .specific[1] = CROOM_WORKSHOP,
		.max_func = NULL, .qps = 1000, .platinum = 5000,
		.cRoom[CROOM_WORKSHOP] = TRUE, .cRoom[CROOM_RECALL] = TRUE },

{ .name = "mason", .func = mob_normal,
		.descr = "Someone who you can't do without for clan rooms",
		.curr_value = cval_mob_normal, .flags = UPGD_NO_BULK, .quantity = 1,
		.max = 1, .specific[0] = CMOB_MASON, .specific[1] = CROOM_RECALL,
		.max_func = NULL, .qps = 100, .platinum = 2500,
		.cRoom[CROOM_RECALL] = TRUE },

{ .name = "trainer", .func = mob_normal,
		.descr = "Someone to train your mobiles",
		.curr_value = cval_mob_normal, .flags = UPGD_NO_BULK, .quantity = 1,
		.max = 1, .specific[0] = CMOB_TRAINER, .specific[1] = CROOM_TROOM,
		.max_func = NULL, .qps = 1200, .platinum = 8000,
		.cRoom[CROOM_TROOM] = TRUE, .cRoom[CROOM_RECALL] = TRUE },

{ .name = "blacksmith", .func = mob_normal,
		.descr = "A pretty essential person for good-quality equipment",
		.curr_value = cval_mob_normal, .flags = UPGD_NO_BULK, .quantity = 1,
		.max = 1, .specific[0] = CMOB_SMITH, .specific[1] = CROOM_SMITHY,
		.max_func = NULL, .qps = 500, .platinum = 12000,
		.cRoom[CROOM_SMITHY] = TRUE, .cRoom[CROOM_RECALL] = TRUE },

{ .name = "brewer", .func = mob_normal, .descr = "Brews up potions",
		.curr_value = cval_mob_normal, .flags = UPGD_NO_BULK, .quantity = 1,
		.max = 1, .specific[0] = CMOB_BREWER, .specific[1] = CROOM_BREWERY,
		.max_func = NULL, .qps = 3000, .platinum = 6000,
		.cRoom[CROOM_BREWERY] = TRUE, .cRoom[CROOM_RECALL] = TRUE },

{ .name = "baker", .func = mob_normal,
		.descr = "This one can bake you some food and pills",
		.curr_value = cval_mob_normal, .flags = UPGD_NO_BULK, .quantity = 1,
		.max = 1, .specific[0] = CMOB_BAKER, .specific[1] = CROOM_BAKERY,
		.max_func = NULL, .qps = 4000, .platinum = 8000,
		.cRoom[CROOM_BAKERY] = TRUE, .cRoom[CROOM_RECALL] = TRUE },

{ .name = "good_priest", .func = mob_normal, .descr = "A good priest",
		.curr_value = cval_mob_normal, .flags = UPGD_NO_BULK, .quantity = 1,
		.max = 1, .specific[0] = CMOB_GPRIEST, .specific[1] = CROOM_GSHRINE,
		.max_func = NULL, .qps = 3000, .platinum = 10000,
		.cRoom[CROOM_GSHRINE] = TRUE, .cRoom[CROOM_RECALL] = TRUE },

{ .name = "evil_priest", .func = mob_normal, .descr = "An evil priest",
		.curr_value = cval_mob_normal, .flags = UPGD_NO_BULK, .quantity = 1,
		.max = 1, .specific[0] = CMOB_EPRIEST, .specific[1] = CROOM_ESHRINE,
		.max_func = NULL, .qps = 3000, .platinum = 10000,
		.cRoom[CROOM_WORKSHOP] = FALSE, .cRoom[CROOM_RECALL] = TRUE },

{ .name = "healer", .func = mob_normal,
		.descr = "Quite handy to have around, good for upgrades too",
		.curr_value = cval_mob_normal, .flags = UPGD_NO_BULK, .quantity = 1,
		.max = 1, .specific[0] = CMOB_HEALER, .specific[1] = -1,
		.max_func = NULL, .qps = 5000, .platinum = 12000,
		.cRoom[CROOM_GSHRINE] = TRUE, .cRoom[CROOM_ESHRINE] = TRUE,
		.cRoom[CROOM_RECALL] = TRUE },

{ .name = "shopkeep", .func = mob_normal,
		.descr = "This one sells all the clan goodies",
		.curr_value = cval_mob_normal, .flags = UPGD_NO_BULK, .quantity = 1,
		.max = 1, .specific[0] = CMOB_SHOPKEEP, .specific[1] = CROOM_SHOP,
		.max_func = NULL, .qps = 0, .platinum = 1000,
		.cRoom[CROOM_SHOP] = TRUE, .cRoom[CROOM_RECALL] = TRUE },

{ .name = "pet", .func = mob_normal,
		.descr = "Something to feed if you're bored",
		.curr_value = cval_mob_normal, .flags = UPGD_NO_BULK, .quantity = 1,
		.max = 1, .specific[0] = CMOB_PET, .specific[1] = CROOM_STABLE,
		.max_func = NULL, .qps = 1000, .platinum = 10000,
		.cRoom[CROOM_TROOM] = TRUE, .cRoom[CROOM_STABLE] = TRUE,
		.cRoom[CROOM_RECALL] = TRUE },

{ .name = "tanner", .func = mob_normal,
		.descr = "This one likes corpses, very very much",
		.curr_value = cval_mob_normal, .flags = UPGD_NO_BULK, .quantity = 1,
		.max = 1, .specific[0] = CMOB_TANNER, .specific[1] = CROOM_TANNERY,
		.max_func = NULL, .qps = 250, .platinum = 2000,
		.cRoom[CROOM_TANNERY] = TRUE, .cRoom[CROOM_RECALL] = TRUE },

{ .name = "questor", .func = mob_normal,
		.descr = "This one gives you quests if you're nice",
		.curr_value = cval_mob_normal, .flags = UPGD_NO_BULK, .quantity = 1,
		.max = 1, .specific[0] = CMOB_QUESTOR, .specific[1] = CROOM_RECALL,
		.max_func = NULL, .qps = 3000, .platinum = 0,
		.cRoom[CROOM_RECALL] = TRUE },

{ .name = "", .func = NULL, .descr = "", .curr_value = NULL, .quantity = 0,
		.max = MAX_NONE, .specific[0] = -1, .specific[1] = -1,
		.max_func = NULL, .qps = 0, .platinum = 0, .cMob[CMOB_TRAINER] = 0,
		.cMob[CMOB_SMITH] = 0, .cMob[CMOB_BREWER] = 0, .cMob[CMOB_BAKER] = 0,
		.cMob[CMOB_MAGE] = 0, .cMob[CMOB_MASON] = 0, .cMob[CMOB_GPRIEST] = 0,
		.cMob[CMOB_EPRIEST] = 0, .cMob[CMOB_HEALER] = 0,
		.cMob[CMOB_SHOPKEEP] = 0, .cMob[CMOB_PET] = 0, .cMob[CMOB_TANNER] = 0,
		.cMob[CMOB_QUESTOR] = 0, .cMob[CMOB_GUARD] = 0,
		.cRoom[CROOM_GSHRINE] = FALSE, .cRoom[CROOM_ESHRINE] = FALSE,
		.cRoom[CROOM_SMITHY] = FALSE, .cRoom[CROOM_WORKSHOP] = FALSE,
		.cRoom[CROOM_SHOP] = FALSE, .cRoom[CROOM_TROOM] = FALSE,
		.cRoom[CROOM_STABLE] = FALSE, .cRoom[CROOM_BAKERY] = FALSE,
		.cRoom[CROOM_BREWERY] = FALSE, .cRoom[CROOM_TANNERY] = FALSE,
		.cRoom[CROOM_RECALL] = TRUE } };

COST_DATA
		obj_prices[] =
		{
				{ .name = "furniture", .func = obj_normal,
						.descr = "Something to lounge around on",
						.curr_value = cval_obj_normal,
						.flags = UPGD_ROOM_OBJ|UPGD_NO_BULK, .quantity = 1,
						.max = MAX_SPECIAL,
						.specific[0] = VNUM_STOCK_OBJ_FURNITURE,
						.specific[1] = ITEM_FURNITURE,
						.max_func = mval_obj_normal, .qps = 0,
						.platinum = 2000, .cRoom[CROOM_RECALL] = TRUE },

				{ .name = "portal", .func = obj_normal,
						.descr = "A quick way to get places",
						.curr_value = cval_obj_normal,
						.flags = UPGD_ROOM_OBJ|UPGD_NO_BULK, .quantity = 1,
						.max = MAX_SPECIAL,
						.specific[0] = VNUM_STOCK_OBJ_PORTAL,
						.specific[1] = ITEM_PORTAL,
						.max_func = mval_obj_normal, .qps = 1250,
						.platinum = 4000, .cMob[CMOB_MAGE] = 75,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "scroll", .func = obj_normal,
						.descr = "Good if you can read",
						.curr_value = cval_obj_normal, .flags = UPGD_NO_BULK,
						.quantity = 1, .max = MAX_SPECIAL,
						.specific[0] = VNUM_STOCK_OBJ_SCROLL,
						.specific[1] = ITEM_SCROLL,
						.max_func = mval_obj_normal, .qps = 500,
						.platinum = 1500, .cMob[CMOB_MAGE] = 1,
						.cMob[CMOB_SHOPKEEP] = 1, .cRoom[CROOM_SHOP] = TRUE,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "wand", .func = obj_normal,
						.descr = "Zapping people can be fun with one of these",
						.curr_value = cval_obj_normal, .flags = UPGD_NO_BULK,
						.quantity = 1, .max = MAX_SPECIAL,
						.specific[0] = VNUM_STOCK_OBJ_WAND,
						.specific[1] = ITEM_WAND, .max_func = mval_obj_normal,
						.qps = 500, .platinum = 1500, .cMob[CMOB_MAGE] = 1,
						.cMob[CMOB_SHOPKEEP] = 1, .cRoom[CROOM_SHOP] = TRUE,
						.cRoom[CROOM_RECALL] = TRUE },

				{
						.name = "staff",
						.func = obj_normal,
						.descr = "Just like the wand, only bigger (size doesn't matter, or does it?)",
						.curr_value = cval_obj_normal, .flags = UPGD_NO_BULK,
						.quantity = 1, .max = MAX_SPECIAL,
						.specific[0] = VNUM_STOCK_OBJ_STAFF,
						.specific[1] = ITEM_STAFF, .max_func = mval_obj_normal,
						.qps = 500, .platinum = 1500, .cMob[CMOB_MAGE] = 1,
						.cMob[CMOB_SHOPKEEP] = 1, .cRoom[CROOM_SHOP] = TRUE,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "weapon", .func = obj_normal,
						.descr = "Good for hurting people",
						.curr_value = cval_obj_normal, .flags = UPGD_NO_BULK,
						.quantity = 1, .max = MAX_SPECIAL,
						.specific[0] = VNUM_STOCK_OBJ_WEAPON,
						.specific[1] = ITEM_WEAPON,
						.max_func = mval_obj_normal, .qps = 500,
						.platinum = 4000, .cMob[CMOB_SMITH] = 1,
						.cMob[CMOB_SHOPKEEP] = 1, .cRoom[CROOM_SHOP] = TRUE,
						.cRoom[CROOM_RECALL] = TRUE },

				{
						.name = "armour",
						.func = obj_normal,
						.descr = "Good for covering those.. bits that need covering",
						.curr_value = cval_obj_normal, .flags = UPGD_NO_BULK,
						.quantity = 1, .max = MAX_SPECIAL,
						.specific[0] = VNUM_STOCK_OBJ_ARMOUR,
						.specific[1] = ITEM_ARMOR, .max_func = mval_obj_normal,
						.qps = 500, .platinum = 4000, .cMob[CMOB_SMITH] = 1,
						.cMob[CMOB_SHOPKEEP] = 1, .cRoom[CROOM_SHOP] = TRUE,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "potion", .func = obj_normal,
						.descr = "Useful when you're out in the field",
						.curr_value = cval_obj_normal, .flags = UPGD_NO_BULK,
						.quantity = 1, .max = MAX_SPECIAL,
						.specific[0] = VNUM_STOCK_OBJ_POTION,
						.specific[1] = ITEM_POTION,
						.max_func = mval_obj_normal, .qps = 1000,
						.platinum = 4000, .cMob[CMOB_BREWER] = 1,
						.cMob[CMOB_MAGE] = 1, .cMob[CMOB_SHOPKEEP] = 1,
						.cRoom[CROOM_SHOP] = TRUE, .cRoom[CROOM_RECALL] = TRUE },

				{ .name = "container", .func = obj_normal,
						.descr = "Holds all your loot",
						.curr_value = cval_obj_normal, .flags = UPGD_NO_BULK,
						.quantity = 1, .max = MAX_SPECIAL,
						.specific[0] = VNUM_STOCK_OBJ_CONTAINER,
						.specific[1] = ITEM_CONTAINER,
						.max_func = mval_obj_normal, .qps = 500,
						.platinum = 6000, .cMob[CMOB_SMITH] = 1,
						.cMob[CMOB_SHOPKEEP] = 1, .cRoom[CROOM_SHOP] = TRUE,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "dcontainer", .func = obj_normal,
						.descr = "An excellent choice for booze",
						.curr_value = cval_obj_normal, .flags = UPGD_NO_BULK,
						.quantity = 1, .max = MAX_SPECIAL,
						.specific[0] = VNUM_STOCK_OBJ_DCONTAINER,
						.specific[1] = ITEM_DRINK_CON,
						.max_func = mval_obj_normal, .qps = 500,
						.platinum = 2000, .cMob[CMOB_SMITH] = 1,
						.cMob[CMOB_BREWER] = 1, .cMob[CMOB_SHOPKEEP] = 1,
						.cRoom[CROOM_SHOP] = TRUE, .cRoom[CROOM_RECALL] = TRUE },

				{ .name = "food", .func = obj_normal,
						.descr = "Something to nibble on when hungry",
						.curr_value = cval_obj_normal, .flags = UPGD_NO_BULK,
						.quantity = 1, .max = MAX_SPECIAL,
						.specific[0] = VNUM_STOCK_OBJ_FOOD,
						.specific[1] = ITEM_FOOD, .max_func = mval_obj_normal,
						.qps = 0, .platinum = 1000, .cMob[CMOB_BAKER] = 1,
						.cMob[CMOB_SHOPKEEP] = 1, .cRoom[CROOM_SHOP] = TRUE,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "boat", .func = obj_normal,
						.descr = "Don't go out to sea without one of these",
						.curr_value = cval_obj_normal, .flags = UPGD_NO_BULK,
						.quantity = 1, .max = MAX_SPECIAL,
						.specific[0] = VNUM_STOCK_OBJ_BOAT,
						.specific[1] = ITEM_BOAT, .max_func = mval_obj_normal,
						.qps = 0, .platinum = 2000, .cMob[CMOB_SMITH] = 1,
						.cMob[CMOB_SHOPKEEP] = 1, .cRoom[CROOM_SHOP] = TRUE,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "fountain", .func = obj_normal,
						.descr = "Water, milk, coke, beer, you pick",
						.curr_value = cval_obj_normal,
						.flags = UPGD_ROOM_OBJ|UPGD_NO_BULK, .quantity = 1,
						.max = MAX_SPECIAL,
						.specific[0] = VNUM_STOCK_OBJ_FOUNTAIN,
						.specific[1] = ITEM_FOUNTAIN,
						.max_func = mval_obj_normal, .qps = 250,
						.platinum = 3000, .cMob[CMOB_SMITH] = 1,
						.cMob[CMOB_MAGE] = 1, .cRoom[CROOM_RECALL] = TRUE },

				{ .name = "slot_machine", .func = obj_normal,
						.descr = "Need to satisfy that gambling addiction?",
						.curr_value = cval_obj_normal,
						.flags = UPGD_ROOM_OBJ|UPGD_NO_BULK, .quantity = 1,
						.max = MAX_SPECIAL,
						.specific[0] = VNUM_STOCK_OBJ_SLOT_MACHINE,
						.specific[1] = ITEM_SLOT_MACHINE,
						.max_func = mval_obj_normal, .qps = 0,
						.platinum = 3000, .cMob[CMOB_SMITH] = 1,
						.cRoom[CROOM_RECALL] = TRUE, .cMob[CMOB_SHOPKEEP] = 1, },

				{ .name = "pill", .func = obj_normal,
						.descr = "Popping pills can be good for you",
						.curr_value = cval_obj_normal, .flags = UPGD_NO_BULK,
						.quantity = 1, .max = MAX_SPECIAL,
						.specific[0] = VNUM_STOCK_OBJ_PILL,
						.specific[1] = ITEM_PILL, .max_func = mval_obj_normal,
						.qps = 2000, .platinum = 8000, .cMob[CMOB_BREWER] = 1,
						.cMob[CMOB_BAKER] = 1, .cMob[CMOB_GPRIEST] = 1,
						.cMob[CMOB_EPRIEST] = 1, .cMob[CMOB_SHOPKEEP] = 1,
						.cRoom[CROOM_SHOP] = TRUE, .cRoom[CROOM_RECALL] = TRUE },

				{ .name = "warp_stone", .func = obj_normal,
						.descr = "Portals on the go with the the right spells",
						.curr_value = cval_obj_normal, .flags = UPGD_NO_BULK,
						.quantity = 1, .max = MAX_SPECIAL,
						.specific[0] = VNUM_STOCK_OBJ_WARP_STONE,
						.specific[1] = ITEM_WARP_STONE,
						.max_func = mval_obj_normal, .qps = 2000,
						.platinum = 5000, .cMob[CMOB_MAGE] = 150,
						.cMob[CMOB_SHOPKEEP] = 1, .cRoom[CROOM_SHOP] = TRUE,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "demon_stone", .func = obj_normal,
						.descr = "A demon in a can (well.. more like a stone)",
						.curr_value = cval_obj_normal, .flags = UPGD_NO_BULK,
						.quantity = 1, .max = MAX_SPECIAL,
						.specific[0] = VNUM_STOCK_OBJ_DEMON_STONE,
						.specific[1] = ITEM_DEMON_STONE,
						.max_func = mval_obj_normal, .qps = 2000,
						.platinum = 5000, .cMob[CMOB_GPRIEST] = 150,
						.cMob[CMOB_EPRIEST] = 150, .cMob[CMOB_SHOPKEEP] = 1,
						.cRoom[CROOM_SHOP] = TRUE, .cRoom[CROOM_RECALL] = TRUE },

				{ .name = "light", .func = obj_normal,
						.descr = "Scared of the dark?",
						.curr_value = cval_obj_normal, .flags = UPGD_NO_BULK,
						.quantity = 1, .max = MAX_SPECIAL,
						.specific[0] = VNUM_STOCK_OBJ_LIGHT,
						.specific[1] = ITEM_LIGHT, .max_func = mval_obj_normal,
						.qps = 500, .platinum = 4000, .cMob[CMOB_SMITH] = 1,
						.cMob[CMOB_SHOPKEEP] = 1, .cRoom[CROOM_SHOP] = TRUE,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "", .func = NULL, .descr = "", .curr_value = NULL,
						.quantity = 0, .max = MAX_NONE, .specific[0] = -1,
						.specific[1] = -1, .max_func = NULL, .qps = 0,
						.platinum = 0, .cMob[CMOB_TRAINER] = 0,
						.cMob[CMOB_SMITH] = 0, .cMob[CMOB_BREWER] = 0,
						.cMob[CMOB_BAKER] = 0, .cMob[CMOB_MAGE] = 0,
						.cMob[CMOB_MASON] = 0, .cMob[CMOB_GPRIEST] = 0,
						.cMob[CMOB_EPRIEST] = 0, .cMob[CMOB_HEALER] = 0,
						.cMob[CMOB_SHOPKEEP] = 0, .cMob[CMOB_PET] = 0,
						.cMob[CMOB_TANNER] = 0, .cMob[CMOB_QUESTOR] = 0,
						.cMob[CMOB_GUARD] = 0, .cRoom[CROOM_GSHRINE] = FALSE,
						.cRoom[CROOM_ESHRINE] = FALSE,
						.cRoom[CROOM_SMITHY] = FALSE,
						.cRoom[CROOM_WORKSHOP] = FALSE,
						.cRoom[CROOM_SHOP] = FALSE,
						.cRoom[CROOM_TROOM] = FALSE,
						.cRoom[CROOM_STABLE] = FALSE,
						.cRoom[CROOM_BAKERY] = FALSE,
						.cRoom[CROOM_BREWERY] = FALSE,
						.cRoom[CROOM_TANNERY] = FALSE,
						.cRoom[CROOM_RECALL] = TRUE } };

COST_DATA room_prices[] =
{
{ .name = "recall", .func = room_recall,
		.descr = "The standard clan recall hall",
		.curr_value = cval_room_normal, .quantity = 1, .max = 1,
		.specific[0] = CROOM_RECALL, .specific[1] = -1, .max_func = NULL,
		.flags = UPGD_NO_BULK, .qps = 0, .platinum = 2000 },

{ .name = "smithy", .func = room_normal,
		.descr = "A smithy to house a blacksmith",
		.curr_value = cval_room_normal, .quantity = 1, .max = 1,
		.specific[0] = CROOM_SMITHY, .specific[1] = -1, .max_func = NULL,
		.flags = UPGD_NO_BULK, .qps = 0, .platinum = 10000,
		.cMob[CMOB_MASON] = 1, .cRoom[CROOM_RECALL] = TRUE },

{ .name = "workshop", .func = room_normal,
		.descr = "A workshop to house a mage", .curr_value = cval_room_normal,
		.quantity = 1, .max = 1, .specific[0] = CROOM_WORKSHOP,
		.specific[1] = -1, .max_func = NULL, .flags = UPGD_NO_BULK,
		.qps = 1000, .platinum = 5000, .cMob[CMOB_MASON] = 1,
		.cRoom[CROOM_RECALL] = TRUE },

{ .name = "good_shrine", .func = room_normal,
		.descr = "A shrine to house a good priest",
		.curr_value = cval_room_normal, .quantity = 1, .max = 1,
		.specific[0] = CROOM_GSHRINE, .specific[1] = -1, .max_func = NULL,
		.flags = UPGD_NO_BULK, .qps = 1000, .platinum = 500,
		.cMob[CMOB_MASON] = 1, .cRoom[CROOM_RECALL] = TRUE },

{ .name = "evil_shrine", .func = room_normal,
		.descr = "A shrine to house an evil priest",
		.curr_value = cval_room_normal, .quantity = 1, .max = 1,
		.specific[0] = CROOM_ESHRINE, .specific[1] = -1, .max_func = NULL,
		.flags = UPGD_NO_BULK, .qps = 1000, .platinum = 500,
		.cMob[CMOB_MASON] = 1, .cRoom[CROOM_RECALL] = TRUE },

{ .name = "shop", .func = room_normal, .descr = "A shop to house a shopkeep",
		.curr_value = cval_room_normal, .quantity = 1, .max = 1,
		.specific[0] = CROOM_SHOP, .specific[1] = -1, .max_func = NULL,
		.flags = UPGD_NO_BULK, .qps = 0, .platinum = 5000,
		.cMob[CMOB_MASON] = 1, .cRoom[CROOM_RECALL] = TRUE },

{ .name = "troom", .func = room_normal, .descr = "A room to house the trainer",
		.curr_value = cval_room_normal, .quantity = 1, .max = 1,
		.specific[0] = CROOM_TROOM, .specific[1] = -1, .max_func = NULL,
		.flags = UPGD_NO_BULK, .qps = 0, .platinum = 5000,
		.cMob[CMOB_MASON] = 1, .cRoom[CROOM_RECALL] = TRUE },

{ .name = "stable", .func = room_normal, .descr = "A room to house a pet",
		.curr_value = cval_room_normal, .quantity = 1, .max = 1,
		.specific[0] = CROOM_STABLE, .specific[1] = -1, .max_func = NULL,
		.flags = UPGD_NO_BULK, .qps = 0, .platinum = 4000,
		.cMob[CMOB_TRAINER] = 1, .cMob[CMOB_MASON] = 1,
		.cRoom[CROOM_TROOM] = TRUE, .cRoom[CROOM_RECALL] = TRUE },

{ .name = "brewery", .func = room_normal, .descr = "A brewery for a brewer",
		.curr_value = cval_room_normal, .quantity = 1, .max = 1,
		.specific[0] = CROOM_BREWERY, .specific[1] = -1, .max_func = NULL,
		.flags = UPGD_NO_BULK, .qps = 1000, .platinum = 3000,
		.cMob[CMOB_MASON] = 1, .cRoom[CROOM_RECALL] = TRUE },

{ .name = "bakery", .func = room_normal, .descr = "A bakery for a baker",
		.curr_value = cval_room_normal, .quantity = 1, .max = 1,
		.specific[0] = CROOM_BAKERY, .specific[1] = -1, .max_func = NULL,
		.flags = UPGD_NO_BULK, .qps = 1000, .platinum = 3000,
		.cMob[CMOB_MASON] = 1, .cRoom[CROOM_RECALL] = TRUE },

{ .name = "tannery", .func = room_normal, .descr = "A tannery for a tanner",
		.curr_value = cval_room_normal, .quantity = 1, .max = 1,
		.specific[0] = CROOM_TANNERY, .specific[1] = -1, .max_func = NULL,
		.flags = UPGD_NO_BULK, .qps = 0, .platinum = 5000,
		.cMob[CMOB_MASON] = 1, .cRoom[CROOM_RECALL] = TRUE },

{ .name = "entrance", .func = room_hall_entrance,
		.descr = "Change the entrance/exit of the clan hall",
		.curr_value = cval_room_hall_entrance, .quantity = 1,
		.max = MAX_SPECIAL, .specific[0] = -1, .specific[1] = -1,
		.max_func = mval_room_hall_entrance, .flags = UPGD_NO_BULK, .qps = 0,
		.platinum = 1000, .cRoom[CROOM_RECALL] = TRUE },

{ .name = "", .func = NULL, .descr = "", .curr_value = NULL, .quantity = 0,
		.max = MAX_NONE, .specific[0] = -1, .specific[1] = -1,
		.max_func = NULL, .qps = 0, .platinum = 0, .cMob[CMOB_TRAINER] = 0,
		.cMob[CMOB_SMITH] = 0, .cMob[CMOB_BREWER] = 0, .cMob[CMOB_BAKER] = 0,
		.cMob[CMOB_MAGE] = 0, .cMob[CMOB_MASON] = 0, .cMob[CMOB_GPRIEST] = 0,
		.cMob[CMOB_EPRIEST] = 0, .cMob[CMOB_HEALER] = 0,
		.cMob[CMOB_SHOPKEEP] = 0, .cMob[CMOB_PET] = 0, .cMob[CMOB_TANNER] = 0,
		.cMob[CMOB_QUESTOR] = 0, .cMob[CMOB_GUARD] = 0,
		.cRoom[CROOM_GSHRINE] = FALSE, .cRoom[CROOM_ESHRINE] = FALSE,
		.cRoom[CROOM_SMITHY] = FALSE, .cRoom[CROOM_WORKSHOP] = FALSE,
		.cRoom[CROOM_SHOP] = FALSE, .cRoom[CROOM_TROOM] = FALSE,
		.cRoom[CROOM_STABLE] = FALSE, .cRoom[CROOM_BAKERY] = FALSE,
		.cRoom[CROOM_BREWERY] = FALSE, .cRoom[CROOM_TANNERY] = FALSE,
		.cRoom[CROOM_RECALL] = TRUE } };

COST_DATA
		mob_upgd_prices[] =
		{
				{
						.name = "level",
						.func = mob_upgd_level,
						.descr = "Increase the level of the mob (cannot exceed level of trainer)",
						.curr_value = cval_mob_upgd_level, .quantity = 1,
						.max = MAX_SPECIAL, .specific[0] = -1,
						.specific[1] = -1, .max_func = mval_mob_upgd_level,
						.qps = 0, .platinum = 250, .cMob[CMOB_TRAINER] = 1,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "hp", .func = mob_upgd_hp,
						.descr = "Increase the hitpoints of the mob",
						.curr_value = cval_mob_upgd_hp, .quantity = 1000,
						.max = 50000, .specific[0] = -1, .specific[1] = -1,
						.max_func = NULL, .qps = 0, .platinum = 500,
						.cMob[CMOB_TRAINER] = 1, .cMob[CMOB_HEALER] = 1,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "mana", .func = mob_upgd_mana,
						.descr = "Increase the mana of the mob",
						.curr_value = cval_mob_upgd_mana, .quantity = 1000,
						.max = 50000, .specific[0] = -1, .specific[1] = -1,
						.max_func = NULL, .qps = 0, .platinum = 400,
						.cMob[CMOB_TRAINER] = 1, .cMob[CMOB_MAGE] = 1,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "damage", .func = mob_upgd_damage,
						.descr = "Increase the damage of the mob",
						.curr_value = cval_mob_upgd_damage, .quantity = 1,
						.max = MAX_SPECIAL, .specific[0] = -1,
						.specific[1] = -1, .max_func = mval_mob_upgd_damage,
						.qps = 25, .platinum = 250, .cMob[CMOB_TRAINER] = 1,
						.cMob[CMOB_MAGE] = 1, .cRoom[CROOM_RECALL] = TRUE },

				{ .name = "imm", .func = mob_upgd_imm,
						.descr = "Add an immunity to the mob",
						.curr_value = cval_mob_upgd_imm,
						.flags = UPGD_NO_BULK|UPGD_FLAG|UPGD_XCOST,
						.quantity = 1, .max = MAX_SPECIAL, .specific[0] = -1,
						.specific[1] = -1, .max_func = mval_xcost_count,
						.xcost = imm_prices, .xreq1 = CMOB_TRAINER,
						.xreq2 = CMOB_MAGE, .qps = 0, .platinum = 0,
						.cMob[CMOB_TRAINER] = 1, .cMob[CMOB_MAGE] = 1,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "res", .func = mob_upgd_res,
						.descr = "Add a resistance to the mob",
						.curr_value = cval_mob_upgd_res,
						.flags = UPGD_NO_BULK|UPGD_FLAG|UPGD_XCOST,
						.quantity = 1, .max = MAX_SPECIAL, .specific[0] = -1,
						.specific[1] = -1, .max_func = mval_xcost_count,
						.xcost = res_prices, .xreq1 = CMOB_TRAINER,
						.xreq2 = CMOB_MAGE, .qps = 0, .platinum = 0,
						.cMob[CMOB_TRAINER] = 1, .cMob[CMOB_MAGE] = 1,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "off", .func = mob_upgd_off,
						.descr = "Add an offensive skill to the mob",
						.curr_value = cval_mob_upgd_off,
						.flags = UPGD_NO_BULK|UPGD_FLAG|UPGD_XCOST,
						.quantity = 1, .max = MAX_SPECIAL, .specific[0] = -1,
						.specific[1] = -1, .max_func = mval_xcost_count,
						.xcost = off_prices, .xreq1 = CMOB_TRAINER,
						.xreq2 = CMOB_MAGE, .qps = 0, .platinum = 0,
						.cMob[CMOB_TRAINER] = 1, .cMob[CMOB_MAGE] = 1,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "damtype", .func = mob_upgd_damtype,
						.descr = "Change the damage type of the mob",
						.curr_value = cval_mob_upgd_damtype,
						.flags = UPGD_NO_BULK|UPGD_XCOST, .quantity = 1,
						.max = MAX_SPECIAL, .specific[0] = -1, .specific[1] = -1,
						.max_func = mval_xcost_count, .xcost = damtype_prices,
						.xreq1 = CMOB_TRAINER, .xreq2 = CMOB_MAX, .qps = 0,
						.platinum = 0, .cMob[CMOB_TRAINER] = 1,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "ac", .func = mob_upgd_ac,
						.descr = "Increase the armour of the mob",
						.curr_value = cval_mob_upgd_ac, .quantity = 1,
						.max = 1500, .specific[0] = -1, .specific[1] = -1,
						.max_func = NULL, .qps = 10, .platinum = 250,
						.cMob[CMOB_TRAINER] = 1, .cRoom[CROOM_RECALL] = TRUE },

				{ .name = "effect", .func = mob_upgd_perm,
						.descr = "Add a permanent effect to the mob",
						.curr_value = cval_mob_upgd_perm,
						.flags = UPGD_NO_BULK|UPGD_FLAG|UPGD_XCOST,
						.quantity = 1, .max = MAX_SPECIAL, .specific[0] = -1,
						.specific[1] = -1, .max_func = mval_xcost_count,
						.xcost = bitaf_prices, .xreq1 = CMOB_MAX,
						.xreq2 = CMOB_MAX, .qps = 0, .platinum = 0,
						.cMob[CMOB_TRAINER] = 1, .cMob[CMOB_MAGE] = 1,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "", .func = NULL, .descr = "", .curr_value = NULL,
						.quantity = 0, .max = MAX_NONE, .specific[0] = -1,
						.specific[1] = -1, .max_func = NULL, .qps = 0,
						.platinum = 0, .cMob[CMOB_TRAINER] = 0,
						.cMob[CMOB_SMITH] = 0, .cMob[CMOB_BREWER] = 0,
						.cMob[CMOB_BAKER] = 0, .cMob[CMOB_MAGE] = 0,
						.cMob[CMOB_MASON] = 0, .cMob[CMOB_GPRIEST] = 0,
						.cMob[CMOB_EPRIEST] = 0, .cMob[CMOB_HEALER] = 0,
						.cMob[CMOB_SHOPKEEP] = 0, .cMob[CMOB_PET] = 0,
						.cMob[CMOB_TANNER] = 0, .cMob[CMOB_QUESTOR] = 0,
						.cMob[CMOB_GUARD] = 0, .cRoom[CROOM_GSHRINE] = FALSE,
						.cRoom[CROOM_ESHRINE] = FALSE,
						.cRoom[CROOM_SMITHY] = FALSE,
						.cRoom[CROOM_WORKSHOP] = FALSE,
						.cRoom[CROOM_SHOP] = FALSE,
						.cRoom[CROOM_TROOM] = FALSE,
						.cRoom[CROOM_STABLE] = FALSE,
						.cRoom[CROOM_BAKERY] = FALSE,
						.cRoom[CROOM_BREWERY] = FALSE,
						.cRoom[CROOM_TANNERY] = FALSE,
						.cRoom[CROOM_RECALL] = TRUE } };

COST_DATA
		obj_upgd_prices[] =
		{
				{ .name = "hp_regen", .func = obj_upgd_v3,
						.descr = "Increase the hitpoint regeneration rate",
						.curr_value = cval_obj_upgd_v3, .quantity = 50,
						.max = 1000, .specific[0] = ITEM_FURNITURE,
						.specific[1] = -1, .max_func = NULL, .qps = 100,
						.platinum = 3000, .cMob[CMOB_HEALER] = 1,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "mana_regen", .func = obj_upgd_v4,
						.descr = "Increase the mana regeneration rate",
						.curr_value = cval_obj_upgd_v4, .quantity = 50,
						.max = 1000, .specific[0] = ITEM_FURNITURE,
						.specific[1] = -1, .max_func = NULL, .qps = 150,
						.platinum = 2000, .cMob[CMOB_MAGE] = 1,
						.cRoom[CROOM_RECALL] = TRUE },

				{
						.name = "capacity",
						.func = obj_upgd_furn_capacity,
						.descr = "Increase the number of people who can use the item at one time",
						.curr_value = cval_obj_upgd_v0, .quantity = 2,
						.max = 20, .specific[0] = ITEM_FURNITURE,
						.specific[1] = -1, .max_func = NULL, .qps = 100,
						.platinum = 1000, .cMob[CMOB_SMITH] = 1,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "duration", .func = obj_upgd_v2,
						.descr = "Increase the duration of the light",
						.curr_value = cval_obj_upgd_v2, .quantity = 1,
						.max = 999, .specific[0] = ITEM_LIGHT,
						.specific[1] = -1, .max_func = NULL, .qps = 50,
						.platinum = 200, .cMob[CMOB_MAGE] = 1,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "add_spell", .func = obj_upgd_add1_spell,
						.descr = "Add a spell to the wand",
						.curr_value = cval_obj_upgd_add1_spell,
						.xcost = spell_prices, .xreq1 = CMOB_MAGE,
						.xreq2 = CMOB_MAX, .flags = UPGD_NO_BULK|UPGD_XCOST,
						.quantity = 1, .max = MAX_NONE,
						.specific[0] = ITEM_WAND, .specific[1] = -1,
						.max_func = NULL, .qps = 0, .platinum = 0,
						.cMob[CMOB_MAGE] = 1, .cRoom[CROOM_RECALL] = TRUE },

				{ .name = "add_spell", .func = obj_upgd_add1_spell,
						.descr = "Add a spell to the staff",
						.curr_value = cval_obj_upgd_add1_spell,
						.xcost = spell_prices, .xreq1 = CMOB_MAGE,
						.xreq2 = CMOB_MAX, .flags = UPGD_NO_BULK|UPGD_XCOST,
						.quantity = 1, .max = MAX_NONE,
						.specific[0] = ITEM_STAFF, .specific[1] = -1,
						.max_func = NULL, .qps = 0, .platinum = 0,
						.cMob[CMOB_MAGE] = 1, .cRoom[CROOM_RECALL] = TRUE },

				{ .name = "add_spell", .func = obj_upgd_add3_spell,
						.descr = "Add a spell to the scroll",
						.curr_value = cval_obj_upgd_add3_spell,
						.xcost = spell_prices, .xreq1 = CMOB_MAGE,
						.xreq2 = CMOB_MAX, .flags = UPGD_NO_BULK|UPGD_XCOST,
						.quantity = 1, .max = MAX_NONE,
						.specific[0] = ITEM_SCROLL, .specific[1] = -1,
						.max_func = NULL, .qps = 0, .platinum = 0,
						.cMob[CMOB_MAGE] = 1, .cRoom[CROOM_RECALL] = TRUE },

				{ .name = "spell_level", .func = obj_upgd_v0,
						.descr = "Upgrade the spell level on the wand",
						.curr_value = cval_obj_upgd_v0, .quantity = 1,
						.max = 101, .specific[0] = ITEM_WAND,
						.specific[1] = -1, .max_func = NULL, .qps = 250,
						.platinum = 250, .cMob[CMOB_MAGE] = 1,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "spell_level", .func = obj_upgd_v0,
						.descr = "Upgrade the spell level on the staff",
						.curr_value = cval_obj_upgd_v0, .quantity = 1,
						.max = 101, .specific[0] = ITEM_STAFF,
						.specific[1] = -1, .max_func = NULL, .qps = 200,
						.platinum = 300, .cMob[CMOB_MAGE] = 1,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "spell_level", .func = obj_upgd_v0,
						.descr = "Upgrade the spell level on the scroll",
						.curr_value = cval_obj_upgd_v0, .quantity = 1,
						.max = 101, .specific[0] = ITEM_SCROLL,
						.specific[1] = -1, .max_func = NULL, .qps = 250,
						.platinum = 250, .cMob[CMOB_MAGE] = 1,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "charges", .func = obj_upgd_charges,
						.descr = "Upgrade the number of charges on the wand",
						.curr_value = cval_obj_upgd_v2, .quantity = 1,
						.max = 50, .specific[0] = ITEM_WAND, .specific[1] = -1,
						.max_func = NULL, .qps = 250, .platinum = 0,
						.cMob[CMOB_MAGE] = 1, .cRoom[CROOM_RECALL] = TRUE },

				{ .name = "charges", .func = obj_upgd_charges,
						.descr = "Upgrade the number of charges on the staff",
						.curr_value = cval_obj_upgd_v2, .quantity = 1,
						.max = 50, .specific[0] = ITEM_STAFF,
						.specific[1] = -1, .max_func = NULL, .qps = 200,
						.platinum = 0, .cMob[CMOB_MAGE] = 1,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "damage", .func = obj_upgd_damage,
						.descr = "Boost the damdice on the weapon",
						.curr_value = cval_obj_upgd_v2, .quantity = 1,
						.max = 14, .specific[0] = ITEM_WEAPON,
						.specific[1] = -1, .max_func = NULL, .qps = 0,
						.platinum = 750, .cMob[CMOB_SMITH] = 1,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "damtype", .func = obj_upgd_damtype,
						.descr = "Change the damage type of the weapon",
						.curr_value = cval_obj_upgd_damtype,
						.xcost = damtype_prices, .xreq1 = CMOB_TRAINER,
						.xreq2 = CMOB_MAGE,
						.flags = UPGD_NO_BULK|UPGD_XCOST|UPGD_FLAG,
						.quantity = 1, .max = MAX_NONE,
						.specific[0] = ITEM_WEAPON, .specific[1] = -1,
						.max_func = NULL, .qps = 0, .platinum = 0,
						.cMob[CMOB_SMITH] = 1, .cMob[CMOB_MAGE] = 1,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "wpn_flag", .func = obj_upgd_wpnflag,
						.descr = "Add a weapon flag",
						.curr_value = cval_obj_upgd_wpnflag,
						.xcost = wpnflag_prices, .xreq1 = CMOB_SMITH,
						.xreq2 = CMOB_MAGE,
						.flags = UPGD_NO_BULK|UPGD_XCOST|UPGD_FLAG,
						.quantity = 1, .max = MAX_NONE,
						.specific[0] = ITEM_WEAPON, .specific[1] = -1,
						.max_func = NULL, .qps = 0, .platinum = 0,
						.cMob[CMOB_SMITH] = 1, .cMob[CMOB_MAGE] = 1,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "add_spell", .func = obj_upgd_add3_spell,
						.descr = "Add a spell to the potion",
						.curr_value = cval_obj_upgd_add3_spell,
						.xcost = spell_prices, .xreq1 = CMOB_MAGE,
						.xreq2 = CMOB_MAX, .flags = UPGD_NO_BULK|UPGD_XCOST,
						.quantity = 1, .max = MAX_NONE,
						.specific[0] = ITEM_POTION, .specific[1] = -1,
						.max_func = NULL, .qps = 0, .platinum = 0,
						.cMob[CMOB_MAGE] = 1, .cRoom[CROOM_RECALL] = TRUE },

				{ .name = "add_spell", .func = obj_upgd_add3_spell,
						.descr = "Add a spell to the pill",
						.curr_value = cval_obj_upgd_add3_spell,
						.xcost = spell_prices, .xreq1 = CMOB_MAGE,
						.xreq2 = CMOB_MAX, .flags = UPGD_NO_BULK|UPGD_XCOST,
						.quantity = 1, .max = MAX_NONE,
						.specific[0] = ITEM_PILL, .specific[1] = -1,
						.max_func = NULL, .qps = 0, .platinum = 0,
						.cMob[CMOB_MAGE] = 1, .cRoom[CROOM_RECALL] = TRUE },

				{ .name = "spell_level", .func = obj_upgd_v0,
						.descr = "Increase the spell levels of the potion",
						.curr_value = cval_obj_upgd_v0, .quantity = 1,
						.max = 115, .specific[0] = ITEM_POTION,
						.specific[1] = -1, .max_func = NULL, .qps = 250,
						.platinum = 0, .cMob[CMOB_BREWER] = 1,
						.cMob[CMOB_MAGE] = 1, .cRoom[CROOM_RECALL] = TRUE },

				{ .name = "spell_level", .func = obj_upgd_v0,
						.descr = "Increase the spell levels of the pill",
						.curr_value = cval_obj_upgd_v0, .quantity = 1,
						.max = 101, .specific[0] = ITEM_PILL,
						.specific[1] = -1, .max_func = NULL, .qps = 300,
						.platinum = 0, .cMob[CMOB_BAKER] = 1,
						.cMob[CMOB_MAGE] = 1, .cRoom[CROOM_RECALL] = TRUE },

				{ .name = "capacity", .func = obj_upgd_cont_capacity,
						.descr = "Increase the capacity of the container",
						.curr_value = cval_obj_upgd_v0, .quantity = 10,
						.max = 500, .specific[0] = ITEM_CONTAINER,
						.specific[1] = -1, .max_func = NULL, .qps = 0,
						.platinum = 250, .cMob[CMOB_SMITH] = 1,
						.cRoom[CROOM_RECALL] = TRUE },

				{
						.name = "capacity",
						.func = obj_upgd_v0,
						.descr = "Increase the capacity of the drink container",
						.curr_value = cval_obj_upgd_v0, .quantity = 5,
						.max = MAX_NONE, .specific[0] = ITEM_DRINK_CON,
						.specific[1] = -1, .max_func = NULL, .qps = 0,
						.platinum = 250, .cMob[CMOB_SMITH] = 1,
						.cMob[CMOB_BREWER] = 1, .cRoom[CROOM_RECALL] = TRUE },

				{
						.name = "duration",
						.func = obj_upgd_v0,
						.descr = "Increase the duration of the \"full\" effect",
						.curr_value = cval_obj_upgd_v0, .quantity = 5,
						.max = 50, .specific[0] = ITEM_FOOD, .specific[1] = -1,
						.max_func = NULL, .qps = 0, .platinum = 300,
						.cMob[CMOB_SMITH] = 1, .cMob[CMOB_BAKER] = 1,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "hitroll", .func = obj_upgd_affected,
						.descr = "Increase the hitroll on the item",
						.curr_value = cval_obj_upgd_affected, .quantity = 1,
						.max = 35, .specific[0] = -1,
						.specific[1] = APPLY_HITROLL, .max_func = NULL,
						.qps = 50, .platinum = 750, .cMob[CMOB_SMITH] = 1,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "damroll", .func = obj_upgd_affected,
						.descr = "Increase the damroll on the item",
						.curr_value = cval_obj_upgd_affected, .quantity = 1,
						.max = 35, .specific[0] = -1,
						.specific[1] = APPLY_DAMROLL, .max_func = NULL,
						.qps = 50, .platinum = 750, .cMob[CMOB_SMITH] = 1,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "hp", .func = obj_upgd_affected,
						.descr = "Increase the hitpoints on the item",
						.curr_value = cval_obj_upgd_affected, .quantity = 1,
						.max = 250, .specific[0] = -1,
						.specific[1] = APPLY_HIT, .max_func = NULL, .qps = 5,
						.platinum = 250, .cMob[CMOB_SMITH] = 1,
						.cMob[CMOB_HEALER] = 1, .cRoom[CROOM_RECALL] = TRUE },

				{ .name = "mana", .func = obj_upgd_affected,
						.descr = "Increase the mana on the item",
						.curr_value = cval_obj_upgd_affected, .quantity = 1,
						.max = 250, .specific[0] = -1,
						.specific[1] = APPLY_MANA, .max_func = NULL, .qps = 10,
						.platinum = 200, .cMob[CMOB_SMITH] = 1,
						.cMob[CMOB_MAGE] = 1, .cRoom[CROOM_RECALL] = TRUE },

				{ .name = "move", .func = obj_upgd_affected,
						.descr = "Increase the moves on the item",
						.curr_value = cval_obj_upgd_affected, .quantity = 1,
						.max = 250, .specific[0] = -1,
						.specific[1] = APPLY_MOVE, .max_func = NULL, .qps = 5,
						.platinum = 200, .cMob[CMOB_SMITH] = 1,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "age", .func = obj_upgd_affected,
						.descr = "Increase the age on the item",
						.curr_value = cval_obj_upgd_affected, .quantity = 1,
						.max = 20, .specific[0] = -1, .specific[1] = APPLY_AGE,
						.max_func = NULL, .qps = 500, .platinum = 1000,
						.cMob[CMOB_SMITH] = 1, .cMob[CMOB_MAGE] = 1,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "hp_regen", .func = obj_upgd_affected,
						.descr = "Increase the hitpoint regeneration",
						.curr_value = cval_obj_upgd_affected, .quantity = 1,
						.max = 5, .specific[0] = -1,
						.specific[1] = APPLY_REGEN, .max_func = NULL,
						.qps = 750, .platinum = 2000, .cMob[CMOB_SMITH] = 1,
						.cMob[CMOB_HEALER] = 1, .cRoom[CROOM_RECALL] = TRUE },

				{ .name = "mana_regen", .func = obj_upgd_affected,
						.descr = "Increase the mana regeneration",
						.curr_value = cval_obj_upgd_affected, .quantity = 1,
						.max = 5, .specific[0] = -1,
						.specific[1] = APPLY_MANA_REGEN, .max_func = NULL,
						.qps = 750, .platinum = 2000, .cMob[CMOB_SMITH] = 1,
						.cMob[CMOB_MAGE] = 1, .cRoom[CROOM_RECALL] = TRUE },

				{ .name = "saves", .func = obj_upgd_affected,
						.descr = "Increase the saves on the item",
						.curr_value = cval_obj_upgd_affected, .quantity = 1,
						.max = 8, .specific[0] = -1,
						.specific[1] = APPLY_SAVES, .max_func = NULL,
						.qps = 1000, .platinum = 0, .cMob[CMOB_SMITH] = 1,
						.cMob[CMOB_MAGE] = 1, .cMob[CMOB_GPRIEST] = 1,
						.cMob[CMOB_EPRIEST] = 1, .cRoom[CROOM_RECALL] = TRUE },

				{ .name = "ac", .func = obj_upgd_affected,
						.descr = "Increase the ac on the item",
						.curr_value = cval_obj_upgd_affected, .quantity = 1,
						.max = 30, .specific[0] = -1, .specific[1] = APPLY_AC,
						.max_func = NULL, .qps = 100, .platinum = 1000,
						.cMob[CMOB_SMITH] = 1, .cRoom[CROOM_RECALL] = TRUE },

				{ .name = "str", .func = obj_upgd_affected,
						.descr = "Increase the strength on the item",
						.curr_value = cval_obj_upgd_affected, .quantity = 1,
						.max = 5, .specific[0] = -1, .specific[1] = APPLY_STR,
						.max_func = NULL, .qps = 250, .platinum = 1000,
						.cMob[CMOB_SMITH] = 1, .cMob[CMOB_MAGE] = 1,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "int", .func = obj_upgd_affected,
						.descr = "Increase the intelligence on the item",
						.curr_value = cval_obj_upgd_affected, .quantity = 1,
						.max = 5, .specific[0] = -1, .specific[1] = APPLY_INT,
						.max_func = NULL, .qps = 250, .platinum = 1000,
						.cMob[CMOB_SMITH] = 1, .cMob[CMOB_MAGE] = 1,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "wis", .func = obj_upgd_affected,
						.descr = "Increase the wisdom on the item",
						.curr_value = cval_obj_upgd_affected, .quantity = 1,
						.max = 5, .specific[0] = -1, .specific[1] = APPLY_WIS,
						.max_func = NULL, .qps = 250, .platinum = 1000,
						.cMob[CMOB_SMITH] = 1, .cMob[CMOB_MAGE] = 1,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "dex", .func = obj_upgd_affected,
						.descr = "Increase the dexterity on the item",
						.curr_value = cval_obj_upgd_affected, .quantity = 1,
						.max = 5, .specific[0] = -1, .specific[1] = APPLY_DEX,
						.max_func = NULL, .qps = 250, .platinum = 1000,
						.cMob[CMOB_SMITH] = 1, .cMob[CMOB_MAGE] = 1,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "con", .func = obj_upgd_affected,
						.descr = "Increase the constitution on the item",
						.curr_value = cval_obj_upgd_affected, .quantity = 1,
						.max = 5, .specific[0] = -1, .specific[1] = APPLY_CON,
						.max_func = NULL, .qps = 250, .platinum = 1000,
						.cMob[CMOB_SMITH] = 1, .cMob[CMOB_MAGE] = 1,
						.cRoom[CROOM_RECALL] = TRUE },

				{ .name = "flag", .func = obj_upgd_objflag,
						.descr = "Add an object flag to the item",
						.curr_value = cval_obj_upgd_objflag,
						.xcost = objflag_prices, .xreq1 = CMOB_SMITH,
						.xreq2 = CMOB_MAGE,
						.flags = UPGD_NO_BULK|UPGD_XCOST|UPGD_FLAG,
						.quantity = 1, .max = MAX_NONE, .specific[0] = -1,
						.specific[1] = -1, .max_func = NULL, .qps = 0,
						.platinum = 0, .cMob[CMOB_SMITH] = 1,
						.cMob[CMOB_MAGE] = 1, .cRoom[CROOM_RECALL] = TRUE },

				{ .name = "wear_loc", .func = obj_upgd_wearloc,
						.descr = "Change the wear location for the object",
						.curr_value = cval_obj_upgd_wearloc,
						.flags = UPGD_NO_BULK|UPGD_FLAG, .quantity = 0,
						.max = MAX_NONE, .specific[0] = -1, .specific[1] = -1,
						.max_func = NULL, .qps = 0, .platinum = 0,
						.cRoom[CROOM_RECALL] = TRUE },

/*				    { .name = "effect",				.func = obj_upgd_affected,
				 .descr = "Add a permanent effect to the item",
				 .curr_value = cval_obj_upgd_affected,		.xcost = bitaf_prices,
				 .xreq1 = CMOB_MAX,			.xreq2 = CMOB_MAX,
				 .flags = UPGD_NO_BULK|UPGD_FLAG|UPGD_XCOST,
				 .quantity = 0,				.max = MAX_NONE,
				 .specific[0] = -1,			.specific[1] = -1,
				 .max_func = NULL,
				 .qps = 0,					.platinum = 0,
				 .cMob[CMOB_MAGE] = 1,
				 .cRoom[CROOM_RECALL] = TRUE  },
*/
				{ .name = "", .func = NULL, .descr = "", .curr_value = NULL,
						.quantity = 0, .max = MAX_NONE, .specific[0] = -1,
						.specific[1] = -1, .max_func = NULL, .qps = 0,
						.platinum = 0, .cMob[CMOB_TRAINER] = 0,
						.cMob[CMOB_SMITH] = 0, .cMob[CMOB_BREWER] = 0,
						.cMob[CMOB_BAKER] = 0, .cMob[CMOB_MAGE] = 0,
						.cMob[CMOB_MASON] = 0, .cMob[CMOB_GPRIEST] = 0,
						.cMob[CMOB_EPRIEST] = 0, .cMob[CMOB_HEALER] = 0,
						.cMob[CMOB_SHOPKEEP] = 0, .cMob[CMOB_PET] = 0,
						.cMob[CMOB_TANNER] = 0, .cMob[CMOB_QUESTOR] = 0,
						.cMob[CMOB_GUARD] = 0, .cRoom[CROOM_GSHRINE] = FALSE,
						.cRoom[CROOM_ESHRINE] = FALSE,
						.cRoom[CROOM_SMITHY] = FALSE,
						.cRoom[CROOM_WORKSHOP] = FALSE,
						.cRoom[CROOM_SHOP] = FALSE,
						.cRoom[CROOM_TROOM] = FALSE,
						.cRoom[CROOM_STABLE] = FALSE,
						.cRoom[CROOM_BAKERY] = FALSE,
						.cRoom[CROOM_BREWERY] = FALSE,
						.cRoom[CROOM_TANNERY] = FALSE,
						.cRoom[CROOM_RECALL] = TRUE } };

COST_DATA room_upgd_prices[] =
{
		{ .name = "hp_regen", .func = room_upgd_hpregen,
				.descr = "Upgrade the hitpoint regeneration rate",
				.curr_value = cval_room_upgd_hpregen, .quantity = 50,
				.max = 600, .specific[0] = -1, .specific[1] = -1,
				.max_func = NULL, .qps = 250, .platinum = 7500,
				.cMob[CMOB_HEALER] = 40, .cRoom[CROOM_RECALL] = TRUE },

		{ .name = "mana_regen", .func = room_upgd_manaregen,
				.descr = "Upgrade the mana regeneration rate",
				.curr_value = cval_room_upgd_manaregen, .quantity = 50,
				.max = 600, .specific[0] = -1, .specific[1] = -1,
				.max_func = NULL, .qps = 250, .platinum = 7500,
				.cMob[CMOB_MAGE] = 40, .cRoom[CROOM_RECALL] = TRUE },

		{ .name = "flag_dark", .func = room_upgd_flag,
				.descr = "Flag/unflag the room as dark",
				.curr_value = cval_room_upgd_flag, .quantity = 1,
				.max = MAX_NONE, .specific[0] = ROOM_DARK, .specific[1] = -1,
				.max_func = NULL, .flags = UPGD_NO_BULK|UPGD_FLAG, .qps = 0,
				.platinum = 1000, .cRoom[CROOM_RECALL] = TRUE },

		{ .name = "flag_nomob", .func = room_upgd_flag,
				.descr = "Flag/unflag the room as no mob",
				.curr_value = cval_room_upgd_flag, .quantity = 1,
				.max = MAX_NONE, .specific[0] = ROOM_NO_MOB, .specific[1] = -1,
				.max_func = NULL, .flags = UPGD_NO_BULK|UPGD_FLAG, .qps = 0,
				.platinum = 4000, .cRoom[CROOM_RECALL] = TRUE },

		{ .name = "flag_indoors", .func = room_upgd_flag,
				.descr = "Flag/unflag the room as indoors",
				.curr_value = cval_room_upgd_flag, .quantity = 1,
				.max = MAX_NONE, .specific[0] = ROOM_INDOORS,
				.specific[1] = -1, .max_func = NULL,
				.flags = UPGD_NO_BULK|UPGD_FLAG, .qps = 0, .platinum = 1000,
				.cRoom[CROOM_RECALL] = TRUE },

		{ .name = "flag_nomagic", .func = room_upgd_flag,
				.descr = "Flag/unflag the room as no magic",
				.curr_value = cval_room_upgd_flag, .quantity = 1,
				.max = MAX_NONE, .specific[0] = ROOM_NO_MAGIC,
				.specific[1] = -1, .max_func = NULL,
				.flags = UPGD_NO_BULK|UPGD_FLAG, .qps = 500, .platinum = 2000,
				.cMob[CMOB_MAGE] = 50, .cRoom[CROOM_RECALL] = TRUE },

		{ .name = "flag_norecall", .func = room_upgd_flag,
				.descr = "Flag/unflag the room as no recall",
				.curr_value = cval_room_upgd_flag, .quantity = 1,
				.max = MAX_NONE, .specific[0] = ROOM_NO_RECALL,
				.specific[1] = -1, .max_func = NULL,
				.flags = UPGD_NO_BULK|UPGD_FLAG, .qps = 250, .platinum = 2000,
				.cMob[CMOB_MAGE] = 40, .cMob[CMOB_GPRIEST] = 40,
				.cMob[CMOB_EPRIEST] = 40, .cRoom[CROOM_RECALL] = TRUE },

		{ .name = "flag_heroesonly", .func = room_upgd_flag,
				.descr = "Flag/unflag the room as heroes only",
				.curr_value = cval_room_upgd_flag, .quantity = 1,
				.max = MAX_NONE, .specific[0] = ROOM_HEROES_ONLY,
				.specific[1] = -1, .max_func = NULL,
				.flags = UPGD_NO_BULK|UPGD_FLAG, .qps = 0, .platinum = 1000,
				.cMob[CMOB_MASON] = 60, .cRoom[CROOM_RECALL] = TRUE },

		{ .name = "flag_law", .func = room_upgd_flag,
				.descr = "Flag/unflag the room as law",
				.curr_value = cval_room_upgd_flag, .quantity = 1,
				.max = MAX_NONE, .specific[0] = ROOM_LAW, .specific[1] = -1,
				.max_func = NULL, .flags = UPGD_NO_BULK|UPGD_FLAG, .qps = 0,
				.platinum = 1000, .cMob[CMOB_MAGE] = 50,
				.cMob[CMOB_GPRIEST] = 50, .cMob[CMOB_EPRIEST] = 50,
				.cRoom[CROOM_RECALL] = TRUE },

		{ .name = "flag_nowhere", .func = room_upgd_flag,
				.descr = "Flag/unflag the room as nowhere",
				.curr_value = cval_room_upgd_flag, .quantity = 1,
				.max = MAX_NONE, .specific[0] = ROOM_NOWHERE,
				.specific[1] = -1, .max_func = NULL,
				.flags = UPGD_NO_BULK|UPGD_FLAG, .qps = 250, .platinum = 2000,
				.cMob[CMOB_MAGE] = 75, .cMob[CMOB_GPRIEST] = 75,
				.cMob[CMOB_EPRIEST] = 75, .cRoom[CROOM_RECALL] = TRUE },

		{ .name = "", .func = NULL, .descr = "", .curr_value = NULL,
				.quantity = 0, .max = MAX_NONE, .specific[0] = -1,
				.specific[1] = -1, .max_func = NULL, .qps = 0, .platinum = 0,
				.cMob[CMOB_TRAINER] = 0, .cMob[CMOB_SMITH] = 0,
				.cMob[CMOB_BREWER] = 0, .cMob[CMOB_BAKER] = 0,
				.cMob[CMOB_MAGE] = 0, .cMob[CMOB_MASON] = 0,
				.cMob[CMOB_GPRIEST] = 0, .cMob[CMOB_EPRIEST] = 0,
				.cMob[CMOB_HEALER] = 0, .cMob[CMOB_SHOPKEEP] = 0,
				.cMob[CMOB_PET] = 0, .cMob[CMOB_TANNER] = 0,
				.cMob[CMOB_QUESTOR] = 0, .cMob[CMOB_GUARD] = 0,
				.cRoom[CROOM_GSHRINE] = FALSE, .cRoom[CROOM_ESHRINE] = FALSE,
				.cRoom[CROOM_SMITHY] = FALSE, .cRoom[CROOM_WORKSHOP] = FALSE,
				.cRoom[CROOM_SHOP] = FALSE, .cRoom[CROOM_TROOM] = FALSE,
				.cRoom[CROOM_STABLE] = FALSE, .cRoom[CROOM_BAKERY] = FALSE,
				.cRoom[CROOM_BREWERY] = FALSE, .cRoom[CROOM_TANNERY] = FALSE,
				.cRoom[CROOM_RECALL] = TRUE } };

/* Spells for scrolls/wands/scrolls: req1=mage */
XCOST_DATA spell_prices[] =
{
/*  { NAME,			BIT,	FLAGS,	AQP,	PLAT,	TYPE,			REQ1,	REQ2	} */
{ "armor", 0, 0, 0, 5000, SPELL_DEFENSIVE, 0, 0, },
{ "bless", 0, 0, 0, 5000, SPELL_DEFENSIVE, 0, 0, },
{ "cancellation", 0, 0, 1000, 5000, SPELL_DEFENSIVE, 40, 0, },
{ "cure_blind", 0, 0, 1500, 5000, SPELL_DEFENSIVE, 0, 0, },
{ "cure_critical", 0, 0, 1000, 5000, SPELL_DEFENSIVE, 40, 0, },
{ "cure_disease", 0, 0, 1000, 5000, SPELL_DEFENSIVE, 0, 0, },
{ "cure_light", 0, 0, 0, 5000, SPELL_DEFENSIVE, 0, 0, },
{ "cure_poison", 0, 0, 1000, 5000, SPELL_DEFENSIVE, 0, 0, },
{ "detect_evil", 0, 0, 500, 5000, SPELL_DEFENSIVE, 0, 0, },
{ "detect_good", 0, 0, 500, 5000, SPELL_DEFENSIVE, 0, 0, },
{ "detect_motion", 0, 0, 500, 5000, SPELL_DEFENSIVE, 60, 0, },
{ "detect_invis", 0, 0, 500, 5000, SPELL_DEFENSIVE, 60, 0, },
{ "detect_location", 0, 0, 1000, 5000, SPELL_DEFENSIVE, 100, 0, },
{ "detect_magic", 0, 0, 500, 5000, SPELL_DEFENSIVE, 0, 0, },
{ "detect_poison", 0, 0, 0, 5000, SPELL_DEFENSIVE, 0, 0, },
{ "displace", 0, 0, 1000, 5000, SPELL_DEFENSIVE, 0, 0, },
{ "farsight", 0, 0, 1000, 5000, SPELL_DEFENSIVE, 0, 0, },
{ "fireshield", 0, 0, 1500, 5000, SPELL_DEFENSIVE, 50, 0, },
{ "fly", 0, 0, 500, 5000, SPELL_DEFENSIVE, 0, 0, },
{ "frenzy", 0, 0, 1000, 5000, SPELL_DEFENSIVE, 0, 0, },
{ "giant_strength", 0, 0, 0, 5000, SPELL_DEFENSIVE, 45, 0, },
{ "haste", 0, 0, 1500, 5000, SPELL_DEFENSIVE, 45, 0, },
{ "heal", 0, 0, 2000, 5000, SPELL_DEFENSIVE, 30, 0, },
{ "iceshield", 0, 0, 1500, 5000, SPELL_DEFENSIVE, 50, 0, },
{ "infravision", 0, 0, 500, 5000, SPELL_DEFENSIVE, 0, 0, },
{ "invisibility", 0, 0, 1000, 5000, SPELL_DEFENSIVE, 0, 0, },
{ "mass_healing", 0, 0, 2000, 5000, SPELL_DEFENSIVE, 40, 0, },
{ "mass_invis", 0, 0, 2000, 5000, SPELL_DEFENSIVE, 0, 0, },
{ "pass_door", 0, 0, 1000, 5000, SPELL_DEFENSIVE, 0, 0, },
{ "protection_evil", 0, 0, 1000, 5000, SPELL_DEFENSIVE, 50, 0, },
{ "protection_good", 0, 0, 1000, 5000, SPELL_DEFENSIVE, 50, 0, },
{ "refresh", 0, 0, 0, 5000, SPELL_DEFENSIVE, 0, 0, },
{ "remove_curse", 0, 0, 1500, 5000, SPELL_DEFENSIVE, 0, 0, },
{ "restore_mana", 0, 0, 1000, 5000, SPELL_DEFENSIVE, 75, 0, },
{ "sanctuary", 0, 0, 2000, 5000, SPELL_DEFENSIVE, 75, 0, },
{ "shield", 0, 0, 0, 5000, SPELL_DEFENSIVE, 40, 0, },
{ "shockshield", 0, 0, 1500, 5000, SPELL_DEFENSIVE, 50, 0, },
{ "stone_skin", 0, 0, 1000, 5000, SPELL_DEFENSIVE, 75, 0, },
{ "acid_rain", 0, 0, 2000, 5000, SPELL_OFFENSIVE, 65, 0, },
{ "acid_blast", 0, 0, 1500, 5000, SPELL_OFFENSIVE, 95, 0, },
{ "blindness", 0, 0, 1500, 5000, SPELL_OFFENSIVE, 0, 0, },
{ "burning_hands", 0, 0, 0, 5000, SPELL_OFFENSIVE, 0, 0, },
{ "call_lightning", 0, 0, 500, 5000, SPELL_OFFENSIVE, 0, 0, },
{ "calm", 0, 0, 0, 5000, SPELL_OFFENSIVE, 0, 0, },
//{ "cause_critical", 0, 0, 1000, 5000, SPELL_OFFENSIVE, 0, 0, },
//{ "cause_light", 0, 0, 0, 5000, SPELL_OFFENSIVE, 0, 0, },
{ "cause_serious", 0, 0, 500, 5000, SPELL_OFFENSIVE, 0, 0, },
{ "chain_lightning", 0, 0, 2000, 5000, SPELL_OFFENSIVE, 0, 0, },
{ "change_sex", 0, 0, 0, 5000, SPELL_OFFENSIVE, 0, 0, },
{ "chill_touch", 0, 0, 0, 5000, SPELL_OFFENSIVE, 0, 0, },
{ "colour_spray", 0, 0, 500, 5000, SPELL_OFFENSIVE, 0, 0, },
{ "curse", 0, 0, 1000, 5000, SPELL_OFFENSIVE, 0, 0, },
{ "demonfire", 0, 0, 1000, 5000, SPELL_OFFENSIVE, 55, 0, },
{ "dispel_evil", 0, 0, 1500, 5000, SPELL_OFFENSIVE, 40, 0, },
{ "dispel_good", 0, 0, 1500, 5000, SPELL_OFFENSIVE, 40, 0, },
{ "dispel_magic", 0, 0, 2000, 5000, SPELL_OFFENSIVE, 45, 0, },
{ "earthquake", 0, 0, 1000, 5000, SPELL_OFFENSIVE, 0, 0, },
{ "energy_drain", 0, 0, 2000, 5000, SPELL_OFFENSIVE, 0, 0, },
{ "faerie_fire", 0, 0, 1500, 5000, SPELL_OFFENSIVE, 0, 0, },
{ "feeble_mind", 0, 0, 2000, 5000, SPELL_OFFENSIVE, 50, 0, },
{ "fireball", 0, 0, 500, 5000, SPELL_OFFENSIVE, 0, 0, },
{ "flamestrike", 0, 0, 500, 5000, SPELL_OFFENSIVE, 0, 0, },
{ "harm", 0, 0, 0, 5000, SPELL_OFFENSIVE, 0, 0, },
{ "heat_metal", 0, 0, 2000, 5000, SPELL_OFFENSIVE, 60, 0, },
{ "holy_word", 0, 0, 1500, 5000, SPELL_OFFENSIVE, 55, 0, },
{ "lightning_bolt", 0, 0, 500, 5000, SPELL_OFFENSIVE, 0, 0, },
{ "magic_missile", 0, 0, 0, 5000, SPELL_OFFENSIVE, 0, 0, },
{ "plague", 0, 0, 1000, 5000, SPELL_OFFENSIVE, 0, 0, },
{ "poison", 0, 0, 1000, 5000, SPELL_OFFENSIVE, 0, 0, },
{ "prismatic_spray", 0, 0, 500, 5000, SPELL_OFFENSIVE, 60, 0, },
{ "ray_of_truth", 0, 0, 1500, 5000, SPELL_OFFENSIVE, 55, 0, },
{ "shocking_grasp", 0, 0, 500, 5000, SPELL_OFFENSIVE, 0, 0, },
{ "sleep", 0, 0, 2000, 5000, SPELL_OFFENSIVE, 0, 0, },
{ "slow", 0, 0, 1500, 5000, SPELL_OFFENSIVE, 60, 0, },
{ "teleport", 0, 0, 1000, 5000, SPELL_OFFENSIVE, 60, 0, },
{ "weaken", 0, 0, 1000, 5000, SPELL_OFFENSIVE, 60, 0, },
{ "acid_breath", 0, 0, 1500, 5000, SPELL_OFFENSIVE, 75, 0, },
{ "fire_breath", 0, 0, 1500, 5000, SPELL_OFFENSIVE, 75, 0, },
{ "frost_breath", 0, 0, 1500, 5000, SPELL_OFFENSIVE, 75, 0, },
{ "gas_breath", 0, 0, 1500, 5000, SPELL_OFFENSIVE, 75, 0, },
{ "lightning_breath", 0, 0, 1500, 5000, SPELL_OFFENSIVE, 75, 0, },
{ "animate", 0, 0, 1000, 5000, SPELL_MISC, 60, 0, },
{ "conjure", 0, 0, 2000, 5000, SPELL_MISC, 60, 0, },
{ "continual_light", 0, 0, 0, 5000, SPELL_MISC, 0, 0, },
{ "control_weather", 0, 0, 0, 5000, SPELL_MISC, 0, 0, },
{ "create_food", 0, 0, 0, 5000, SPELL_MISC, 0, 0, },
{ "create_rose", 0, 0, 0, 5000, SPELL_MISC, 0, 0, },
{ "create_spring", 0, 0, 0, 5000, SPELL_MISC, 0, 0, },
{ "create_water", 0, 0, 0, 5000, SPELL_MISC, 0, 0, },
{ "dispel_invis", 0, 0, 500, 5000, SPELL_MISC, 0, 0, },
{ "empower", 0, 0, 1000, 5000, SPELL_MISC, 0, 0, },
{ "enchant_weapon", 0, 0, 1500, 5000, SPELL_MISC, 0, 0, },
{ "enchant_armor", 0, 0, 1500, 5000, SPELL_MISC, 0, 0, },
{ "faerie_fog", 0, 0, 1000, 5000, SPELL_MISC, 0, 0, },
{ "fireproof", 0, 0, 1000, 5000, SPELL_MISC, 0, 0, },
{ "floating_disc", 0, 0, 500, 5000, SPELL_MISC, 0, 0, },
{ "identify", 0, 0, 1000, 5000, SPELL_MISC, 45, 0, },
{ "know_alignment", 0, 0, 0, 5000, SPELL_MISC, 0, 0, },
{ "recharge", 0, 0, 500, 5000, SPELL_MISC, 0, 0, },
{ "resurrect", 0, 0, 1000, 5000, SPELL_MISC, 0, 0, },
{ "", 0, 0, 0, 0, 0, 0, 0, } };

/* Affected by/shielded by flags */
XCOST_DATA
		bitaf_prices[] =
		{
		/*  { NAME,		BIT,			FLAGS,			AQP,	PLAT,	TYPE,		REQ1,	REQ2	} */
		{ "detect_evil", AFF_DETECT_EVIL, XUPGD_OBJ, 200, 4000, BIT_AFFECT, 0,
				0, },
		{ "detect_magic", AFF_DETECT_MAGIC, XUPGD_MOB | XUPGD_OBJ, 200, 4000,
				BIT_AFFECT, 0, 0, },
		{ "detect_motion", AFF_DETECT_MOTION, XUPGD_MOB | XUPGD_OBJ, 700, 8000,
				BIT_AFFECT, 0, 0, },
		{ "detect_good", AFF_DETECT_GOOD, XUPGD_OBJ, 200, 4000, BIT_AFFECT, 0,
				0, },
		{ "infrared", AFF_INFRARED, XUPGD_MOB | XUPGD_OBJ, 200, 4000,
				BIT_AFFECT, 0, 0, },
		{ "farsight", AFF_FARSIGHT, XUPGD_OBJ, 700, 8000, BIT_AFFECT, 0, 0, },
		{ "sneak", AFF_SNEAK, XUPGD_MOB | XUPGD_OBJ, 2000, 16000, BIT_AFFECT,
				0, 0, },
		{ "flying", AFF_FLYING, XUPGD_MOB | XUPGD_OBJ, 1300, 12000, BIT_AFFECT,
				0, 0, },
		{ "pass_door", AFF_PASS_DOOR, XUPGD_MOB | XUPGD_OBJ, 1300, 12000,
				BIT_AFFECT, 0, 0, },
		{ "haste", AFF_HASTE, XUPGD_MOB, 1300, 12000, BIT_AFFECT, 0, 0, },
		{ "dark_vision", AFF_DARK_VISION, XUPGD_MOB | XUPGD_OBJ, 200, 4000,
				BIT_AFFECT, 0, 0, },
		{ "berserk", AFF_BERSERK, XUPGD_MOB, 1300, 12000, BIT_AFFECT, 0, 0, },
				{ "swim", AFF_SWIM, XUPGD_MOB | XUPGD_OBJ, 700, 8000,
						BIT_AFFECT, 0, 0, },
				{ "regeneration", AFF_REGENERATION, XUPGD_MOB, 700, 8000,
						BIT_AFFECT, 0, 0, },
				{ "invisible", SHD_INVISIBLE, XUPGD_MOB | XUPGD_OBJ, 700, 8000,
						BIT_SHIELD, 0, 0, },
				{ "iceshield", SHD_ICE, XUPGD_MOB, 1300, 12000, BIT_SHIELD, 0,
						0, },
				{ "fireshield", SHD_FIRE, XUPGD_MOB, 1300, 12000, BIT_SHIELD,
						0, 0, },
				{ "shockshield", SHD_SHOCK, XUPGD_MOB, 1300, 12000, BIT_SHIELD,
						0, 0, },
				{ "sanctuary", SHD_SANCTUARY, XUPGD_MOB, 2800, 20000,
						BIT_SHIELD, 0, 0, },
				{ "", 0, 0, 0, 0, 0, 0, 0, } };

/* Weapon flags: req1=smith, req2=mage */
XCOST_DATA wpnflag_prices[] =
{
/*  { NAME,		BIT,			FLAGS,	AQP,	PLAT,	TYPE,	REQ1,	REQ2	} */
{ "flaming", WEAPON_FLAMING, 0, 500, 7500, 0, 0, 80, },
{ "frost", WEAPON_FROST, 0, 500, 7500, 0, 0, 80, },
{ "vampiric", WEAPON_VAMPIRIC, 0, 500, 7500, 0, 0, 60, },
{ "sharp", WEAPON_SHARP, 0, 1200, 7500, 0, 60, 0, },
{ "vorpal", WEAPON_VORPAL, 0, 2000, 7500, 0, 60, 40, },
{ "two-handed", WEAPON_TWO_HANDS, 0, 0, 7500, 0, 80, 0, },
{ "shocking", WEAPON_SHOCKING, 0, 500, 7500, 0, 0, 80, },
{ "poisoned", WEAPON_POISON, 0, 500, 7500, 0, 35, 40, },
{ "", 0, 0, 0, 0, 0, 0, 0, } };

/* Object flags: req1=smith, req2=mage */
XCOST_DATA objflag_prices[] =
{
/*  { NAME,		BIT,			FLAGS,	AQP,	PLAT,	TYPE,	REQ1,	REQ2	} */
{ "glowing", ITEM_GLOW, 0, 200, 2500, 0, 50, 100, },
{ "humming", ITEM_HUM, 0, 800, 2500, 0, 0, 40, },
{ "dark", ITEM_DARK, 0, 200, 2500, 0, 0, 40, },
{ "evil", ITEM_EVIL, 0, 200, 2500, 0, 0, 40, },
{ "invis", ITEM_INVIS, 0, 600, 2500, 0, 0, 60, },
{ "magic", ITEM_MAGIC, 0, 200, 2500, 0, 0, 40, },
{ "nodrop", ITEM_NODROP, 0, 600, 2500, 0, 0, 45, },
{ "bless", ITEM_BLESS, 0, 400, 2500, 0, 0, 0, },
{ "anti_good", ITEM_ANTI_GOOD, 0, 200, 2500, 0, 0, 50, },
{ "anti_evil", ITEM_ANTI_EVIL, 0, 200, 2500, 0, 0, 50, },
{ "anti_neutral", ITEM_ANTI_NEUTRAL, 0, 200, 2500, 0, 0, 50, },
{ "noremove", ITEM_NOREMOVE, 0, 400, 2500, 0, 0, 75, },
{ "nopurge", ITEM_NOPURGE, 0, 400, 2500, 0, 0, 60, },
{ "nosac", ITEM_NOSAC, 0, 400, 2500, 0, 0, 45, },
{ "nonmetal", ITEM_NONMETAL, 0, 800, 2500, 0, 100, 40, },
{ "nolocate", ITEM_NOLOCATE, 0, 1200, 2500, 0, 75, 60, },
{ "meltdrop", ITEM_MELT_DROP, 0, 600, 2500, 0, 0, 60, },
{ "sellextract", ITEM_SELL_EXTRACT, 0, 400, 2500, 0, 80, 40, },
{ "burnproof", ITEM_BURN_PROOF, 0, 800, 2500, 0, 50, 100, },
{ "nouncurse", ITEM_NOUNCURSE, 0, 600, 2500, 0, 0, 50, },
{ "rotdeath", ITEM_ROT_DEATH, 0, 400, 2500, 0, 0, 60, },
{ "", 0, 0, 0, 0, 0, 0, 0, } };

/* Damage types for mobs - bits loaded in by looking up attack table on boot: req1=trainer req2=mage for obj */
XCOST_DATA damtype_prices[] =
{
/*  { NAME,		BIT,	FLAGS,	AQP,	PLAT,	TYPE,	REQ1,	REQ2	} */
{ "slice", 0, 0, 0, 10000, 0, 0, 0, },
{ "stab", 0, 0, 0, 10000, 0, 0, 0, },
{ "whip", 0, 0, 0, 10000, 0, 0, 0, },
{ "claw", 0, 0, 0, 10000, 0, 0, 0, },
{ "blast", 0, 0, 2000, 5000, 0, 0, 75, },
{ "pound", 0, 0, 0, 10000, 0, 0, 0, },
{ "crush", 0, 0, 0, 10000, 0, 0, 0, },
{ "grep", 0, 0, 0, 10000, 0, 0, 0, },
{ "bite", 0, 0, 0, 10000, 0, 0, 0, },
{ "pierce", 0, 0, 0, 10000, 0, 0, 0, },
{ "suction", 0, 0, 2000, 5000, 0, 0, 0, },
{ "beating", 0, 0, 0, 10000, 0, 0, 0, },
{ "digestion", 0, 0, 2000, 5000, 0, 50, 0, },
{ "charge", 0, 0, 2000, 5000, 0, 0, 0, },
{ "slap", 0, 0, 0, 10000, 0, 0, 0, },
{ "punch", 0, 0, 0, 10000, 0, 0, 0, },
{ "wrath", 0, 0, 4000, 2500, 0, 60, 100, },
{ "magic", 0, 0, 4000, 2500, 0, 60, 100, },
{ "divine", 0, 0, 4000, 2500, 0, 60, 100, },
{ "cleave", 0, 0, 0, 10000, 0, 0, 0, },
{ "scratch", 0, 0, 0, 10000, 0, 0, 0, },
{ "peck", 0, 0, 0, 10000, 0, 0, 0, },
{ "peckb", 0, 0, 0, 10000, 0, 0, 0, },
{ "chop", 0, 0, 0, 10000, 0, 0, 0, },
{ "sting", 0, 0, 0, 10000, 0, 0, 0, },
{ "smash", 0, 0, 0, 10000, 0, 0, 0, },
{ "shbite", 0, 0, 2000, 5000, 0, 50, 75, },
{ "flbite", 0, 0, 2000, 5000, 0, 40, 75, },
{ "frbite", 0, 0, 2000, 5000, 0, 70, 75, },
{ "acbite", 0, 0, 2000, 5000, 0, 50, 75, },
{ "chomp", 0, 0, 0, 10000, 0, 0, 0, },
{ "drain", 0, 0, 4000, 2500, 0, 50, 80, },
{ "thrust", 0, 0, 0, 10000, 0, 0, 0, },
{ "slime", 0, 0, 2000, 5000, 0, 50, 0, },
{ "shock", 0, 0, 2000, 5000, 0, 50, 0, },
{ "thwack", 0, 0, 0, 10000, 0, 0, 0, },
{ "wave", 0, 0, 2000, 5000, 0, 60, 50, },
{ "flame", 0, 0, 2000, 5000, 0, 40, 0, },
{ "chill", 0, 0, 2000, 5000, 0, 70, 0, },
{ "typo", 0, 0, 0, 0, 0, 0, 0, },
{ "", 0, 0, 0, 0, 0, 0, 0, } };

/* Mobile resistances: req1=trainer, req2=mage */
XCOST_DATA res_prices[] =
{
/*  { NAME,		BIT,		FLAGS,	AQP,	PLAT,	TYPE,	REQ1,	REQ2	} */
{ "summon", RES_SUMMON, 0, 1000, 5000, 0, 0, 40, },
{ "charm", RES_CHARM, 0, 1000, 5000, 0, 0, 40, },
{ "magic", RES_MAGIC, 0, 3000, 15000, 0, 0, 60, },
{ "weapon", RES_WEAPON, 0, 3000, 15000, 0, 60, 0, },
{ "bash", RES_BASH, 0, 2500, 12500, 0, 50, 0, },
{ "pierce", RES_PIERCE, 0, 2500, 12500, 0, 50, 0, },
{ "slash", RES_SLASH, 0, 2500, 12500, 0, 50, 0, },
{ "fire", RES_FIRE, 0, 2000, 10000, 0, 40, 50, },
{ "cold", RES_COLD, 0, 2000, 10000, 0, 40, 50, },
{ "lightning", RES_LIGHTNING, 0, 2000, 10000, 0, 40, 50, },
{ "acid", RES_ACID, 0, 2000, 10000, 0, 35, 50, },
{ "poison", RES_POISON, 0, 1000, 5000, 0, 0, 45, },
{ "negative", RES_NEGATIVE, 0, 2000, 10000, 0, 0, 40, },
{ "holy", RES_HOLY, 0, 2000, 10000, 0, 0, 40, },
{ "energy", RES_ENERGY, 0, 2000, 10000, 0, 0, 40, },
{ "mental", RES_MENTAL, 0, 2000, 10000, 0, 0, 70, },
{ "disease", RES_DISEASE, 0, 1000, 5000, 0, 0, 40, },
{ "drowning", RES_DROWNING, 0, 2000, 10000, 0, 0, 0, },
{ "light", RES_LIGHT, 0, 2000, 10000, 0, 0, 0, },
{ "sound", RES_SOUND, 0, 2000, 10000, 0, 0, 0, },
{ "wood", RES_WOOD, 0, 2000, 10000, 0, 0, 0, },
{ "silver", RES_SILVER, 0, 2000, 10000, 0, 0, 0, },
{ "iron", RES_IRON, 0, 2000, 10000, 0, 0, 0, },
{ "", 0, 0, 0, 0, 0, 0, 0, } };

/* Mobile immunities: req1=trainer, req2=mage */
XCOST_DATA imm_prices[] =
{
/*  { NAME,		BIT,		FLAGS,			AQP,	PLAT,	TYPE,	REQ1,	REQ2	} */
{ "summon", IMM_SUMMON, 0, 2000, 10000, 0, 0, 60, },
{ "charm", IMM_CHARM, 0, 2000, 10000, 0, 0, 60, },
{ "bash", IMM_BASH, XUPGD_IMM_SPECIAL, 6000, 30000, 0, 100, 0, },
{ "pierce", IMM_PIERCE, XUPGD_IMM_SPECIAL, 6000, 30000, 0, 100, 0, },
{ "slash", IMM_SLASH, XUPGD_IMM_SPECIAL, 6000, 20000, 0, 100, 0, },
{ "fire", IMM_FIRE, 0, 4000, 20000, 0, 70, 100, },
{ "cold", IMM_COLD, 0, 4000, 20000, 0, 70, 100, },
{ "lightning", IMM_LIGHTNING, 0, 4000, 20000, 0, 70, 100, },
{ "acid", IMM_ACID, 0, 4000, 20000, 0, 35, 50, },
{ "poison", IMM_POISON, 0, 2000, 10000, 0, 0, 90, },
{ "negative", IMM_NEGATIVE, 0, 4000, 20000, 0, 0, 80, },
{ "holy", IMM_HOLY, 0, 4000, 20000, 0, 0, 80, },
{ "energy", IMM_ENERGY, 0, 4000, 20000, 0, 0, 80, },
{ "mental", IMM_MENTAL, 0, 4000, 20000, 0, 0, 120, },
{ "disease", IMM_DISEASE, 0, 2000, 10000, 0, 0, 80, },
{ "drowning", IMM_DROWNING, 0, 4000, 20000, 0, 0, 80, },
{ "light", IMM_LIGHT, 0, 4000, 20000, 0, 0, 40, },
{ "sound", IMM_SOUND, 0, 4000, 20000, 0, 0, 40, },
{ "wood", IMM_WOOD, 0, 4000, 20000, 0, 0, 40, },
{ "silver", IMM_SILVER, 0, 4000, 20000, 0, 0, 40, },
{ "iron", IMM_IRON, 0, 4000, 20000, 0, 0, 40, },
{ "", 0, 0, 0, 0, 0, 0, 0, } };

/* Offensive bits for mobiles: req1=trainer, req2=mage */
XCOST_DATA off_prices[] =
{
/*  { NAME,		BIT,			FLAGS,	AQP,	PLAT,	TYPE,	REQ1,	REQ2	} */
{ "area attack", OFF_AREA_ATTACK, 0, 1000, 15000, 0, 75, 0, },
{ "backstab", OFF_BACKSTAB, 0, 500, 10000, 0, 50, 0, },
{ "bash", OFF_BASH, 0, 500, 10000, 0, 35, 0, },
{ "berserk", OFF_BERSERK, 0, 0, 5000, 0, 35, 0, },
{ "disarm", OFF_DISARM, 0, 0, 5000, 0, 45, 0, },
{ "dodge", OFF_DODGE, 0, 0, 5000, 0, 0, 0, },
{ "fade", OFF_FADE, 0, 500, 10000, 0, 45, 60, },
{ "fast", OFF_FAST, 0, 500, 10000, 0, 50, 0, },
{ "kick", OFF_KICK, 0, 0, 5000, 0, 60, 0, },
{ "kick_dirt", OFF_KICK_DIRT, 0, 500, 10000, 0, 35, 0, },
{ "parry", OFF_PARRY, 0, 0, 5000, 0, 0, 0, },
{ "rescue", OFF_RESCUE, 0, 0, 5000, 0, 0, 0, },
{ "trip", OFF_TRIP, 0, 500, 10000, 0, 0, 0, },
{ "assist_all", ASSIST_ALL, 0, 0, 5000, 0, 0, 0, },
{ "assist_align", ASSIST_ALIGN, 0, 0, 5000, 0, 0, 0, },
{ "assist_race", ASSIST_RACE, 0, 0, 5000, 0, 0, 0, },
{ "assist_players", ASSIST_PLAYERS, 0, 0, 5000, 0, 0, 0, },
{ "assist_guard", ASSIST_GUARD, 0, 0, 5000, 0, 0, 0, },
{ "assist_vnum", ASSIST_VNUM, 0, 0, 5000, 0, 0, 0, },
{ "", 0, 0, 0, 0, 0, 0, 0, } };

/********************************* MEMORY MANAGEMENT **********************************/

/* Make a new piece of PKILL_DATA */
PKILL_DATA * new_pkill_data(void)
{
	PKILL_DATA *pkilldata;

	// Allocate memory/use pre-freed
	if (pkill_data_free)
	{
		pkilldata = pkill_data_free; // Make pkilldata point to first free PKILL_DATA
		pkill_data_free = pkill_data_free->next; // Make pkill_data_free point to next freed PKILL_DATA
	}
	else
		pkilldata = alloc_perm(sizeof(*pkilldata)); // No free PKILL_DATA, allocate a perm

	// Initialise values
	pkilldata->kPoints = 0;
	pkilldata->kills = 0;
	pkilldata->deaths = 0;
	pkilldata->wins = 0;
	pkilldata->losses = 0;

	// Validate to mark it as initialised
	VALIDATE( pkilldata );

	// All set to go!
	return pkilldata;
}

/* Free a chunk of PKILL_DATA */
void free_pkill_data(PKILL_DATA *pkilldata)
{
	// Return if already freed
	if (IS_VALID( pkilldata ))
		return;

	// Mark as freed and whack onto the free list
	INVALIDATE( pkilldata );
	pkilldata->next = pkill_data_free; // Make pkilldata->next point to first item on free list
	pkill_data_free = pkilldata; // Make the first item on free list point to pkilldata
}

/* Make a new piece of CLAN_DATA */
CLAN_DATA * new_clan_data(bool get_id)
{
	CLAN_DATA *clan;
	int i;

	// Allocate memory/use pre-freed
	if (clan_free)
	{
		clan = clan_free; // Make clan point to first free CLAN_DATA
		clan_free = clan_free->next; // Make clan_free point to next freed CLAN_DATA
	}
	else
		clan = alloc_perm(sizeof(*clan)); // No free CLAN_DATA, allocate a perm

	// Initialise values
	clan->next = NULL;
	clan->enemy = NULL;
	clan->Penemy.data = NULL;
	clan->pkilldata = new_pkill_data();
	clan->upgrade = NULL;
	clan->xupgrade = NULL;
	clan->u_rost = NULL;

	for (i = 0; i < CMOB_MAX; i++)
		clan->cMob[i] = NULL;

	for (i = 0; i < CROOM_MAX; i++)
		clan->cRoom[i] = NULL;

	for (i = 0; i < 2; i++)
		clan->skills[i] = -1;

	clan->name = str_dup("newclan");
	clan->c_name = str_dup("New Clan");

	for (i = 0; i < TOTAL_CLAN_RANK; i++)
	{
		clan->r_name[i] = str_dup(clan_rank_table[i].name);
		clan->r_who[i] = str_dup(clan_rank_table[i].who_name);
	}

	clan->description = str_dup("");
	clan->platinum = 0;
	clan->qps = 0;
	clan->exit_changed = 0;
	clan->edit_time = 0;
	clan->u_quantity = 0;

	if (get_id)
		clan->id = generate_clan_id(); // Get identifier for clan
	else
		clan->id = 0;

	clan->members = 0;
	clan->vnum[0] = 0;
	clan->vnum[1] = 0;
	clan->entrance_vnum = 0;
	clan->pkill = FALSE;

	// Validate to mark it as initialised
	VALIDATE( clan );

	// Add it to clan list and up max_clan number
	clan->next = clan_list; // Make clan->next point to first item on the clan list
	clan_list = clan; // Make clan the first item on the clan list
	++max_clan;

	// All set to go!
	return clan;
}

/* Free a chunk of CLAN_DATA */
void free_clan_data(CLAN_DATA *clan)
{
	int i;

	// Return if already freed
	if (IS_VALID( clan ))
		return;

	// Free strings first
	free_string(clan->name);
	free_string(clan->c_name);

	for (i = 0; i < TOTAL_CLAN_RANK; i++)
	{
		free_string(clan->r_name[i]);
		free_string(clan->r_who[i]);
	}

	free_string(clan->description);

	// If clan pkilldata hasn't been freed yet, free it
	if (clan->pkilldata)
		free_pkill_data(clan->pkilldata);

	// Mark as free and put on the clan_free list
	INVALIDATE( clan );
	clan->next = clan_free; // Make clan->next point to first item on the clan_free list
	clan_free = clan; // Make clan the first item on the clan_free list
	--max_clan;
}

/* Make a new piece of ROSTER_DATA */
ROSTER_DATA * new_roster_data(bool get_id)
{
	ROSTER_DATA *roster;

	// Allocate memory
	if (roster_free)
	{
		roster = roster_free; // Make roster point to first free ROSTER_DATA
		roster_free = roster_free->next; // Make roster_free point to next freed ROSTER_DATA
	}
	else
		roster = alloc_perm(sizeof(*roster)); // No free ROSTER_DATA, allocate a perm

	// Initialise values
	roster->next = NULL;
	roster->character = NULL;
	roster->clan = get_clan_by_id(CLAN_NONE);
	roster->petition = NULL;
	roster->pkilldata = NULL;
	roster->name = str_dup("");
	roster->last_login = 0;
	roster->penalty_time = 0;
	roster->dPoints = 0;
	roster->flags = 0;
	roster->bounty = 0;
	if (get_id)
		roster->id = generate_roster_id();
	else
		roster->id = 0;
	roster->trust = TRUST_ALL;

	// Validate to mark it as initialised and add to roster list
	VALIDATE( roster );
	roster->next = roster_list; // Make roster->next point to the first item on the roster list
	roster_list = roster; // Make roster the first item on the roster list

	// All set to go!
	return roster;
}

/* Free a chunk of ROSTER_DATA */
void free_roster_data(ROSTER_DATA *roster)
{
	// Return if already freed
	if (IS_VALID( roster ))
		return;

	// Free strings first
	free_string(roster->name);

	// Free pkilldata if not already done
	if (roster->pkilldata)
		free_pkill_data(roster->pkilldata);

	// Mark as free and add to roster list
	INVALIDATE( roster );
	roster->next = roster_free; // Make roster->next point to the first item on the roster_free list
	roster_free = roster; // Make roster the first item on the roster_free list
}

/********************************** HELPER FUNCTIONS **********************************/

/* Lookup a vnum on the vnum_type tables */
char * vnum_lookup(const struct vnum_table *table, int vnum)
{
	static char buf[MAX_STRING_LENGTH];

	for (; table->name[0] != '\0'; table++)
		if (table->vnum == vnum)
			break;
	if (table->vnum < 0)
		strcpy(buf, "-");
	else
		strcpy(buf, capitalize(table->name));

	return buf;
}

/* Generate a unique clan id */
int generate_clan_id(void)
{
	CLAN_DATA *clan;
	int id, i, j;
	bool found;

	for (i = 0; i < 50; i++) // Prevent infinite loops
	{
		id = current_time * 2;
		if (id < 0)
			id *= -1;

		for (j = 0; j <= i && id > 500; j++)
			id -= number_range(100, 300);

		if (id < 0)
			id *= -1;
		if (id < 500)
			id += number_range(500, 3000);

		// Check if any clan has this id
		for (found = FALSE, clan = clan_list, i = 0; clan != NULL; clan
				= clan->next, i++)
		{
			if (clan->id == id)
				found = TRUE;
		}
		if (!found)
			return id;
	}

	printf_debug("generate_clan_id: unable to retrieve unique id.");
	return -1;
}

long get_total_points(ROSTER_DATA *roster)
{
	if (roster == NULL)
		return -1;

	return roster->pkilldata->kPoints * 2 + roster->dPoints;
}

/* Get the index of the players rank on the rank table */
int get_r_rank_index(ROSTER_DATA *roster)
{
	if (roster == NULL)
		return -1;

	return get_p_rank_index(get_total_points(roster));
}

/* Find the rank if given points */
int get_p_rank_index(long points)
{
	int i;

	for (i = (TOTAL_CLAN_RANK - 1); i > 0; i--)
		if (points >= clan_rank_table[i].points)
			break;

	return i;
}

void clanskill_check(CHAR_DATA *ch) {
	ROSTER_DATA *roster;

	if (IS_NPC(ch))
		return;

	GET_ROSTER(ch, roster);
	int i;

	for (i = 0; clan_skill_table[i].cmd[0] != '\0'; i++)
	{
		if (clan_skill_table[i].gsn)
		{
			if ((roster == NULL || !clan_has_skill(roster->clan, i))
					&& is_affected(ch, *(clan_skill_table[i].gsn)))
				affect_strip(ch, *(clan_skill_table[i].gsn));
		}
		else if (!str_cmp(clan_skill_table[i].cmd, "modi"))
		{
			if (roster == NULL || !clan_has_skill(roster->clan, i))
			{
				if (is_affected(ch, gsn_concentration))
					affect_strip(ch, gsn_concentration);
				if (is_affected(ch, gsn_modis_anger))
					affect_strip(ch, gsn_modis_anger);
			}
		}
	}
}

bool clan_has_skill(CLAN_DATA *clan, int skill) {
	int i;
	if (clan == NULL)
		return FALSE;
	for (i = 0; i < 2; i++)
		if (clan->skills[i] == skill)
			return TRUE;
	return FALSE;
}

/* Get clan data by looking up id - faster than by name */
CLAN_DATA * get_clan_by_id(int id)
{
	CLAN_DATA *clan;

	for (clan = clan_list; clan != NULL; clan = clan->next)
		if (clan->id == id)
			return clan;

	return NULL;
}

/* Get clan data by looking up name */
CLAN_DATA * get_clan_by_name(char *name)
{
	CLAN_DATA *clan;

	if (name[0] == '\0')
		return NULL;

	for (clan = clan_list; clan != NULL; clan = clan->next)
	{
		if (UPPER( name[0] ) != UPPER( clan->name[0] )) // Compare first letter first
			continue;
		else if (!str_cmp(clan->name, name))
			return clan;
	}

	return NULL;
}

CLAN_DATA * clan_lookup(char *name)
{
	return get_clan_by_name(name);
}

/* Get clan data by looking up vnums */
CLAN_DATA * get_clan_by_vnum(int vnum)
{
	CLAN_DATA *clan;

	for (clan = clan_list; clan != NULL; clan = clan->next)
	{
		if (clan->vnum[0] == clan->vnum[1])
			continue;
		if (vnum >= clan->vnum[0] && vnum <= clan->vnum[1])
			return clan;
	}
	return NULL;
}

/* Get a player's clan */
CLAN_DATA * get_clan_by_ch(CHAR_DATA *ch)
{
	ROSTER_DATA *roster;

	if (ch->pIndexData)
		return get_clan_by_vnum(ch->pIndexData->vnum);
	GET_ROSTER( ch, roster );

	if (roster == NULL || roster->clan == NULL)
		return get_clan_by_id(CLAN_NONE);
	return roster->clan;
}

/* Get a clan deathroom if clanned and there is one or put them in the temple */
ROOM_INDEX_DATA * get_clan_deathroom(CHAR_DATA *ch)
{
	ROOM_INDEX_DATA *pRoom;
	ROSTER_DATA *roster;
	GET_ROSTER( ch, roster );

	if (roster == NULL || roster->clan->independent
			|| roster->clan->cRoom[CROOM_RECALL] == NULL)
//		pRoom = get_room_index(ROOM_VNUM_TEMPLE);
		pRoom = get_room_index(2909);
	else
		pRoom = roster->clan->cRoom[CROOM_RECALL];

	if (pRoom == NULL) // If no temple, fall back to the market square
	{
		printf_debug("ROOM_VNUM_TEMPLE (%d) == NULL", ROOM_VNUM_TEMPLE );
		pRoom = get_room_index(VNUM_MARKET_SQUARE);
	}

	return pRoom;
}

/* Generate a unique roster id */
int generate_roster_id(void)
{
	ROSTER_DATA *roster;
	int id, i, j;
	bool found;

	for (i = 0; i < 1000; i++) // Prevent infinite loops
	{
		id = current_time * number_range(1, 5);

		if (id < 0)
			id *= -1;

		for (j = 0; j <= i && id > 500; j++)
			id -= number_range(100, 300);

		if (id < 0)
			id *= -1;
		if (id < 500)
			id += number_range(500, 3000);

		// Check if any roster has this id
		for (found = FALSE, roster = roster_list, i = 0; roster != NULL; roster
				= roster->next, i++)
			if (roster->id == id)
				found = TRUE;

		if (!found)
			return id;
	}

	printf_debug("generate_roster_id: unable to retrieve unique id.");
	return -1;
}

/* Get roster data by char data - faster */
ROSTER_DATA * get_roster_by_id(int id)
{
	ROSTER_DATA *roster;

	for (roster = roster_list; roster != NULL; roster = roster->next)
		if (id == roster->id)
			return roster;

	return NULL;
}

/* Get roster data by player name */
ROSTER_DATA * get_roster_by_name(char *name, bool prefix)
{
	ROSTER_DATA *roster;

	if (name[0] == '\0')
		return NULL;

	for (roster = roster_list; roster != NULL; roster = roster->next)
	{
		if (IS_SET( roster->flags, ROST_DELETED ))
			continue;
		else if (UPPER( name[0] ) != UPPER( roster->name[0] )) // Compare first letter first
			continue;
		else if ((prefix && !str_prefix(name, roster->name)) || !str_cmp(name,
				roster->name))
			return roster;
	}

	return NULL;
}

/* Update roster for specific character */
void update_roster(CHAR_DATA *ch, bool quit)
{
	ROSTER_DATA *roster;

	if (ch == NULL || IS_NPC(ch) || ch->pcdata == NULL)
		return;

	roster = ch->pcdata->roster;

	if (quit)
	{
		if (!roster && (roster = get_roster_by_name(ch->name, FALSE )) == NULL) // Check pcdata first, if unable to find, check by character name
		{
			printf_debug(
					"update_roster: unable to find roster data for quitting character %s",
					ch->name);
			return;
		}
		roster->last_login = current_time;
		roster->character = NULL;
		if (IS_IMMORTAL( ch ) && !IS_SET( roster->flags, ROST_IMMORTAL ))
			SET_BIT( roster->flags, ROST_IMMORTAL );
		else if (!IS_IMMORTAL( ch ) && IS_SET( roster->flags, ROST_IMMORTAL ))
			REMOVE_BIT( roster->flags, ROST_IMMORTAL );
		SET_ROSTER( ch, NULL );
	}
	else
	{
		if (!roster && (roster = get_roster_by_name(ch->name, FALSE )) == NULL) // New characters
		{
			roster = new_roster_data(TRUE);

			roster->character = ch;
			roster->clan = get_clan_by_id(CLAN_NONE);
			roster->pkilldata = new_pkill_data();
			free_string(roster->name);
			roster->name = str_dup(ch->name);
			roster->last_login = current_time;

			SET_ROSTER( ch, roster );
			save_all_rosters();
			return;
		}
		REMOVE_BIT( roster->flags, ROST_INACTIVE );
		REMOVE_BIT( roster->flags, ROST_AWAY );
		roster->last_login = current_time;
		roster->character = ch;
		if (IS_IMMORTAL( ch ) && !IS_SET( roster->flags, ROST_IMMORTAL ))
			SET_BIT( roster->flags, ROST_IMMORTAL );
		else if (!IS_IMMORTAL( ch ) && IS_SET( roster->flags, ROST_IMMORTAL ))
			REMOVE_BIT( roster->flags, ROST_IMMORTAL );
		SET_ROSTER( ch, roster );
		clanskill_check(ch);
	}
	save_roster_data(roster);
}

/* Update rosters, flags, killed by stuff */
void update_all_rosters(void)
{
	char buf[MAX_STRING_LENGTH];
	ROSTER_DATA *roster;
	bool penalty = FALSE;
	FILE *fp;

	if (roster_next_update <= 0 || roster_next_update <= current_time)
		roster_next_update = current_time + TIME_ROSTER_UPD_TICK;
	else
		return;

	if (roster_next_penalty <= 0)
		roster_next_penalty = current_time + TIME_ROSTER_PEN_TICK;
	else if (roster_next_penalty <= current_time)
	{
		penalty = TRUE;
		roster_next_penalty = current_time + TIME_ROSTER_PEN_TICK;
	}

	for (roster = roster_list; roster != NULL; roster = roster->next)
	{
		if (IS_SET( roster->flags, ROST_DELETED ))
			continue;

		sprintf(buf, "%s%s%s%s", PLAYER_DIR, initial(roster->name), "/", capitalize(roster->name));
		fp = NULL;

		if (roster->character)
			roster->last_login = current_time;
		// Check if character exists.
		else if ((fp = fopen(buf, "r")) == NULL)
		{
			mark_delete_roster(roster);
			continue;
		}
		else if (!roster->clan->independent)
		{
			if (!IS_SET( roster->flags, ROST_AWAY )
					|| !IS_SET( roster->flags, ROST_EXILE_GRACE ))
			{
				if ((current_time - roster->last_login) > TIME_WEEK
						&& roster->trust == TRUST_LDR)
					SET_BIT( roster->flags, ROST_INACTIVE );

				else if ((current_time - roster->last_login) > (2 * TIME_WEEK))
					SET_BIT( roster->flags, ROST_INACTIVE );
			}
		}
		if (fp != NULL)
			fclose(fp);

		if (IS_SET( roster->flags, ROST_EXILE_GRACE ) && current_time
				>= roster->penalty_time)
			exile_player(roster);
		else if (penalty && roster->dPoints > 0
				&& IS_SET( roster->flags, ROST_EXILE_GRACE ))
			roster->dPoints -= 10;
		if (IS_SET( roster->flags, ROST_NEWBIE )
		&& (roster->pkilldata->kPoints >= 10
		|| (roster->character && roster->character->played > 360000)))
		{
			REMOVE_BIT( roster->flags, ROST_NEWBIE );
			if (roster->character)
				send_to_char(
						"{RYour newbie flag has expired. You are now a FULL player-killer.\n\r",
						roster->character);
		}
	}

	save_all_rosters();
}

/* Mark roster for deletion */
void mark_delete_roster(ROSTER_DATA *roster)
{
	if (IS_SET( roster->flags, ROST_DELETED ))
		return;

	SET_BIT( roster->flags, ROST_DELETED );
	roster->penalty_time = current_time + TIME_ROSTER_DELETE;
}

/* Clan transaction logging */
void append_clan_log(CLAN_DATA *clan, int type, char *name, char *string)
{
	FILE *fp;
	char file[100];

	sprintf(file, "%s%d.log", is_main_server() ? CLAN_HISTORY_FOLDER
			: TEST_CLAN_HISTORY_FOLDER, clan->id);

	smash_tilde(string);

	if ((fp = fopen(file, "a")) == NULL)
	{
		printf_system(
				"append_clan_log: unable to open %s clan log for appending.",
				clan->name);
		return;
	}

	fprintf(fp, ">%d %ld %s %s\n", type, current_time, name, string);

	fclose(fp);

	if (type == ENTRY_DELETE)
	{
		char fileB[100];
		sprintf(fileB, "%s%d.del", is_main_server() ? CLAN_HISTORY_FOLDER
				: TEST_CLAN_HISTORY_FOLDER, clan->id);
		rename(file, fileB);
	}
}

/* Display clan log */
void parse_clan_log(CHAR_DATA *ch, CLAN_DATA *clan, void *filter, int ftype)
{
	BUFFER *output;
	FILE *fp;
	char buf[MAX_STRING_LENGTH], file[100];
	bool found = FALSE;

	sprintf(file, "%s%d.log", is_main_server() ? CLAN_HISTORY_FOLDER
			: TEST_CLAN_HISTORY_FOLDER, clan->id);

	if ((fp = fopen(file, "r")) == NULL)
	{
		perror(file);
		sprintf(buf, "Unable to open %s clan log file", clan->name);
		send_to_char(buf, ch);
		return;
	}

	output = new_buf();

	while (!feof(fp))
	{
		char c, *name, *string;
		int type;
		time_t time;

		while (!feof(fp) && (c = fread_letter(fp)) != '>')
			;

		if (feof(fp))
			break;

		type = fread_number(fp);
		time = fread_number(fp);
		name = fread_word(fp);
		string = fread_string_eol(fp);

		if (time < (current_time - (2 * TIME_WEEK)) || (type == ENTRY_GUILD
				&& !IS_IMMORTAL( ch )))
			goto LoopEnd;

		switch (ftype)
		// If the entry doesn't pass the filter check, then we'll skip
		{ // past the displaying bit straight to freeing string
		case FILTER_NAME:
			if (str_cmp(((char *) filter), name))
				goto LoopEnd;
			break;
		case FILTER_TYPE:
			if (type != (*(long *) filter))
				goto LoopEnd;
			break;
		case FILTER_TIME:
			if (time < (*(long *) filter))
				goto LoopEnd;
			break;
		case FILTER_KEYWORD:
			if (str_infix(((char *) filter), string))
				goto LoopEnd;
			break;
		default:
			break;
		}

		found = TRUE;

		sprintf(buf, "{B[{w%.24s{B:%s{B] {W%s: {w%s{x\n\r", ctime(&time),
				log_entry_string[type].display, name, string);
		add_buf(output, buf);

		LoopEnd: free_string(string);
	}
	fclose(fp);

	if (found)
		page_to_char(output->string, ch);
	else
		send_to_char("There is no clan log file.\n\r", ch);

	free_buf(output);
}

/* Strip all spaces */
char * strip_spaces(char *str)
{
	static char buf[MAX_STRING_LENGTH];
	int i;

	for (i = 0; *str; str++)
	{
		if (*str == ' ')
			continue;
		else
		{
			buf[i] = *str;
			i++;
		}
	}

	buf[i] = '\0';
	return buf;
}

/* Strip spaces at the end and beginning only */
char * strip_bespaces(char *str)
{
	static char buf[MAX_STRING_LENGTH], temp[3];
	int i;

	buf[0] = '\0';

	for (;; str++)
	{
		if (*str == '{' && *(++str) != '{')
		{
			temp[0] = *(str - 1);
			temp[1] = *str;
			temp[2] = '\0';
			strcat(buf, temp);
		}
		else if (!isspace( *str ))
			break;
	}

	strcat(buf, str);

	for (i = (strlen(buf) - 1); i > 0; i--)
	{
		if (buf[i - 1] == '{' && buf[i] != '{')
		{
			i--;
			continue;
		}
		if (!isspace( buf[i] ))
			break;
	}
	buf[i + 1] = '\0';
	return buf;
}

/* Strip all colour codes and special characters */
char * strip_spec_char_col(char *str)
{
	static char buf[MAX_STRING_LENGTH];
	int i;

	for (i = 0; *str; str++)
	{
		if (*str == '{') {
			if (*(++str) == '\0')
				break;
		} else if ((*str >= 'a' && *str <= 'z') || (*str >= 'A' && *str <= 'Z')
				|| *str == ' ')
		{
			buf[i] = LOWER(*str);
			i++;
		}
	}
	buf[i] = '\0';
	return buf;
}

/* Process a short description and turn it into a valid keyword name */
char * process_name(char *str)
{
	static char buf[MAX_STRING_LENGTH];
	char temp[MAX_STRING_LENGTH];
	char *retval;
	int i;

	sprintf(buf, " %s ", strip_spec_char_col(str));

	for (i = 0; process_name_word[i][0] != '\0'; i++)
	{
		strcpy(temp, process_name_word[i]);
		str_replace(buf, temp, " ");
	}
	for (i = 0; buf[i] != '\0'; i++)
		buf[i] = LOWER( buf[i] );

	retval = buf;
	while (*retval != '\0' && *retval == ' ')
		retval++;

	return retval;
}

/* Return a two-letter penalty code - hierarchical arrangement */
char * get_pen_code(ROSTER_DATA *roster)
{
	static char pCode[3];

	if (IS_SET( roster->flags, ROST_EXILE_GRACE ))
		strcpy(pCode, "EX");
	else if (IS_SET( roster->flags, ROST_INACTIVE ))
		strcpy(pCode, "IN");
	else
		strcpy(pCode, "--");
	return pCode;
}

/* Get the area that holds all the clan vnums */
AREA_DATA * get_clan_area(void)
{
	AREA_DATA *pArea;

	for (pArea = area_first; pArea != NULL; pArea = pArea->next)
	{
		if (pArea->min_vnum == VNUM_CLAN_LOWER && pArea->max_vnum
				== VNUM_CLAN_UPPER)
			return pArea;
	}

	printf_debug("get_clan_area: No clan area!\n\r");
	return NULL;
}

/* Find a free mob vnum for a clan */
int get_clan_mvnum(CLAN_DATA *clan)
{
	int i;

	for (i = clan->vnum[0]; i <= clan->vnum[1]; i++)
		if (get_mob_index(i) == NULL)
			return i;

	return -1;
}

/* Find a free obj vnum for a clan */
int get_clan_ovnum(CLAN_DATA *clan)
{
	int i;

	for (i = clan->vnum[0]; i <= clan->vnum[1]; i++)
		if (get_obj_index(i) == NULL)
			return i;

	return -1;
}

/* Find a free room vnum for a clan */
int get_clan_rvnum(CLAN_DATA *clan)
{
	int i;

	for (i = clan->vnum[0]; i <= clan->vnum[1]; i++)
		if (get_room_index(i) == NULL)
			return i;

	return -1;
}

/* Assign a set of clan area vnums */
void assign_clan_vnums(CLAN_DATA *clan)
{
	CLAN_DATA *checkClan;
	int vnum;
	bool found;

	for (vnum = VNUM_CLAN_LOWER; vnum < (VNUM_CLAN_UPPER - 1); vnum = (vnum
			+ VNUM_CLAN_BLOCK))
	{
		for (found = FALSE, checkClan = clan_list; checkClan != NULL; checkClan
				= checkClan->next)
			if (vnum >= checkClan->vnum[0] && vnum <= checkClan->vnum[1])
				found = TRUE;
		if (!found)
		{
			clan->vnum[0] = vnum;
			clan->vnum[1] = vnum + (VNUM_CLAN_BLOCK - 1);
			return;
		}
	}
	printf_debug("assign_clan_vnums: no more free vnums for clan %d.\n\r", clan->id);
	return;
}

/* Get the number of clan eq items that can be worn */
int clan_eq_wear_max(CHAR_DATA *ch)
{
	ROSTER_DATA *roster;

	GET_ROSTER( ch, roster );

	if (roster == NULL)
		return 0;

	if (IS_IMMORTAL(ch))
		return 10;

	if (roster->clan->independent || roster->trust < TRUST_MEMBER)
		return 0;

	return URANGE( 0, get_r_rank_index( roster ), 5 );
}

PF_FILT_COND_DEF( clanhall_conditions )
{
	// No lockable doors.
	if (IS_SET(exit->rs_flags, EX_ISDOOR) && IS_SET(exit->rs_flags, EX_LOCKED))
		return TRUE;
	ROOM_INDEX_DATA *room = exit->u1.to_room;
	// No non-basic sectors.
	if (room->sector_type == SECT_WATER_NOSWIM
	||  room->sector_type == SECT_AIR)
		return TRUE;

	RESET_DATA *pReset;
	MOB_INDEX_DATA *pMob;
	// No aggies.
	for (pReset = room->reset_first; pReset != NULL; pReset = pReset->next)
	{
		if (pReset->command != 'M')
			continue;

		if ((pMob = get_mob_index(pReset->arg1)) == NULL)
			continue;

		if (IS_SET( pMob->act, ACT_AGGRESSIVE ) && pMob->level > 40)
			return TRUE;
	}

	return FALSE;
}

/* Check the directions from the market square to desired clan hall entrance */
bool check_path(ROOM_INDEX_DATA *end)
{
	return bfs_pathfind(
		get_room_index(VNUM_MARKET_SQUARE),
		pf_end_at_room(end),
		&clanhall_conditions,
		PF_MAX_DEPTH,
		FALSE
	) >= 0;
}

void reset_clan_mob(MOB_INDEX_DATA *pMob, bool delete)
{
	ROOM_INDEX_DATA *pRoom;
	CHAR_DATA *mob, *mob_next, *master = NULL;

	if (pMob == NULL)
		return;

	CLAN_DATA *clan = get_clan_by_vnum(pMob->vnum);

	for (mob = char_list; mob != NULL; master = NULL, mob = mob_next)
	{
		mob_next = mob->next;

		if (!IS_NPC( mob ))
			continue;

		if (mob->pIndexData != pMob)
			continue;

		if ((pRoom = mob->in_room) == NULL)
		{
			extract_char(mob, TRUE );
			continue;
		}

		// Only update the mob if they're currently in friendly territory.
		if (get_clan_by_vnum(pRoom->vnum) != clan)
			continue;

		if (mob->master && mob->master->pet == mob)
		{
			master = mob->master;
			mob->master->pet = NULL;
			mob->master = NULL;
		}
		extract_char(mob, TRUE );

		if (delete)
			continue;

		if (master)
		{
			// Pets.
			mob = create_mobile(pMob);
			char_to_room(mob, pRoom);
			mob->master = master;
			master->pet = mob;
		}
		else // Everything else should reset in place.
			reset_room(pRoom);
	}
}

int wear_map[] = {
	ITEM_LIGHT,
	ITEM_WEAR_FINGER,
	ITEM_WEAR_FINGER,
	ITEM_WEAR_NECK,
	ITEM_WEAR_NECK,
	ITEM_WEAR_BODY,
	ITEM_WEAR_HEAD,
	ITEM_WEAR_LEGS,
	ITEM_WEAR_FEET,
	ITEM_WEAR_HANDS,
	ITEM_WEAR_ARMS,
	ITEM_WEAR_SHIELD,
	ITEM_WEAR_ABOUT,
	ITEM_WEAR_WAIST,
	ITEM_WEAR_WRIST,
	ITEM_WEAR_WRIST,
	ITEM_WIELD,
	ITEM_HOLD,
	ITEM_WEAR_FLOAT,
	ITEM_WIELD,
	ITEM_WEAR_FACE,
	ITEM_WEAR_EAR,
	ITEM_RESERVED,
	ITEM_WEAR_ANKLE,
	ITEM_WEAR_ANKLE,
	ITEM_WEAR_CLAN,
	ITEM_WEAR_RELIG,
	ITEM_WEAR_PATCH,
	ITEM_WEAR_BACK
};

void reset_clan_obj(OBJ_INDEX_DATA *pObj, bool delete)
{
	AUCTION_DATA *auc;
	CHAR_DATA *carrier = NULL;
	CLAN_DATA *clan;
	ROOM_INDEX_DATA *room = NULL;
	OBJ_DATA *obj, *obj_next, *container = NULL, *obj_content = NULL,
			*objCheck = NULL, *objCheck_next;
	char buf[MAX_STRING_LENGTH];
	int loc = WEAR_NONE;

	if (pObj == NULL)
		return;

	clan = get_clan_by_vnum(pObj->vnum);

	for (obj = object_list; obj != NULL; loc = WEAR_NONE, room = NULL, carrier
			= NULL, container = NULL, obj_content = NULL, obj = obj_next)
	{
		obj_next = obj->next;

		if (obj->pIndexData != pObj)
			continue;

		for (auc = auction_list; auc != NULL; auc = auc->next)
		{
			if (auc->item != NULL && obj == auc->item)
			{
				if (auc->high_bidder != NULL)
				{
					add_cost(auc->high_bidder, auc->bid_amount, auc->bid_type);
					send_to_char("\n\rYour bid has been returned to you.\n\r",
							auc->high_bidder);
				}
				free_auction(auc);
				break;
			}
		}
		if (auc != NULL && auc->owner != NULL)
			carrier = auc->owner;
		if (obj->in_room != NULL)
			room = obj->in_room;
		else if (obj->carried_by != NULL)
		{
			carrier = obj->carried_by;

			if (obj->wear_loc < sizeof(wear_map))
				loc = obj->wear_loc;
		}
		else if (obj->in_obj != NULL)
			container = obj->in_obj;
		else
		{
			extract_obj(obj);
			continue;
		}
		if ((obj_content = obj->contains) != NULL)
		{
			obj->contains = NULL;
			for (objCheck = obj_content; objCheck != NULL; objCheck
					= objCheck_next)
			{
				objCheck_next = objCheck->next_content;
				if (delete)
				{
					obj_from_obj(objCheck);
					if (room)
						obj_to_room(objCheck, room);
					else if (carrier)
						obj_to_char(objCheck, carrier);
					else if (container)
						obj_to_obj(objCheck, container);
				}
				else
					objCheck->in_obj = NULL;
			}
		}
		extract_obj(obj);

		if (delete)
			continue;

		obj = create_object(pObj, pObj->level);

		if (room)
			obj_to_room(obj, room);
		else if (carrier)
		{
			if (IS_NPC( carrier ) && clan->cMob[CMOB_SHOPKEEP]
					&& clan->cMob[CMOB_SHOPKEEP] == carrier->pIndexData
					&& (room = carrier->in_room) != NULL)
			{
				extract_char(carrier, TRUE );
				reset_room(room);
			}
			else {
				obj_to_char(obj, carrier);
				if (loc == WEAR_NONE)
					;
				else if (loc < sizeof(wear_map)
				&& IS_SET(obj->wear_flags, wear_map[loc])) {
					equip_char(carrier, obj, loc);
				} else {
					sprintf(buf,"{R***NOTE*** A clan item({x%s{R) has changed and has been unequipped as it now occupies a different slot.{x\n\r",
						obj->short_descr);
					send_to_char(buf, carrier);
				}
			}
		}
		else if (container)
			obj_to_obj(obj, container);

		if (obj_content)
		{
			obj->contains = obj_content;
			for (objCheck = obj_content; objCheck != NULL; objCheck
					= objCheck->next_content)
				objCheck->in_obj = obj;
		}
	}
}

/*********************************** SAVING/LOADING ***********************************/
/* Order: clans --> rosters */

/* Load a clan */
void load_clan_data(FILE *fp)
{
	CLAN_DATA *clan = NULL;
	bool foundHash = FALSE;

	while (!feof(fp)) // Loop through each (line) section and load in data
	{
		char letter; // Section reference letter
		char *check;
		int i, j;

		letter = fread_letter(fp);

		switch (letter)
		{
		case '#':
			clan = new_clan_data(FALSE);
			clan->id = fread_number(fp);
			clan->pkill = fread_number(fp);
			clan->independent = fread_number(fp);
			clan->edit_time = fread_number(fp);
			foundHash = TRUE;
			break;
		case '$':
			return;
		case 'A':
			clan->qps = fread_number(fp);
			break;
		case 'C':
			free_string(clan->c_name);
			clan->c_name = fread_string(fp);
			break;
		case 'D':
			free_string(clan->description);
			clan->description = fread_string(fp);
			break;
		case 'E':
			clan->enemy = get_clan_by_id(fread_number(fp));
			if (clan->enemy) // Make sure the enemy of this clan's enemy is this clan (heh).
				clan->enemy->enemy = clan; // If two clans are warring against each other, this will work on the second clan.
			break;
		case 'F':
			clan->Penemy.id = fread_number(fp);
			break;
		case 'G':
			clan->exit_changed = fread_number(fp);
			break;
		case 'H':
			clan->entrance_vnum = fread_number(fp);
			break;
		case 'K':
			clan->pkilldata->kPoints = fread_number(fp);
			clan->pkilldata->kills = fread_number(fp);
			clan->pkilldata->deaths = fread_number(fp);
			clan->pkilldata->wins = fread_number(fp);
			clan->pkilldata->losses = fread_number(fp);
			break;
		case 'M':
			i = fread_number(fp);
			j = fread_number(fp);
			if (j > 0)
				clan->cMob[i] = get_mob_index(j);
			break;
		case 'N':
			free_string(clan->name);
			clan->name = str_dup(fread_word(fp));
			break;
		case 'P':
			clan->platinum = fread_number(fp);
			break;
		case 'R':
			i = fread_number(fp);
			free_string(clan->r_name[i]);
			clan->r_name[i] = fread_string(fp);
			break;
		case 'S':
			check = fread_word(fp);
			for (i = 0; clan_skill_table[i].cmd[0] != '\0'; i++)
			{
				if (!str_cmp(clan_skill_table[i].cmd, check))
				{
					if (clan->skills[0] >= 0)
						clan->skills[1] = i;
					else
						clan->skills[0] = i;
					break;
				}
			}
			if (clan_skill_table[i].cmd[0] == '\0')
			{
				printf_debug( "load_clan_data: clan skill %s not found",
						check);
			}
			break;
		case 'T':
			i = fread_number(fp);
			j = fread_number(fp);
			if (j > 0)
				clan->cRoom[i] = get_room_index(j);
			break;
		case 'W':
			i = fread_number(fp);
			free_string(clan->r_who[i]);
			clan->r_who[i] = str_dup(fread_word(fp));
			break;
		case 'V':
			clan->vnum[0] = fread_number(fp);
			clan->vnum[1] = fread_number(fp);
			break;
		default:
			printf_debug("load_clan_data: read invalid section %c", letter);
			fread_string_eol(fp);
			break;
		}
		if (!foundHash)
		{
			printf_system("load_clan_data: # not found");
			exit(1);
		}
	}
}

/* Load all clan data */
void load_all_clans(void)
{
	FILE *fpList;

	if ((fpList = fopen(
			is_main_server() ? CLAN_DATA_LIST : TEST_CLAN_DATA_LIST, "r"))
			== NULL)
	{
		perror(is_main_server() ? CLAN_DATA_LIST : TEST_CLAN_DATA_LIST);
		exit(1); // Stop running if clan list isn't found
	}

	while (!feof(fpList))
	{
		FILE *fp;

		char file[100], *word;

		word = fread_word(fpList);

		if (!str_cmp(word, "END")) // Check for end of file marker
			break;

		sprintf(file, "%s%s", is_main_server() ? CLAN_DATA_FOLDER
				: TEST_CLAN_DATA_FOLDER, word);

		if ((fp = fopen(file, "r")) == NULL) // Open clan file
		{
			perror(file);
			fclose(fpList);
			exit(1); // Stop running if clan file isn't found
		}

		load_clan_data(fp);
		fclose(fp);
	}
	fclose(fpList);
}

/* Save a clan */
void save_clan_data(CLAN_DATA *clan)
{
	FILE *fp;
	char file[100];
	int i;

	sprintf(file, "%s%d.dat", is_main_server() ? CLAN_DATA_FOLDER
			: TEST_CLAN_DATA_FOLDER, clan->id);

	if ((fp = fopen(file, "w")) == NULL) // Open clan file
	{
		printf_system(
				"save_clan_data: unable to open clan file %s for writing", file);
		return; // Return if clan file isn't writable
	}

	fprintf(fp, "# %d %d %d %ld\n", clan->id, clan->pkill, clan->independent,
			clan->edit_time);

	if (clan->enemy)
		fprintf(fp, "E %d\n", clan->enemy->id);
	else if (clan->Penemy.data)
		fprintf(fp, "F %d\n", clan->Penemy.data->id);

	fprintf(fp, "K %ld %d %d %d %d\n", clan->pkilldata->kPoints,
			clan->pkilldata->kills, clan->pkilldata->deaths,
			clan->pkilldata->wins, clan->pkilldata->losses);

	for (i = 0; i < CMOB_MAX; i++)
		if (clan->cMob[i] != NULL)
			fprintf(fp, "M %d %d\n", i, clan->cMob[i]->vnum);

	for (i = 0; i < CROOM_MAX; i++)
		if (clan->cRoom[i] != NULL)
			fprintf(fp, "T %d %d\n", i, clan->cRoom[i]->vnum);

	fprintf(fp, "N %s\n", clan->name);
	fprintf(fp, "C %s~\n", clan->c_name);

	for (i = 0; i < TOTAL_CLAN_RANK; i++)
	{
		fprintf(fp, "R %d %s~\n", i, clan->r_name[i]);
		fprintf(fp, "W %d %s\n", i, clan->r_who[i]);
	}

	fprintf(fp, "D %s~\n", clan->description);
	fprintf(fp, "P %ld\n", clan->platinum);
	fprintf(fp, "A %ld\n", clan->qps);
	fprintf(fp, "G %ld\n", clan->exit_changed);
	fprintf(fp, "V %d %d\n", clan->vnum[0], clan->vnum[1]);
	fprintf(fp, "H %d\n", clan->entrance_vnum);

	// Write clan skills
	for (i = 0; i < 2; i++)
		if (clan->skills[i] >= 0)
			fprintf(fp, "S %s\n", clan_skill_table[clan->skills[i]].cmd);

	fprintf(fp, "$\n"); // Write end of file marker
	fclose(fp);
}

/* Save all clan data */
void save_all_clans(void)
{
	CLAN_DATA *clan;
	FILE *fpList;
	int maxID, capID;

	if ((fpList = fopen(TEMP_FILE, "w")) == NULL) // Use temp file so if anything goes wrong, the old clan list still exists
	{
		printf_system("save_all_clans: unable to open temp file for writing clan list");
		return;
	}

	for (capID = -1;;) // More complicated saving technique to order the clan list
	{
		for (maxID = -1, clan = clan_list; clan != NULL; clan = clan->next)
		{
			if (capID >= 0 && clan->id >= capID)
				continue;

			if (clan->id > maxID)
				maxID = clan->id;
		}

		capID = maxID;
		clan = get_clan_by_id(maxID);
		fprintf(fpList, "%d.dat\n", // Write entry to clan list file
				clan->id);
		save_clan_data(clan);

		if (capID == 0)
			break;
	}

	fprintf(fpList, "END\n");
	fclose(fpList);
	rename(TEMP_FILE, is_main_server() ? CLAN_DATA_LIST : TEST_CLAN_DATA_LIST );
}

/* Load a roster and assign clan data */
void load_roster_data(FILE *fp)
{
	ROSTER_DATA *roster = NULL;
	char letter;
	int value;
	bool foundHash = FALSE;

	while (!feof(fp))
	{
		letter = fread_letter(fp);

		switch (letter)
		{
		case '#':
			roster = new_roster_data(FALSE);
			roster->id = fread_number(fp);
			foundHash = TRUE;
			break;
		case '$':
			return;
		case 'B':
			roster->bounty = fread_number(fp);
			break;
		case 'C':
			value = fread_number(fp);
			roster->clan = get_clan_by_id(value);
			if (roster->clan)
				(roster->clan->members)++;
			else
			{
				roster->clan = get_clan_by_id(CLAN_NONE);
				printf_debug(
						"load_roster_data: invalid clan id: %d for roster %s",
						value, roster->name ? roster->name : "no name");
				(roster->clan->members)++;
			}
			break;
		case 'D':
			roster->dPoints = fread_number(fp);
			break;
		case 'F':
			roster->flags = fread_flag(fp);
			break;
		case 'L':
			roster->last_login = fread_number(fp);
			break;
		case 'N':
			free_string(roster->name);
			roster->name = str_dup(fread_word(fp));
			break;
		case 'O':
			break;
		case 'P':
			roster->penalty_time = fread_number(fp);
			break;
		case 'Q':
			roster->petition = get_clan_by_id(fread_number(fp));
			break;
		case 'S':
			roster->pkilldata = new_pkill_data();
			roster->pkilldata->kPoints = fread_number(fp);
			roster->pkilldata->kills = fread_number(fp);
			roster->pkilldata->deaths = fread_number(fp);
			roster->pkilldata->wins = fread_number(fp);
			roster->pkilldata->losses = fread_number(fp);
			break;
		case 'T':
			roster->trust = fread_number(fp);
			break;
		default:
			printf_debug("load_roster_data: read invalid section %c", letter);
			fread_string_eol(fp);
			break;
		}
		if (!foundHash)
		{
			printf_debug( "load_roster_data: # not found");
			break;
		}
	}
}

/* Load all rosters */
void load_all_rosters(void)
{
	FILE *fp, *fpList;
	char file[100];

	if ((fpList = fopen(is_main_server() ? ROSTER_DATA_LIST
			: TEST_ROSTER_DATA_LIST, "r")) == NULL)
	{
		perror(is_main_server() ? ROSTER_DATA_LIST : TEST_ROSTER_DATA_LIST);
		exit(1);
	}

	while (!feof(fpList))
	{
		char *word;

		word = fread_word(fpList);

		if (!str_cmp(word, "END"))
			break;

		sprintf(file, "%s%s", is_main_server() ? ROSTER_DATA_FOLDER
				: TEST_ROSTER_DATA_FOLDER, word);

		if ((fp = fopen(file, "r")) == NULL)
		{
			perror(file);
			continue;
		}
		load_roster_data(fp);
		fclose(fp);
	}
	fclose(fpList);
	update_all_rosters();
}

/* Save a roster */
void save_roster_data(ROSTER_DATA *roster)
{
	FILE *fp;
	char file[100];

	sprintf(file, "%s%d.rost", is_main_server() ? ROSTER_DATA_FOLDER
			: TEST_ROSTER_DATA_FOLDER, roster->id);

	if ((fp = fopen(file, "w")) == NULL)
	{
		printf_system("save_roster_data: unable to open file for writing roster");
		return;
	}

	fprintf(fp, "# %d\n", roster->id);

	fprintf(fp, "N %s\n", roster->name);

	fprintf(fp, "P %ld\n", roster->penalty_time);

	fprintf(fp, "S %ld %d %d %d %d\n", roster->pkilldata->kPoints,
			roster->pkilldata->kills, roster->pkilldata->deaths,
			roster->pkilldata->wins, roster->pkilldata->losses);

	fprintf(fp, "F %s\n", print_flags(roster->flags));

	if (!IS_SET( roster->flags, ROST_DELETED ))
	{
		fprintf(fp, "C %d\n", roster->clan ? roster->clan->id : CLAN_NONE );

		if (roster->petition)
			fprintf(fp, "Q %d\n", roster->petition->id);

		fprintf(fp, "L %ld\n", roster->last_login);

		fprintf(fp, "D %ld\n", roster->dPoints);

		fprintf(fp, "B %ld\n", roster->bounty);

		fprintf(fp, "T %d\n", roster->trust);
	}

	fprintf(fp, "$\n");
	fclose(fp);
}

/* Save all roster data */
void save_all_rosters(void)
{
	FILE *fpList;
	ROSTER_DATA *roster;

	if ((fpList = fopen(TEMP_FILE, "w")) == NULL) // Open temp data file so previously saved roster data isn't wiped out if theres a problem
	{
		printf_system("save_all_rosters: unable to open temp data file for writing roster list");
		return;
	}

	for (roster = roster_list; roster != NULL; roster = roster->next)
	{
		if (IS_SET( roster->flags, ROST_DELETED ) && roster->penalty_time
				<= current_time)
			continue;
		fprintf(fpList, "%d.rost\n", roster->id);
		save_roster_data(roster);
	}

	fprintf(fpList, "END\n");
	fclose(fpList);
	rename(TEMP_FILE, is_main_server() ? ROSTER_DATA_LIST
			: TEST_ROSTER_DATA_LIST );
}

/* Load the spamkill list */
void load_pkhist_data(void)
{
	FILE *fp;
	int maxVal;

	if ((fp = fopen(PKHIST_DATA_FILE, "r")) == NULL) {
		printf_system("Error opening PKHIST_DATA_FILE");
		return;
	}
	if ((maxVal = fread_number(fp)) <= 0)
	{
		fclose(fp);
		return;
	}

	if ((pkhist_table = malloc(sizeof(PKHIST_DATA) * maxVal)) == NULL)
	{
		printf_system("Error allocating memory for pkhist_table");
		exit(1);
	}

	while (!feof(fp))
	{
		char letter;

		letter = fread_letter(fp);

		if (letter == '$')
			break;
		else if (letter == '#')
		{
			max_pkhist_entries++;
			pkhist_table[max_pkhist_entries - 1].victim = get_roster_by_id(
					fread_number(fp));
			pkhist_table[max_pkhist_entries - 1].killer = get_roster_by_id(
					fread_number(fp));
			pkhist_table[max_pkhist_entries - 1].vClan = get_clan_by_id(
					fread_number(fp));
			pkhist_table[max_pkhist_entries - 1].kClan = get_clan_by_id(
					fread_number(fp));
			pkhist_table[max_pkhist_entries - 1].time = fread_number(fp);
			pkhist_table[max_pkhist_entries - 1].vLevel = fread_number(fp);
			pkhist_table[max_pkhist_entries - 1].kLevel = fread_number(fp);
			pkhist_table[max_pkhist_entries - 1].flags = fread_flag(fp);

			if (pkhist_table[max_pkhist_entries - 1].killer == NULL
					|| pkhist_table[max_pkhist_entries - 1].victim == NULL)
				max_pkhist_entries--;
		}
		else
		{
			printf_debug("load_pkhist_data: read invalid section %c", letter);
			fread_string_eol(fp);
		}
	}
	fclose(fp);

	if (max_pkhist_entries < maxVal)
	{
		PKHIST_DATA *pkhist;

		if (max_pkhist_entries == 0)
		{
			free(pkhist_table);
			pkhist_table = NULL;
			return;
		}
		else if ((pkhist = realloc(pkhist_table, sizeof(PKHIST_DATA)
				* max_pkhist_entries)) == NULL)
		{
			printf_system("load_pkhist_data: unable to reallocate memory after one or more null entries.");
			return;
		}
		pkhist_table = pkhist;
	}
}

void save_pkhist_data(void)
{
	FILE *fp;
	int i;
	int max_hist = UMIN(max_pkhist_entries, 100);

	if (!pkhist_table || max_pkhist_entries <= 0)
		return;
	if ((fp = fopen(PKHIST_DATA_FILE, "w")) == NULL)
	{
		printf_system("save_pkhist_data: unable to open temp data file for writing roster list");
		return;
	}

	fprintf(fp, "%d\n", max_hist);

	for (i = UMAX(max_pkhist_entries-max_hist, 0); i < max_pkhist_entries; i++)
		fprintf(fp, "# %d %d %d %d %ld %d %d %s\n", pkhist_table[i].victim->id,
				pkhist_table[i].killer->id,
				pkhist_table[i].vClan ? pkhist_table[i].vClan->id : -1,
				pkhist_table[i].kClan ? pkhist_table[i].kClan->id : -1,
				pkhist_table[i].time, pkhist_table[i].vLevel,
				pkhist_table[i].kLevel, print_flags(pkhist_table[i].flags));

	fprintf(fp, "$\n");
	fclose(fp);
}

/* Fix clan data, upgrade data, etc */
void fix_clan_boot(void)
{
	CLAN_DATA *clan;
	char buf[MAX_STRING_LENGTH];
	int i, j;

	// Fix petitioned enemies
	for (clan = clan_list; clan != NULL; clan = clan->next)
		if (clan->Penemy.id)
			clan->Penemy.data = get_clan_by_id(clan->Penemy.id);

	for (i = 0; spell_prices[i].name[0] != '\0'; i++)
	{
		j = -1;
		strcpy(buf, spell_prices[i].name);
		while (buf[++j] != '\0')
			if (buf[j] == '_')
				buf[j] = ' ';
		if ((spell_prices[i].bit = skill_lookup(buf)) < 0)
		{
			printf_debug( "Unknown spell for spell_prices: %s.",
					spell_prices[i].name);
		}
	}
	for (i = 0; damtype_prices[i].name[0] != '\0'; i++)
	{
		if ((damtype_prices[i].bit = attack_lookup(damtype_prices[i].name))
				<= 0)
		{
			printf_debug( "Possible unknown damtype for damtype_prices: %s.",
					damtype_prices[i].name);
		}
	}
}

/********************************** UPGRADE FUNCTIONS *********************************/

/* Find out whether clan has requirements for upgrade */
bool upgrade_req_pass(CHAR_DATA *ch, COST_DATA *table, bool showPass)
{
	BUFFER *output = new_buf();
	CLAN_DATA *clan;
	char buf[MAX_STRING_LENGTH], level[100];
	int i;
	bool pass, roompass = TRUE, mobpass = TRUE, found;

	EDIT_CLAN( ch, clan );

	sprintf(buf, "{C%s\n\r", capitalize(table->name));
	add_buf(output, buf);

	add_buf(output, "{WRoom requirements:\n\r");

	for (found = FALSE, i = 0; i < CROOM_MAX; i++)
	{
		if (!table->cRoom[i])
			continue;
		pass = FALSE;
		found = TRUE;

		if (i == CROOM_GSHRINE && table->cRoom[CROOM_GSHRINE]
				== table->cRoom[CROOM_ESHRINE])
		{
			if (clan->cRoom[CROOM_GSHRINE] != NULL
					|| clan->cRoom[CROOM_ESHRINE] != NULL)
			{
				if (showPass)
				{
					sprintf(buf, " {w%-15s{B: {Gyes\n\r", "Either shrine");
					add_buf(output, buf);
				}
			}
			else
			{
				sprintf(buf, " {w%-15s{B: {Rno\n\r", "Either shrine");
				add_buf(output, buf);
				roompass = FALSE;
			}
			i++;
			continue;
		}
		else if (table->cRoom[i] && clan->cRoom[i] != NULL)
			pass = TRUE;

		if (pass && showPass)
		{
			sprintf(buf, " {w%-15s{B: {Gyes\n\r", capitalize(
					croom_table[i].name));
			add_buf(output, buf);
		}
		else if (!pass)
		{
			roompass = FALSE;
			sprintf(buf, " {w%-15s{B: {Rno\n\r",
					capitalize(croom_table[i].name));
			add_buf(output, buf);
		}
	}

	if (!showPass && roompass && found)
		add_buf(output, "{w All room requirements have passed.\n\r");
	else if (!found)
		add_buf(output, "{w There are no room requirements.\n\r");

	if (IS_SET( table->flags, UPGD_XCOST ))
		add_buf(output, "{w There might be additional requirements.\n\r");

	add_buf(output, "{WMobile requirements:\n\r");

	for (found = FALSE, i = 0; i < CMOB_MAX; i++)
	{
		if (table->cMob[i] <= 0)
			continue;
		pass = FALSE;
		found = TRUE;

		if (i == CMOB_GPRIEST && table->cMob[CMOB_GPRIEST]
				== table->cMob[CMOB_EPRIEST])
		{
			if ((clan->cMob[CMOB_GPRIEST] && clan->cMob[CMOB_GPRIEST]->level
					>= table->cMob[CMOB_GPRIEST]) || (clan->cMob[CMOB_EPRIEST]
					&& clan->cMob[CMOB_EPRIEST]->level
							>= table->cMob[CMOB_GPRIEST]))
			{
				if (showPass)
				{
					sprintf(buf, " {wLevel {C%3d{b:{w %-15s{B- {Gyes\n\r",
							table->cMob[CMOB_GPRIEST], "Either priest");
					add_buf(output, buf);
				}

			}
			else
			{
				level[0] = '\0';

				if (clan->cMob[CMOB_GPRIEST])
					sprintf(level, "{W({wG{W:{w %d{B) ",
							clan->cMob[CMOB_GPRIEST]->level);

				if (clan->cMob[CMOB_EPRIEST])
				{
					sprintf(buf, "{W({wE{W:{w %d{B)",
							clan->cMob[CMOB_EPRIEST]->level);
					strcat(level, buf);
				}

				sprintf(buf, " {wLevel {C%3d{b:{w %-15s{B- {Rno%s\n\r",
						table->cMob[CMOB_GPRIEST], "Either priest", level);
				add_buf(output, buf);
			}
			i++;
			continue;
		}
		else if (clan->cMob[i] && clan->cMob[i]->level >= table->cMob[i])
			pass = TRUE;

		if (pass && showPass)
		{
			sprintf(buf, " {wLevel {C%3d{B:{w %-15s{B- {Gyes\n\r",
					table->cMob[i], capitalize(cmob_table[i].name));
			add_buf(output, buf);
		}
		else if (!pass)
		{
			mobpass = FALSE;

			if (clan->cMob[i])
				sprintf(level, "{W({wCurr{W: {w%d{W)", clan->cMob[i]->level);
			else
				strcpy(level, "");

			sprintf(buf, " {wLevel {C%3d{B:{w %-15s{B- {Rno %s\n\r",
					table->cMob[i], capitalize(cmob_table[i].name), level);
			add_buf(output, buf);
		}
	}

	if (!showPass && mobpass && found)
		add_buf(output, "{w All mob requirements have passed.\n\r");
	else if (!found)
		add_buf(output, "{w There are no mob requirements.\n\r");

	if (IS_SET( table->flags, UPGD_XCOST ))
		add_buf(output, "{w There might be additional requirements.\n\r");

	add_buf(output, "{WCost:\n\r");

	if (IS_SET( table->flags, UPGD_XCOST ))
		add_buf(output, "{wPrice is variable.\n\r");
	else
	{
		if (table->platinum > 0)
		{
			sprintf(buf, " {wPlatinum:{Y %7d {B- %s\n\r", table->platinum,
					clan->platinum >= table->platinum ? "{Gyes" : "{Rno");
			add_buf(output, buf);
		}

		if (table->qps > 0)
		{
			sprintf(buf, " {wQuest:  {Y %7d {B- %s\n\r", table->qps, clan->qps
					>= table->qps ? "{Gyes" : "{Rno");
			add_buf(output, buf);
		}

		if (table->platinum <= 0 && table->qps <= 0)
			add_buf(output, " {GFree.\n\r");
	}

	add_buf(output,
			"\n\r{GYes{w/{Rno{w shows whether requirement is fulfilled or not.{x\n\r");
	if (IS_SET( table->flags, UPGD_XCOST ))
		add_buf(
				output,
				"Additional arguments are required - append {Rlist{x to the previous argument to see a list.\n\r");
	if (!showPass && (!roompass || !mobpass))
	{
		add_buf(output,
				"{ROne or more of the requirements still need fulfilling.{x\n\r");
		page_to_char(output->string, ch);
	}
	else if (showPass)
		page_to_char(output->string, ch);

	free_buf(output);
	return (roompass & mobpass);
}

/* Process upgrade requests - return TRUE if possible */
bool upgrade_process(CHAR_DATA *ch, char *argument, COST_DATA *table, bool buy)
{
	CLAN_DATA *clan;
	MOB_INDEX_DATA *pMob = NULL;
	OBJ_INDEX_DATA *pObj = NULL;
	ROSTER_DATA *roster;
	XCOST_DATA *xcost = NULL;
	char item[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
	long plat, qps;
	int quantity = 1, current, max, cMobIndex = CMOB_MAX;

	if (argument[0] == '\0')
	{
		send_to_char("Please specify an item.\n\r", ch);
		return FALSE;
	}

	argument = one_argument(argument, item);
	EDIT_CLAN( ch, clan );
	GET_ROSTER( ch, roster );

	if (table == obj_upgd_prices)
		EDIT_OBJ( ch, pObj );
	else if (table == mob_upgd_prices)
	{
		EDIT_MOB( ch, pMob );
		for (cMobIndex = 0; cMobIndex < CMOB_MAX; cMobIndex++)
			if (pMob == clan->cMob[cMobIndex])
				break;
	}

	for (; table->name[0] != '\0'; table++)
	{
		if (pObj && table->specific[0] != -1 && pObj->item_type
				!= table->specific[0])
			continue;

		if (pMob && table->specific[0] != -1 && cMobIndex < CMOB_MAX
				&& cMobIndex != table->specific[0])
			continue;

		if (!str_prefix(item, table->name))
			break;
	}
	if (table->name[0] == '\0')
	{
		send_to_char("Item not found. Use the list option to see a list.\n\r",
				ch);
		return FALSE;
	}

	if (IS_SET( table->flags, UPGD_XCOST ))
	{
		argument = one_argument(argument, item);

		if (!str_cmp(item, "list"))
		{
			xupgrade_list_table(ch, table);
			return FALSE;
		}
	}

	if (!upgrade_req_pass(ch, table, !buy))
		return FALSE;

	if (!buy)
		return TRUE;

	if (IS_SET( table->flags, UPGD_XCOST ))
	{
		if (item[0] == '\0')
		{
			send_to_char(
					"This item has different types. Please specify the type.\n\r",
					ch);
			send_to_char(
					"Use {Glist{x as an argument (e.g. \"{Gobj buy add_spell list{x\") to view the types.\n\r",
					ch);
			return FALSE;
		}
		for (xcost = table->xcost; xcost->name[0] != '\0'; xcost++)
			if (!str_prefix(item, xcost->name))
				break;

		if (xcost->name[0] == '\0')
		{
			send_to_char(
					"What type of this upgrade would you like? - Use \"{Gbuy req <item> list{x\" for a list.\n\r",
					ch);
			return FALSE;
		}

		if (table->xreq1 != CMOB_MAX)
		{
			if (clan->cMob[table->xreq1] == NULL
					|| clan->cMob[table->xreq1]->level < xcost->req1)
			{
				sprintf(
						buf,
						"You need a {R%s{x at level {R%d{x for this specific type.\n\r",
						cmob_table[table->xreq1].name, xcost->req1);
				send_to_char(buf, ch);
				return FALSE;
			}
		}
		if (table->xreq2 != CMOB_MAX)
		{
			if (clan->cMob[table->xreq2] == NULL
					|| clan->cMob[table->xreq2]->level < xcost->req2)
			{
				sprintf(
						buf,
						"You need a {R%s{x at level {R%d{x for this specific type.\n\r",
						cmob_table[table->xreq2].name, xcost->req2);
				send_to_char(buf, ch);
				return FALSE;
			}
		}
		if (clan->platinum < xcost->platinum)
		{
			sprintf(
					buf,
					"The clan only has {R%ld{x platinum and the upgrade requires at least {R%d{x platinum.\n\r",
					clan->platinum, xcost->platinum);
			send_to_char(buf, ch);
			return FALSE;
		}
		if (clan->qps < xcost->qps)
		{
			sprintf(
					buf,
					"The clan only has {R%ld{x Quest points and the upgrade requires at least {R%d{x Quest points.\n\r",
					clan->qps, xcost->qps);
			send_to_char(buf, ch);
			return FALSE;
		}
	}

	if (is_number(argument))
	{
		quantity = atoi(argument);

		if (IS_SET( table->flags, UPGD_NO_BULK ))
		{
			send_to_char(
					"Quantity argument ignored - this item doesn't come in bulk.\n\r",
					ch);
			quantity = 1;
		}

		if (quantity <= 0)
		{
			send_to_char("The quantity must be greater than 0.\n\r", ch);
			return FALSE;
		}

	}
	else if (!IS_SET( table->flags, UPGD_NO_BULK ))
	{
		send_to_char("Please specify a quantity.\n\r", ch);
		return FALSE;
	}

	current = ((*table->curr_value)(ch, table));

	if (table->max == MAX_SPECIAL)
	{
		max = ((*table->max_func)(ch, table));
		if (current >= max)
		{
			send_to_char("The current quantity is already at the maximum.\n\r",
					ch);
			return FALSE;
		}
		if ((current + (quantity * (table->quantity))) > max)
		{
			send_to_char(
					"You have specified an amount that would exceed the maximum.\n\r",
					ch);
			return FALSE;
		}
	}
	else if (table->max != MAX_NONE)
	{
		if (current >= table->max)
		{
			send_to_char("The current quantity is already at the maximum.\n\r",
					ch);
			return FALSE;
		}
		if ((current + (quantity * (table->quantity))) > table->max)
		{
			send_to_char(
					"You have specified an amount that would exceed the maximum.\n\r",
					ch);
			return FALSE;
		}
	}

	plat = quantity * (table->platinum);
	if (IS_SET( table->flags, UPGD_XCOST ))
		plat += quantity * (xcost->platinum);

	qps = quantity * (table->qps);
	if (IS_SET( table->flags, UPGD_XCOST ))
		qps += quantity * (xcost->qps);

	if (qps > 0 && plat > 0 && clan->platinum < plat && clan->qps < qps)
	{
		sprintf(
				buf,
				"The clan cannot cover the price of {R%ld{x platinum and {R%ld{x Quest points\n\r",
				plat, qps);
		send_to_char(buf, ch);
		return FALSE;
	}
	else if (qps > 0 && clan->qps < qps)
	{
		sprintf(buf,
				"The clan cannot cover the price of {R%ld{x Quest points\n\r",
				qps);
		send_to_char(buf, ch);
		return FALSE;
	}
	else if (plat > 0 && clan->platinum < plat)
	{
		sprintf(buf,
				"The clan cannot cover the price of {R%ld{x platinum.\n\r",
				plat);
		send_to_char(buf, ch);
		return FALSE;
	}

	clan->u_quantity = quantity;
	clan->upgrade = table;
	clan->u_rost = roster;
	if (IS_SET( table->flags, UPGD_XCOST ))
		clan->xupgrade = xcost;
	else
		clan->xupgrade = NULL; // Prevent any mixups
	upgd_handler(ch, "");
	return TRUE;
}

/* If a clan has a pending upgrade, input processing is switched from cedit handler to here */
void upgd_handler(CHAR_DATA *ch, char *argument)
{
	CLAN_DATA *clan;
	char buf[MAX_STRING_LENGTH], arg[MAX_INPUT_LENGTH];
	int total, plat = 0, qps = 0;

	EDIT_CLAN( ch, clan );

	argument = one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		BUFFER *output = new_buf();

		add_buf(output, "You have a clan upgrade pending:\n\r");

		sprintf(buf, "{WType:{w %s\n\r", capitalize(clan->upgrade->name));
		add_buf(output, buf);

		if (clan->u_quantity > 1)
		{
			total = (clan->u_quantity) * (clan->upgrade->quantity);
			sprintf(buf, "{WQuantity: {w%2d {Wx {w%4d {W= {G%d\n\r",
					clan->u_quantity, clan->upgrade->quantity, total);
			add_buf(output, buf);

			if (clan->upgrade->max == MAX_NONE)
				sprintf(buf, " {WTotal quantity post-purchase: {g%d\n\r", total
						+ ((*clan->upgrade->curr_value)(ch, clan->upgrade)));
			else
				sprintf(
						buf,
						" {WTotal quantity post-purchase: {g%d {W({wMax: %d{W)\n\r",
						total
								+ ((*clan->upgrade->curr_value)(ch,
										clan->upgrade)), clan->upgrade->max
								== MAX_SPECIAL ? ((*clan->upgrade->max_func)(
								ch, clan->upgrade)) : clan->upgrade->max);

			add_buf(output, buf);

			if (clan->upgrade->qps || (clan->xupgrade && clan->xupgrade->qps))
			{
				total = (clan->u_quantity) * (clan->upgrade->qps);

				if (clan->xupgrade)
					total += (clan->u_quantity) * (clan->xupgrade->qps);

				sprintf(buf, "{YQ{yuest{W cost: {w%2d {Wx {w%4d {W= {Y%d\n\r",
						clan->u_quantity, clan->xupgrade ? clan->upgrade->qps
								+ clan->xupgrade->qps : clan->upgrade->qps,
						total);
				add_buf(output, buf);

				sprintf(buf,
						" {WClan {YQ{yuest{W amount post-purchase:{R %ld\n\r",
						(clan->qps) - total);
				add_buf(output, buf);
			}
			if (clan->upgrade->platinum || (clan->xupgrade
					&& clan->xupgrade->platinum))
			{
				total = (clan->u_quantity) * (clan->upgrade->platinum);

				if (clan->xupgrade)
					total += (clan->u_quantity) * (clan->xupgrade->platinum);

				sprintf(buf,
						"{CP{clatinum {Wcost: {w%2d {Wx {w%4d {W= {Y%d\n\r",
						clan->u_quantity,
						clan->xupgrade ? clan->upgrade->platinum
								+ clan->xupgrade->platinum
								: clan->upgrade->platinum, total);
				add_buf(output, buf);

				sprintf(
						buf,
						" {WClan {CP{clatinum{W amount post-purchase:{R %ld\n\r",
						(clan->platinum) - total);
				add_buf(output, buf);
			}
		}
		else
		{
			sprintf(buf, "{WQuantity: {G%d\n\r", clan->upgrade->quantity);
			add_buf(output, buf);

			if (clan->upgrade->max == MAX_NONE)
				sprintf(buf, " {WTotal quantity post-purchase: {g%d\n\r",
						clan->upgrade->quantity
								+ ((*clan->upgrade->curr_value)(ch,
										clan->upgrade)));
			else
				sprintf(
						buf,
						" {WTotal quantity post-purchase: {g%d {W({wMax: %d{W)\n\r",
						clan->upgrade->quantity
								+ ((*clan->upgrade->curr_value)(ch,
										clan->upgrade)), clan->upgrade->max
								== MAX_SPECIAL ? ((*clan->upgrade->max_func)(
								ch, clan->upgrade)) : clan->upgrade->max);

			if (clan->upgrade->qps || (clan->xupgrade && clan->xupgrade->qps))
			{
				sprintf(buf, "{YQ{yuest{W cost: {Y%d\n\r",
						clan->xupgrade ? clan->upgrade->qps
								+ clan->xupgrade->qps : clan->upgrade->qps);
				add_buf(output, buf);

				sprintf(buf,
						" {WClan {YQ{yuest{W amount post-purchase:{R %ld\n\r",
						(clan->qps) - (clan->xupgrade ? clan->upgrade->qps
								+ clan->xupgrade->qps : clan->upgrade->qps));
				add_buf(output, buf);
			}
			if (clan->upgrade->platinum || (clan->xupgrade
					&& clan->xupgrade->platinum))
			{
				sprintf(buf, "{CP{clatinum {Wcost: {Y%d\n\r",
						clan->xupgrade ? clan->upgrade->platinum
								+ clan->xupgrade->platinum
								: clan->upgrade->platinum);
				add_buf(output, buf);

				sprintf(
						buf,
						" {WClan {CP{clatinum{W amount post-purchase:{R %ld\n\r",
						(clan->platinum)
								- (clan->xupgrade ? clan->upgrade->platinum
										+ clan->xupgrade->platinum
										: clan->upgrade->platinum));
				add_buf(output, buf);
			}
		}
		add_buf(output, "{xType {Gconfirm{x to accept, {Rclear{x to clear.\n\r");
		send_to_char(output->string, ch);
		free_buf(output);
	}
	else if (!str_cmp(arg, "confirm"))
	{
		int state = ((*clan->upgrade->func)(ch, argument, clan->upgrade,
				clan->xupgrade));

		if (state == UPGRADE_SUCCESS)
		{
			send_to_char("Upgrade successful.\n\r", ch);

			if (clan->upgrade->qps || (clan->xupgrade && clan->xupgrade->qps))
			{
				qps = (clan->u_quantity) * (clan->upgrade->qps);

				if (clan->xupgrade)
					qps += (clan->u_quantity) * (clan->xupgrade->qps);

				clan->qps -= qps;
				sprintf(
						buf,
						"{R%d {YQ{yuest{x points have been deducted from your clan accounts.\n\r",
						qps);
				send_to_char(buf, ch);

				sprintf(buf,
						"{YQ{yuest{x points remaining in accounts: {Y%ld\n\r",
						clan->qps);
				send_to_char(buf, ch);
			}

			if (clan->upgrade->platinum || (clan->xupgrade
					&& clan->xupgrade->platinum))
			{
				plat = (clan->u_quantity) * (clan->upgrade->platinum);

				if (clan->xupgrade)
					plat += (clan->u_quantity) * (clan->xupgrade->platinum);

				clan->platinum -= plat;
				sprintf(
						buf,
						"{R%d {CP{clatinum{x have been deducted from your clan accounts.\n\r",
						plat);
				send_to_char(buf, ch);

				sprintf(buf, "{CP{clatinum{x remaining in accounts: {Y%ld\n\r",
						clan->platinum);
				send_to_char(buf, ch);
			}

			sprintf(
					buf,
					"{w%d {Wx {w%s{W({wblock quantity: %d{W){x, {YQ{yuest{x cost: {Y%d{x, {CP{clatinum{x cost: {Y%d",
					clan->u_quantity, clan->upgrade->name,
					clan->upgrade->quantity, qps, plat);
			append_clan_log(clan, ENTRY_UPGRADE, ch->name, buf);

			clan->upgrade = NULL;
			clan->xupgrade = NULL;
			clan->u_quantity = 0;
			save_area(get_clan_area());
			save_clan_data(clan);
			int i;
			for (i = clan->vnum[0]; i <= clan->vnum[1]; i++) {
				reset_clan_obj(get_obj_index(i), FALSE);
				reset_clan_mob(get_mob_index(i), FALSE);
			}
		}
		else if (state == UPGRADE_REQ_ARG)
			send_to_char(
					"To confirm the upgrade, type {Rconfirm <required arguments>{x\n\r",
					ch);
		else
		{
			send_to_char(
					"Clan upgrade failed. Please contact an immortal.\n\r", ch);
			sprintf(buf, "%s clan upgrade failed for clan %s, quantity %d.",
					clan->upgrade->name, clan->name, clan->u_quantity);
			printf_debug(buf);
			wiznet(buf, ch, NULL, WIZ_DEBUG, 0, 0);

			clan->upgrade = NULL;
			clan->xupgrade = NULL;
			clan->u_rost = NULL;
			clan->u_quantity = 0;

			save_clan_data(clan);
			save_area(get_clan_area());
		}
	}
	else if (!str_cmp(arg, "clear"))
	{
		clan->upgrade = NULL;
		clan->xupgrade = NULL;
		clan->u_rost = NULL;
		clan->u_quantity = 0;
		send_to_char("Pending clan upgrade cleared.\n\r", ch);
	}
	else
		upgd_handler(ch, "");
}

void upgrade_list_table(CHAR_DATA *ch, COST_DATA *table)
{
	BUFFER *output = new_buf();
	CLAN_DATA *clan;
	MOB_INDEX_DATA *pMob = NULL;
	OBJ_INDEX_DATA *pObj = NULL;
	char buf[MAX_STRING_LENGTH];
	int cval, mval, cMobIndex = CMOB_MAX;

	EDIT_CLAN( ch, clan );

	add_buf(output,
			"{WName             Qnty    Plat    QPs    Curr/Max    Description\n\r");

	if (table == obj_upgd_prices)
		EDIT_OBJ( ch, pObj );
	else if (table == mob_upgd_prices)
	{
		EDIT_MOB( ch, pMob );
		for (cMobIndex = 0; cMobIndex < CMOB_MAX; cMobIndex++)
			if (pMob == clan->cMob[cMobIndex])
				break;
	}

	for (; table->name[0] != '\0'; table++)
	{
		if (pObj && table->specific[0] != -1 && pObj->item_type
				!= table->specific[0])
			continue;

		if (pMob && table->specific[0] != -1 && cMobIndex < CMOB_MAX
				&& cMobIndex != table->specific[0])
			continue;

		if (table->max == MAX_SPECIAL)
			mval = ((*table->max_func)(ch, table));
		else
			mval = table->max;

		cval = ((*table->curr_value)(ch, table));

		sprintf(buf, "{w%-15s  {R%4d  {w%6d  {w%6d  {c%5d/{C%-5d  {w%s\n\r",
				capitalize(table->name), table->quantity, table->platinum,
				table->qps, cval, mval, table->descr);
		add_buf(output, buf);
	}

	add_buf(output,
			"\n\r{WUse the {Greq <item>{W option to view full requirements.{x\n\r");

	page_to_char(output->string, ch);
	free_buf(output);
}

void xupgrade_list_table(CHAR_DATA *ch, COST_DATA *table)
{
	BUFFER *output = new_buf();
	CLAN_DATA *clan;
	XCOST_DATA *xcost;
	char buf[MAX_STRING_LENGTH], flags[10];
	int req1 = 0, req2 = 0;

	EDIT_CLAN( ch, clan );

	sprintf(buf, "{WName             Type  Flags   Plat   Qps   %-11s  ",
			table->xreq1 == CMOB_MAX ? "n/a" : capitalize(
					cmob_table[table->xreq1].name));
	add_buf(output, buf);

	sprintf(buf, "%-11s\n\r", table->xreq2 == CMOB_MAX ? "n/a" : capitalize(
			cmob_table[table->xreq2].name));
	add_buf(output, buf);

	if (table->xreq1 != CMOB_MAX && clan->cMob[table->xreq1] != NULL)
		req1 = clan->cMob[table->xreq1]->level;

	if (table->xreq2 != CMOB_MAX && clan->cMob[table->xreq2] != NULL)
		req2 = clan->cMob[table->xreq2]->level;

	for (xcost = table->xcost; xcost->name[0] != '\0'; xcost++)
	{
		flags[0] = '\0';
		if (IS_SET( xcost->flags, XUPGD_IMM_SPECIAL ))
			strcat(flags, "*");
		else
			strcat(flags, "-");
		sprintf(
				buf,
				"{w%-15s  {w%-4s  {w%5s  {y%5d  {Y%5d  {C%-3d {B- [{%c%3d{B]  {C%-3d {B- [{%c%3d{B]\n\r",
				capitalize(xcost->name),
				table->xcost == spell_prices ? (xcost->type == SPELL_DEFENSIVE ? "def"
						: xcost->type == SPELL_OFFENSIVE ? "off" : "misc")
						: "-", flags, xcost->platinum, xcost->qps, xcost->req1,
				xcost->req1 < req1 ? 'G' : 'R', req1, xcost->req2, xcost->req2
						< req2 ? 'G' : 'R', req2);
		add_buf(output, buf);
	}
	add_buf(output, "{x");
	page_to_char(output->string, ch);
	free_buf(output);
}

CALC_FUN( mval_xcost_count )
{
	if (upgrade->xcost == NULL)
		return 0;

	XCOST_DATA *xcost;
	int count = 0;
	for (xcost = upgrade->xcost; xcost->name[0] != '\0'; xcost++)
		count++;

	return count;
}

/* Start of mob item functions */
CALC_FUN( cval_mob_normal )
{
	CLAN_DATA *clan;
	EDIT_CLAN( ch, clan );

	if (clan->cMob[upgrade->specific[0]] != NULL)
		return 1;

	return 0;
}
UPGD_FUN( mob_normal )
{
	CLAN_DATA *clan;
	MOB_INDEX_DATA *pMob, *pStock;
	RESET_DATA *pReset;
	ROOM_INDEX_DATA *pRoom;
	char buf[MAX_STRING_LENGTH];
	int vnum, iHash, i;
	EDIT_CLAN( ch, clan );

	// Check for room first
	if (upgrade->specific[0] == CMOB_HEALER)
	{
		if (clan->cRoom[CROOM_GSHRINE] == NULL)
		{
			if (clan->cRoom[CROOM_ESHRINE] == NULL)
				return UPGRADE_FAIL;
			pRoom = clan->cRoom[CROOM_ESHRINE];
		}
		else
			pRoom = clan->cRoom[CROOM_GSHRINE];
	}
	else if (upgrade->specific[0] == CMOB_PET)
	{
		if ((pRoom = get_room_index(VNUM_CLAN_PET_ROOM)) == NULL)
			return UPGRADE_FAIL;
	}
	else
	{
		if ((pRoom = clan->cRoom[upgrade->specific[1]]) == NULL)
			return UPGRADE_FAIL;
	}

	if (upgrade->specific[0] >= CMOB_MAX)
		return UPGRADE_FAIL;
	else
		vnum = cmob_table[upgrade->specific[0]].vnum;

	if ((pStock = get_mob_index(vnum)) == NULL)
		return UPGRADE_FAIL;

	if ((vnum = get_clan_mvnum(clan)) <= 0)
		return UPGRADE_FAIL;

	pMob = new_mob_index();
	pMob->area = get_clan_area();
	pMob->vnum = vnum;
	//    pMob->clan_data.data	= clan;

	strcpy(buf, pStock->player_name);
	str_replace_c(buf, "%s", clan->name);
	free_string(pMob->player_name);
	pMob->player_name = str_dup(buf);

	strcpy(buf, pStock->short_descr);
	str_replace_c(buf, "%s", strip_bespaces(clan->c_name));
	free_string(pMob->short_descr);
	pMob->short_descr = str_dup(buf);

	strcpy(buf, pStock->long_descr);
	str_replace_c(buf, "%s", strip_bespaces(clan->c_name));
	free_string(pMob->long_descr);
	pMob->long_descr = str_dup(buf);

	strcpy(buf, pStock->description);
	str_replace_c(buf, "%s", strip_bespaces(clan->c_name));
	free_string(pMob->description);
	pMob->description = str_dup(buf);

	pMob->act = pStock->act;
	pMob->affected_by = pStock->affected_by;
	pMob->shielded_by = pStock->shielded_by;
	pMob->alignment = pStock->alignment;
	pMob->level = pStock->level;
	pMob->hitroll = pStock->hitroll;
	pMob->spec_fun = pStock->spec_fun;

	for (i = 0; i <= 3; i++)
	{
		pMob->hit[i] = pStock->hit[i];
		pMob->mana[i] = pStock->mana[i];
		pMob->damage[i] = pStock->damage[i];
		pMob->ac[i] = pStock->ac[i];
	}
	pMob->ac[4] = pStock->ac[4];

	pMob->dam_type = pStock->dam_type;
	pMob->off_flags = pStock->off_flags;
	pMob->imm_flags = pStock->imm_flags;
	pMob->res_flags = pStock->res_flags;
	pMob->vuln_flags = pStock->vuln_flags;
	pMob->start_pos = pStock->start_pos;
	pMob->default_pos = pStock->default_pos;
	pMob->sex = pStock->sex;
	pMob->race = pStock->race;
	pMob->wealth = pStock->wealth;
	pMob->form = pStock->form;
	pMob->parts = pStock->parts;
	pMob->size = pStock->size;
	free_string(pMob->material);
	pMob->material = str_dup(pStock->material);
	free_string(pMob->die_descr);
	pMob->die_descr = str_dup(pStock->die_descr);
	free_string(pMob->say_descr);
	pMob->say_descr = str_dup(pStock->say_descr);
	pMob->round_dam = pStock->round_dam;

	if (pStock->pShop)
	{
		pMob->pShop = new_shop();
		pMob->pShop->keeper = pMob->vnum;

		for (i = 0; i < MAX_TRADE; i++)
			pMob->pShop->buy_type[i] = pStock->pShop->buy_type[i];

		pMob->pShop->profit_buy = pStock->pShop->profit_buy;
		pMob->pShop->profit_sell = pStock->pShop->profit_sell;
		pMob->pShop->open_hour = pStock->pShop->open_hour;
		pMob->pShop->close_hour = pStock->pShop->close_hour;

		if (!shop_first)
			shop_first = pMob->pShop;
		if (shop_last)
			shop_last->next = pMob->pShop;

		shop_last = pMob->pShop;
	}

	iHash = vnum % MAX_KEY_HASH;
	pMob->next = mob_index_hash[iHash];
	mob_index_hash[iHash] = pMob;
	clan->cMob[upgrade->specific[0]] = pMob;

	pReset = new_reset_data();
	pReset->command = 'M';
	pReset->arg1 = pMob->vnum;
	pReset->arg2 = 1;
	pReset->arg3 = pRoom->vnum;
	pReset->arg4 = 1;

	add_reset(pRoom, pReset, -1);
	save_area(pRoom->area);
	reset_room(pRoom);

	return UPGRADE_SUCCESS;
}

/* Start of object item functions */
CALC_FUN( cval_obj_normal ) // Max should be VNUM_CLAN_BLOCK
{
	CLAN_DATA *clan;
	OBJ_INDEX_DATA *pObj;
	int count = 0, iHash;

	EDIT_CLAN( ch, clan );

	for (iHash = 0; iHash < MAX_KEY_HASH; iHash++)
	{
		for (pObj = obj_index_hash[iHash]; pObj != NULL; pObj = pObj->next)
			if (pObj->vnum >= clan->vnum[0] && pObj->vnum <= clan->vnum[1]
					&& pObj->item_type == upgrade->specific[1])
				count++;
	}

	return count;
}
CALC_FUN( mval_obj_normal ) // Max should be VNUM_CLAN_BLOCK
{
	CLAN_DATA *clan;
	OBJ_INDEX_DATA *pObj;
	int count = 0, objcount = 0, iHash;

	EDIT_CLAN( ch, clan );

	for (iHash = 0; iHash < MAX_KEY_HASH; iHash++)
	{
		for (pObj = obj_index_hash[iHash]; pObj != NULL; pObj = pObj->next)
		{
			if (pObj->vnum >= clan->vnum[0] && pObj->vnum <= clan->vnum[1])
			{
				if (pObj->item_type == upgrade->specific[1])
					objcount++;
				count++;
			}
		}
	}

	return (objcount + VNUM_CLAN_BLOCK - count);
}
UPGD_FUN( obj_normal )
{
	CHAR_DATA *keeper;
	CLAN_DATA *clan;
	OBJ_INDEX_DATA *pObj, *pStock;
	RESET_DATA *pReset;
	ROOM_INDEX_DATA *pRoom;
	char buf[MAX_STRING_LENGTH];
	int vnum, i = 0, iHash, v0 = -1, v2 = -1, v3 = -1;

	EDIT_CLAN( ch, clan );

	if (IS_SET( upgrade->flags, UPGD_ROOM_OBJ ))
	{
		argument = one_argument(argument, buf);
		if (buf[0] == '\0')
		{
			send_to_char(
					"Please specify the clan room you want to load the object in.\n\r",
					ch);
			return UPGRADE_REQ_ARG;
		}
		for (i = 0; i < CROOM_MAX; i++)
		{
			if (!str_cmp(buf, croom_table[i].name))
				break;
		}
		if (i == CROOM_MAX || clan->cRoom[i] == NULL)
		{
			send_to_char("Please specify a valid, existing room.\n\r", ch);
			return UPGRADE_REQ_ARG;
		}
		pRoom = clan->cRoom[i];
	}
	else
	{
		if ((pRoom = clan->cRoom[CROOM_SHOP]) == NULL)
			return UPGRADE_FAIL;
		else if (clan->cMob[CMOB_SHOPKEEP] == NULL)
			return UPGRADE_FAIL;
	}

	argument = one_argument(argument, buf);

	switch (upgrade->specific[1])
	{
	case ITEM_FOUNTAIN:
		if (buf[0] == '\0')
		{
			send_to_char("Please specify the liquid type.\n\r", ch);
			return UPGRADE_REQ_ARG;
		}
		if ((v2 = liq_lookup(buf)) < 0)
		{
			send_to_char("Invalid liquid type.\n\r", ch);
			return UPGRADE_REQ_ARG;
		}
		break;
	case ITEM_PORTAL:
		if (buf[0] == '\0')
		{
			send_to_char(
					"Please specify the area you would like the portal to go to.\n\r",
					ch);
			strcpy(buf, "{w   ");
			for (i = 0; portloc_table[i].name[0] != '\0'; i++)
			{
				strcat(buf, portloc_table[i].name);
				strcat(buf, " ");
			}
			strcat(buf, "{x\n\r");
			send_to_char(buf, ch);
			return UPGRADE_REQ_ARG;
		}

		for (i = 0; portloc_table[i].name[0] != '\0'; i++)
			if (!str_prefix(buf, portloc_table[i].name))
				break;

		if ((v3 = portloc_table[i].vnum) <= 0)
		{
			send_to_char("Invalid area.\n\r", ch);
			return UPGRADE_REQ_ARG;
		}
		break;
	case ITEM_WEAPON:
		if (buf[0] == '\0')
		{
			send_to_char("Please specify the type of weapon.\n\r", ch);
			return UPGRADE_REQ_ARG;
		}
		if ((v0 = flag_value(weapon_class, buf)) == NO_FLAG)
		{
			send_to_char("Weapon type not found.\n\r", ch);
			return UPGRADE_REQ_ARG;
		}
		break;
	default:
		break;
	}

	if ((pStock = get_obj_index(upgrade->specific[0])) == NULL)
		return UPGRADE_FAIL;

	if ((vnum = get_clan_ovnum(clan)) <= 0)
		return UPGRADE_FAIL;

	pObj = new_obj_index();
	pObj->vnum = vnum;
	pObj->area = get_clan_area();

	strcpy(buf, pStock->name);
	str_replace_c(buf, "%s", clan->name);
	if (upgrade->specific[1] == ITEM_PORTAL)
	{
		strcat(buf, " ");
		strcat(buf, portloc_table[i].name);
	}
	free_string(pObj->name);
	pObj->name = str_dup(buf);

	strcpy(buf, pStock->short_descr);
	str_replace_c(buf, "%s", strip_bespaces(clan->c_name));
	free_string(pObj->short_descr);
	pObj->short_descr = str_dup(buf);

	strcpy(buf, pStock->description);
	str_replace_c(buf, "%s", strip_bespaces(clan->c_name));
	free_string(pObj->description);
	pObj->description = str_dup(buf);

	free_string(pObj->material);
	pObj->material = str_dup(pStock->material);

	pObj->item_type = pStock->item_type;
	pObj->extra_flags = pStock->extra_flags;
	pObj->wear_flags = pStock->wear_flags;
	pObj->level = pStock->level;
	pObj->condition = pStock->condition;
	pObj->count = pStock->count;
	pObj->weight = pStock->weight;
	pObj->cost = pStock->cost;

	for (i = 0; i < 5; i++)
		pObj->value[i] = pStock->value[i];

	pObj->class = pStock->class;
	pObj->timer = pStock->timer;
	pObj->class_restrict_flags = pStock->class_restrict_flags;

	if (v0 > -1)
		pObj->value[0] = v0;
	if (v2 > -1)
		pObj->value[2] = v2;
	if (v3 > -1)
		pObj->value[3] = v3;

	iHash = vnum % MAX_KEY_HASH;
	pObj->next = obj_index_hash[iHash];
	obj_index_hash[iHash] = pObj;

	pReset = new_reset_data();

	if (IS_SET( upgrade->flags, UPGD_ROOM_OBJ ))
	{
		pReset->command = 'O';
		pReset->arg1 = pObj->vnum;
		pReset->arg2 = 0;
		pReset->arg3 = pRoom->vnum;
		pReset->arg4 = 0;
	}
	else
	{
		pReset->command = 'G';
		pReset->arg1 = pObj->vnum;
		pReset->arg2 = 0;
		pReset->arg3 = WEAR_NONE;
		pReset->arg4 = 0;
	}

	add_reset(pRoom, pReset, -1);
	save_area(pObj->area);
	if (IS_SET( upgrade->flags, UPGD_ROOM_OBJ ))
		obj_to_room(create_object(pObj, pObj->level), pRoom);
	else
	{
		for (keeper = pRoom->people; keeper; keeper = keeper->next_in_room)
			if (keeper->pIndexData && keeper->pIndexData
					== clan->cMob[CMOB_SHOPKEEP])
				break;
		if (keeper)
			extract_char(keeper, TRUE );
	}
	reset_room(pRoom);

	return UPGRADE_SUCCESS;
}

/* Start of room item functions */
CALC_FUN( cval_room_normal )
{
	CLAN_DATA *clan;
	EDIT_CLAN( ch, clan );

	if (clan->cRoom[upgrade->specific[0]] != NULL)
		return 1;

	return 0;
}
UPGD_FUN( room_normal )
{
	CLAN_DATA *clan;
	ROOM_INDEX_DATA *pStock, *pRoom;
	char buf[MAX_STRING_LENGTH];
	int vnum, exit, iHash;

	EDIT_CLAN( ch, clan );

	if (ch->in_room == NULL)
		return UPGRADE_FAIL;

	if (upgrade->specific[0] != CROOM_RECALL)
	{
		if (!is_clan_room(ch->in_room, clan, TRUE ))
		{
			send_to_char("Please move to a room owned by the clan.\n\r", ch);
			return UPGRADE_REQ_ARG;
		}
		if (argument[0] == '\0')
		{
			send_to_char(
					"Please specify the direction you wish the new room to be built in.\n\r",
					ch);
			return UPGRADE_REQ_ARG;
		}
	}

	switch (UPPER(argument[0]))
	{
	case 'N':
		exit = DIR_NORTH;
		break;
	case 'E':
		exit = DIR_EAST;
		break;
	case 'S':
		exit = DIR_SOUTH;
		break;
	case 'W':
		exit = DIR_WEST;
		break;
	case 'U':
		exit = DIR_UP;
		break;
	case 'D':
		exit = DIR_DOWN;
		break;
	default:
		send_to_char("Direction must be n, e, s, w, u, or d.\n\r", ch);
		return UPGRADE_REQ_ARG;
	}

	if (ch->in_room->exit[exit] || ch->in_room->exit[exit + 6])
	{
		send_to_char(
				"The direction you specified already has a room. Choose another.\n\r",
				ch);
		return UPGRADE_REQ_ARG;
	}

	if (upgrade->specific[0] > CROOM_MAX)
		return UPGRADE_FAIL;
	else
		vnum = croom_table[upgrade->specific[0]].vnum;

	if ((pStock = get_room_index(vnum)) == NULL)
	{
		sprintf(buf, "UPGD_FUN room_normal: stock vnum %d for %s(%d) == NULL",
				vnum, upgrade->name, upgrade->specific[0]);
		printf_debug(buf);
		wiznet(buf, ch, NULL, WIZ_DEBUG, 0, 0);
		return UPGRADE_FAIL;
	}

	if ((vnum = get_clan_rvnum(clan)) < 0)
		return UPGRADE_FAIL;

	pRoom = new_room_index();
	pRoom->area = get_clan_area();
	pRoom->vnum = vnum;
	//    pRoom->clan_data.data	= clan;

	strcpy(buf, pStock->name);
	str_replace_c(buf, "%s", strip_bespaces(clan->c_name));
	free_string(pRoom->name);
	pRoom->name = str_dup(buf);

	strcpy(buf, pStock->description);
	str_replace_c(buf, "%s", strip_bespaces(clan->c_name));
	free_string(pRoom->description);
	pRoom->description = str_dup(buf);

	pRoom->room_flags = pStock->room_flags;
	pRoom->light = pStock->light;
	pRoom->sector_type = pStock->sector_type;
	pRoom->heal_rate = pStock->heal_rate;
	pRoom->mana_rate = pStock->mana_rate;

	ch->in_room->exit[exit] = new_exit();
	ch->in_room->exit[exit]->u1.to_room = pRoom;
	ch->in_room->exit[exit]->orig_door = exit;
	pRoom->exit[rev_dir[exit]] = new_exit();
	pRoom->exit[rev_dir[exit]]->u1.to_room = ch->in_room;
	pRoom->exit[rev_dir[exit]]->orig_door = rev_dir[exit];

	iHash = vnum % MAX_KEY_HASH;
	pRoom->next = room_index_hash[iHash];
	room_index_hash[iHash] = pRoom;

	if (upgrade->specific[0] != CROOM_MAX)
		clan->cRoom[upgrade->specific[0]] = pRoom;

	if (upgrade->specific[0] == CROOM_RECALL)
	{
		clan->entrance_vnum = ch->in_room->vnum;
		clan->exit_changed = current_time;
	}
	save_area(pRoom->area);
	save_area(ch->in_room->area);

	return UPGRADE_SUCCESS;
}

UPGD_FUN( room_recall )
{
	CLAN_DATA *clan;

	EDIT_CLAN( ch, clan );

	if (ch->in_room == NULL)
		return UPGRADE_FAIL;

	if (is_clan_room(ch->in_room, NULL, FALSE ))
	{
		send_to_char(
				"Please go to the room where you want the entrance of the clan hall to be.\n\r",
				ch);
		return UPGRADE_REQ_ARG;
	}

	if (argument[0] == '\0')
	{
		send_to_char(
				"Please provide the direction you want the clan hall to be in.\n\r"
					"An example would be \"{Rconfirm u{x\".\n\r",
				ch);
		return UPGRADE_REQ_ARG;
	}

	if (!check_path(ch->in_room)) {
		send_to_char("There isn't a clear path from the Market Square to here.\r\n", ch);
		return UPGRADE_FAIL;
	}

	return room_normal(ch, argument, upgrade, xupgrade);
}

CALC_FUN( cval_room_hall_entrance )
{
	CLAN_DATA *clan;
	EDIT_CLAN( ch, clan );

	if (clan->cRoom[CROOM_RECALL] == NULL)
		return 1;

	return 0;
}
CALC_FUN( mval_room_hall_entrance )
{
	CLAN_DATA *clan;
	EDIT_CLAN( ch, clan );

	if (clan->cRoom[CROOM_RECALL] == NULL
			|| (current_time - clan->exit_changed) < TIME_DAY)
		return 0;

	return 1;
}
UPGD_FUN( room_hall_entrance )
{
	CLAN_DATA *clan;
	int exit, i;

	EDIT_CLAN( ch, clan );

	if (ch->in_room == NULL)
		return UPGRADE_FAIL;

	if (is_clan_room(ch->in_room, NULL, FALSE ))
	{
		send_to_char(
				"Please go to the room where you want the entrance of the clan hall to be.\n\r",
				ch);
		return UPGRADE_REQ_ARG;
	}

	if (argument[0] == '\0')
	{
		send_to_char(
				"Please provide the direction you want the clan hall to be in.\n\r"
					"An example would be \"{Rconfirm u{x\", if you want it to be above you.\n\r",
				ch);
		return UPGRADE_REQ_ARG;
	}

	if (!check_path(ch->in_room)) {
		send_to_char("There isn't a clear path from the Market Square to here.\r\n", ch);
		return UPGRADE_FAIL;
	}

	switch (UPPER(argument[0]))
	{
	case 'N':
		exit = DIR_NORTH;
		break;
	case 'E':
		exit = DIR_EAST;
		break;
	case 'S':
		exit = DIR_SOUTH;
		break;
	case 'W':
		exit = DIR_WEST;
		break;
	case 'U':
		exit = DIR_UP;
		break;
	case 'D':
		exit = DIR_DOWN;
		break;
	default:
		send_to_char("Direction must be n, e, s, w, u, or d.\n\r", ch);
		return UPGRADE_REQ_ARG;
	}

	if (ch->in_room->exit[exit] != NULL
	&&  ch->in_room->exit[exit]->u1.to_room != NULL) {
		send_to_char("You cannot have a clanhall in that direction.\r\n", ch);
		return UPGRADE_REQ_ARG;
	}

	for (i = 0; i < MAX_DIR; i++)
		if (clan->cRoom[CROOM_RECALL]->exit[i]
				&& clan->cRoom[CROOM_RECALL]->exit[i]->u1.to_room
				&& clan->cRoom[CROOM_RECALL]->exit[i]->u1.to_room->vnum
						== clan->entrance_vnum)
			break;

	if (i == MAX_DIR)
	{
		printf_debug("room_hall_entrance: no original clan hall entrance.\n\r");
		return UPGRADE_FAIL;
	}

	if (ch->in_room == clan->cRoom[CROOM_RECALL]->exit[i]->u1.to_room) {
		send_to_char("Your clanhall already uses this room as an entrance.\r\n", ch);
		return UPGRADE_REQ_ARG;
	}
	AREA_DATA *oldArea = clan->cRoom[CROOM_RECALL]->exit[i]->u1.to_room->area;
	
	free_exit(clan->cRoom[CROOM_RECALL]->exit[i]->u1.to_room->exit[rev_dir[i]]);
	clan->cRoom[CROOM_RECALL]->exit[i]->u1.to_room->exit[rev_dir[i]] = NULL;

	free_exit(clan->cRoom[CROOM_RECALL]->exit[i]);
	clan->cRoom[CROOM_RECALL]->exit[i] = NULL;

	if (ch->in_room->exit[exit] != NULL)
		free_exit(ch->in_room->exit[exit]);
	ch->in_room->exit[exit] = new_exit();
	ch->in_room->exit[exit]->u1.to_room = clan->cRoom[CROOM_RECALL];
	ch->in_room->exit[exit]->orig_door = exit;

	if (clan->cRoom[CROOM_RECALL]->exit[rev_dir[exit]] != NULL)
	{
		if (clan->cRoom[CROOM_RECALL]->exit[i] != NULL)
			free_exit(clan->cRoom[CROOM_RECALL]->exit[i]);
		clan->cRoom[CROOM_RECALL]->exit[i] = new_exit();
		clan->cRoom[CROOM_RECALL]->exit[i]->u1.to_room = ch->in_room;
		clan->cRoom[CROOM_RECALL]->exit[i]->orig_door = i;
	}
	else
	{
		if (clan->cRoom[CROOM_RECALL]->exit[rev_dir[exit]] != NULL)
			free_exit(clan->cRoom[CROOM_RECALL]->exit[rev_dir[exit]]);
		clan->cRoom[CROOM_RECALL]->exit[rev_dir[exit]] = new_exit();
		clan->cRoom[CROOM_RECALL]->exit[rev_dir[exit]]->u1.to_room
				= ch->in_room;
		clan->cRoom[CROOM_RECALL]->exit[rev_dir[exit]]->orig_door
				= rev_dir[exit];
	}

	clan->entrance_vnum = ch->in_room->vnum;
	clan->exit_changed = current_time;

	save_area(clan->cRoom[CROOM_RECALL]->area);
	save_area(ch->in_room->area);
	save_area(oldArea);
	return UPGRADE_SUCCESS;
}

/* Start of mob upgrade functions */
CALC_FUN( cval_mob_upgd_level )
{
	MOB_INDEX_DATA *pMob;
	EDIT_MOB( ch, pMob );

	return pMob->level;
}
CALC_FUN( mval_mob_upgd_level )
{
	CLAN_DATA *clan;
	MOB_INDEX_DATA *pMob;
	int i;
	EDIT_MOB( ch, pMob );
	EDIT_CLAN( ch, clan );

	for (i = 0; i < CMOB_MAX; i++)
		if (pMob == clan->cMob[i])
			break;

	if (i == CMOB_TRAINER)
		return 180;

	if (clan->cMob[CMOB_TRAINER] == NULL)
		return pMob->level;
	else
		return clan->cMob[CMOB_TRAINER]->level;
}
UPGD_FUN( mob_upgd_level )
{
	CLAN_DATA *clan;
	MOB_INDEX_DATA *pMob;
	EDIT_MOB( ch, pMob );
	EDIT_CLAN( ch, clan );

	pMob->level += upgrade->quantity * clan->u_quantity;
	pMob->hitroll = pMob->level * 2;
	return UPGRADE_SUCCESS;
}

CALC_FUN( cval_mob_upgd_hp )
{
	MOB_INDEX_DATA *pMob;
	EDIT_MOB( ch, pMob );

	return pMob->hit[DICE_BONUS];
}
UPGD_FUN( mob_upgd_hp )
{
	CLAN_DATA *clan;
	MOB_INDEX_DATA *pMob;
	EDIT_MOB( ch, pMob );
	EDIT_CLAN( ch, clan );

	pMob->hit[DICE_BONUS] += upgrade->quantity * clan->u_quantity;
	return UPGRADE_SUCCESS;
}

CALC_FUN( cval_mob_upgd_mana )
{
	MOB_INDEX_DATA *pMob;
	EDIT_MOB( ch, pMob );

	return pMob->mana[DICE_BONUS];
}
UPGD_FUN( mob_upgd_mana )
{
	CLAN_DATA *clan;
	MOB_INDEX_DATA *pMob;
	EDIT_MOB( ch, pMob );
	EDIT_CLAN( ch, clan );

	pMob->mana[DICE_BONUS] += upgrade->quantity * clan->u_quantity;
	return UPGRADE_SUCCESS;
}

CALC_FUN( cval_mob_upgd_damage )
{
	MOB_INDEX_DATA *pMob;
	EDIT_MOB( ch, pMob );

	return pMob->damage[DICE_BONUS];
}
CALC_FUN( mval_mob_upgd_damage )
{
	CLAN_DATA *clan;
	MOB_INDEX_DATA *pMob;
	int i;
	EDIT_MOB( ch, pMob );
	EDIT_CLAN( ch, clan );

	for (i = 0; i < CMOB_MAX; i++)
		if (pMob == clan->cMob[i])
			break;
	if (i == CMOB_GUARD || i == CMOB_PET)
		return 500;

	return 0;
}
UPGD_FUN( mob_upgd_damage )
{
	CLAN_DATA *clan;
	MOB_INDEX_DATA *pMob;
	EDIT_MOB( ch, pMob );
	EDIT_CLAN( ch, clan );

	pMob->damage[DICE_BONUS] += upgrade->quantity * clan->u_quantity;
	return UPGRADE_SUCCESS;
}

CALC_FUN( cval_mob_upgd_imm )
{
	MOB_INDEX_DATA *pMob;
	XCOST_DATA *xcost;
	int count = 0;
	EDIT_MOB( ch, pMob );

	for (xcost = imm_prices; xcost->name[0] != '\0'; xcost++)
		if (IS_SET( pMob->imm_flags, xcost->bit ))
			count++;

	return count;
}
UPGD_FUN( mob_upgd_imm )
{
	CLAN_DATA *clan;
	MOB_INDEX_DATA *pMob;
	XCOST_DATA *xcost;
	EDIT_CLAN( ch, clan );
	EDIT_MOB( ch, pMob );

	if (argument[0] == '\0')
	{
		send_to_char(
				"Please specify whether you want to remove/add the immunity.\n\r",
				ch);
		return UPGRADE_REQ_ARG;
	}

	if (!str_prefix(argument, "add"))
	{
		if (IS_SET( pMob->imm_flags, xupgrade->bit ))
		{
			send_to_char("Flag already present.\n\r", ch);
			return UPGRADE_REQ_ARG;
		}
		if (IS_SET( xupgrade->flags, XUPGD_IMM_SPECIAL ))
		{
			for (xcost = upgrade->xcost; xcost->name[0] != '\0'; xcost++)
			{
				if (IS_SET( xcost->flags, XUPGD_IMM_SPECIAL )
						&& IS_SET( pMob->imm_flags, xcost->bit ))
				{
					send_to_char(
							"You may only have one of this type of flag (starred ones on list) set.\n\r",
							ch);
					return UPGRADE_REQ_ARG;
				}
			}
		}
		SET_BIT( pMob->imm_flags, xupgrade->bit );
	}
	else if (!str_prefix(argument, "remove"))
	{
		if (!IS_SET( pMob->imm_flags, xupgrade->bit ))
		{
			send_to_char("Flag already not present.\n\r", ch);
			return UPGRADE_REQ_ARG;
		}
		clan->platinum = clan->platinum + xupgrade->platinum / 3;
		clan->qps = clan->qps + xupgrade->qps / 3;
		REMOVE_BIT( pMob->imm_flags, xupgrade->bit );
	}
	else
		return mob_upgd_imm(ch, "", upgrade, xupgrade);

	return UPGRADE_SUCCESS;
}

CALC_FUN( cval_mob_upgd_res )
{
	MOB_INDEX_DATA *pMob;
	XCOST_DATA *xcost;
	int count = 0;
	EDIT_MOB( ch, pMob );

	for (xcost = res_prices; xcost->name[0] != '\0'; xcost++)
		if (IS_SET( pMob->res_flags, xcost->bit ))
			count++;

	return count;
}
UPGD_FUN( mob_upgd_res )
{
	CLAN_DATA *clan;
	MOB_INDEX_DATA *pMob;
	EDIT_CLAN( ch, clan );
	EDIT_MOB( ch, pMob );

	if (argument[0] == '\0')
	{
		send_to_char(
				"Please specify whether you want to remove/add the resistance.\n\r",
				ch);
		return UPGRADE_REQ_ARG;
	}

	if (!str_prefix(argument, "add"))
	{
		if (IS_SET( pMob->res_flags, xupgrade->bit ))
		{
			send_to_char("Flag already present.\n\r", ch);
			return UPGRADE_REQ_ARG;
		}
		SET_BIT( pMob->res_flags, xupgrade->bit );
	}
	else if (!str_prefix(argument, "remove"))
	{
		if (!IS_SET( pMob->res_flags, xupgrade->bit ))
		{
			send_to_char("Flag already not present.\n\r", ch);
			return UPGRADE_REQ_ARG;
		}
		clan->platinum = clan->platinum + xupgrade->platinum / 3;
		clan->qps = clan->qps + xupgrade->qps / 3;
		REMOVE_BIT( pMob->res_flags, xupgrade->bit );
	}
	else
		return mob_upgd_res(ch, "", upgrade, xupgrade);

	return UPGRADE_SUCCESS;
}

CALC_FUN( cval_mob_upgd_off )
{
	MOB_INDEX_DATA *pMob;
	XCOST_DATA *xcost;
	int count = 0;
	EDIT_MOB( ch, pMob );

	for (xcost = off_prices; xcost->name[0] != '\0'; xcost++)
		if (IS_SET( pMob->off_flags, xcost->bit ))
			count++;

	return count;
}
UPGD_FUN( mob_upgd_off )
{
	CLAN_DATA *clan;
	MOB_INDEX_DATA *pMob;
	EDIT_CLAN( ch, clan );
	EDIT_MOB( ch, pMob );

	if (argument[0] == '\0')
	{
		send_to_char(
				"Please specify whether you want to remove/add the offensive skill.\n\r",
				ch);
		return UPGRADE_REQ_ARG;
	}

	if (!str_prefix(argument, "add"))
	{
		if (IS_SET( pMob->off_flags, xupgrade->bit ))
		{
			send_to_char("Flag already present.\n\r", ch);
			return UPGRADE_REQ_ARG;
		}
		SET_BIT( pMob->off_flags, xupgrade->bit );
	}
	else if (!str_prefix(argument, "remove"))
	{
		if (!IS_SET( pMob->off_flags, xupgrade->bit ))
		{
			send_to_char("Flag already not present.\n\r", ch);
			return UPGRADE_REQ_ARG;
		}
		clan->platinum = clan->platinum + xupgrade->platinum / 3;
		clan->qps = clan->qps + xupgrade->qps / 3;
		REMOVE_BIT( pMob->off_flags, xupgrade->bit );
	}
	else
		return mob_upgd_off(ch, "", upgrade, xupgrade);

	return UPGRADE_SUCCESS;
}

CALC_FUN( cval_mob_upgd_damtype )
{
	return 1;
}
UPGD_FUN( mob_upgd_damtype )
{
	MOB_INDEX_DATA *pMob;
	EDIT_MOB( ch, pMob );

	if (pMob->dam_type == xupgrade->bit)
	{
		send_to_char("The mob already has that damtype.\n\r", ch);
		return UPGRADE_REQ_ARG;
	}

	pMob->dam_type = xupgrade->bit;
	return UPGRADE_SUCCESS;
}

CALC_FUN( cval_mob_upgd_ac )
{
	MOB_INDEX_DATA *pMob;
	EDIT_MOB( ch, pMob );

	return -(pMob->ac[0]);
}
UPGD_FUN( mob_upgd_ac )
{
	CLAN_DATA *clan;
	MOB_INDEX_DATA *pMob;
	int i;
	EDIT_MOB( ch, pMob );
	EDIT_CLAN( ch, clan );

	for (i = 0; i < 4; i++)
		pMob->ac[i] -= upgrade->quantity * clan->u_quantity;

	return UPGRADE_SUCCESS;
}

CALC_FUN( cval_mob_upgd_perm )
{
	MOB_INDEX_DATA *pMob;
	XCOST_DATA *xcost;
	int count = 0;
	EDIT_MOB( ch, pMob );

	for (xcost = bitaf_prices; xcost->name[0] != '\0'; xcost++)
		if (IS_SET( pMob->affected_by, xcost->bit )
				|| IS_SET( pMob->shielded_by, xcost->bit ))
			count++;

	return count;
}
UPGD_FUN( mob_upgd_perm )
{
	CLAN_DATA *clan;
	MOB_INDEX_DATA *pMob;
	long *flags;
	EDIT_MOB( ch, pMob );
	EDIT_CLAN( ch, clan );

	if (argument[0] == '\0')
	{
		send_to_char(
				"Please specify whether you want to remove/add the perm effect.\n\r",
				ch);
		return UPGRADE_REQ_ARG;
	}

	if (xupgrade->type == BIT_AFFECT)
		flags = &(pMob->affected_by);
	else
		flags = &(pMob->shielded_by);

	if (!str_prefix(argument, "add"))
	{
		if (IS_SET( *flags, xupgrade->bit ))
		{
			send_to_char("Flag already present.\n\r", ch);
			return UPGRADE_REQ_ARG;
		}
		clan->platinum = clan->platinum + xupgrade->platinum / 3;
		clan->qps = clan->qps + xupgrade->qps / 3;
		SET_BIT( *flags, xupgrade->bit );
	}
	else if (!str_prefix(argument, "remove"))
	{
		if (!IS_SET( *flags, xupgrade->bit ))
		{
			send_to_char("Flag already not present.\n\r", ch);
			return UPGRADE_REQ_ARG;
		}
		REMOVE_BIT( *flags, xupgrade->bit );
	}
	else
		return mob_upgd_perm(ch, "", upgrade, xupgrade);

	return UPGRADE_SUCCESS;
}

/* Start of object upgrade functions */
UPGD_FUN( obj_upgd_furn_capacity )
{
	CLAN_DATA *clan;
	OBJ_INDEX_DATA *pObj;
	EDIT_OBJ( ch, pObj );
	EDIT_CLAN( ch, clan );

	pObj->value[0] += upgrade->quantity * clan->u_quantity;
	pObj->value[1] += upgrade->quantity * clan->u_quantity * 100;
	return UPGRADE_SUCCESS;
}

CALC_FUN( cval_obj_upgd_v0 )
{
	OBJ_INDEX_DATA *pObj;
	EDIT_OBJ( ch, pObj );

	return pObj->value[0];
}
UPGD_FUN( obj_upgd_v0 )
{
	CLAN_DATA *clan;
	OBJ_INDEX_DATA *pObj;
	EDIT_OBJ( ch, pObj );
	EDIT_CLAN( ch, clan );

	pObj->value[0] += upgrade->quantity * clan->u_quantity;
	return UPGRADE_SUCCESS;
}

CALC_FUN( cval_obj_upgd_v2 )
{
	OBJ_INDEX_DATA *pObj;
	EDIT_OBJ( ch, pObj );

	return pObj->value[2];
}
UPGD_FUN( obj_upgd_v2 )
{
	CLAN_DATA *clan;
	OBJ_INDEX_DATA *pObj;
	EDIT_OBJ( ch, pObj );
	EDIT_CLAN( ch, clan );

	pObj->value[2] += upgrade->quantity * clan->u_quantity;
	return UPGRADE_SUCCESS;
}

CALC_FUN( cval_obj_upgd_v3 )
{
	OBJ_INDEX_DATA *pObj;
	EDIT_OBJ( ch, pObj );

	return pObj->value[3];
}
UPGD_FUN( obj_upgd_v3 )
{
	CLAN_DATA *clan;
	OBJ_INDEX_DATA *pObj;
	EDIT_OBJ( ch, pObj );
	EDIT_CLAN( ch, clan );

	pObj->value[3] += upgrade->quantity * clan->u_quantity;
	return UPGRADE_SUCCESS;
}

CALC_FUN( cval_obj_upgd_v4 )
{
	OBJ_INDEX_DATA *pObj;
	EDIT_OBJ( ch, pObj );

	return pObj->value[4];
}
UPGD_FUN( obj_upgd_v4 )
{
	CLAN_DATA *clan;
	OBJ_INDEX_DATA *pObj;
	EDIT_OBJ( ch, pObj );
	EDIT_CLAN( ch, clan );

	pObj->value[4] += upgrade->quantity * clan->u_quantity;
	return UPGRADE_SUCCESS;
}

CALC_FUN( cval_obj_upgd_add1_spell )
{
	OBJ_INDEX_DATA *pObj;
	EDIT_OBJ( ch, pObj );

	if (pObj->value[3] > 0 && pObj->value[3] < MAX_SKILL)
		return 1;

	return 0;
}
UPGD_FUN( obj_upgd_add1_spell )
{
	OBJ_INDEX_DATA *pObj;
	EDIT_OBJ( ch, pObj );

	pObj->value[3] = xupgrade->bit;
	return UPGRADE_SUCCESS;
}

CALC_FUN( cval_obj_upgd_add3_spell )
{
	OBJ_INDEX_DATA *pObj;
	int i, count = 0;
	EDIT_OBJ( ch, pObj );

	for (i = 1; i <= 4; i++)
	{
		if (pObj->value[i] > 0 && pObj->value[i] < MAX_SKILL)
			count++;
	}
	return count;
}
UPGD_FUN( obj_upgd_add3_spell )
{
	OBJ_INDEX_DATA *pObj;
	int slot;
	EDIT_OBJ( ch, pObj );

	for (slot = 1; slot < 4; slot++)
	{
		if (pObj->value[slot] == xupgrade->bit)
		{
			send_to_char(
					"You can only have one of each type of spell and this item already has this spell.\n\r",
					ch);
			send_to_char("Please clear the upgrade.\n\r", ch);
			return UPGRADE_REQ_ARG;
		}
	}

	if (argument[0] == '\0' || !is_number(argument))
	{
		send_to_char(
				"Please specfiy a slot number to put the spell into.\n\r"
					"{RNote: {xThe spell you specify WILL replace the preset spell in the slot.\n\r",
				ch);
		return UPGRADE_REQ_ARG;
	}

	if ((slot = atoi(argument)) < 1 || slot > 3)
	{
		send_to_char("The slot number must be between 1 and 3.\n\r", ch);
		return UPGRADE_REQ_ARG;
	}

	pObj->value[slot] = xupgrade->bit;
	return UPGRADE_SUCCESS;
}

UPGD_FUN( obj_upgd_charges )
{
	CLAN_DATA *clan;
	OBJ_INDEX_DATA *pObj;
	EDIT_OBJ( ch, pObj );
	EDIT_CLAN( ch, clan );

	pObj->value[1] += upgrade->quantity * clan->u_quantity;
	pObj->value[2] += upgrade->quantity * clan->u_quantity;
	return UPGRADE_SUCCESS;
}

UPGD_FUN( obj_upgd_damage )
{
	CLAN_DATA *clan;
	OBJ_INDEX_DATA *pObj;
	EDIT_OBJ( ch, pObj );
	EDIT_CLAN( ch, clan );

	pObj->value[1] += upgrade->quantity * clan->u_quantity;
	pObj->value[2] += upgrade->quantity * clan->u_quantity;
	return UPGRADE_SUCCESS;
}

CALC_FUN( cval_obj_upgd_damtype )
{
	return 1;
}
UPGD_FUN( obj_upgd_damtype )
{
	OBJ_INDEX_DATA *pObj;
	EDIT_OBJ( ch, pObj );

	pObj->value[3] = xupgrade->bit;
	return UPGRADE_SUCCESS;
}

CALC_FUN( cval_obj_upgd_wpnflag )
{
	OBJ_INDEX_DATA *pObj;
	XCOST_DATA *xcost;
	int count = 0;
	EDIT_OBJ( ch, pObj );

	for (xcost = wpnflag_prices; xcost->name[0] != '\0'; xcost++)
		if (IS_SET( pObj->value[4], xcost->bit ))
			count++;

	return count;
}
UPGD_FUN( obj_upgd_wpnflag )
{
	CLAN_DATA *clan;
	OBJ_INDEX_DATA *pObj;
	EDIT_CLAN( ch, clan );
	EDIT_OBJ( ch, pObj );

	if (argument[0] == '\0')
	{
		send_to_char(
				"Please specify whether you want to remove/add the weapon flag.\n\r",
				ch);
		return UPGRADE_REQ_ARG;
	}

	if (!str_prefix(argument, "add"))
	{
		if (IS_SET( pObj->value[4], xupgrade->bit ))
		{
			send_to_char("Flag already present.\n\r", ch);
			return UPGRADE_REQ_ARG;
		}
		SET_BIT( pObj->value[4], xupgrade->bit );
	}
	else if (!str_prefix(argument, "remove"))
	{
		if (!IS_SET( pObj->value[4], xupgrade->bit ))
		{
			send_to_char("Flag already not present.\n\r", ch);
			return UPGRADE_REQ_ARG;
		}
		clan->platinum = clan->platinum + xupgrade->platinum / 3;
		clan->qps = clan->qps + xupgrade->qps / 3;
		REMOVE_BIT( pObj->value[4], xupgrade->bit );
	}
	else
		return obj_upgd_wpnflag(ch, "", upgrade, xupgrade);

	return UPGRADE_SUCCESS;
}

UPGD_FUN( obj_upgd_cont_capacity )
{
	CLAN_DATA *clan;
	OBJ_INDEX_DATA *pObj;
	EDIT_CLAN( ch, clan );
	EDIT_OBJ( ch, pObj );

	pObj->value[0] += upgrade->quantity * clan->u_quantity;
	pObj->value[3] += (upgrade->quantity / 5) * clan->u_quantity;
	pObj->value[4] -= 1;
	return UPGRADE_SUCCESS;
}

CALC_FUN( cval_obj_upgd_affected )
{
	AFFECT_DATA *af;
	OBJ_INDEX_DATA *pObj;
	int count = 0;
	EDIT_OBJ( ch, pObj );

	for (af = pObj->affected; af != NULL; af = af->next)
		if (af->where == TO_OBJECT && af->location == upgrade->specific[1])
			count += af->modifier;

	if (upgrade->specific[1] == APPLY_SAVES || upgrade->specific[1] == APPLY_AC)
		count *= -1;
	return count;
}
UPGD_FUN( obj_upgd_affected )
{
	AFFECT_DATA *af;
	CLAN_DATA *clan;
	OBJ_INDEX_DATA *pObj;

	EDIT_OBJ( ch, pObj );
	EDIT_CLAN( ch, clan );

	if (!IS_SET( pObj->wear_flags, ITEM_TAKE ))
	{
		send_to_char(
				"Effects cannot be added to this item. Please clear the transaction.\n\r",
				ch);
		return UPGRADE_REQ_ARG;
	}

	for (af = pObj->affected; af != NULL; af = af->next)
		if (af->where == TO_OBJECT && af->location == upgrade->specific[1])
			break;

	if (af == NULL)
	{
		af = new_affect();
		af->where = TO_OBJECT;
		af->location = upgrade->specific[1];
		af->type = -1;
		af->duration = -1;
		af->modifier = 0;
		af->bitvector = 0;
		af->level = pObj->level;
		af->next = pObj->affected;
		pObj->affected = af;
	}

	if (upgrade->specific[1] == APPLY_SAVES || upgrade->specific[1] == APPLY_AC)
		af->modifier = af->modifier - (upgrade->quantity * clan->u_quantity);
	else
		af->modifier = af->modifier + (upgrade->quantity * clan->u_quantity);
	return UPGRADE_SUCCESS;
}

CALC_FUN( cval_obj_upgd_objflag )
{
	OBJ_INDEX_DATA *pObj;
	XCOST_DATA *xcost;
	int count = 0;
	EDIT_OBJ( ch, pObj );

	for (xcost = objflag_prices; xcost->name[0] != '\0'; xcost++)
		if (IS_SET( pObj->extra_flags, xcost->bit ))
			count++;

	return count;
}
UPGD_FUN( obj_upgd_objflag )
{
	CLAN_DATA *clan;
	OBJ_INDEX_DATA *pObj;
	EDIT_CLAN( ch, clan );
	EDIT_OBJ( ch, pObj );

	if (argument[0] == '\0')
	{
		send_to_char(
				"Please specify whether you want to remove/add the object flag.\n\r",
				ch);
		return UPGRADE_REQ_ARG;
	}

	if (!str_prefix(argument, "add"))
	{
		if (IS_SET( pObj->extra_flags, xupgrade->bit ))
		{
			send_to_char("Flag already present.\n\r", ch);
			return UPGRADE_REQ_ARG;
		}
		SET_BIT( pObj->extra_flags, xupgrade->bit );
	}
	else if (!str_prefix(argument, "remove"))
	{
		if (!IS_SET( pObj->extra_flags, xupgrade->bit ))
		{
			send_to_char("Flag already not present.\n\r", ch);
			return UPGRADE_REQ_ARG;
		}
		clan->platinum = clan->platinum + xupgrade->platinum / 3;
		clan->qps = clan->qps + xupgrade->qps / 3;
		REMOVE_BIT( pObj->extra_flags, xupgrade->bit );
	}
	else
		return obj_upgd_objflag(ch, "", upgrade, xupgrade);

	return UPGRADE_SUCCESS;
}

CALC_FUN( cval_obj_upgd_wearloc )
{
	OBJ_INDEX_DATA *pObj;
	EDIT_OBJ( ch, pObj );

	if (IS_SET( pObj->wear_flags, ITEM_TAKE ))
		return 0;

	return 1;
}
UPGD_FUN( obj_upgd_wearloc )
{
	OBJ_INDEX_DATA *pObj;
	int i;
	long restricted_wearloc = ITEM_TAKE | ITEM_WEAR_BACK | ITEM_WEAR_FLOAT;
	EDIT_OBJ( ch, pObj );

	if (!IS_SET( pObj->wear_flags, ITEM_TAKE ))
	{
		send_to_char("You cannot change the wear loc on this item.\n\r", ch);
		return UPGRADE_REQ_ARG;
	}

	if (argument[0] == '\0')
	{
		send_to_char(
				"{WAvailable wear locations (original wearloc will be replaced):\n\r{w  ",
				ch);
		for (i = 0; wear_flags[i].name; i++)
		{
			if (IS_SET( restricted_wearloc, wear_flags[i].bit ))
				continue;
			send_to_char(wear_flags[i].name, ch);
			send_to_char(" ", ch);
		}
		send_to_char("\n\r", ch);
		return UPGRADE_REQ_ARG;
	}

	for (i = 0; wear_flags[i].name; i++)
		if (!str_prefix(argument, wear_flags[i].name))
			break;

	if (wear_flags[i].name == NULL)
	{
		send_to_char("Wear flag not found.\n\r", ch);
		return UPGRADE_REQ_ARG;
	}

	if (IS_SET( restricted_wearloc, wear_flags[i].bit ))
	{
		send_to_char("Invalid wear location.\n\r", ch);
		return UPGRADE_REQ_ARG;
	}

	pObj->wear_flags = ITEM_TAKE | wear_flags[i].bit;
	return UPGRADE_SUCCESS;
}

/* Start of room upgrade functions */
CALC_FUN( cval_room_upgd_hpregen )
{
	ROOM_INDEX_DATA *pRoom;
	EDIT_ROOM( ch, pRoom );
	return pRoom->heal_rate;
}
UPGD_FUN( room_upgd_hpregen )
{
	CLAN_DATA *clan;
	ROOM_INDEX_DATA *pRoom;

	EDIT_ROOM( ch, pRoom );
	EDIT_CLAN( ch, clan );

	if (clan->u_quantity < 1)
		clan->u_quantity = 1;

	pRoom->heal_rate += (clan->u_quantity) * (upgrade->quantity);
	return UPGRADE_SUCCESS;
}

CALC_FUN( cval_room_upgd_manaregen )
{
	ROOM_INDEX_DATA *pRoom;
	EDIT_ROOM( ch, pRoom );
	return pRoom->mana_rate;
}
UPGD_FUN( room_upgd_manaregen )
{
	CLAN_DATA *clan;
	ROOM_INDEX_DATA *pRoom;

	EDIT_ROOM( ch, pRoom );
	EDIT_CLAN( ch, clan );

	if (clan->u_quantity < 1)
		clan->u_quantity = 1;

	pRoom->mana_rate += (clan->u_quantity) * (upgrade->quantity);
	return UPGRADE_SUCCESS;
}

CALC_FUN( cval_room_upgd_flag )
{
	ROOM_INDEX_DATA *pRoom;
	EDIT_ROOM( ch, pRoom );
	return ((int) IS_SET( pRoom->room_flags, upgrade->specific[0] ));
}
UPGD_FUN( room_upgd_flag )
{
	CLAN_DATA *clan;
	ROOM_INDEX_DATA *pRoom;
	EDIT_CLAN( ch, clan );
	EDIT_ROOM( ch, pRoom );

	if (IS_SET( pRoom->room_flags, upgrade->specific[0] ))
	{
		if (!str_cmp(argument, "remove"))
		{
			REMOVE_BIT( pRoom->room_flags, upgrade->specific[0] );
			clan->platinum = clan->platinum + upgrade->platinum / 3;
			clan->qps = clan->qps + upgrade->qps / 3;
		}
		else
		{
			send_to_char(
					"Flag already set - append {Rremove{x as an argument to remove it.\n\r",
					ch);
			return UPGRADE_REQ_ARG;
		}

		return UPGRADE_SUCCESS;
	}

	SET_BIT( pRoom->room_flags, upgrade->specific[0] );
	return UPGRADE_SUCCESS;
}

/**************************************** OLC *****************************************/

/* Finish editing clan and save clan */
void cedit_done(CHAR_DATA *ch)
{
	CLAN_DATA *clan;

	EDIT_CLAN( ch, clan );

	if (clan->upgrade != NULL)
	{
		clan->upgrade = NULL;
		clan->xupgrade = NULL;
		clan->u_quantity = 0;
		clan->u_rost = NULL;
		send_to_char("Pending upgrade cleared.\n\r", ch);
	}

	SET_EDIT_ITEM( ch, NULL );
	SET_EDIT_CLAN( ch, NULL );
	SET_EDIT_TYPE( ch, ED_NONE );
	save_all_clans();
}

/* Show the available cedit commands */
void show_cedit_commands(CHAR_DATA *ch, char *argument,
		const struct cedit_cmd_type *table)
{
	BUFFER *output = new_buf();
	ROSTER_DATA *roster;
	char buf[MAX_STRING_LENGTH], trustStr[20];
	int i, trust;
	bool immortal = IS_IMMORTAL( ch ), description = FALSE;

	GET_ROSTER( ch, roster );

	if (argument[0] != '\0' && !str_prefix(argument, "help"))
		description = TRUE;

	if (immortal)
		trust = get_trust(ch);
	else
		trust = roster->trust;

	add_buf(output, "Available commands:\n\r");

	for (i = 1; table->cmd[0] != '\0'; table++)
	{
		if ((immortal && table->trust == TRUST_NONE) || (!immortal
				&& table->m_trust == TRUST_NONE))
			continue;

		if ((immortal && trust < table->trust) || (!immortal && trust
				< table->m_trust))
			continue;

		if (immortal)
			sprintf(trustStr, "{C%d", table->trust);
		else
			sprintf(trustStr, "%s", table->m_trust == TRUST_ALL ? "{GALL"
					: table->m_trust == TRUST_MEMBER ? "{gMEM" : table->m_trust
							== TRUST_VLR ? "{CVLR" : table->m_trust
							== TRUST_LDR ? "{RLDR" : "{w---");

		if (description)
			sprintf(buf, "{B[%s{B]{w %-12s {B:{w %s\n\r", trustStr, table->cmd,
					table->description);
		else
			sprintf(buf, "{B[%s{B]{w %-12s ", trustStr, table->cmd);
		add_buf(output, buf);

		if (!description && i % 4 == 0 && i > 0)
			add_buf(output, "\n\r");
		i++;
	}
	if (!description && (i - 1) % 4 != 0)
		add_buf(output, "\n\r");

	if (i < 1)
		add_buf(output, "None.\n\r");

	if (!description)
		add_buf(output,
				"{WType \"{Gcommand help{W\" for a list of command descriptions.\n\r");

	add_buf(output, "{x");

	send_to_char(output->string, ch);
	free_buf(output);
}

/* Get clan mob to edit */
MOB_INDEX_DATA * get_cmob_by_type_name(CHAR_DATA *ch, char *name)
{
	CLAN_DATA *clan;
	int i;

	EDIT_CLAN( ch, clan );
	if (!clan)
		return NULL;

	for (i = 0; i < CMOB_MAX; i++)
	{
		if (clan->cMob[i] && !str_cmp(name, cmob_table[i].name))
			return clan->cMob[i];
	}

	return NULL;
}

CEDIT_FUN( cedit_create )
{
	CLAN_DATA *clan;
	char *name, buf[MAX_STRING_LENGTH];

	if (argument[0] == '\0')
	{
		send_to_char("Cedit syntax:  create <clan colour name>\n\r"
			"Normal syntax: cedit create <clan colour name>\n\r", ch);
		return FALSE;
	}
	if (strlen_color(argument) > MAX_CLAN_WHO_LENGTH)
	{
		sprintf(
				buf,
				"The length of the colour name cannot be longer than %d characters",
				MAX_CLAN_WHO_LENGTH );
		send_to_char(buf, ch);
		return FALSE;
	}

	clan = new_clan_data(TRUE);
	send_to_char("Clan created.\n\r", ch);

	free_string(clan->c_name);
	clan->c_name = str_dup(argument);

	name = strip_spaces(argument);
	name = strip_spec_char_col(name);
	if (*name)
	{
		free_string(clan->name);
		clan->name = str_dup(name);
	}
	else
		send_to_char("Don't forget to input a meaningful keyword name.\n\r", ch);

	clan->edit_time = current_time + (2 * TIME_HOUR);

	assign_clan_vnums(clan);
	append_clan_log(clan, ENTRY_CREATE, ch->name, "{wClan created");

	SET_EDIT_ITEM( ch, NULL );
	SET_EDIT_CLAN( ch, clan );
	SET_EDIT_TYPE( ch, ED_CEDIT );
	save_all_clans();

	cedit_show(ch, "");
	return TRUE;
}

CEDIT_FUN( cedit_delete )
{
	CHAR_DATA *mob, *mob_next;
	CLAN_DATA *clan, *findclan;
	MOB_INDEX_DATA *pMob, *pMob_prev;
	OBJ_DATA *obj, *obj_next;
	OBJ_INDEX_DATA *pObj, *pObj_prev;
	RESET_DATA *pReset, *prevReset = NULL;
	ROOM_INDEX_DATA *pRoom, *pRoom_prev;
	ROSTER_DATA *roster;
	int i, iHash;
	bool objMove = FALSE;

	EDIT_CLAN( ch, clan );

	if (argument[0] == '\0' || str_cmp(argument, clan->name))
	{
		send_to_char(
				"The argument to the command must equal the clan name.\n\r", ch);
		return FALSE;
	}

	if (clan->id == CLAN_NONE || clan->id == CLAN_LONER || clan->id
			== CLAN_PK_OUTCAST || clan->id == CLAN_NONPK_OUTCAST || clan->id
			== CLAN_IMMORTAL)
	{
		send_to_char("This clan is important and cannot be deleted.\n\r", ch);
		return FALSE;
	}

	for (roster = roster_list; roster != NULL; roster = roster->next)
	{
		if (roster->clan == NULL)
			continue;
		if (roster->clan == clan)
		{
			if (clan->pkill)
				roster->clan = get_clan_by_id(CLAN_LONER);
			else
				roster->clan = get_clan_by_id(CLAN_NONE);
			roster->trust = TRUST_ALL;
			if (roster->character && (IS_EDITING_CLAN( roster->character, clan ) ||
					roster->clan == clan))
			{
				cedit_done(roster->character);
				send_to_char("{RYou clan has just been deleted!{x\n\r", ch);
			}
		}
		if (roster->petition == clan)
		{
			roster->petition = NULL;
			if (roster->character)
				send_to_char(
						"{RThe clan you have petitioned for membership was just deleted.{x\n\r",
						ch);
		}
	}

	for (i = 0; i < max_pkhist_entries; i++)
	{
		if (pkhist_table[i].vClan != NULL && pkhist_table[i].vClan == clan)
			pkhist_table[i].vClan = NULL;

		if (pkhist_table[i].kClan != NULL && pkhist_table[i].kClan == clan)
			pkhist_table[i].kClan = NULL;
	}

	// Find all the objects/mobs in the world and delete them - kick anyone out of the hall
	// Not going to check for resets because if there are resets outside the clan's area, then some immortal has done something they shouldn't have done
	for (mob = char_list; mob != NULL; mob = mob_next)
	{
		mob_next = mob->next;
		if (mob->in_room && mob->in_room->vnum >= clan->vnum[0]
				&& mob->in_room->vnum <= clan->vnum[1] && !IS_NPC( mob )) // Move any players out
		{
			char_from_room(mob);
			char_to_room(mob, get_room_index(VNUM_MARKET_SQUARE));
			send_to_char(
					"{RYou have been transferred to the Market Square because the clan that owns this room is being deleted.\n\r"
						"{GHave a nice day!{x\n\r", mob);
			do_look(mob, "");
		}
		if (mob->pIndexData && mob->pIndexData->vnum >= clan->vnum[0]
				&& mob->pIndexData->vnum <= clan->vnum[1])
			extract_char(mob, TRUE );
	}
	for (i = clan->vnum[0]; i <= clan->vnum[1]; i++)
		reset_clan_obj(get_obj_index(i), TRUE );
	for (obj = object_list; obj != NULL; obj = obj_next)
	{
		obj_next = obj->next;
		if (obj->in_room != NULL && is_clan_room(obj->in_room, clan, TRUE ))
		{
			obj_from_room(obj);
			obj_to_room(obj, ch->in_room ? ch->in_room : get_room_index(
					VNUM_CLAN_PET_ROOM));
			objMove = TRUE;
		}
	}

	if (clan->entrance_vnum && (pRoom = get_room_index(clan->entrance_vnum))
			!= NULL)
	{
		for (i = 0; i < MAX_DIR; i++)
		{
			if (pRoom->exit[i] && pRoom->exit[i]->u1.to_room
					== clan->cRoom[CROOM_RECALL])
			{
				free_exit(pRoom->exit[i]);
				pRoom->exit[i] = NULL;
				break;
			}
		}
		save_area(pRoom->area);
	}

	for (i = clan->vnum[0]; i <= clan->vnum[1]; i++)
	{
		if ((pRoom = get_room_index(i)) != NULL)
		{
			iHash = pRoom->vnum % MAX_KEY_HASH;
			pRoom_prev = room_index_hash[iHash];

			if (pRoom_prev->next == NULL)
				room_index_hash[iHash] = NULL;
			else if (pRoom_prev == pRoom)
				room_index_hash[iHash] = pRoom->next;
			else
			{
				for (; pRoom_prev; pRoom_prev = pRoom_prev->next)
				{
					if (pRoom_prev->next == pRoom)
					{
						pRoom_prev->next = pRoom->next;
						break;
					}
				}
			}
			free_room_index(pRoom);
		}
	}

	if (clan->cMob[CMOB_PET] && (pRoom = get_room_index(VNUM_CLAN_PET_ROOM))
			!= NULL)
	{
		for (i = 0, pReset = pRoom->reset_first; pReset != NULL; prevReset
				= pReset, i++, pReset = pReset->next)
		{
			if (i > 0 && pRoom->reset_first == pReset) // Reset linked list doesn't end at null, but loops round - this will only happen if pet reset not found
				break;
			if (pReset->command != 'M' || pReset->arg1
					!= clan->cMob[CMOB_PET]->vnum)
				continue;

			if (prevReset == NULL)
			{
				prevReset = pRoom->reset_last;
				if (prevReset != NULL)
					prevReset->next = pReset->next;
				pRoom->reset_first = pReset->next;
			}
			else
			{
				if (pRoom->reset_last == pReset)
					pRoom->reset_last = prevReset;
				prevReset->next = pReset->next;
			}
			free_reset_data(pReset);
			break;
		}
	}

	for (i = clan->vnum[0]; i <= clan->vnum[1]; i++)
	{
		if ((pMob = get_mob_index(i)) != NULL)
		{
			iHash = pMob->vnum % MAX_KEY_HASH;
			pMob_prev = mob_index_hash[iHash];

			if (pMob_prev->next == NULL)
				mob_index_hash[iHash] = NULL;
			else if (pMob_prev == pMob)
				mob_index_hash[iHash] = pMob->next;
			else
			{
				for (; pMob_prev; pMob_prev = pMob_prev->next)
				{
					if (pMob_prev->next == pMob)
					{
						pMob_prev->next = pMob->next;
						break;
					}
				}
			}
			free_mob_index(pMob);
		}
	}

	for (i = clan->vnum[0]; i <= clan->vnum[1]; i++)
	{
		if ((pObj = get_obj_index(i)) != NULL)
		{
			iHash = pObj->vnum % MAX_KEY_HASH;
			pObj_prev = obj_index_hash[iHash];

			if (pObj_prev->next == NULL)
				obj_index_hash[iHash] = NULL;
			else if (pObj_prev == pObj)
				obj_index_hash[iHash] = pObj->next;
			else
			{
				for (; pObj_prev; pObj_prev = pObj_prev->next)
				{
					if (pObj_prev->next == pObj)
					{
						pObj_prev->next = pObj->next;
						break;
					}
				}
			}
			free_obj_index(pObj);
		}
	}

	append_clan_log(clan, ENTRY_DELETE, ch->name, "{y-{Y>{RDELETED{Y<{y-");

	for (findclan = clan_list; findclan != NULL; findclan = findclan->next)
	{
		if (findclan->enemy != NULL && findclan->enemy == clan)
			findclan->enemy = NULL;
		if (findclan->Penemy.data && findclan->Penemy.data == clan)
			findclan->Penemy.data = NULL;
	}

	if (clan == clan_list)
		clan_list = clan->next;
	else
	{
		for (findclan = clan_list; findclan != NULL; findclan = findclan->next)
		{
			if (findclan->next == clan)
			{
				findclan->next = clan->next;
				break;
			}
		}
	}

	SET_EDIT_ITEM( ch, NULL );
	SET_EDIT_CLAN( ch, NULL );
	SET_EDIT_TYPE( ch, ED_NONE );

	free_clan_data(clan);
	send_to_char("Clan deleted.\n\r", ch);
	save_all_clans();
	save_all_rosters();
	save_area(get_clan_area());

	if (objMove)
	{
		if (ch->in_room)
			do_get(ch, "all");
		else
			send_to_char(
					"{RSome objects have been moved to the clan pet reset room.{x\n\r",
					ch);
	}

	return TRUE;
}

CEDIT_FUN( cedit_deposit )
{
	CLAN_DATA *clan;
	ROSTER_DATA *roster;
	char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
	int value, prev;

	EDIT_CLAN( ch, clan );
	GET_ROSTER( ch, roster );

	if (argument[0] == '\0')
	{
		send_to_char("Cedit syntax: deposit <amount> questpoints\n\r"
			"              deposit <amount> platinum\n\r", ch);
		return FALSE;
	}

	argument = one_argument(argument, arg);

	if (!is_number(arg))
	{
		send_to_char("Please provide a numerical amount.\n\r", ch);
		return FALSE;
	}

	if ((value = atoi(arg)) <= 0 && !IS_IMMORTAL(ch))
	{
		send_to_char("Amount must be greater than zero.\n\r", ch);
		return FALSE;
	}

	if (argument[0] == '\0')
	{
		send_to_char("Please specify either questpoints or platinum.\n\r", ch);
		return FALSE;
	}
	if (!str_prefix(argument, "questpoints"))
	{
		if (ch->questpoints < value)
		{
			send_to_char("You don't have enough quest points!\n\r", ch);
			return FALSE;
		}
		sprintf(buf, "{R%d {YQ{yuest", value);
		clan->qps += value;
		ch->questpoints -= value;
		value *= POINT_VALUE_QP;
	}
	else if (!str_prefix(argument, "platinum"))
	{
		if (ch->platinum < value)
		{
			send_to_char("You don't have enough platinum!\n\r", ch);
			return FALSE;
		}
		sprintf(buf, "{R%d {CP{clatinum", value);
		clan->platinum += value;
		ch->platinum -= value;
		value *= POINT_VALUE_PLATINUM;
	}
	else
	{
		send_to_char("Please specify either questpoints or platinum.\n\r", ch);
		return FALSE;
	}

	append_clan_log(clan, ENTRY_DEPOSIT, ch->name, buf); // Log transaction

	// Bonus player for donating only if clan is theirs
	// (ie don't bonus immortals if the clan isn't the immortal clan)
	if (clan == roster->clan && (!IS_IMMORTAL( ch ) || (IS_IMMORTAL( ch )
			&& clan->id == CLAN_IMMORTAL)))
	{
		prev = get_r_rank_index(roster);
		roster->dPoints += value;

		sprintf(
				buf,
				"{GYou have received {Y%d{G donation points for your donation.{x\n\r",
				value);
		send_to_char(buf, ch);

		if ((value = get_r_rank_index(roster)) > prev) // Advanced in clan rank
		{
			sprintf(buf, "{GYour rank has advanced to {x%s{x!\n\r",
					clan->r_name[value]);
			send_to_char(buf, ch);
		}
	}

	return TRUE;
}

/* Clan description */
CEDIT_FUN( cedit_desc )
{
	CLAN_DATA *clan;
	EDIT_CLAN( ch, clan );

	if (!IS_IMMORTAL( ch ) && clan->edit_time < current_time)
	{
		send_to_char(
				"You do not have anymore time to edit ranks. Use {Rtime buy{x to buy more time.\n\r",
				ch);
		return FALSE;
	}

	if (argument[0] == '\0')
	{
		string_append(ch, &clan->description);
		append_clan_log(clan, ENTRY_CHANGED, ch->name,
				"Clan description changed.");
		return TRUE;
	}

	send_to_char("Cedit syntax: desc - set the clan description\n\r", ch);
	return FALSE;
}

/* Put a player in a clan */
CEDIT_FUN( cedit_guild )
{
	CLAN_DATA *clan;
	ROSTER_DATA *roster;
	char buf[MAX_STRING_LENGTH];

	EDIT_CLAN( ch, clan );

	if (argument[0] == '\0')
	{
		send_to_char(
				"Cedit syntax: guild <character> - guild character into this clan\n\r",
				ch);
		return FALSE;
	}

	for (roster = roster_list; roster != NULL; roster = roster->next)
	{
		if (IS_SET( roster->flags, ROST_DELETED ))
			continue;
		if (str_cmp(argument, roster->name))
			continue;

		// Check if already in the clan
		if (roster->clan == clan)
		{
			sprintf(buf, "That player is already in %s{x.\n\r", strip_bespaces(
					clan->c_name));
			send_to_char(buf, ch);
			return FALSE;
		}

		// Check max members
		if (!clan->independent && ((clan->pkill && clan->members
				>= MAX_PK_CLAN_MEMBERS) || (!clan->pkill && clan->members
				>= MAX_NONPK_CLAN_MEMBERS)))
		{
			sprintf(buf, "%s{x has enough members already.\n\r",
					strip_bespaces(clan->c_name));
			send_to_char(buf, ch);
			return FALSE;
		}

		// Lower number of members in previous clan if was in one and log
		if (!roster->clan->independent)
		{
			(roster->clan->members)--;
			sprintf(buf, "Removed from clan by {R%s{w.", ch->name);
			if ((roster->character && (roster->character != ch
					|| !IS_IMMORTAL( roster->character )))
					|| !roster->character)
				append_clan_log(roster->clan, ENTRY_GUILD, roster->name, buf);
		}

		roster->clan = clan;
		if (!clan->independent)
		{
			(clan->members)++; // Up member number in new clan if not independent
			roster->trust = TRUST_MEMBER; // Set trust as member

			// Log addition
			sprintf(buf, "Made member of clan by {R%s{w.", ch->name);

			if ((roster->character && (roster->character != ch
					|| !IS_IMMORTAL( roster->character )))
					|| !roster->character)
				append_clan_log(clan, ENTRY_GUILD, roster->name, buf);

		}
		else
			roster->trust = TRUST_ALL; // Trust to 0 if independent

		REMOVE_BIT( roster->flags, ROST_EXILE_GRACE ); // If in exile grace period, remove flag
		REMOVE_BIT( roster->flags, ROST_INACTIVE ); // If flagged inactive, unflag

		if (clan->id == CLAN_NONPK_OUTCAST || clan->id == CLAN_PK_OUTCAST)
			roster->penalty_time = current_time + TIME_WEEK; // Set penalty time if outcasted
		else
			roster->penalty_time = 0; // Clear penalty time if not outcasted

		if (roster->character) // Tell the guy whats happened
		{
			sprintf(buf, "You have been made a member of %s{x by %s.\n\r",
					strip_bespaces(clan->c_name), (roster->character
							&& roster->character == ch) ? "yourself" : ch->name);
			send_to_char(buf, roster->character);
		}

		if (roster->character == NULL || (roster->character
				&& roster->character != ch))
		{
			sprintf(buf, "{R%s{x has been made a member of %s{x.\n\r",
					roster->name, strip_bespaces(clan->c_name));
			send_to_char(buf, ch);
		}
		save_roster_data(roster);
		return TRUE;
	}
	send_to_char("Player not found.\n\r", ch);
	return FALSE;
}

CEDIT_FUN( cedit_indep )
{
	CLAN_DATA *clan;

	EDIT_CLAN( ch, clan );

	if (clan->id == CLAN_NONE || clan->id == CLAN_LONER || clan->id
			== CLAN_PK_OUTCAST || clan->id == CLAN_NONPK_OUTCAST || clan->id
			== CLAN_IMMORTAL)
	{
		send_to_char("This cannot be changed.\n\r", ch);
		return FALSE;
	}

	if (argument[0] == '\0')
	{
		send_to_char(
				"Cedit syntax: independent yes/no - sets the clan as a ranked clan\n\r",
				ch);
		return FALSE;
	}
	else if (!str_cmp(argument, "yes"))
	{
		clan->independent = TRUE;
		send_to_char("Clan made independent.\n\r", ch);
	}
	else if (!str_cmp(argument, "no"))
	{
		clan->independent = FALSE;
		send_to_char("Clan is no longer independent.\n\r", ch);
	}
	else
	{
		cedit_indep(ch, "");
		return FALSE;
	}
	return TRUE;
}

CEDIT_FUN( cedit_log )
{
	CLAN_DATA *clan;
	char arg[MAX_INPUT_LENGTH];
	EDIT_CLAN( ch, clan );

	if (argument[0] == '\0')
	{
		send_to_char("Cedit syntax: log show - show clan log\n\r", ch);
		return FALSE;
	}

	argument = one_argument(argument, arg);

	if (str_prefix(arg, "show"))
	{
		cedit_log(ch, "");
		return FALSE;
	}

	send_to_char("{WClan log entries:{x\n\r", ch);
	parse_clan_log(ch, clan, NULL, FILTER_NONE );

	return FALSE;
}

CEDIT_FUN( cedit_member )
{
	CLAN_DATA *clan;
	ROSTER_DATA *roster, *chRoster;
	char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];

	EDIT_CLAN( ch, clan );
	GET_ROSTER( ch, chRoster );

	if (argument[0] == '\0')
	{
		send_to_char(
				"Cedit syntax: member list                   - list current members\n\r"
					"              member trust <player> <level> - trust player to open up parts of the cedit sytem to them\n\r"
					"              member exile <player> <grace period in days/none>\n\r"
					"               {RNote:{x The grace period is optional, however once marked for exile, theres no turning back.\n\r"
					"                     During the grace period, the player may leave the clan voluntarily,\n\r"
					"                     incurring less of a point cost to them and placing them in the loner\n\r"
					"                     clan. If they do not leave, they are automatically exiled from the clan,\n\r"
					"                     placed in the outcast clan and would lose more donation points.\n\r"
					"                     Without a grace period specified, the latter happens straight away\n\r",
				ch);
		return FALSE;
	}

	argument = one_argument(argument, arg);

	if (!str_prefix(arg, "list"))
	{
		BUFFER *output = new_buf();
		int count = 0;

		add_buf(
				output,
				"{B+-----------------------------------------------------------------------------------------+\n\r");
		add_buf(
				output,
				"{B|{W No. {B|{W Trust {B|{W Rnk {B|{W Name         {B|{W K Pnts {B|{W D Pnts {B|{W Last log off             {B|{W Penalty {B|\n\r");
		add_buf(
				output,
				"{B+-----------------------------------------------------------------------------------------+\n\r");

		for (roster = roster_list; roster != NULL; roster = roster->next)
		{
			if (IS_SET( roster->flags, ROST_DELETED ) || roster->clan != clan)
				continue;

			count++;
			sprintf(
					buf,
					"{B|{R %3d {B| %s {B|{x %s {B|{w %s {B|{R %6ld {B|{Y %6ld {B|{%c %-24.24s {B|",
					count, roster->trust == TRUST_LDR ? " {GLDR "
							: (roster->trust == TRUST_VLR ? " {MVLR "
									: " {w--- "), clan->independent ? "{D---"
							: clan->r_who[get_r_rank_index(roster)],
					end_string(roster == chRoster ? "{WYou" : capitalize(
							roster->name), 12), roster->pkilldata->kPoints,
					roster->dPoints, roster->character ? 'G' : 'w',
					roster->character ? "Online" : ctime(&(roster->last_login)));
			add_buf(output, buf);

			if (roster->penalty_time > 0)
				sprintf(buf, "{R %s {B-{R %2ld {B|\n\r", get_pen_code(roster),
						(current_time - roster->penalty_time) / TIME_DAY );
			else
				strcpy(buf, "{G None    {B|\n\r");

			add_buf(output, buf);
		}
		add_buf(
				output,
				"{B+-----------------------------------------------------------------------------------------+{x\n\r");

		if (count > 0)
			page_to_char(output->string, ch);
		else
			send_to_char("There are no current clan members.\n\r", ch);

		free_buf(output);
	}

	else if (!str_prefix(arg, "trust"))
	{
		int trust;

		if (argument[0] == '\0')
		{
			send_to_char("Who would you like to trust?\n\r", ch);
			return FALSE;
		}

		argument = one_argument(argument, arg);

		if (argument[0] == '\0')
		{
			if (IS_IMMORTAL( ch ))
				send_to_char(
						"Valid trust levels are {Rleader{x, {Rvice{x and {Rmember{x.\n\r",
						ch);
			else
				send_to_char(
						"Valid trust levels are {Rvice{x and {Rmember{x.\n\r",
						ch);
			return FALSE;
		}
		else if (!str_prefix(argument, "leader") && IS_IMMORTAL( ch ))
			trust = TRUST_LDR;
		else if (!str_prefix(argument, "vice"))
			trust = TRUST_VLR;
		else if (!str_prefix(argument, "member"))
			trust = TRUST_MEMBER;
		else
			return cedit_member(ch, "trust a ");

		for (roster = roster_list; roster != NULL; roster = roster->next)
		{
			if (IS_SET( roster->flags, ROST_DELETED ))
				continue;
			if (roster->clan == clan && !str_prefix(arg, roster->name))
				break;
		}

		if (!roster)
		{
			send_to_char("Player not found.\n\r", ch);
			return FALSE;
		}
		else if (roster == chRoster)
		{
			send_to_char("You trust yourself enough... or do you?\n\r", ch);
			return FALSE;
		}

		if (IS_SET( roster->flags, ROST_EXILE_GRACE ))
		{
			send_to_char("They have been marked for exile.\n\r", ch);
			return FALSE;
		}

		roster->trust = trust;
		send_to_char("Trust level changed.\n\r", ch);

		sprintf(buf, "{R%s{w trusted to {R%s", roster->name,
				trust == TRUST_VLR ? "vice-leader"
						: trust == TRUST_LDR ? "leader" : "member");

		append_clan_log(clan, ENTRY_TRUST, ch->name, buf);

		strcat(buf, "{x\n\r");
		send_to_char(buf, ch);
	}

	else if (!str_cmp(arg, "exile"))
	{
		char p_name[MAX_INPUT_LENGTH];
		int grace;

		argument = one_argument(argument, p_name);

		if (p_name[0] == '\0')
		{
			send_to_char("Please provide a player name.\n\r", ch);
			return FALSE;
		}

		if (!str_cmp(argument, "none"))
			grace = 0;
		else if (is_number(argument))
		{
			grace = atoi(argument);
			if (grace <= 0)
			{
				send_to_char(
						"Please specify a number of days grace over 0, or type {Rnone{x if you don't want to give any.\n\r",
						ch);
				return FALSE;
			}
			else if (grace >= 7)
			{
				send_to_char(
						"The number of days grace must be less than a week.\n\r",
						ch);
				return FALSE;
			}
		}
		else
		{
			send_to_char(
					"Please specify the grace period in days or type {Rnone{x.\n\r",
					ch);
			return FALSE;
		}

		for (roster = roster_list; roster != NULL; roster = roster->next)
		{
			if (IS_SET( roster->flags, ROST_DELETED ))
				continue;
			if (roster->clan == clan && !str_cmp(roster->name, p_name))
				break;
		}

		if (!roster)
		{
			send_to_char("Player not found.\n\r", ch);
			return FALSE;
		}
		else if (roster == chRoster)
		{
			send_to_char("You can't exile yourself.\n\r", ch);
			return FALSE;
		}

		if (IS_SET( roster->flags, ROST_EXILE_GRACE ))
		{
			send_to_char("They have already been marked for exile.\n\r", ch);
			return FALSE;
		}

		if (grace > 0)
		{
			REMOVE_BIT( roster->flags, ROST_INACTIVE );
			SET_BIT( roster->flags, ROST_EXILE_GRACE );
			roster->penalty_time = current_time + (grace * TIME_DAY);
			roster->trust = TRUST_ALL; // Won't get to wear clan eq or make clan donations

			sprintf(
					buf,
					"%s will be exiled in %d days if they do not leave voluntarily.\n\r",
					roster->name, grace);
			send_to_char(buf, ch);

			if (roster->character)
			{
				sprintf(
						buf,
						"{Y>{y>{Y>{R You must voluntarily leave the clan within{W %d {Rdays or you will be exiled. {Y<{y<{Y<{x\n\r",
						grace);
				send_to_char(buf, ch);
				send_to_char(
						"Use the {Gloner{x command to leave the clan. You will lose fewer donation points if you do so.\n\r",
						ch);
			}

			sprintf(
					buf,
					"{R%s{w marked for exile with grace period set at {R%d{w days.",
					roster->name, grace);
		}
		else
		{
			sprintf(buf, "%s has been exiled from the clan.\n\r", roster->name);
			send_to_char(buf, ch);

			sprintf(buf, "{R%s{w exiled with {Rno{w grace period set.",
					roster->name);
			exile_player(roster);
		}

		append_clan_log(clan, ENTRY_EXILE, ch->name, buf);

	}
	else
	{
		cedit_member(ch, "");
		return FALSE;
	}

	return TRUE;
}

CEDIT_FUN( cedit_mob )
{
	CLAN_DATA *clan;
	char arg[MAX_INPUT_LENGTH];
	int i;

	EDIT_CLAN( ch, clan );

	if (clan->independent)
	{
		send_to_char("Only proper clans may have assets.\n\r", ch);
		return FALSE;
	}

	if (argument[0] == '\0')
	{
		send_to_char(
				"Cedit syntax: mob edit <type>    - edit the specified mob(e.g. blacksmith)\n\r"
					"              mob buy <item>     - buy a mob\n\r"
					"              mob req <item>     - find out what the mob needs and costs to buy\n\r"
					"              mob list           - list the available mobs for sale\n\r",
				ch);
		return FALSE;
	}

	argument = one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		cedit_mob(ch, "");
		return FALSE;
	}

	if (!str_prefix(arg, "edit"))
	{
		for (i = 0; i < CMOB_MAX; i++)
			if (!str_prefix(argument, cmob_table[i].name))
				break;

		if (i == CMOB_MAX)
		{
			send_to_char("Mobile type not found.\n\r", ch);
			return FALSE;
		}
		if (clan->cMob[i] == NULL)
		{
			send_to_char("The clan does not have that type of mobile.\n\r", ch);
			return FALSE;
		}

		SET_EDIT_ITEM( ch, clan->cMob[i] );
		SET_EDIT_TYPE( ch, ED_MCEDIT );
	}
	else if (!str_prefix(arg, "buy"))
		return upgrade_process(ch, argument, mob_prices, TRUE );
	else if (!str_prefix(arg, "list"))
	{
		upgrade_list_table(ch, mob_prices);
		return FALSE;
	}
	else if (!str_prefix(arg, "req"))
		return upgrade_process(ch, argument, mob_prices, FALSE );
	else
		cedit_mob(ch, "");

	return FALSE;
}

CEDIT_FUN( cedit_name )
{
	CLAN_DATA *clan;
	char buf[MAX_STRING_LENGTH], arg[MAX_INPUT_LENGTH], *string;
	EDIT_CLAN( ch, clan );

	if (!IS_IMMORTAL( ch ) && clan->edit_time < current_time)
	{
		send_to_char(
				"You do not have anymore time to edit ranks. Use {Rtime buy{x to buy more time.\n\r",
				ch);
		return FALSE;
	}

	if (argument[0] == '\0')
	{
		send_to_char(
				"Cedit syntax: name <clan who name>       - set the clan who name\n\r",
				ch);
		if (IS_IMMORTAL( ch ))
			send_to_char(
					"              name fix <non-colour name> - set the keyword name if autogeneration from the who name goes bad\n\r",
					ch);
		return FALSE;
	}

	one_argument(argument, arg);

	if (IS_IMMORTAL( ch ) && !str_cmp(arg, "fix"))
	{
		argument = one_argument(argument, arg);

		if (argument[0] == '\0')
		{
			cedit_name(ch, "");
			return FALSE;
		}

		string = strip_spec_char_col(argument);
		string = strip_spaces(string);

		if (strlen(string) > MAX_CLAN_WHO_LENGTH)
		{
			sprintf(
					buf,
					"Clan keyword names cannot be longer than %d characters(not including colours).\n\r",
					MAX_CLAN_WHO_LENGTH );
			send_to_char(buf, ch);
			return FALSE;
		}

		free_string(clan->name);
		clan->name = str_dup(string);

		sprintf(buf, "Clan keyword name changed to {R%s{x\n\r", string);
		send_to_char(buf, ch);
		append_clan_log(clan, ENTRY_CHANGED, ch->name, buf);
		return TRUE;
	}

	if (strlen_color(argument) > MAX_CLAN_WHO_LENGTH)
	{
		sprintf(
				buf,
				"Clan who names cannot be longer than %d characters(not including colours).\n\r",
				MAX_CLAN_WHO_LENGTH );
		send_to_char(buf, ch);
		return FALSE;
	}

	string = strip_spec_char_col(argument);
	string = strip_spaces(string);

	free_string(clan->c_name);
	clan->c_name = str_dup(argument);

	free_string(clan->name);
	clan->name = str_dup(string);

	sprintf(buf,
			"Clan name changed to %s{x, keyword name auto-changed to {R%s{x.",
			strip_bespaces(clan->c_name), clan->name);
	append_clan_log(clan, ENTRY_CHANGED, ch->name, buf);
	strcat(buf, "\n\r");
	send_to_char(buf, ch);

	return TRUE;
}

CEDIT_FUN( cedit_obj )
{
	CLAN_DATA *clan;
	OBJ_INDEX_DATA *pObj;
	char arg[MAX_INPUT_LENGTH];
	int vnum;

	EDIT_CLAN( ch, clan );

	if (clan->independent)
	{
		send_to_char("Only proper clans may have assets.\n\r", ch);
		return FALSE;
	}

	if (argument[0] == '\0')
	{
		send_to_char(
				"Cedit syntax: obj edit <number>  - edit the specified obj vnum\n\r"
					"              obj buy <item>     - buy a obj\n\r"
					"              obj req <item>     - find out what the obj needs and costs to buy\n\r"
					"              obj list           - list the available objs for sale\n\r",
				ch);
		return FALSE;
	}

	argument = one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		cedit_obj(ch, "");
		return FALSE;
	}

	if (!str_prefix(arg, "edit"))
	{
		if (!is_number(argument))
		{
			cedit_obj(ch, "");
			return FALSE;
		}
		vnum = atoi(argument);

		if ((pObj = get_obj_index(vnum)) == NULL)
		{
			send_to_char("Object not found.\n\r", ch);
			return FALSE;
		}

		if (!is_full_clan_obj_index(pObj, clan, TRUE ))
		{
			send_to_char("Object not found.\n\r", ch);
			return FALSE;
		}

		SET_EDIT_ITEM( ch, pObj ); // in edit part of function
		SET_EDIT_TYPE( ch, ED_OCEDIT );
	}
	else if (!str_prefix(arg, "buy"))
		return upgrade_process(ch, argument, obj_prices, TRUE );
	else if (!str_prefix(arg, "list"))
	{
		upgrade_list_table(ch, obj_prices);
		return FALSE;
	}
	else if (!str_prefix(arg, "req"))
		return upgrade_process(ch, argument, obj_prices, FALSE );
	else
		cedit_obj(ch, "");

	return FALSE;
}

CEDIT_FUN( cedit_petition )
{
	CLAN_DATA *clan;
	ROSTER_DATA *roster;
	char buf[MAX_STRING_LENGTH], arg[MAX_STRING_LENGTH];
	;
	int count = 0;
	bool fAccept = FALSE, fReject = FALSE;

	EDIT_CLAN( ch, clan );

	if (argument[0] == '\0')
	{
		send_to_char(
				"Cedit syntax: petition list                 - lists current petitions.\n\r"
					"              petition accept <number/name> - accept petition.\n\r"
					"              petition reject <number/name> - reject petition.\n\r",
				ch);
		return FALSE;
	}

	argument = one_argument(argument, arg);

	if (!str_prefix(arg, "list"))
	{
		BUFFER *output = new_buf();
		for (roster = roster_list; roster != NULL; roster = roster->next)
		{
			if (IS_SET( roster->flags, ROST_DELETED ))
				continue;
			if (roster->petition != clan)
				continue;

			count++;
			sprintf(buf, "{R%3d{B){w %10s {B- {w%6ld KP\n\r", count,
					roster->name, roster->pkilldata->kPoints);
			add_buf(output, buf);
		}
		add_buf(output, "{x");
		if (count > 0)
			send_to_char(output->string, ch);
		else
			send_to_char("There are no current petitions.\n\r", ch);

		free_buf(output);
		return TRUE;
	}
	if (!str_prefix(arg, "accept"))
		fAccept = TRUE;
	else if (!str_prefix(arg, "reject"))
		fReject = TRUE;

	if (fAccept || fReject)
	{
		int numArg = 0;

		if (fAccept && ((clan->pkill && clan->members >= MAX_PK_CLAN_MEMBERS)
				|| (!clan->pkill && clan->members >= MAX_NONPK_CLAN_MEMBERS))
				&& !clan->independent)
		{
			send_to_char("You have enough members already!\n\r", ch);
			return FALSE;
		}

		if (argument[0] == '\0')
		{
			send_to_char(
					"Please specify the petition number or name of the person.\n\r",
					ch);
			return FALSE;
		}

		if (is_number(argument))
			numArg = atoi(argument);

		for (roster = roster_list; roster != NULL; roster = roster->next)
		{
			if (IS_SET( roster->flags, ROST_DELETED ))
				continue;
			if (roster->petition != clan)
				continue;

			count++;

			if (numArg == count || !str_prefix(argument, roster->name))
			{
				roster->petition = NULL;
				if (fAccept)
				{
					if (roster->clan && !roster->clan->independent)
						roster->clan->members--;
					roster->clan = clan;
					roster->trust = TRUST_MEMBER;
					(clan->members)++;
					send_to_char("Petition accepted.\n\r", ch);

					if (roster->character) // Notify player if online
					{
						sprintf(
								buf,
								"{GCongratulations! You are now a member of clan %s{G!{x\n\r",
								strip_bespaces(clan->c_name));
						send_to_char(buf, roster->character);
					}

					sprintf(buf, "{R%s{w is granted clan membership.",
							roster->name);
					append_clan_log(clan, ENTRY_ACCEPT, ch->name, buf);

				}
				else
				{
					send_to_char("Petition rejected.\n\r", ch);
					if (roster->character) // Notify player if online
					{
						sprintf(
								buf,
								"{RYour petition to clan %s{R for membership was rejected.{x\n\r",
								strip_bespaces(clan->c_name));
						send_to_char(buf, roster->character);
					}
				}

				return TRUE;
			}
		}
		send_to_char("Petition request not found.\n\r", ch);
		return FALSE;
	}

	cedit_petition(ch, "");
	return FALSE;
}

CEDIT_FUN( cedit_pkill )
{
	CLAN_DATA *clan;

	EDIT_CLAN( ch, clan );

	if (clan->id == CLAN_NONE || clan->id == CLAN_LONER || clan->id
			== CLAN_PK_OUTCAST || clan->id == CLAN_NONPK_OUTCAST || clan->id
			== CLAN_IMMORTAL)
	{
		send_to_char("This cannot be changed.\n\r", ch);
		return FALSE;
	}

	if (argument[0] == '\0')
	{
		send_to_char(
				"Cedit syntax: pkill yes/no - sets the clan as a pkill clan or not\n\r",
				ch);
		return FALSE;
	}
	else if (!str_cmp(argument, "yes"))
	{
		clan->pkill = TRUE;
		send_to_char("Clan made pkill.\n\r", ch);
	}
	else if (!str_cmp(argument, "no"))
	{
		clan->pkill = FALSE;
		send_to_char("Clan is no longer a pkill clan.\n\r", ch);
	}
	else
	{
		cedit_pkill(ch, "");
		return FALSE;
	}
	return TRUE;
}

CEDIT_FUN( cedit_rank )
{
	CLAN_DATA *clan;
	char arg[MAX_INPUT_LENGTH], r_name[MAX_STRING_LENGTH];
	int rank;
	EDIT_CLAN( ch, clan );

	if (!IS_IMMORTAL( ch ) && clan->edit_time < current_time)
	{
		send_to_char(
				"You do not have anymore time to edit ranks. Use {Rtime buy{x to buy more time.\n\r",
				ch);
		return FALSE;
	}

	if (argument[0] == '\0')
	{
		send_to_char(
				"Cedit syntax: rank <rank number> who <name>  - sets the who name of the rank\n\r"
					"              rank <rank number> name <name> - sets the full name of the rank\n\r",
				ch);
		return FALSE;
	}
	argument = one_argument(argument, arg);

	if (!is_number(arg))
	{
		send_to_char("Please specify a numerical rank.\n\r", ch);
		return FALSE;
	}
	rank = atoi(arg);

	if (rank < 0 || rank > TOTAL_CLAN_RANK)
	{
		send_to_char("Please specify a valid rank number.\n\r", ch);
		return FALSE;
	}

	argument = one_argument(argument, arg);

	if (!str_cmp(arg, "who"))
	{
		strcpy(r_name, strip_spaces(argument));
		if (strlen_color(r_name) > 3)
		{
			send_to_char(
					"Rank who names must not be longer than 3 characters(without colours).\n\r",
					ch);
			return FALSE;
		}
		free_string(clan->r_who[rank]);
		clan->r_who[rank] = str_dup(r_name);

		sprintf(r_name, "Rank {R%d{x who name changed to %s{x.", rank,
				clan->r_who[rank]);
		append_clan_log(clan, ENTRY_CHANGED, ch->name, r_name);
		strcat(r_name, "\n\r");
		send_to_char(r_name, ch);
	}
	else if (!str_cmp(arg, "name"))
	{
		if (strlen_color(argument) > MAX_CLAN_RANK_LENGTH)
		{
			sprintf(
					r_name,
					"Full rank names must not exceed %d characters(without colours).\n\r",
					MAX_CLAN_RANK_LENGTH );
			send_to_char(r_name, ch);
			return FALSE;
		}
		free_string(clan->r_name[rank]);
		clan->r_name[rank] = str_dup(argument);

		sprintf(r_name, "Rank {R%d{x full name changed to %s{x.", rank,
				clan->r_name[rank]);
		append_clan_log(clan, ENTRY_CHANGED, ch->name, r_name);
		strcat(r_name, "\n\r");
		send_to_char(r_name, ch);
	}
	else
	{
		cedit_rank(ch, "");
		return FALSE;
	}

	return TRUE;
}

CEDIT_FUN( cedit_room )
{
	CLAN_DATA *clan;
	char arg[MAX_INPUT_LENGTH];
	int i;

	EDIT_CLAN( ch, clan );

	if (clan->independent)
	{
		send_to_char("Only proper clans may have assets.\n\r", ch);
		return FALSE;
	}

	if (argument[0] == '\0')
	{
		send_to_char(
				"Cedit syntax: room edit <type>    - edit the specified room(e.g. smithy)\n\r"
					"              room buy <item>     - buy a room\n\r"
					"              room req <item>     - find out what the room needs and costs to buy\n\r"
					"              room list           - list the available rooms for sale\n\r",
				ch);
		return FALSE;
	}

	argument = one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		cedit_room(ch, "");
		return FALSE;
	}

	if (!str_prefix(arg, "edit"))
	{
		for (i = 0; i < CROOM_MAX; i++)
			if (!str_prefix(argument, croom_table[i].name))
				break;

		if (i == CROOM_MAX)
		{
			send_to_char("Room type not found.\n\r", ch);
			return FALSE;
		}
		if (clan->cRoom[i] == NULL)
		{
			send_to_char("The clan does not have that type of room.\n\r", ch);
			return FALSE;
		}

		SET_EDIT_ITEM( ch, clan->cRoom[i] );
		SET_EDIT_TYPE( ch, ED_RCEDIT );
	}
	else if (!str_prefix(arg, "buy"))
		return upgrade_process(ch, argument, room_prices, TRUE );
	else if (!str_prefix(arg, "list"))
	{
		upgrade_list_table(ch, room_prices);
		return FALSE;
	}
	else if (!str_prefix(arg, "req"))
		return upgrade_process(ch, argument, room_prices, FALSE );
	else
		cedit_room(ch, "");

	return FALSE;
}

CEDIT_FUN( cedit_show )
{
	BUFFER *output = new_buf();
	CLAN_DATA *clan;
	OBJ_INDEX_DATA *pObj;
	char buf[MAX_STRING_LENGTH], borderbuf[MAX_STRING_LENGTH], *temp,
			temp2[MAX_STRING_LENGTH];
	int i, time;
	bool found;

	EDIT_CLAN( ch, clan );

	add_buf(
			output,
			"{B+- {GCEDIT MAIN {B-----------------------------------------------------------+\n\r");

	sprintf(buf, "{WUID: {R%d", clan->id);
	sprintf(borderbuf, "{B| %s {B|\n\r", end_string(buf, 70));
	add_buf(output, borderbuf);

	sprintf(buf, "{WMembers:{w %2d  {WPKill: %s  {WIndependent: {w%s",
			clan->members, clan->pkill ? "{ryes" : "{wno",
			clan->independent ? "yes" : "no");
	sprintf(borderbuf, "{B| %s {B|\n\r", end_string(buf, 70));
	add_buf(output, borderbuf);

	if (IS_IMMORTAL( ch ))
	{
		sprintf(buf, "{WLower clan vnum: {w%7d  {WUpper clan vnum: {w%7d",
				clan->vnum[0], clan->vnum[1]);
		sprintf(borderbuf, "{B| %s {B|\n\r", end_string(buf, 70));
		add_buf(output, borderbuf);
	}

	if (clan->enemy)
	{
		sprintf(buf, "{WClan enemy: {x%s", strip_bespaces(clan->enemy->c_name));
		sprintf(borderbuf, "{B| %s {B|\n\r", end_string(buf, 70));
		add_buf(output, borderbuf);
	}

	if (clan->Penemy.data)
	{
		sprintf(buf, "{WClan petitioned to be enemy: {x%s", strip_bespaces(
				clan->Penemy.data->c_name));
		sprintf(borderbuf, "{B| %s {B|\n\r", end_string(buf, 70));
		add_buf(output, borderbuf);
	}

	time = (clan->edit_time - current_time) / TIME_MINUTE;
	if (time > 0)
	{
		sprintf(
				buf,
				"{B|-{G Basic data ({R%4d {gminutes editing time{G){B -------------------------------|\n\r",
				time);
		add_buf(output, buf);
	}
	else
		add_buf(
				output,
				"{B|-{G Basic data {B-----------------------------------------------------------|\n\r");

	sprintf(borderbuf, "%s", end_string(clan->c_name, MAX_CLAN_WHO_LENGTH ));
	sprintf(buf, "{W Clan name:{B [{w%s{B] {W Clan who name: {B[{x%s{B]",
			end_string(clan->name, MAX_CLAN_WHO_LENGTH ), borderbuf);
	sprintf(borderbuf, "{B| %s {B|\n\r", end_string(buf, 70));
	add_buf(output, borderbuf);

	for (i = (TOTAL_CLAN_RANK - 1); i >= 0; i--)
	{
		sprintf(borderbuf, "%s", end_string(clan->r_who[i], 3));
		sprintf(buf,
				"{W  Rank {w%d {B[{x%s{B] [{x%s{B]          {w%6d {WPoints", i,
				borderbuf, end_string(clan->r_name[i], MAX_CLAN_RANK_LENGTH ),
				clan_rank_table[i].points);
		sprintf(borderbuf, "{B| %s {B|\n\r", end_string(buf, 70));
		add_buf(output, borderbuf);
	}

	for (temp = clan->description, i = 0; *temp; i++, temp++)
	{
		if ((*temp == '\n' && *(temp + 1) == '\r') || (*temp == '\r' && *(temp
				+ 1) == '\n'))
		{
			temp2[i] = ' ';
			temp++;
			continue;
		}
		else if (*temp == '\n' || *temp == '\r')
		{
			temp2[i] = ' ';
			continue;
		}

		temp2[i] = *temp;
	}
	temp2[i] = '\0';

	temp = temp2;
	temp = length_argument(temp, borderbuf, 55);
	sprintf(buf, "  {WDescription: {w%s", borderbuf);
	sprintf(borderbuf, "{B| %s {B|\n\r", end_string(buf, 70));
	add_buf(output, borderbuf);

	while (*temp != '\0')
	{
		temp = length_argument(temp, buf, 55);
		sprintf(borderbuf, "{B|                {w%s {B|\n\r", end_string(buf,
				55));
		add_buf(output, borderbuf);
	}

	add_buf(
			output,
			"{B|-{G Funds {B----------------------------------------------------------------|\n\r");

	sprintf(buf, "{W  Platinum:     {w%8ld  {WQuest points:     {w%8ld",
			clan->platinum, clan->qps);
	sprintf(borderbuf, "{B| %s {B|\n\r", end_string(buf, 70));
	add_buf(output, borderbuf);

	add_buf(
			output,
			"{B|-{G Skills {B---------------------------------------------------------------|\n\r");

	for (found = FALSE, i = 0; i < 2; i++) {
		if (clan->skills[i] < 0)
			continue;
		found = TRUE;
		sprintf(buf, "{x  %s {W: %s {B-{w %s", end_string(
				clan_skill_table[clan->skills[i]].name, 21),
				clan_skill_table[clan->skills[i]].major ? "{CMajor" : "{cMinor",
				clan->r_name[get_p_rank_index(clan_skill_table[clan->skills[i]].points)]);
		sprintf(borderbuf, "{B| %s {B|\n\r", end_string(buf, 70));
		add_buf(output, borderbuf);
	}

	if (!found)
		add_buf(
				output,
				"{B|   {wNone.                                                                {B|\n\r");

	add_buf(
			output,
			"{B|-{G Rooms {B----------------------------------------------------------------|\n\r");

	for (found = FALSE, i = 0; i < CROOM_MAX; i++)
	{
		if (clan->cRoom[i] == NULL)
			continue;

		found = TRUE;

		sprintf(buf, "{W  %-22.22s: {B%s", capitalize(croom_table[i].name),
				clan->cRoom[i]->name);
		sprintf(borderbuf, "{B| %s {B|\n\r", end_string(buf, 70));
		add_buf(output, borderbuf);
	}

	if (!found)
		add_buf(
				output,
				"{B|   {wNone.                                                                {B|\n\r");

	add_buf(
			output,
			"{B|-{G Mobs {B-----------------------------------------------------------------|\n\r");

	for (found = FALSE, i = 0; i < CMOB_MAX; i++)
	{
		if (clan->cMob[i] == NULL)
			continue;

		found = TRUE;

		sprintf(buf, "{W  L{C%3d{W %-17.17s: {x%s", clan->cMob[i]->level,
				capitalize(cmob_table[i].name), clan->cMob[i]->short_descr);
		sprintf(borderbuf, "{B| %s {B|\n\r", end_string(buf, 70));
		add_buf(output, borderbuf);
	}

	if (!found)
		add_buf(
				output,
				"{B|   {wNone.                                                                {B|\n\r");

	add_buf(
			output,
			"{B|-{G Objects {B--------------------------------------------------------------|\n\r");

	for (found = FALSE, i = clan->vnum[0]; i <= clan->vnum[1]; i++)
	{
		if ((pObj = get_obj_index(i)) == NULL)
			continue;

		found = TRUE;

		sprintf(buf, "  {W{R%-5d{W %-16.16s: {x%s", i, capitalize(flag_string(
				type_flags, pObj->item_type)), pObj->short_descr);
		sprintf(borderbuf, "{B| %s {B|\n\r", end_string(buf, 70));
		add_buf(output, borderbuf);
	}

	if (!found)
		add_buf(
				output,
				"{B|   {wNone.                                                                {B|\n\r");

	add_buf(
			output,
			"{B+------------------------------------------------------------------------+{x\n\r");

	page_to_char(output->string, ch);
	free_buf(output);
	return FALSE;
}

CEDIT_FUN( cedit_skill )
{
	SET_EDIT_TYPE( ch, ED_SCEDIT );
	scedit_show(ch, argument);
	return TRUE;
}

CEDIT_FUN( cedit_time )
{
	CLAN_DATA *clan;
	char buf[MAX_STRING_LENGTH];
	int time;

	EDIT_CLAN( ch, clan );

	if (argument[0] == '\0')
	{
		send_to_char(
				"Cedit syntax: time buy    - buy another 30 minutes editing time for clan data\n\r"
					"              time status - see how long you have left to edit basic clan data\n\r"
					"        Note: another 30 minutes will cost 300 platinum.\n\r",
				ch);
		return FALSE;
	}
	else if (!str_prefix(argument, "buy"))
	{
		if (clan->platinum < 300)
		{
			send_to_char(
					"You cannot afford another 30 minutes of editing time.\n\r",
					ch);
			return FALSE;
		}
		clan->platinum -= 300;
		append_clan_log(clan, ENTRY_UPGRADE, ch->name,
				"Editing time purchased: {R300 {CP{clatinum.");
		clan->edit_time = ((clan->edit_time < current_time) ? current_time
				: clan->edit_time) + (30 * TIME_MINUTE);
		send_to_char(
				"You have another {R30{x minutes to edit your clan name, description and ranks.\n\r",
				ch);
		return TRUE;
	}
	else if (!str_prefix(argument, "status"))
	{
		time = (clan->edit_time - current_time) / TIME_MINUTE;

		if (time <= 0)
			strcpy(
					buf,
					"You do not have any more time remaining to edit your clan name, description and ranks. Use {Rtime buy{x to buy more.\n\r");
		else
			sprintf(
					buf,
					"You have %d minutes remaining to edit your clan name, description and ranks.\n\r",
					time);

		send_to_char(buf, ch);
		return FALSE;
	}

	cedit_time(ch, "");
	return FALSE;
}

CEDIT_FUN( cedit_war )
{
	CLAN_DATA *clan, *warClan;
	DESCRIPTOR_DATA *d;
	ROSTER_DATA *roster, *chRoster;
	char buf[MAX_STRING_LENGTH], arg[MAX_INPUT_LENGTH], temp[MAX_STRING_LENGTH];
	EDIT_CLAN( ch, clan );
	GET_ROSTER( ch, chRoster );

	if (argument[0] == '\0')
	{
		send_to_char(
				"Cedit syntax: war declare <clan name> - declare war on a clan (must be accepted by them)\n\r"
					"              war cancel              - end a war with a clan or cancel a request\n\r"
					"              war status              - see who you are at war at or who you have petitioned or who have petitioned you\n\r"
					"              war accept <clan name>  - accept a declaration of war\n\r",
				ch);
		return FALSE;
	}

	argument = one_argument(argument, arg);

	if (!str_prefix(arg, "declare"))
	{
		if (argument[0] == '\0')
		{
			cedit_war(ch, "");
			return FALSE;
		}

		if ((warClan = get_clan_by_name(argument)) == NULL)
		{
			send_to_char("The clan you specified was not found.\n\r", ch);
			return FALSE;
		}

		if (clan == warClan)
		{
			send_to_char("Do you really hate your clan that much?\n\r", ch);
			return FALSE;
		}

		if (clan->independent)
		{
			send_to_char("Only proper clans may wage war.\n\r", ch);
			return FALSE;
		}

		if (warClan->independent)
		{
			send_to_char("You can only declare war on proper clans.\n\r", ch);
			return FALSE;
		}

		if (clan->pkill && !warClan->pkill)
		{
			send_to_char(
					"You can only declare war on player-killing clans.\n\r", ch);
			return FALSE;
		}

		if (!clan->pkill && warClan->pkill)
		{
			send_to_char(
					"You can only declare war on a non-player-killing clan.\n\r",
					ch);
			return FALSE;
		}

		if (clan->Penemy.data)
		{
			sprintf(buf, "You have already petitioned %s{x for war.\n\r",
					strip_bespaces(clan->Penemy.data->c_name));
			return FALSE;
		}

		if (clan->enemy)
		{
			sprintf(buf, "You are already at war with %s{x.\n\r",
					strip_bespaces(clan->enemy->c_name));
			send_to_char(buf, ch);
			return FALSE;
		}

		if (warClan->enemy)
		{
			strcpy(temp, strip_bespaces(warClan->enemy->c_name));
			sprintf(buf, "%s{x is already at war with %s{x.\n\r",
					strip_bespaces(warClan->c_name), temp);
			send_to_char(buf, ch);
			return FALSE;
		}

		clan->Penemy.data = warClan;
		sprintf(
				buf,
				"%s{x has petitioned for war against your clan. Use {Rwar accept{x in {Rcedit{x to accept.\n\r",
				strip_bespaces(clan->c_name));

		for (roster = roster_list; roster != NULL; roster = roster->next)
		{
			if (roster->clan != warClan || roster->character == NULL)
				continue;

			send_to_char(buf, roster->character);
		}

		sprintf(buf, "War requested against %s{x.", strip_bespaces(
				warClan->c_name));
		append_clan_log(clan, ENTRY_WAR, ch->name, buf);
		sprintf(buf, "Request for war received from %s{x.", strip_bespaces(
				clan->c_name));
		append_clan_log(warClan, ENTRY_WAR, ch->name, buf);
		strcat(buf, "\n\r");
		send_to_char(buf, ch);
		return TRUE;
	}
	else if (!str_prefix(arg, "cancel"))
	{
		if (clan->Penemy.data != NULL)
		{
			send_to_char("Petition cancelled.\n\r", ch);
			sprintf(buf, "War request against %s{x withdrawn.\n\r",
					strip_bespaces(clan->Penemy.data->c_name));

			clan->Penemy.data = NULL;
			append_clan_log(clan, ENTRY_WAR, ch->name, buf);
			return FALSE;
		}
		else if ((warClan = clan->enemy) == NULL)
		{
			send_to_char("This clan doesn't have an enemy at the moment.\n\r",
					ch);
			return FALSE;
		}

		send_to_char("War cancelled.\n\r", ch);
		clan->enemy = NULL;
		warClan->enemy = NULL;

		strcpy(temp, strip_bespaces(warClan->c_name));
		sprintf(
				buf,
				"{c>{C>{c>{C PEACE {c>{C>{c> {WThe war between clan {x%s{W and clan {x%s{W has ended.{x\n\r",
				strip_bespaces(clan->c_name), temp);

		for (d = descriptor_list; d != NULL; d = d->next)
		{
			if (d->connected != CON_PLAYING || d->character == NULL
					|| IS_SET( d->character->comm, COMM_QUIET ))
				continue;

			send_to_char(buf, d->character);
		}

		sprintf(buf, "War ended against %s{x.", strip_bespaces(warClan->c_name));
		append_clan_log(clan, ENTRY_WAR, ch->name, buf);
		sprintf(buf, "War ended against %s{x.", strip_bespaces(clan->c_name));
		append_clan_log(warClan, ENTRY_WAR, ch->name, buf);
		strcat(buf, "\n\r");
		send_to_char(buf, ch);
		return TRUE;
	}
	else if (!str_prefix(arg, "status"))
	{
		BUFFER *output;
		bool found = FALSE, pfound = FALSE;

		if (clan->enemy)
		{
			sprintf(buf, "Currently at war with: %s{x\n\r", strip_bespaces(
					clan->enemy->c_name));
			send_to_char(buf, ch);
			return FALSE;
		}

		output = new_buf();

		if (clan->Penemy.data)
		{
			sprintf(buf, "Currently petitioning for war: %s{x\n\r",
					strip_bespaces(clan->Penemy.data->c_name));
			add_buf(output, buf);
			pfound = TRUE;
		}

		for (warClan = clan_list; warClan != NULL; warClan = warClan->next)
		{
			if (warClan->Penemy.data == NULL || warClan->Penemy.data != clan)
				continue;

			if (!found)
				add_buf(output, "Current petitions for war by: ");
			found = TRUE;

			add_buf(output, strip_bespaces(warClan->c_name));
			add_buf(output, "\n\r                              {x");
		}

		if (found || pfound)
			page_to_char(output->string, ch);
		else
			send_to_char("Nothing going on here at the moment.\n\r", ch);

		free_buf(output);
	}
	else if (!str_prefix(arg, "accept"))
	{
		CLAN_DATA *checkClan;

		if (argument[0] == '\0')
		{
			send_to_char(
					"Please specify the name of the clan you would like to accept that has petitioned your clan for war.\n\r",
					ch);
			return FALSE;
		}

		if ((warClan = get_clan_by_name(argument)) == NULL)
		{
			send_to_char("The clan you specified was not found.\n\r", ch);
			return FALSE;
		}

		if (warClan->Penemy.data == NULL || warClan->Penemy.data != clan)
		{
			send_to_char("That clan hasn't petitioned for war with you.\n\r",
					ch);
			return FALSE;
		}

		clan->Penemy.data = NULL;
		warClan->Penemy.data = NULL;
		clan->enemy = warClan;
		warClan->enemy = clan;

		strcpy(temp, strip_bespaces(warClan->c_name));
		sprintf(
				buf,
				"{r>{R>{r>{R WAR {r>{R>{r> {GWar has broken out between clan {x%s{G and clan {x%s{G!{x\n\r",
				strip_bespaces(clan->c_name), temp);

		for (d = descriptor_list; d != NULL; d = d->next)
		{
			if (d->connected != CON_PLAYING || d->character == NULL
					|| IS_SET( d->character->comm, COMM_QUIET ))
				continue;

			send_to_char(buf, d->character);
		}

		for (checkClan = clan_list; checkClan != NULL; checkClan
				= checkClan->next)
		{
			if (checkClan->Penemy.data == NULL || (checkClan->Penemy.data
					!= clan && checkClan->Penemy.data != warClan))
				continue;

			checkClan->Penemy.data = NULL;
		}

		sprintf(buf, "War started against %s{x.", strip_bespaces(
				warClan->c_name));
		append_clan_log(clan, ENTRY_WAR, ch->name, buf);
		sprintf(buf, "War started against %s{x.", strip_bespaces(clan->c_name));
		append_clan_log(warClan, ENTRY_WAR, ch->name, buf);
		save_clan_data(warClan);
		return TRUE;
	}
	else
		cedit_war(ch, "");

	return FALSE;
}

CEDIT_FUN( mcedit_buy )
{
	CLAN_DATA *clan;
	char arg[MAX_INPUT_LENGTH], whole_arg[MAX_INPUT_LENGTH];
	EDIT_CLAN( ch, clan );

	if (argument[0] == '\0')
	{
		send_to_char(
				"MCedit syntax: buy <item>     - buy a mob upgrade for the current mob\n\r"
					"               buy list       - show the possible upgrades available\n\r"
					"               buy req <item> - find out what the mob upgrade needs and costs to buy\n\r",
				ch);
		return FALSE;
	}

	strcpy(whole_arg, argument);
	argument = one_argument(argument, arg);

	if (!str_cmp(arg, "list"))
	{
		upgrade_list_table(ch, mob_upgd_prices);
		return FALSE;
	}
	else if (!str_cmp(arg, "req"))
		return (upgrade_process(ch, argument, mob_upgd_prices, FALSE ));
	else
		return (upgrade_process(ch, whole_arg, mob_upgd_prices, TRUE ));
}

CEDIT_FUN( mcedit_desc )
{
	CLAN_DATA *clan;
	MOB_INDEX_DATA *pMob;
	EDIT_CLAN( ch, clan );
	EDIT_MOB( ch, pMob );

	if (argument[0] == '\0')
	{
		string_append(ch, &pMob->description);
		return TRUE;
	}

	send_to_char(
			"MCedit syntax: desc - set the description (the one you see when you look at the mob)\n\r",
			ch);
	return FALSE;
}

CEDIT_FUN( mcedit_long )
{
	CLAN_DATA *clan;
	MOB_INDEX_DATA *pMob;
	char buf[MAX_STRING_LENGTH];
	EDIT_CLAN( ch, clan );
	EDIT_MOB( ch, pMob );

	if (argument[0] == '\0')
	{
		send_to_char(
				"MCedit syntax: long <long description> - set the long description (the one you see when you are in a room with it and type look)\n\r"
					"         {RNote:{x If you put in %s, it will be replaced by your clan colour name (saves typing).\n\r",
				ch);
		return FALSE;
	}

	if (strlen_color(argument) < 8)
	{
		send_to_char(
				"Provide a reasonable length long description please.\n\r", ch);
		return FALSE;
	}

	free_string(pMob->long_descr);
	str_replace_c(argument, "%s", strip_bespaces(clan->c_name));
	strcat(argument, "\n\r");
	pMob->long_descr = str_dup(argument);

	sprintf(buf, "Mob long description changed to: %s{x", pMob->long_descr);
	send_to_char(buf, ch);

	return TRUE;
}

CEDIT_FUN( mcedit_short )
{
	CLAN_DATA *clan;
	MOB_INDEX_DATA *pMob;
	char name[MAX_STRING_LENGTH];
	int length;
	EDIT_CLAN( ch, clan );
	EDIT_MOB( ch, pMob );

	if (argument[0] == '\0')
	{
		send_to_char(
				"MCedit syntax: short <short description> - set the short description (the one you see when you interact with the mob)\n\r"
					"         {RNote:{x If you put in %s, it will be replaced by your clan colour name (saves typing).\n\r",
				ch);
		return FALSE;
	}

	if ((length = strlen_color(argument)) < 5 || length > 70)
	{
		send_to_char(
				"Provide a reasonable length short description please.\n\r", ch);
		return FALSE;
	}

	str_replace_c(argument, "%s{x", strip_bespaces(clan->c_name));
	free_string(pMob->short_descr);
	pMob->short_descr = str_dup(argument);

	str_replace_c(argument, strip_bespaces(clan->c_name), clan->name);
	strcpy(name, process_name(argument));

	if (str_infix(clan->name, name))
	{
		if (name[strlen(name) - 1] != ' ')
			strcat(name, " ");

		strcat(name, clan->name);
	}

	free_string(pMob->player_name);
	pMob->player_name = str_dup(name);

	sprintf(name, "Mob short description changed to: %s{x\n\r",
			pMob->short_descr);
	send_to_char(name, ch);

	sprintf(name, "Mob name (keywords) automatically changed to: %s\n\r",
			pMob->player_name);
	send_to_char(name, ch);

	return TRUE;
}

CEDIT_FUN( mcedit_show )
{
	BUFFER *output = new_buf();
	MOB_INDEX_DATA *pMob;
	char buf[MAX_STRING_LENGTH];
	EDIT_MOB( ch, pMob );

	sprintf(buf, "{WKeyword name:    {B[{w%s{B]\n\r", pMob->player_name);
	add_buf(output, buf);

	sprintf(buf, "{WLevel            {B[{R%d{B]\n\r", pMob->level);
	add_buf(output, buf);

	sprintf(buf, "{WHitpoints:       {B[{G%d{B]\n\r", pMob->hit[DICE_BONUS]);
	add_buf(output, buf);

	sprintf(buf, "{WMana:            {B[{M%d{B]\n\r", pMob->mana[DICE_BONUS]);
	add_buf(output, buf);

	sprintf(buf, "{WDamage:          {B[{C%d{B]\n\r", pMob->damage[DICE_BONUS]);
	add_buf(output, buf);

	sprintf(buf, "{WDamage type:     {B[{w%s{B]\n\r",
			attack_table[pMob->dam_type].name);
	add_buf(output, buf);

	sprintf(buf, "{WOffensive bits:  {B[{w%s{B]\n\r", flag_string(off_flags,
			pMob->off_flags));
	add_buf(output, buf);

	sprintf(buf, "{WImmunity bits:   {B[{w%s{B]\n\r", flag_string(imm_flags,
			pMob->imm_flags));
	add_buf(output, buf);

	sprintf(buf, "{WResistance bits: {B[{w%s{B]\n\r", flag_string(res_flags,
			pMob->res_flags));
	add_buf(output, buf);

	sprintf(buf, "{WEffect bits:     {B[{w%s %s{B]\n\r", flag_string(
			affect_flags, pMob->affected_by), pMob->shielded_by == 0 ? ""
			: flag_string(shield_flags, pMob->shielded_by));
	add_buf(output, buf);

	sprintf(buf, "{WAC values:       {B[{WPierce{B:{w %d{B]\n\r", pMob->ac[0]);
	add_buf(output, buf);

	sprintf(buf, "                 {B[{WSlash{B:{w %d{B]\n\r", pMob->ac[1]);
	add_buf(output, buf);

	sprintf(buf, "                 {B[{WBash{B:{w %d{B]\n\r", pMob->ac[2]);
	add_buf(output, buf);

	sprintf(buf, "                 {B[{WExotic{B:{w %d{B]\n\r", pMob->ac[3]);
	add_buf(output, buf);

	sprintf(buf, "{WShort descr:     {B[{x%s{B]\n\r", pMob->short_descr);
	add_buf(output, buf);

	sprintf(buf, "{WLong descr:\n\r  {x%s{x", pMob->long_descr);
	add_buf(output, buf);

	sprintf(buf, "{WDescription:\n\r{x  %s\n\r", pMob->description);
	add_buf(output, buf);

	page_to_char(output->string, ch);
	free_buf(output);
	return FALSE;
}

CEDIT_FUN( ocedit_buy )
{
	CLAN_DATA *clan;
	char arg[MAX_INPUT_LENGTH], whole_arg[MAX_INPUT_LENGTH];
	EDIT_CLAN( ch, clan );

	if (argument[0] == '\0')
	{
		send_to_char(
				"OCedit syntax: buy <item>     - buy a obj upgrade for the current obj\n\r"
					"               buy list       - show the possible upgrades available\n\r"
					"               buy req <item> - find out what the obj upgrade needs and costs to buy\n\r",
				ch);
		return FALSE;
	}

	strcpy(whole_arg, argument);
	argument = one_argument(argument, arg);

	if (!str_cmp(arg, "list"))
	{
		upgrade_list_table(ch, obj_upgd_prices);
		return FALSE;
	}
	else if (!str_cmp(arg, "req"))
		return (upgrade_process(ch, argument, obj_upgd_prices, FALSE ));
	else
		return (upgrade_process(ch, whole_arg, obj_upgd_prices, TRUE ));
}

CEDIT_FUN( ocedit_long )
{
	CLAN_DATA *clan;
	OBJ_INDEX_DATA *pObj;
	char buf[MAX_STRING_LENGTH];
	EDIT_CLAN( ch, clan );
	EDIT_OBJ( ch, pObj );

	if (argument[0] == '\0')
	{
		send_to_char(
				"OCedit syntax: long <long description> - set the long description (the one you see when you are in a room with it and type look)\n\r"
					"         {RNote:{x If you put in %s, it will be replaced by your clan colour name (saves typing).\n\r",
				ch);
		return FALSE;
	}

	if (strlen_color(argument) < 8)
	{
		send_to_char(
				"Provide a reasonable length long description please.\n\r", ch);
		return FALSE;
	}

	free_string(pObj->description);
	str_replace_c(argument, "%s", strip_bespaces(clan->c_name));
	//    strcat( argument, "\n\r" );
	pObj->description = str_dup(argument);

	sprintf(buf, "Obj long description changed to: %s{x", pObj->description);
	send_to_char(buf, ch);

	return TRUE;
}

CEDIT_FUN( ocedit_sell )
{
	CLAN_DATA *clan;
	OBJ_INDEX_DATA *pObj;
	EDIT_CLAN( ch, clan );
	EDIT_OBJ( ch, pObj );

	send_to_char("Out of order, sorry!\n\r", ch);
	return FALSE;
}

CEDIT_FUN( ocedit_short )
{
	CLAN_DATA *clan;
	OBJ_INDEX_DATA *pObj;
	char name[MAX_STRING_LENGTH];
	int length;
	EDIT_CLAN( ch, clan );
	EDIT_OBJ( ch, pObj );

	if (argument[0] == '\0')
	{
		send_to_char(
				"OCedit syntax: short <short description> - set the short description (the one you see when you interact with the object)\n\r"
					"         {RNote:{x If you put in %s, it will be replaced by your clan colour name (saves typing).\n\r",
				ch);
		return FALSE;
	}

	if ((length = strlen_color(argument)) < 5 || length > 70)
	{
		send_to_char(
				"Provide a reasonable length short description please.\n\r", ch);
		return FALSE;
	}

	str_replace_c(argument, "%s{x", strip_bespaces(clan->c_name));
	free_string(pObj->short_descr);
	pObj->short_descr = str_dup(argument);

	str_replace_c(argument, strip_bespaces(clan->c_name), clan->name);
	strcpy(name, process_name(argument));

	if (str_infix(clan->name, name))
	{
		if (name[strlen(name) - 1] != ' ')
			strcat(name, " ");

		strcat(name, clan->name);
	}

	free_string(pObj->name);
	pObj->name = str_dup(name);

	sprintf(name, "Obj short description changed to: %s{x\n\r",
			pObj->short_descr);
	send_to_char(name, ch);

	sprintf(name, "Obj name (keywords) automatically changed to: %s\n\r",
			pObj->name);
	send_to_char(name, ch);

	return TRUE;
}

CEDIT_FUN( ocedit_show )
{
	AFFECT_DATA *paf;
	BUFFER *output = new_buf();
	CLAN_DATA *clan;
	OBJ_INDEX_DATA *pObj;
	char buf[MAX_STRING_LENGTH];
	EDIT_CLAN( ch, clan );
	EDIT_OBJ( ch, pObj );

	sprintf(buf, "{WKeyword name:    {B[{w%s{B]\n\r", pObj->name);
	add_buf(output, buf);

	sprintf(buf, "{WLevel            {B[{R%d{B]\n\r", pObj->level);
	add_buf(output, buf);

	sprintf(buf, "{WType:            {B[{w%s{B]\n\r", flag_string(type_flags,
			pObj->item_type));
	add_buf(output, buf);

	sprintf(buf, "{WWear locations:  {B[{w%s{B]\n\r", flag_string(wear_flags,
			pObj->wear_flags));
	add_buf(output, buf);

	sprintf(buf, "{WObject flags:    {B[{w%s{B]\n\r", flag_string(extra_flags,
			pObj->extra_flags));
	add_buf(output, buf);

	sprintf(buf, "{WShort Descr:     {B[{w%s{B]\n\r", pObj->short_descr);
	add_buf(output, buf);

	sprintf(buf, "{WLong Descr:      {B[{w%s{B]\n\r\n\r", pObj->description);
	add_buf(output, buf);

	switch (pObj->item_type)
	// Lazy so I used Asgard's obj value stuff, just stripped down a bit
	{
	default: /* No values. */
		break;

	case ITEM_LIGHT:
		if (pObj->value[2] == -1 || pObj->value[2] == 999) /* ROM OLC */
			strcpy(buf, "{WLight:           {GInfinite\n\r");
		else
			sprintf(buf, "{WLight:           {B[{R%d{B]\n\r", pObj->value[2]);
		add_buf(output, buf);
		break;

	case ITEM_WAND:
	case ITEM_STAFF:
		sprintf(buf, "{WLevel:           {B[{R%d{B]\n\r"
			"{WCharges Total:   {B[{R%d{B]\n\r"
			"{WSpell:           {B[{w%s{B]\n\r\n\r", pObj->value[0],
				pObj->value[1],
				pObj->value[3] != -1 ? skill_table[pObj->value[3]].name
						: "none");
		add_buf(output, buf);
		break;

	case ITEM_PORTAL:
		sprintf(buf, "{WCharges:        {B[{R%d{B]\n\r"
			"{WExit Flags:     {B[{w%s{B]\n\r"
			"{WPortal Flags:   {B[{w%s{B]\n\r"
			"{WGoes to (area): {B[{w%s{B]\n\r\n\r", pObj->value[0],
				flag_string(exit_flags, pObj->value[1]), flag_string(
						portal_flags, pObj->value[2]), vnum_lookup(
						portloc_table, pObj->value[3]));
		add_buf(output, buf);
		break;

	case ITEM_FURNITURE:
		sprintf(buf, "{WMax people:      {B[{R%d{B]\n\r"
			"{WMax weight:      {B[{R%d{B]\n\r"
			"{WFurniture Flags: {B[{w%s{B]\n\r"
			"{WHeal bonus:      {B[{R%d{B]\n\r"
			"{WMana bonus:      {B[{R%d{B]\n\r\n\r", pObj->value[0],
				pObj->value[1], flag_string(furniture_flags, pObj->value[2]),
				pObj->value[3], pObj->value[4]);
		add_buf(output, buf);
		break;

	case ITEM_SCROLL:
	case ITEM_POTION:
	case ITEM_PILL:
		sprintf(buf, "{WLevel:           {B[{R%d{B]\n\r"
			"{WSpell 1:         {B[{w%s{B]\n\r"
			"{WSpell 2:         {B[{w%s{B]\n\r"
			"{WSpell 3:         {B[{w%s{B]\n\r"
			"{WSpell 4:         {B[{w%s{B]\n\r\n\r", pObj->value[0],
				pObj->value[1] != -1 ? skill_table[pObj->value[1]].name
						: "none",
				pObj->value[2] != -1 ? skill_table[pObj->value[2]].name
						: "none",
				pObj->value[3] != -1 ? skill_table[pObj->value[3]].name
						: "none",
				pObj->value[4] != -1 ? skill_table[pObj->value[4]].name
						: "none");
		add_buf(output, buf);
		break;

		/* ARMOR for ROM */

		/*        case ITEM_ARMOR:
		 sprintf( buf,
		 "{WAc pierce:       {B[{R%d{B]\n\r"
		 "{WAc bash:         {B[{R%d{B]\n\r"
		 "{WAc slash:        {B[{R%d{B]\n\r"
		 "{WAc exotic:       {B[{R%d{B]\n\r",
		 pObj->value[0],
		 pObj->value[1],
		 pObj->value[2],
		 pObj->value[3] );
		 add_buf( output, buf );
		 add_buf( output, "{WAC is {wvariable{W.\n\r" );
		 break;
		 */
		/* WEAPON changed in ROM: */
		/* I had to split the output here, I have no idea why, but it helped -- Hugin */
		/* It somehow fixed a bug in showing scroll/pill/potions too ?! */
	case ITEM_WEAPON:
		sprintf(buf, "{WAverage damage:  {B[{R%d{B]\n\r", (1 + pObj->value[2])
				* pObj->value[1] / 2);
		add_buf(output, buf);
		sprintf(buf, "{WWeapon type:     {B[{w%s{B]\n\r", flag_string(
				weapon_class, pObj->value[0]));
		add_buf(output, buf);
		sprintf(buf, "{WNumber of dice:  {B[{R%d{B]\n\r", pObj->value[1]);
		add_buf(output, buf);
		sprintf(buf, "{WType of dice:    {B[{R%d{B]\n\r", pObj->value[2]);
		add_buf(output, buf);
		sprintf(buf, "{WDamage type:     {B[{w%s{B]\n\r",
				attack_table[pObj->value[3]].name);
		add_buf(output, buf);
		sprintf(buf, "{WWpn flags:       {B[{w%s{B]\n\r\n\r", flag_string(
				weapon_type2, pObj->value[4]));
		add_buf(output, buf);
		break;

	case ITEM_CONTAINER:
		sprintf(buf, "{WWeight:          {B[{R%d{W kg{B]\n\r"
			"{WFlags:           {B[{w%s{B]\n\r"
			"{WCapacity:        {B[{R%d{B]\n\r"
			"{WWeight Mult:     {B[{R%d{B]\n\r\n\r", pObj->value[0],
				flag_string(container_flags, pObj->value[1]), pObj->value[3],
				pObj->value[4]);
		add_buf(output, buf);
		break;

	case ITEM_DRINK_CON:
		sprintf(buf, "{WCapacity:        {B[{R%d{B]\n\r", pObj->value[0]);
		add_buf(output, buf);
		break;

	case ITEM_FOUNTAIN:
		sprintf(buf, "{WLiquid Total:    {B[{R%d{B]\n\r"
			"{WLiquid:          {B[{w%s{B]\n\r\n\r", pObj->value[0],
				liq_table[pObj->value[2]].liq_name);
		add_buf(output, buf);
		break;

	case ITEM_FOOD:
		sprintf(buf, "{WFood hours:      {B[{R%d{B]\n\r\n\r", pObj->value[0]);
		add_buf(output, buf);
		break;
	}

	for (paf = pObj->affected; paf != NULL; paf = paf->next)
	{
		if (paf->modifier != 0 && paf->location != APPLY_NONE)
		{
			sprintf(buf, "{WModifies:        {B[{w%s{B] {Wby {B[{R%d{B]\n\r",
					affect_loc_name(paf->location), paf->modifier);
			add_buf(output, buf);
		}
		if (paf->bitvector)
		{
			switch (paf->where)
			{
			case TO_AFFECTS:
				sprintf(buf, "{WAffects user:    {B[{w%s{B]\n\r",
						affect_bit_name(paf->bitvector));
				break;
			case TO_OBJECT:
				sprintf(buf, "{WAdds obj flag:   {B[{w%s{B]\n\r",
						extra_bit_name(paf->bitvector));
				break;
			case TO_WEAPON:
				break;
			case TO_IMMUNE:
				sprintf(buf, "{WImmunity to:     {B[{w%s{B]\n\r", imm_bit_name(
						paf->bitvector));
				break;
			case TO_RESIST:
				sprintf(buf, "{WResistance to:   {B[{w%s{B]\n\r", imm_bit_name(
						paf->bitvector));
				break;
			case TO_VULN:
				sprintf(buf, "{WVulnerability to:{B[{w%s{B]\n\r", imm_bit_name(
						paf->bitvector));
				break;
			case TO_SHIELDS:
				sprintf(buf, "{WAdds shield:     {B[{w%s{B]\n\r",
						shield_bit_name(paf->bitvector));
				break;
			default:
				sprintf(buf,
						"{WAdds unknown affect {B[{R%d{B] {Wat {B[{R%d{B]\n\r",
						paf->bitvector, paf->where);
				break;

			}
			if (paf->where != TO_WEAPON)
				add_buf(output, buf);
		}
	}

	add_buf(output, "{x");
	page_to_char(output->string, ch);
	free_buf(output);
	return FALSE;
}

CEDIT_FUN( rcedit_buy )
{
	CLAN_DATA *clan;
	char arg[MAX_INPUT_LENGTH], whole_arg[MAX_INPUT_LENGTH];
	EDIT_CLAN( ch, clan );

	if (argument[0] == '\0')
	{
		send_to_char(
				"RCedit syntax: buy <item>     - buy a room upgrade for the current room\n\r"
					"               buy list       - show the possible upgrades available\n\r"
					"               buy req <item> - find out what the room upgrade needs and costs to buy\n\r",
				ch);
		return FALSE;
	}

	strcpy(whole_arg, argument);
	argument = one_argument(argument, arg);

	if (!str_cmp(arg, "list"))
	{
		upgrade_list_table(ch, room_upgd_prices);
		return FALSE;
	}
	else if (!str_cmp(arg, "req"))
		return (upgrade_process(ch, argument, room_upgd_prices, FALSE ));
	else
		return (upgrade_process(ch, whole_arg, room_upgd_prices, TRUE ));
}

CEDIT_FUN( rcedit_desc )
{
	CLAN_DATA *clan;
	ROOM_INDEX_DATA *pRoom;
	EDIT_CLAN( ch, clan );
	EDIT_ROOM( ch, pRoom );

	if (argument[0] == '\0')
	{
		string_append(ch, &pRoom->description);
		return TRUE;
	}

	send_to_char("RCedit syntax: desc - set the room description\n\r", ch);
	return FALSE;
}

CEDIT_FUN( rcedit_exdesc )
{
	CLAN_DATA *clan;
	EXTRA_DESCR_DATA *ex;
	ROOM_INDEX_DATA *pRoom;
	char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
	int i;
	EDIT_CLAN( ch, clan );
	EDIT_ROOM( ch, pRoom );

	if (argument[0] == '\0')
	{
		send_to_char(
				"RCedit syntax: exdesc add <keyword>    - add an extra description\n\r"
					"               exdesc delete <keyword> - delete an extra description\n\r"
					"               exdesc edit <keyword>   - edit an existing extra description\n\r"
					"               exdesc format <keyword> - format the extra description\n\r",
				ch);
		return FALSE;
	}

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if (!str_prefix(arg1, "add"))
	{
		if (arg2[0] == '\0')
		{
			send_to_char("Please specify a keyword to add\n\r", ch);
			return FALSE;
		}
		for (ex = pRoom->extra_descr, i = 0; ex != NULL; ex = ex->next, i++)
			if (!str_cmp(arg2, ex->keyword))
				break;

		if (ex != NULL)
		{
			send_to_char(
					"There is already an existing extra description with that keyword.\n\r",
					ch);
			return FALSE;
		}

		if (i >= 2)
		{
			send_to_char(
					"There can only be 2 extra descriptions per clan room.\n\r",
					ch);
			return FALSE;
		}

		ex = new_extra_descr();

		free_string(ex->keyword);
		ex->keyword = str_dup(arg2);

		free_string(ex->description);
		ex->description = str_dup("");

		ex->next = pRoom->extra_descr;
		pRoom->extra_descr = ex;

		string_append(ch, &ex->description);
	}
	else if (!str_prefix(arg1, "edit"))
	{
		if (arg2[0] == '\0')
		{
			send_to_char("Please specify a keyword to edit\n\r", ch);
			return FALSE;
		}

		for (ex = pRoom->extra_descr; ex != NULL; ex = ex->next)
			if (!str_cmp(arg2, ex->keyword))
				break;

		if (ex == NULL)
		{
			send_to_char(
					"There isn't an extra description with the keyword you specified.\n\r",
					ch);
			return FALSE;
		}

		string_append(ch, &ex->description);
	}
	else if (!str_prefix(arg1, "delete"))
	{
		EXTRA_DESCR_DATA *ex_prev;

		if (arg2[0] == '\0')
		{
			send_to_char("Please specify a keyword to delete\n\r", ch);
			return FALSE;
		}

		for (ex = pRoom->extra_descr, ex_prev = NULL; ex != NULL; ex_prev = ex, ex
				= ex->next)
			if (!str_cmp(arg2, ex->keyword))
				break;

		if (ex == NULL)
		{
			send_to_char(
					"There isn't an extra description with the keyword you specified.\n\r",
					ch);
			return FALSE;
		}

		if (ex_prev == NULL)
			pRoom->extra_descr = ex->next;
		else
			ex_prev->next = ex->next;

		free_extra_descr(ex);
		send_to_char("Extra description deleted.\n\r", ch);
	}
	else if (!str_prefix(arg1, "format"))
	{
		if (arg2[0] == '\0')
		{
			send_to_char("Please specify a keyword to format\n\r", ch);
			return FALSE;
		}

		for (ex = pRoom->extra_descr; ex != NULL; ex = ex->next)
			if (!str_cmp(arg2, ex->keyword))
				break;

		if (ex == NULL)
		{
			send_to_char(
					"There isn't an extra description with the keyword you specified.\n\r",
					ch);
			return FALSE;
		}

		ex->description = format_string(ex->description);
		send_to_char("Extra description formatted.\n\r", ch);
	}
	else
	{
		rcedit_exdesc(ch, "");
		return FALSE;
	}

	return TRUE;
}

CEDIT_FUN( rcedit_name )
{
	CLAN_DATA *clan;
	ROOM_INDEX_DATA *pRoom;
	char buf[MAX_STRING_LENGTH];
	EDIT_CLAN( ch, clan );
	EDIT_ROOM( ch, pRoom );

	if (argument[0] == '\0')
	{
		send_to_char("RCedit syntax: name <room name>\n\r", ch);
		return FALSE;
	}

	if (strlen_color(argument) < 4)
	{
		send_to_char("Provide a reasonable length name please.\n\r", ch);
		return FALSE;
	}

	free_string(pRoom->name);
	pRoom->name = str_dup(argument);

	sprintf(buf, "Room name changed to: {B%s{x\n\r", pRoom->name);
	send_to_char(buf, ch);

	return TRUE;
}

CEDIT_FUN( rcedit_show )
{
	BUFFER *output = new_buf();
	CLAN_DATA *clan;
	ROOM_INDEX_DATA *pRoom;
	char buf[MAX_STRING_LENGTH];
	EDIT_CLAN( ch, clan );
	EDIT_ROOM( ch, pRoom );

	sprintf(buf, "{WRoom name:       {y[{B%s{y]\n\r", pRoom->name);
	add_buf(output, buf);

	sprintf(buf, "{WHP regen rate:   {B[{R%d{B]\n\r", pRoom->heal_rate);
	add_buf(output, buf);

	sprintf(buf, "{WMana regen rate: {B[{R%d{B]\n\r", pRoom->mana_rate);
	add_buf(output, buf);

	sprintf(buf, "{WRoom flags:      {B[{w%s{B]\n\r", flag_string(room_flags,
			pRoom->room_flags));
	add_buf(output, buf);

	if (pRoom->extra_descr)
	{
		EXTRA_DESCR_DATA *ed;

		add_buf(output, "{WExdesc keywords: {B[{w");

		for (ed = pRoom->extra_descr; ed; ed = ed->next)
		{
			add_buf(output, ed->keyword);
			if (ed->next)
				add_buf(output, " ");
		}
		add_buf(output, "{B]\n\r");
	}

	sprintf(buf, "{WDescription:\n\r{x  %s", pRoom->description);
	add_buf(output, buf);

	add_buf(output, "{x");
	page_to_char(output->string, ch);
	free_buf(output);
	return FALSE;
}

CEDIT_FUN( scedit_buy )
{
	CLAN_DATA *clan;
	char buf[MAX_STRING_LENGTH];
	int i;
	bool major = FALSE, minor = FALSE;
	EDIT_CLAN( ch, clan );

	if (argument[0] == '\0')
	{
		send_to_char(
				"SCedit syntax: buy <clan skill {Rcommand{x name> - buy a clan skill\n\r"
					"         {RNote:{x once bought, clan skills can be sold back, but only for\n\r"
					"                   one third of the original price, so pick carefully.\n\r",
				ch);
		return FALSE;
	}

	for (i = 0; clan_skill_table[i].cmd[0] != '\0'; i++)
		if (!str_prefix(argument, clan_skill_table[i].cmd))
			break;

	if (clan_skill_table[i].cmd[0] == '\0')
	{
		send_to_char("Clan skill not found.\n\r", ch);
		return FALSE;
	}

	int j, freeslot = -1;
	for (j = 0; j < 2; j++) {
		if (clan->skills[j] == i) {
			send_to_char("The clan already has that skill.\r\n", ch);
			return FALSE;
		} else if (clan->skills[j] == -1)
			freeslot = j;
		else {
			if (clan_skill_table[clan->skills[j]].major)
				major = TRUE;
			else
				minor = TRUE;
		}
	}

	if (freeslot == -1) {
		send_to_char("Your clan cannot purchase any more skills.\r\n", ch);
		return FALSE;
	}

	if ((clan_skill_table[i].major && major)
	||  (!clan_skill_table[i].major && minor))
	{
		send_to_char(
				"Each clan is allowed one major skill and one minor skill.\n\r",
				ch);
		return FALSE;
	}

	if (clan->platinum < clan_skill_table[i].platinum || clan->qps
			< clan_skill_table[i].qps)
	{
		send_to_char(
				"There isn't enough in the clan accounts to purchase that skill.\n\r",
				ch);
		return FALSE;
	}

	clan->skills[freeslot] = i;
	clan->platinum -= clan_skill_table[i].platinum;
	clan->qps -= clan_skill_table[i].qps;
	sprintf(buf, "%s{x clan skill purchased for %d platinum and %d questpoints",
		clan_skill_table[i].name, clan_skill_table[i].platinum, clan_skill_table[i].qps);
	append_clan_log(clan, ENTRY_UPGRADE, ch->name, buf);
	sprintf(buf, "%s{x clan skill purchased.\n\r", clan_skill_table[i].name);
	send_to_char(buf, ch);

	return TRUE;
}

CEDIT_FUN( scedit_sell )
{
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *vch;
	CLAN_DATA *clan;
	int i;

	EDIT_CLAN( ch, clan );

	if (argument[0] == '\0')
	{
		send_to_char(
				"SCedit syntax: sell <clan skill {Rcommand {xname> - sell skill for a {Rthird{x of the original price\n\r",
				ch);
		return FALSE;
	}

	for (i = 0; clan_skill_table[i].cmd[0] != '\0'; i++)
		if (!str_prefix(argument, clan_skill_table[i].cmd))
			break;

	if (clan_skill_table[i].cmd[0] == '\0')
	{
		send_to_char("Clan skill not found.\n\r", ch);
		return FALSE;
	}

	if (!clan_has_skill(clan, i)) {
		send_to_char("You do not own that skill.\r\n", ch);
		return FALSE;
	}

	if (clan_skill_table[i].platinum > 0)
		clan->platinum += clan_skill_table[i].platinum / 3;

	if (clan_skill_table[i].qps > 0)
		clan->qps += clan_skill_table[i].qps / 3;

	sprintf(buf, "%s{x clan skill sold for %d platinum and %d questpoints",
			clan_skill_table[i].name, (clan_skill_table[i].platinum/3), (clan_skill_table[i].qps/3));
	append_clan_log(clan, ENTRY_UPGRADE, ch->name, buf);

	int j;
	for (j = 0; j < 2; j++)
		if (clan->skills[j] == i)
			clan->skills[j] = -1;
	send_to_char("Skill sold.\n\r", ch);

	for (vch = char_list; vch != NULL; vch = vch->next)
		clanskill_check(ch);
	return TRUE;
}

CEDIT_FUN( scedit_show )
{
	BUFFER *output = new_buf();
	CLAN_DATA *clan;
	char buf[MAX_STRING_LENGTH], borderbuf[MAX_STRING_LENGTH], *temp;
	int i;
	EDIT_CLAN( ch, clan );

	add_buf(
			output,
			"{B+- {GCEDIT SKILLS {B---------------------------------------------------------+\n\r");

	for (i = 0; clan_skill_table[i].cmd[0] != '\0'; i++)
	{
		if (i > 0)
			add_buf(
					output,
					"{B|------------------------------------------------------------------------|\n\r");

		sprintf(buf, "{WName:{x %s {WCommand: {w%s", end_string(
				clan_skill_table[i].name, 24), clan_skill_table[i].cmd);
		sprintf(borderbuf, "{B| %s {B|\n\r", end_string(buf, 70));
		add_buf(output, borderbuf);

		if (clan_has_skill(clan, i))
		{
			sprintf(buf, "{WClaimed by this clan{W.");
			sprintf(borderbuf, "{B| %s {B|\n\r", end_string(buf, 70));
			add_buf(output, borderbuf);
		}

		sprintf(buf, "{WType: %s                    {WRank required:{w %s",
				clan_skill_table[i].major ? "{CMajor" : "{cMinor",
				clan->r_name[get_p_rank_index(clan_skill_table[i].points)]);
		sprintf(borderbuf, "{B| %s {B|\n\r", end_string(buf, 70));
		add_buf(output, borderbuf);

		sprintf(buf, "{WPlatinum:{w  %-7d             {WQuest points:{w %d",
				clan_skill_table[i].platinum, clan_skill_table[i].qps);
		sprintf(borderbuf, "{B| %s {B|\n\r", end_string(buf, 70));
		add_buf(output, borderbuf);

		temp = clan_skill_table[i].description;
		temp = length_argument(temp, borderbuf, 57);
		sprintf(buf, "{WDescription: {w%s", borderbuf);
		sprintf(borderbuf, "{B| %s {B|\n\r", end_string(buf, 70));
		add_buf(output, borderbuf);

		while (*temp != '\0')
		{
			temp = length_argument(temp, buf, 57);
			sprintf(borderbuf, "{B|              {w%s {B|\n\r", end_string(buf,
					57));
			add_buf(output, borderbuf);
		}
	}

	add_buf(
			output,
			"{B+------------------------------------------------------------------------+{x\n\r");
	page_to_char(output->string, ch);
	free_buf(output);
	return FALSE;
}

/* Handler for input whilst in main cedit */
void cedit(CHAR_DATA *ch, char *argument)
{
	CLAN_DATA *clan;
	ROSTER_DATA *roster;
	char command[MAX_INPUT_LENGTH], whole_argument[MAX_INPUT_LENGTH];
	int cmd, trust;
	bool immortal = IS_IMMORTAL( ch );

	smash_tilde(argument);

	EDIT_CLAN( ch, clan );
	GET_ROSTER( ch, roster );

	if (clan->upgrade != NULL && clan->u_rost == roster)
	{
		upgd_handler(ch, argument);
		return;
	}

	strcpy(whole_argument, argument);
	argument = one_argument(argument, command);

	if (command[0] == '\0')
	{
		send_to_char(
				"Use {Rcommands{x to show available commands, or {Rdone{x to exit the editor.\n\r",
				ch);
		cedit_show(ch, "");
		return;
	}
	else if (!str_cmp(command, "done"))
	{
		cedit_done(ch);
		return;
	}
	else if (!str_prefix(command, "commands"))
	{
		show_cedit_commands(ch, argument, main_cedit_table);
		return;
	}

	if (immortal)
		trust = get_trust(ch);
	else
		trust = roster->trust;

	for (cmd = 0; main_cedit_table[cmd].cmd[0] != '\0'; cmd++)
	{
		if ((immortal && main_cedit_table[cmd].trust == TRUST_NONE)
				|| (!immortal && main_cedit_table[cmd].m_trust == TRUST_NONE))
			continue;

		if ((immortal && trust < main_cedit_table[cmd].trust) || (!immortal
				&& trust < main_cedit_table[cmd].m_trust) || str_prefix(
				command, main_cedit_table[cmd].cmd))
			continue;

		printf_player(ch, "(CEDIT) %s", whole_argument);
		if ((*main_cedit_table[cmd].func)(ch, argument))
			save_clan_data(clan);
		return;
	}
	//    send_to_char( "Command not found. Type {Rcommands{x for a list of commands.\n\r", ch );
	interpret(ch, whole_argument);
}

/* Handler for input whilst in mob cedit */
void mcedit(CHAR_DATA *ch, char *argument)
{
	CLAN_DATA *clan;
	MOB_INDEX_DATA *pMob;
	ROSTER_DATA *roster;
	char command[MAX_INPUT_LENGTH], whole_argument[MAX_INPUT_LENGTH];
	int cmd, trust;
	bool immortal = IS_IMMORTAL( ch );

	smash_tilde(argument);

	EDIT_CLAN( ch, clan );
	GET_ROSTER( ch, roster );

	if (clan->upgrade != NULL && clan->u_rost == roster)
	{
		upgd_handler(ch, argument);
		return;
	}

	strcpy(whole_argument, argument);
	argument = one_argument(argument, command);

	if (command[0] == '\0')
	{
		send_to_char(
				"Use {Rcommands{x to show available commands, or {Rdone{x to exit the editor.\n\r",
				ch);
		mcedit_show(ch, "");
		return;
	}
	else if (!str_cmp(command, "done"))
	{
		cedit_done(ch);
		return;
	}
	else if (!str_prefix(command, "commands"))
	{
		show_cedit_commands(ch, argument, mob_cedit_table);
		return;
	}

	if (immortal)
		trust = get_trust(ch);
	else
		trust = roster->trust;

	for (cmd = 0; mob_cedit_table[cmd].cmd[0] != '\0'; cmd++)
	{
		if ((immortal && mob_cedit_table[cmd].trust == TRUST_NONE)
				|| (!immortal && mob_cedit_table[cmd].m_trust == TRUST_NONE))
			continue;

		if ((immortal && trust < mob_cedit_table[cmd].trust) || (!immortal
				&& trust < mob_cedit_table[cmd].m_trust) || str_prefix(command,
				mob_cedit_table[cmd].cmd))
			continue;

		printf_player(ch, "(MCEDIT) %s", whole_argument);
		if ((*mob_cedit_table[cmd].func)(ch, argument))
		{
			EDIT_MOB( ch, pMob );
			reset_clan_mob(pMob, FALSE );
			save_clan_data(clan);
			save_area(get_clan_area());
		}
		return;
	}
	//    send_to_char( "Command not found. Type {Rcommands{x for a list of commands.\n\r", ch );
	interpret(ch, whole_argument);
}

/* Handler for input whilst in obj cedit */
void ocedit(CHAR_DATA *ch, char *argument)
{
	CLAN_DATA *clan;
	OBJ_INDEX_DATA *pObj;
	ROSTER_DATA *roster;
	char command[MAX_INPUT_LENGTH], whole_argument[MAX_INPUT_LENGTH];
	int cmd, trust;
	bool immortal = IS_IMMORTAL( ch );

	smash_tilde(argument);

	EDIT_CLAN( ch, clan );
	GET_ROSTER( ch, roster );

	if (clan->upgrade != NULL && clan->u_rost == roster)
	{
		upgd_handler(ch, argument);
		return;
	}

	strcpy(whole_argument, argument);
	argument = one_argument(argument, command);

	if (command[0] == '\0')
	{
		send_to_char(
				"Use {Rcommands{x to show available commands, or {Rdone{x to exit the editor.\n\r",
				ch);
		ocedit_show(ch, "");
		return;
	}
	else if (!str_cmp(command, "done"))
	{
		cedit_done(ch);
		return;
	}
	else if (!str_prefix(command, "commands"))
	{
		show_cedit_commands(ch, argument, obj_cedit_table);
		return;
	}

	if (immortal)
		trust = get_trust(ch);
	else
		trust = roster->trust;

	for (cmd = 0; obj_cedit_table[cmd].cmd[0] != '\0'; cmd++)
	{
		if ((immortal && obj_cedit_table[cmd].trust == TRUST_NONE)
				|| (!immortal && obj_cedit_table[cmd].m_trust == TRUST_NONE))
			continue;

		if ((immortal && trust < obj_cedit_table[cmd].trust) || (!immortal
				&& trust < obj_cedit_table[cmd].m_trust) || str_prefix(command,
				obj_cedit_table[cmd].cmd))
			continue;

		printf_player(ch, "(OCEDIT) %s", whole_argument);
		if ((*obj_cedit_table[cmd].func)(ch, argument))
		{
			EDIT_OBJ( ch, pObj );
			reset_clan_obj(pObj, FALSE );
			save_clan_data(clan);
			save_area(get_clan_area());
		}
		return;
	}
	//    send_to_char( "Command not found. Type {Rcommands{x for a list of commands.\n\r", ch );
	interpret(ch, whole_argument);
}

/* Handler for input whilst in room cedit */
void rcedit(CHAR_DATA *ch, char *argument)
{
	CLAN_DATA *clan;
	ROSTER_DATA *roster;
	char command[MAX_INPUT_LENGTH], whole_argument[MAX_INPUT_LENGTH];
	int cmd, trust;
	bool immortal = IS_IMMORTAL( ch );

	smash_tilde(argument);

	EDIT_CLAN( ch, clan );
	GET_ROSTER( ch, roster );

	if (clan->upgrade != NULL && clan->u_rost == roster)
	{
		upgd_handler(ch, argument);
		return;
	}

	strcpy(whole_argument, argument);
	argument = one_argument(argument, command);

	if (command[0] == '\0')
	{
		send_to_char(
				"Use {Rcommands{x to show available commands, or {Rdone{x to exit the editor.\n\r",
				ch);
		rcedit_show(ch, "");
		return;
	}
	else if (!str_cmp(command, "done"))
	{
		cedit_done(ch);
		return;
	}
	else if (!str_prefix(command, "commands"))
	{
		show_cedit_commands(ch, argument, room_cedit_table);
		return;
	}

	if (immortal)
		trust = get_trust(ch);
	else
		trust = roster->trust;

	for (cmd = 0; room_cedit_table[cmd].cmd[0] != '\0'; cmd++)
	{
		if ((immortal && room_cedit_table[cmd].trust == TRUST_NONE)
				|| (!immortal && room_cedit_table[cmd].m_trust == TRUST_NONE))
			continue;

		if ((immortal && trust < room_cedit_table[cmd].trust) || (!immortal
				&& trust < room_cedit_table[cmd].m_trust) || str_prefix(
				command, room_cedit_table[cmd].cmd))
			continue;

		if ((*room_cedit_table[cmd].func)(ch, argument))
		{
			printf_player(ch, "(RCEDIT) %s", whole_argument);
			save_clan_data(clan);
			save_area(get_clan_area());
		}
		return;
	}
	//    send_to_char( "Command not found. Type {Rcommands{x for a list of commands.\n\r", ch );
	interpret(ch, whole_argument);
}

/* Handler for input whilst in skill cedit */
void scedit(CHAR_DATA *ch, char *argument)
{
	CLAN_DATA *clan;
	ROSTER_DATA *roster;
	char command[MAX_INPUT_LENGTH], whole_argument[MAX_INPUT_LENGTH];
	int cmd, trust;
	bool immortal = IS_IMMORTAL( ch );

	smash_tilde(argument);
	strcpy(whole_argument, argument);
	argument = one_argument(argument, command);

	EDIT_CLAN( ch, clan );
	GET_ROSTER( ch, roster );

	if (command[0] == '\0')
	{
		send_to_char(
				"Use {Rcommands{x to show available commands, or {Rdone{x to exit the editor.\n\r",
				ch);
		scedit_show(ch, "");
		return;
	}
	else if (!str_cmp(command, "done"))
	{
		cedit_done(ch);
		return;
	}
	else if (!str_prefix(command, "commands"))
	{
		show_cedit_commands(ch, argument, skill_cedit_table);
		return;
	}

	if (immortal)
		trust = get_trust(ch);
	else
		trust = roster->trust;

	for (cmd = 0; skill_cedit_table[cmd].cmd[0] != '\0'; cmd++)
	{
		if ((immortal && skill_cedit_table[cmd].trust == TRUST_NONE)
				|| (!immortal && skill_cedit_table[cmd].m_trust == TRUST_NONE))
			continue;

		if ((immortal && trust < skill_cedit_table[cmd].trust) || (!immortal
				&& trust < skill_cedit_table[cmd].m_trust) || str_prefix(
				command, skill_cedit_table[cmd].cmd))
			continue;

		printf_player(ch, "(SCEDIT) %s", whole_argument);
		if ((*skill_cedit_table[cmd].func)(ch, argument))
			save_clan_data(clan);
		return;
	}
	//    send_to_char( "Command not found. Type {Rcommands{x for a list of commands.\n\r", ch );
	interpret(ch, whole_argument);
}

/* Function for cedit COMMAND */
void do_cedit(CHAR_DATA *ch, char *argument)
{
	CLAN_DATA *clan;
	ROSTER_DATA *roster;
	char buf[MAX_STRING_LENGTH], arg[MAX_INPUT_LENGTH];

	if (!HAS_ROSTER( ch )) // No mobs.
		return;

	argument = one_argument(argument, arg);

	smash_tilde(argument);
	GET_ROSTER( ch, roster );

	if (IS_IMMORTAL( ch ))
	{
		if (get_trust(ch) < SUPREME)
		{
			send_to_char("You don't get access to this. Get back to work!\n\r",
					ch);
			return;
		}
		if (!str_cmp(arg, "list"))
		{
			BUFFER *output = new_buf();
			int i;

			for (i = 1, clan = clan_list; clan != NULL; i++, clan = clan->next)
			{
				sprintf(buf, "{R%3d{B){x %s{w %s ", i, end_string(clan->c_name,
						MAX_CLAN_WHO_LENGTH ), clan->name);
				add_buf(output, end_string(buf, 40));
				if (i % 2 == 0 // Organise as four per line
						|| clan->next == NULL)
					add_buf(output, "{x\n\r");
			}
			if (i > 1)
				send_to_char(output->string, ch);
			else
				send_to_char("There are no clans!\n\r", ch);

			free_buf(output);
			return;
		}
		else if (!str_cmp(arg, "create"))
		{
			cedit_create(ch, argument);
			return;
		}
		else if ((clan = get_clan_by_name(arg)) == NULL)
		{
			send_to_char(
					"Clan not found. Use cedit list for a list of clans.\n\r",
					ch);
			return;
		}
		if ((clan->id == CLAN_NONE || clan->id == CLAN_LONER || clan->id
				== CLAN_PK_OUTCAST || clan->id == CLAN_NONPK_OUTCAST
				|| clan->id == CLAN_IMMORTAL) && str_cmp(ch->name, "Nico"))
		{
			send_to_char("This clan cannot be edited.\n\r", ch);
			return;
		}
	}
	else
	{
		clan = roster->clan;

		if (clan->independent)
		{
			if (IS_SET( roster->flags, ROST_CONFIRM_NEW ))
			{
				char c_name[MAX_STRING_LENGTH], *name;

				REMOVE_BIT( roster->flags, ROST_CONFIRM_NEW );

				strcpy(c_name, argument);
				name = strip_spaces(argument);
				name = strip_spec_char_col(name);

				if (!str_cmp(arg, "confirm") && *name != '\0')
				{
					if (strlen_color(c_name) > MAX_CLAN_WHO_LENGTH)
					{
						send_to_char(
								"The clan who name(ie the one you provided) has to be 12 characters or less.\n\r",
								ch);
						return;
					}

					if (ch->platinum < PRICE_CLAN_PLATINUM || ch->questpoints
							< PRICE_CLAN_QP) // Bust the cheaters
					{
						send_to_char("Where did all the dough go?\n\r", ch);
						return;
					}

					clan = new_clan_data(TRUE);
					free_string(clan->name);
					clan->name = str_dup(name);
					free_string(clan->c_name);
					clan->c_name = str_dup(c_name);
					clan->edit_time = current_time + (2 * TIME_HOUR);

					if (roster->clan->pkill)
						clan->pkill = TRUE;

					roster->trust = TRUST_LDR; // Set trust to leader trust
					roster->clan = clan; // Make character's clan equal new clan
					roster->petition = NULL; // Stop any accidental petition accepts
					ch->platinum -= PRICE_CLAN_PLATINUM; // Deduct price from ch to put into clan
					ch->questpoints -= PRICE_CLAN_QP; // Ditto

					clan->qps = PRICE_CLAN_QP; // Price of clan is invested into clan
					clan->platinum = PRICE_CLAN_PLATINUM; // Price of clan is invested into clan
					(clan->members)++;

					assign_clan_vnums(clan);

					send_to_char(
							"{GCongratulations! You have created your clan!{x\n\r"
								"You have two day to edit basic clan data such as name and clan ranks.\n\r"
								"After this time, you will need to buy extra time - this does not effect clan upgrades..\n\r"
								"{RAny inappropriate clan names or ranks will be dealt with.{x\n\r",
							ch);

					append_clan_log(clan, ENTRY_CREATE, ch->name,
							"{wClan created");
					save_all_clans();
					save_roster_data(roster);
				}
				else
				{
					send_to_char("Clan creation status removed.\n\r", ch);
					return;
				}
			}
			else if (!str_cmp(arg, "buy"))
			{
				if (IS_SET(roster->flags, ROST_NEWBIE))
				{
					send_to_char(
							"Newbies can't create clans. Type \"{Rnotnewbie{x\" to un-newbie yourself.\r\n",
							ch);
					return;
				}

				if (ch->platinum < PRICE_CLAN_PLATINUM || ch->questpoints
						< PRICE_CLAN_QP) // Say no to the poor people
				{
					sprintf(
							buf,
							"You need {R%d{x platinum and {R%d{x Quest points to buy a clan.\n\r",
							PRICE_CLAN_PLATINUM, PRICE_CLAN_QP );
					send_to_char(buf, ch);
					return;
				}

				SET_BIT( roster->flags, ROST_CONFIRM_NEW );
				send_to_char(
						"Type \"{Gcedit confirm <clanname>{x\" to confirm your decision to buy a clan.\n\r"
							"The clan name you specify can be the who name of the clan, but it cannot exceed 12 characters.\n\r",
						ch);
				sprintf(
						buf,
						"The cost will be {R%d{x platinum and {R%d{x Quest points.\n\r",
						PRICE_CLAN_PLATINUM, PRICE_CLAN_QP );
				send_to_char(buf, ch);
				send_to_char(
						"Please note, if you are inactive for a long period of time, you will lose your leadership status.\n\r"
							"Type \"{Rcedit <anything else>{x\" to cancel.\n\r",
						ch);
				return;
			}
			else
			{
				sprintf(
						buf,
						"Use {Rcedit buy{x to buy a clan for {Y%d{x platinum and {Y%d{x Quest points.\n\r",
						PRICE_CLAN_PLATINUM, PRICE_CLAN_QP );
				send_to_char(buf, ch);
				return;
			}
		}
	}
	SET_EDIT_ITEM( ch, NULL );
	SET_EDIT_CLAN( ch, clan );
	SET_EDIT_TYPE( ch, ED_CEDIT );

	//cedit_show(ch, "");
}

/****************************** CLAN MANAGEMENT COMMANDS ******************************/

/* For Immortals to guild people without going into cedit */
void do_guild(CHAR_DATA *ch, char *argument)
{
	CLAN_DATA *clan;
	char name[MAX_INPUT_LENGTH];

	argument = one_argument(argument, name);

	if (argument[0] == '\0' || name[0] == '\0')
	{
		send_to_char("Syntax: guild <character> <clan>\n\r", ch);
		return;
	}

	if ((clan = get_clan_by_name(argument)) == NULL)
	{
		send_to_char("Clan not found.\n\r", ch);
		return;
	}

	SET_EDIT_CLAN( ch, clan );
	if (cedit_guild(ch, name))
		save_clan_data(clan);
	SET_EDIT_CLAN( ch, NULL );
}

/* For Immortals to set the newbie flag */
void do_newbie(CHAR_DATA *ch, char *argument)
{
	ROSTER_DATA *roster;

	if (argument[0] == '\0')
	{
		send_to_char(
				"Syntax: newbie <player> - set the newbie flag on a player\n\r",
				ch);
		return;
	}
	if ((roster = get_roster_by_name(argument, FALSE)) == NULL)
	{
		send_to_char("Player not found.\n\r", ch);
		return;
	}
	if (IS_SET( roster->flags, ROST_NEWBIE ))
	{
		send_to_char("That player is already a newbie.\n\r", ch);
		return;
	}
	if (!roster->clan->pkill)
	{
		send_to_char("That player is not a player-killer.\n\r", ch);
		return;
	}
	SET_BIT( roster->flags, ROST_NEWBIE );
	send_to_char("Player newbiefied.\n\r", ch);
	if (roster->character)
		send_to_char("Your newbie flag has been set.\n\r", roster->character);
}

/* Remove newbie flag */
void do_notnewbie(CHAR_DATA *ch, char *argument)
{
	ROSTER_DATA *roster;

	if (!HAS_ROSTER( ch ))
		return;

	GET_ROSTER( ch, roster );

	if (!IS_SET( roster->flags, ROST_NEWBIE ))
	{
		send_to_char("You've already un-newbified.\n\r", ch);
		return;
	}
	if (!str_cmp(argument, "confirm"))
	{
		REMOVE_BIT( roster->flags, ROST_NEWBIE );
		send_to_char("Welcome to the world of proper player-killing!\n\r", ch);
		return;
	}
	send_to_char(
			"If you un-newbify yourself, you leave yourself open to full player-killing.\n\r"
				"It is recommended that you find yourself a clan to join before you un-newbify.\n\r"
				"If you are ready to remove your newbie flag, type \"{Rnotnewbie confirm{x\".\n\r",
			ch);
}

/* Command to become a player-killer */
void do_go_pkill(CHAR_DATA *ch, char *argument)
{
	ROSTER_DATA *roster;

	if (!HAS_ROSTER( ch ))
		return;

	GET_ROSTER( ch, roster );

	if (ch->pcdata->tier == 1 && ch->level > 25)
	{
		send_to_char("The deadline for this tier was level 25.\n\r"
			"Last chance will be next tier, before reaching level 26.\n\r", ch);
		return;
	}
	else if (ch->pcdata->tier == 3 || (ch->pcdata->tier == 2 && ch->level > 25))
	{
		send_to_char(
				"You should have decided this earlier. Level 25, tier 2 was the deadline.\n\r",
				ch);
		return;
	}
	if (roster->clan->pkill)
	{
		send_to_char("You are already a player-killer.\n\r", ch);
		return;
	}
	if (IS_SET( roster->flags, ROST_CONFIRM_PK ))
	{
		REMOVE_BIT( roster->flags, ROST_CONFIRM_PK );
		if (!str_cmp(argument, "yes"))
		{
			roster->clan = get_clan_by_id(CLAN_LONER);
			(roster->clan->members)++;
			SET_BIT( roster->flags, ROST_NEWBIE );
			send_to_char(
					"Congratulations, you are now a player-killer!\n\r"
						"To start with, you will be marked as a newbie.\n\r"
						"This means you can only player-kill other newbie player-killers.\n\r"
						"In turn, only other newbie player-killers can player-kill you.\n\r"
						"You can only join a clan if you remove this flag using {Rnotnewbie{x.\n\r"
						"It is recommended you build your character before doing so.\n\r",
					ch);
			return;
		}
		send_to_char("Player-kill confirmation status removed.\n\r", ch);
		return;
	}

	send_to_char(
			"Player-killing isn't for the faint of heart but can be rewarding.\n\r"
				"Expect to be killed - even the best lose sometimes.\n\r"
				"Type \"{Rpkill yes{x\" to confirm your choice or \"{Gpkill <anything>{x\" to cancel.\n\r"
				"The deadline for this choice is level 25, tier 2.\n\r", ch);
	SET_BIT( roster->flags, ROST_CONFIRM_PK );
}

/* Petition clan for membership */
void do_petition(CHAR_DATA *ch, char *argument)
{
	CHAR_DATA *vch;
	CLAN_DATA *clan;
	ROSTER_DATA *roster, *vchRoster;
	char buf[MAX_STRING_LENGTH];

	if (!HAS_ROSTER( ch ))
		return;

	GET_ROSTER( ch, roster );

	if (!roster->clan->independent)
	{
		send_to_char("Type loner to leave your clan.\n\r"
			"{RNOTE: {xIt will cost you a third of your donation points.\n\r",
				ch);
		return;
	}

	if (roster->clan->id == CLAN_PK_OUTCAST || roster->clan->id
			== CLAN_NONPK_OUTCAST)
	{
		send_to_char("You're an outcast. No one wants you.\n\r", ch);
		return;
	}

	if (IS_SET( roster->flags, ROST_NEWBIE ))
	{
		send_to_char(
				"Newbies can't join clans - type notnewbie to unnewbie yourself.\n\r",
				ch);
		return;
	}

	if (argument[0] == '\0')
	{
		send_to_char(
				"Syntax: petition <clan> - petition a clan for membership\n\r"
					"        petition cancel - cancel your current clan petition\n\r",
				ch);
		if (roster->petition)
		{
			sprintf(buf,
					"You are currently petitioning %s{x for membership.\n\r",
					strip_bespaces(roster->petition->c_name));
			send_to_char(buf, ch);
		}
		return;
	}

	if (!str_cmp(argument, "cancel"))
	{
		send_to_char("Clan petition cleared.\n\r", ch);

		sprintf(
				buf,
				"{RPETITION CANCEL {r>{R>{r> {w%s {Whas cancelled their petition for clan membership.{x\n\r",
				ch->name);

		for (vch = char_list; vch != NULL; vch = vch->next)
		{
			if (!HAS_ROSTER( vch ))
				continue;

			GET_ROSTER( vch, vchRoster );

			if (roster->petition == vchRoster->clan)
				send_to_char(buf, vch);
		}
		roster->petition = NULL;
		return;
	}

	if ((clan = get_clan_by_name(argument)) == NULL)
	{
		send_to_char("Clan not found.\n\r", ch);
		return;
	}

	if (roster->petition)
	{
		sprintf(buf, "You have already petitioned %s{x for membership.\n\r",
				strip_bespaces(roster->petition->c_name));
		send_to_char(buf, ch);
		send_to_char("Type {Rpetition cancel{x to cancel petition.\n\r", ch);
		return;
	}

	if (roster->clan->pkill && !clan->pkill)
	{
		send_to_char(
				"You're a player killer - you cannot petition that clan as its a non-pk clan.\n\r",
				ch);
		return;
	}
	else if (!roster->clan->pkill && clan->pkill)
	{
		send_to_char(
				"You're not a player killer - you cannot petition that clan as its a player-killing clan.\n\r",
				ch);
		return;
	}

	if (clan->independent)
	{
		send_to_char("Petition a proper clan.\n\r", ch);
		return;
	}

	if (((clan->pkill && clan->members >= MAX_PK_CLAN_MEMBERS) || (!clan->pkill
			&& clan->members >= MAX_NONPK_CLAN_MEMBERS)) && !clan->independent)
	{
		send_to_char(
				"The clan has already reached the maximum number of memebers.\n\r",
				ch);
		return;
	}

	roster->petition = clan;
	sprintf(buf, "Clan %s{x petitioned for membership.\n\r", strip_bespaces(
			clan->c_name));
	send_to_char(buf, ch);

	sprintf(
			buf,
			"{GPETITION {r>{R>{r> {w%s {Whas petitioned for clan membership.{x\n\r",
			ch->name);
	for (vch = char_list; vch != NULL; vch = vch->next)
	{
		if (!HAS_ROSTER( vch ))
			continue;

		GET_ROSTER( vch, vchRoster );

		if (clan == vchRoster->clan)
			send_to_char(buf, vch);
	}
}

/* Escape the grasps of a clan */
void do_loner(CHAR_DATA *ch, char *argument)
{
	ROSTER_DATA *roster;
	char buf[MAX_STRING_LENGTH];

	if (!HAS_ROSTER( ch ))
		return;

	GET_ROSTER( ch, roster );

	if (roster->clan->independent)
	{
		send_to_char("You can't be any more alone.\n\r", ch);
		return;
	}

	if (roster->trust == TRUST_LDR)
	{
		send_to_char(
				"You're the leader of your clan - ask an immortal first!\n\r",
				ch);
		return;
	}

	if (IS_SET( roster->flags, ROST_CONFIRM_LONER ))
	{
		REMOVE_BIT( roster->flags, ROST_CONFIRM_LONER );
		if (!str_cmp(argument, "yes"))
		{
			roster->dPoints = roster->dPoints * 2 / 3;
			roster->trust = TRUST_ALL;
			roster->penalty_time = 0;
			REMOVE_BIT( roster->flags, ROST_INACTIVE );

			if (IS_SET( roster->flags, ROST_EXILE_GRACE ))
			{
				REMOVE_BIT( roster->flags, ROST_EXILE_GRACE );
				send_to_char(
						"You have avoided being put into the outcast clan for voluntarily leaving.\n\r",
						ch);
			}

			if (roster->clan && !roster->clan->independent)
				roster->clan->members--;
			if (roster->clan->pkill)
				roster->clan = get_clan_by_id(CLAN_LONER);
			else
				roster->clan = get_clan_by_id(CLAN_NONE);

			send_to_char("You are now a {Rloner{x.\n\r", ch);
			sprintf(buf,
					"Your donation points have been reduced to {R%ld{x.\n\r",
					roster->dPoints);
			send_to_char(buf, ch);
			return;
		}
		send_to_char("Loner confirmation status removed.\n\r", ch);
	}
	else
	{
		SET_BIT( roster->flags, ROST_CONFIRM_LONER );

		send_to_char(
				"If you choose to loner yourself, you will lose a third of your points.\n\r"
					"Type {Rloner yes{x to confirm, or loner with anything else to clear.\n\r",
				ch);
		if (IS_SET( roster->flags, ROST_EXILE_GRACE ))
			send_to_char(
					"This is a better option than getting exiled, as you lose fewer points.\n\r"
						"You will also be able to get reclanned straight away, rather than waiting a week.\n\r",
					ch);
	}
}

/* Exile a player */
bool exile_player(ROSTER_DATA *roster)
{
	if (roster->clan->independent)
		return FALSE;

	if (IS_SET( roster->flags, ROST_EXILE_GRACE )) // If they were graced
	{
		REMOVE_BIT( roster->flags, ROST_EXILE_GRACE );
		if (roster->character)
			send_to_char(
					"{RThe grace period in which you could quit voluntarily has expired.{x\n\r",
					roster->character);
		append_clan_log(roster->clan, ENTRY_EXILE, roster->name,
				"{wAutomatically exiled from clan - end of grace");
	}

	if (roster->clan && !roster->clan->independent)
		roster->clan->members--;

	if (roster->clan->pkill)
		roster->clan = get_clan_by_id(CLAN_PK_OUTCAST); // PK go here
	else
		roster->clan = get_clan_by_id(CLAN_NONPK_OUTCAST); // Non-pk go here

	roster->trust = TRUST_ALL; // Trust back to 0
	roster->dPoints /= 2; // Halve their donation points
	roster->penalty_time = current_time + TIME_WEEK; // They get out of the outcast clan in a week

	REMOVE_BIT( roster->flags, ROST_INACTIVE );
	REMOVE_BIT( roster->flags, ROST_CONFIRM_NEW );
	REMOVE_BIT( roster->flags, ROST_CONFIRM_PK );
	REMOVE_BIT( roster->flags, ROST_CONFIRM_LONER );

	if (roster->character)
		send_to_char("{RYou have been exiled from your clan!{x\n\r",
				roster->character);

	return FALSE;
}

/********************************** GENERAL COMMANDS **********************************/

/* Clan talk */
void do_ctalk(CHAR_DATA *ch, char *argument)
{
	CLAN_DATA *clan = NULL;
	DESCRIPTOR_DATA *d;
	ROSTER_DATA *roster = NULL, *vchRoster;
	char buf[MAX_STRING_LENGTH], arg[MAX_INPUT_LENGTH];

	if (IS_NPC( ch ))
	{
		if ((clan = get_clan_by_vnum(ch->pIndexData->vnum)) == NULL)
			return;
	}
	else if (IS_IMMORTAL( ch ) && get_trust(ch) >= IMPLEMENTOR)
	{
		argument = one_argument(argument, arg);
		if (arg[0] == '\0' || (clan = get_clan_by_name(arg)) == NULL)
		{
			send_to_char("Syntax: clan <clan name> <what you want to say>\n\r",
					ch);
			return;
		}
	}
	else
	{
		GET_ROSTER( ch, roster );
		if (roster == NULL)
			return;

		clan = roster->clan;

		if (clan->independent)
		{
			send_to_char("You aren't in a clan!\n\r", ch);
			return;
		}
	}

	if (argument[0] == '\0')
	{
		if (IS_SET( ch->comm, COMM_NOCLAN ))
		{
			send_to_char("{RClan{x channel is now {GON{x\n\r", ch);
			REMOVE_BIT( ch->comm, COMM_NOCLAN );
		}
		else
		{
			send_to_char("{RClan{x channel is now {ROFF{x\n\r", ch);
			SET_BIT( ch->comm, COMM_NOCLAN );
		}
		return;
	}

	if (!IS_IMMORTAL(ch))
	{
		if (IS_SET(ch->comm, COMM_NOCHANNELS))
		{
			send_to_char("The gods have revoked your channel priviliges.\n\r",
					ch);
			return;
		}
		else if (ch->in_room->vnum == ROOM_VNUM_CORNER)
		{
			send_to_char(
					"Just keep your nose in the corner like a good little player.\n\r",
					ch);
			return;
		}
		//        else if ( IS_SET(ch->in_room->room_flags,ROOM_SILENCED) )
		//        {
		//            send_to_char( "You have lost your powers of speech in this room!\n\r",ch );
		//            return;
		//        }
	}

	REMOVE_BIT( ch->comm, COMM_NOCLAN );

	sprintf(buf, "{y[{x%s {W-{x %s{y]{x You '{M%s{x'\n\r", strip_bespaces(
			clan->c_name), IS_NPC(ch) ? "Mob" : (IS_IMMORTAL(ch) && clan->id
			!= CLAN_IMMORTAL) ? "{WI{wm{Dm" : clan->r_who[get_r_rank_index(
			roster)], argument);
	send_to_char(buf, ch);

	sprintf(buf, "{y[{x%s {W-{x %s{y]{x %s '{M%s{x'\n\r", strip_bespaces(
			clan->c_name), IS_NPC(ch) ? "Mob" : (IS_IMMORTAL(ch) && clan->id
			!= CLAN_IMMORTAL) ? "{WI{wm{Dm" : clan->r_who[get_r_rank_index(
			roster)], PERS(ch,ch), argument);

	for (d = descriptor_list; d != NULL; d = d->next)
	{
		if (d->connected != CON_PLAYING || d->character == NULL || d->character
				== ch || IS_SET(d->character->comm, COMM_QUIET)
				|| IS_SET(d->character->comm, COMM_NOCLAN))
			continue;

		GET_ROSTER( d->character, vchRoster );
		if (vchRoster == NULL)
			continue;

		if ((IS_IMMORTAL( d->character ) && get_trust(d->character)
				>= IMPLEMENTOR) || vchRoster->clan == clan)
			send_to_char(buf, d->character);
	}
}

/* Whack a bounty on someones head or show the current bounties */
void do_bounty(CHAR_DATA *ch, char *argument)
{
	DESCRIPTOR_DATA *d;
	ROSTER_DATA *vchRoster;
	char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
	int amt;

	if (argument[0] == '\0')
	{
		send_to_char("Syntax: bounty <player> <platinum>\n\r"
			"        bounty list\n\r", ch);
		return;
	}

	argument = one_argument(argument, arg);

	if (!str_cmp(arg, "list"))
	{
		BUFFER *output = new_buf();
		int capBounty, maxBounty;
		bool found = FALSE;

		add_buf(output, "{B+-{G Bounty List{B ------------------+{x\n\r");
		add_buf(output, "{B| {WName               {B| {WBounty    {B|\n\r");

		for (capBounty = -1;;)
		{
			for (vchRoster = roster_list, maxBounty = 0; vchRoster != NULL; vchRoster
					= vchRoster->next)
			{
				if (IS_SET( vchRoster->flags, ROST_DELETED ))
					continue;
				if (capBounty > -1 && vchRoster->bounty >= capBounty)
					continue;

				if (vchRoster->bounty <= maxBounty)
					continue;

				maxBounty = vchRoster->bounty;
			}
			if (maxBounty <= 0)
				break;
			found = TRUE;
			for (vchRoster = roster_list; vchRoster != NULL; vchRoster
					= vchRoster->next)
			{
				if (IS_SET( vchRoster->flags, ROST_DELETED )
						|| vchRoster->bounty != maxBounty)
					continue;

				sprintf(
						buf,
						"{B|{w %s {B|{Y %-9ld {B|\n\r",
						(vchRoster->character && vchRoster->character == ch) ? end_string(
								"{RYou", 18)
								: end_string(vchRoster->name, 18),
						vchRoster->bounty);
				add_buf(output, buf);
			}
			capBounty = maxBounty;
		}

		add_buf(output, "{B+--------------------------------+{x\n\r");

		if (found)
			page_to_char(output->string, ch);
		else
			send_to_char("No one has a bounty at the moment.\n\r", ch);

		free_buf(output);
		return;
	}
	else if ((vchRoster = get_roster_by_name(arg, FALSE)) == NULL)
	{
		send_to_char("Player not found.\n\r", ch);
		return;
	}

	if (!is_number(argument) || (amt = atoi(argument)) < 50)
	{
		send_to_char("The minimum bounty is 50 platinum.\n\r", ch);
		return;
	}

	if (ch->platinum < amt)
	{
		send_to_char("You cannot cover the cost.\r\n", ch);
		return;
	}
	ch->platinum -= amt;
	vchRoster->bounty += amt;
	save_roster_data(vchRoster);
	sprintf(
			buf,
			"{CBOUNTY {r>{R>{r> {WA bounty of {Y%d{W platinum has been offered for the head of {R%s{W! (Total bounty {Y%ld{W){x\n\r",
			amt, vchRoster->name, vchRoster->bounty);

	for (d = descriptor_list; d != NULL; d = d->next)
	{
		if (d->connected != CON_PLAYING || d->character == NULL
				|| IS_SET( d->character->comm, COMM_QUIET ))
			continue;

		send_to_char(buf, d->character);
	}
}

/*************************************** CHARTS ***************************************/
// Overall clan charts, individual charts, roster, clist

/* Displays all the clans and stats */
void do_clist(CHAR_DATA *ch, char *argument)
{
	BUFFER *output = new_buf();
	CLAN_DATA *clan;
	ROSTER_DATA *roster;
	char buf[MAX_STRING_LENGTH], name[MAX_STRING_LENGTH], *temp,
			temp2[MAX_STRING_LENGTH];
	int trust = get_trust(ch), i;

	GET_ROSTER( ch, roster );

	add_buf(
			output,
			"{B+- {WThe Clans of "GAME_COL_NAME"{B --------------------------------------------+\n\r");

	for (clan = clan_list; clan != NULL; clan = clan->next)
	{
		sprintf(name, "%s {W({w%s{W)", clan->c_name, clan->name);
		sprintf(buf, "{B| %6s {%cClan:{x %s                   {B|\n\r",
				clan->pkill ? "{R>{r>{RPK{r<{R<" : "{yNON-PK", (roster
						&& roster->clan == clan) ? 'G' : 'W', end_string(name,
						46));
		add_buf(output, buf);

		if (trust >= SUPREME)
			sprintf(
					buf,
					"{B|  {WMembers:{w %6d  {W%s Points:{w %6ld  {WQuest:{w   %6ld  {WPlatinum:{w     %6ld {B|\n\r",
					clan->members, clan->pkill ? "PKill" : "Arena",
					clan->pkilldata->kPoints, clan->qps, clan->platinum);
		else
			sprintf(
					buf,
					"{B|  {WMembers:{w %6d  {W%s Points:{w %6ld                                        {B|\n\r",
					clan->members, clan->pkill ? "PKill" : "Arena",
					clan->pkilldata->kPoints);
		add_buf(output, buf);

		sprintf(
				buf,
				"{B|  {WKills:{g   %6d  {WDeaths:{r       %6d  {WWins:{g    %6d  {WLosses:{r       %6d {B|\n\r",
				clan->pkilldata->kills, clan->pkilldata->deaths,
				clan->pkilldata->wins, clan->pkilldata->losses);
		add_buf(output, buf);

		for (temp = clan->description, i = 0; *temp; i++, temp++)
		{
			if ((*temp == '\n' && *(temp + 1) == '\r') || (*temp == '\r'
					&& *(temp + 1) == '\n'))
			{
				temp2[i] = ' ';
				temp++;
				continue;
			}
			else if (*temp == '\n' || *temp == '\r')
			{
				temp2[i] = ' ';
				continue;
			}

			temp2[i] = *temp;
		}
		temp2[i] = '\0';

		temp = temp2;
		temp = length_argument(temp, name, 64);
		sprintf(buf, "{WDescription: {w%s", name);
		sprintf(name, "{B| %s {B|\n\r", end_string(buf, 77));
		add_buf(output, name);

		while (*temp != '\0')
		{
			temp = length_argument(temp, buf, 64);
			sprintf(name, "{B|              {w%s {B|\n\r", end_string(buf, 64));
			add_buf(output, name);
		}

		add_buf(
				output,
				"{B+-------------------------------------------------------------------------------+\n\r");
	}

	add_buf(output, "{x");
	page_to_char(output->string, ch);
	free_buf(output);
}

/* Show details on a clan */
void do_clan_roster(CHAR_DATA *ch, char *argument)
{
	BUFFER *output;
	CLAN_DATA *clan;
	ROSTER_DATA *roster, *chRoster;
	char buf[MAX_STRING_LENGTH], name[500], rankName[500];
	int maxPoints, capPoints, i;
	bool found = FALSE;

	if (argument[0] == '\0')
	{
		send_to_char(
				"Syntax: roster <clanname> - display the roster of a clan\n\r",
				ch);
		return;
	}

	if ((clan = get_clan_by_name(argument)) == NULL)
	{
		send_to_char("Clan not found.\n\r", ch);
		return;
	}

	if (clan->id == CLAN_IMMORTAL && !IS_IMMORTAL( ch ))
	{
		send_to_char("Mind your own business.\n\r", ch);
		return;
	}

	GET_ROSTER( ch, chRoster );
	output = new_buf();

	sprintf(buf, "{B+- {GClan Roster of{x %s {B", strip_bespaces(clan->c_name));
	add_buf(output, buf);

	for (i = (67 - strlen_color(strip_bespaces(clan->c_name))); i > 0; i--)
		add_buf(output, "-");

	add_buf(output, "+\n\r");

	add_buf(
			output,
			"{B|{W Rank              {B| {WName               {B| {WPoints {B| {WKills  {B| {WDeaths {B| {WWins   {B| {WLosses {B|\n\r");

	for (capPoints = -1;;) // Initialise capPoints as -1 because we haven't found out what the highest value is yet
	{
		for (maxPoints = -1, roster = roster_list; roster != NULL; roster
				= roster->next)
		{
			if (IS_SET( roster->flags, ROST_DELETED ))
				continue;
			// Display only those in the desired clan
			if (roster->clan != clan || (capPoints >= 0 && get_total_points(
					roster) >= capPoints)) // Next highest value must be below previous highest value
				continue; // (only if its not the very first highest value)
			else if (get_total_points(roster) > maxPoints) // See if its the highest value of this round
				maxPoints = get_total_points(roster);
		}
		if (maxPoints < 0) // Break out from it all if there aren't any more records that are less than cap points
			break;
		for (roster = roster_list; roster != NULL; roster = roster->next)
		{
			if (IS_SET( roster->flags, ROST_DELETED ))
				continue;
			if (roster->clan == clan && get_total_points(roster) == maxPoints) // Go through all the players with the same point value as the current max value
			{
				found = TRUE;
				sprintf(name, "%s%s",
						(chRoster && roster == chRoster) ? "{GYou"
								: capitalize(roster->name), roster->trust
								== TRUST_LDR ? " {W({RLdr{W)" : roster->trust
								== TRUST_VLR ? " {W({CVlr{W)" : "");

				sprintf(rankName, "%s",
						clan->independent ? "{wI{Dndependent      "
								: end_string(clan->r_name[get_r_rank_index(
										roster)], MAX_CLAN_RANK_LENGTH ));

				sprintf(
						buf,
						"{B|{w %s {B|{w %s {B|{Y %-6ld {B|{g %-6d {B|{r %-6d {B|{g %-6d {B|{r %-6d {B|\n\r",
						rankName, end_string(name, 18),
						get_total_points(roster), roster->pkilldata->kills,
						roster->pkilldata->deaths, roster->pkilldata->wins,
						roster->pkilldata->losses);
				add_buf(output, buf);
			}
		}
		capPoints = maxPoints;
	}

	add_buf(
			output,
			"{B+-------------------------------------------------------------------------------------+{x\n\r");

	if (found)
		page_to_char(output->string, ch);
	else
	{
		sprintf(buf, "There isn't anyone in %s{x\n\r", strip_bespaces(
				clan->c_name));
		send_to_char(buf, ch);
	}
	free_buf(output);
}

/* Display the top 20 */
void do_rank(CHAR_DATA *ch, char *argument)
{
	BUFFER *output;
	ROSTER_DATA *roster, *chRoster;
	char buf[MAX_STRING_LENGTH], arg[MAX_INPUT_LENGTH];
	int maxPoints, capPoints, rankCount;
	bool pkOnly = FALSE, npkOnly = FALSE;

	if (argument[0] == '\0')
	{
		send_to_char(
				"Syntax: rank pk             - shows the top 20 PK players\n\r"
					"        rank nonpk          - shows the top 20 NON-PK players\n\r"
					"        rank all            - shows the top 20 PK/NON-PK players\n\r"
					"        rank <person> arena - shows how many points you would get for killing the person in the arena\n\r"
					"        rank <person> pk    - shows how many points you would get for killing the person in pk\n\r",
				ch);
		return;
	}
	argument = one_argument(argument, arg);
	GET_ROSTER( ch, chRoster );

	if (!str_cmp(arg, "pk"))
		pkOnly = TRUE;
	else if (!str_cmp(arg, "nonpk"))
		npkOnly = TRUE;
	else if (str_cmp(arg, "all"))
	{
		if (chRoster == NULL)
			return;
		if ((roster = get_roster_by_name(arg, FALSE)) == NULL)
		{
			send_to_char("Player not found.\n\r", ch);
			return;
		}
		if (roster == chRoster)
		{
			send_to_char(
					"Suicide is a sin. You don't gain points from it.\n\r", ch);
			return;
		}
		if (!str_cmp(argument, "arena"))
		{
			sprintf(
					buf,
					"You would receive {R%d{x kill points for killing {R%s{x in the arena.\n\r",
					rank_kill(chRoster, roster, TRUE, TRUE ), roster->name);
			send_to_char(buf, ch);
			return;
		}
		else if (!str_cmp(argument, "pk"))
		{
			if (!chRoster->clan->pkill || !roster->clan->pkill)
			{
				send_to_char(
						"You both have to be player killers to use this option.\n\r",
						ch);
				return;
			}
			sprintf(
					buf,
					"You would receive {R%d{x kill points for killing {R%s{x in pk.\n\r",
					rank_kill(chRoster, roster, FALSE, TRUE ), roster->name);
			send_to_char(buf, ch);
			return;
		}
		send_to_char(
				"Please specify {Rpk{x or {Rarena{x after the player's name.\n\r",
				ch);
		return;
	}

	output = new_buf();

	if (pkOnly)
	{
		add_buf(
				output,
				"{B+-{G Top 20 PK Players {B------------------------------------------------------------------+\n\r");
		add_buf(
				output,
				"{B|{W No. {B|{W Clan         {B|{W Name               {B|{W Points {B|{W Kills  {B|{W Deaths {B|{W Wins   {B|{W Losses {B|\n\r");
	}
	else if (npkOnly)
	{
		add_buf(
				output,
				"{B+-{G Top 20 NON-PK Players {B--------------------------------------------------------------+\n\r");
		add_buf(
				output,
				"{B|{W No. {B|{W Clan         {B|{W Name               {B|{W Points {B|{W Kills  {B|{W Deaths {B|{W Wins   {B|{W Losses {B|\n\r");
	}
	else
	{
		add_buf(
				output,
				"{B+-{G Top 20 ALL Players {B-----------------------------------------------------------------------+\n\r");
		add_buf(
				output,
				"{B|{W PK  {B|{W No. {B|{W Clan         {B|{W Name               {B|{W Points {B|{W Kills  {B|{W Deaths {B|{W Wins   {B|{W Losses {B|\n\r");
	}

	for (capPoints = -1, rankCount = 0;;) // Initialise capPoints as -1 because we haven't found out what the highest value is yet
	{
		for (maxPoints = -1, roster = roster_list; roster != NULL; roster
				= roster->next)
		{
			if (capPoints >= 0 && roster->pkilldata->kPoints >= capPoints) // Next highest value must be below previous highest value
				continue; // (only if its not the very first highest value)
			else if (roster->pkilldata->kPoints > maxPoints) // See if its the highest value of this round
				maxPoints = roster->pkilldata->kPoints;
		}
		if (maxPoints <= 0) // Break out from it all if there aren't any more records that are less than cap points and less than 0
			break;
		for (roster = roster_list; roster != NULL; roster = roster->next)
		{
			if ((pkOnly && !roster->clan->pkill) || (npkOnly
					&& roster->clan->pkill))
				continue;
			if (rankCount < 20 && roster->pkilldata->kPoints == maxPoints) // Go through all the players with the same point value as the current max value
			{
				rankCount++;
				if (!pkOnly && !npkOnly)
					sprintf(
							buf,
							"{B| %3s {B|{R %-3d {B|{x %s {B|{w %-18s {B|{Y %-6ld {B|{g %-6d {B|{r %-6d {B|{g %-6d {B|{r %-6d {B|\n\r",
							roster->clan->pkill ? "{YYes" : "{yNo ", rankCount,
							end_string(roster->clan->c_name,
									MAX_CLAN_WHO_LENGTH ), (chRoster && roster
									== chRoster) ? "You" : capitalize(
									roster->name), roster->pkilldata->kPoints,
							roster->pkilldata->kills,
							roster->pkilldata->deaths, roster->pkilldata->wins,
							roster->pkilldata->losses);
				else
					sprintf(
							buf,
							"{B|{R %-3d {B|{x %s {B|{w %-18s {B|{Y %-6ld {B|{g %-6d {B|{r %-6d {B|{g %-6d {B|{r %-6d {B|\n\r",
							rankCount, end_string(roster->clan->c_name,
									MAX_CLAN_WHO_LENGTH ), (chRoster && roster
									== chRoster) ? "You" : capitalize(
									roster->name), roster->pkilldata->kPoints,
							roster->pkilldata->kills,
							roster->pkilldata->deaths, roster->pkilldata->wins,
							roster->pkilldata->losses);

				add_buf(output, buf);
			}
		}
		if (rankCount >= 20) // Only top twenty
			break;
		capPoints = maxPoints;
	}

	if (pkOnly || npkOnly)
		add_buf(
				output,
				"{B+--------------------------------------------------------------------------------------+{x\n\r");
	else
		add_buf(
				output,
				"{B+--------------------------------------------------------------------------------------------+{x\n\r");

	if (rankCount > 0)
		page_to_char(output->string, ch);
	else
		send_to_char(
				"There isn't a top 20! Go kill someone and become number one!\n\r",
				ch);

	free_buf(output);
}

/* Display the table of pk history */
void do_pk_history(CHAR_DATA *ch, char *argument)
{
	BUFFER *output;
	char buf[MAX_STRING_LENGTH], temp[MAX_STRING_LENGTH], flgs[30];
	int i, invalid;

	if (max_pkhist_entries <= 0)
	{
		send_to_char("There isn't any PK history.\n\r", ch);
		return;
	}

	output = new_buf();
	add_buf(
			output,
			"{B+-{G Pkill History {B--------------------------------------------------------------------------------------------------+\n\r");
	add_buf(
			output,
			"{B| {WFLG {B|{W Killer            {B|{W Clan         {B|{W Lvl {B|{W Victim            {B|{W Clan         {B|{W Lvl {B|{W Time                     {B|\n\r");

	for (i = 0; i < max_pkhist_entries; i++)
	{
		if (pkhist_table[i].killer == NULL || pkhist_table[i].victim == NULL)
			continue;

		if (IS_SET( pkhist_table[i].flags, PKHIST_INVALID )
				|| (pkhist_table[i].time + TIME_SPAMKILL_LIMIT) < current_time)
			invalid = TRUE;
		else
			invalid = FALSE;

		sprintf(temp, "%s", pkhist_table[i].kClan ? end_string(
				pkhist_table[i].kClan->c_name, MAX_CLAN_WHO_LENGTH )
				: end_string("Unknown", MAX_CLAN_WHO_LENGTH ));
		sprintf(
				flgs,
				"%s%s%s",
				IS_SET( pkhist_table[i].flags, PKHIST_ARENA ) ? "{CA" : " ",
				(invalid || IS_SET( pkhist_table[i].flags, PKHIST_CHINVALID )) ? "{RI"
						: "{GV",
				(IS_SET( pkhist_table[i].flags, PKHIST_CHINVALID ) && !invalid) ? "{rE"
						: " ");
		sprintf(
				buf,
				"{B| %s {B|{w %-17.17s {B|{x %s {B|{R %-3d {B|{w %-17.17s {B|{w %s {B|{R %-3d {B|{w %24.24s {B|\n\r",
				flgs, pkhist_table[i].killer->name, temp,
				pkhist_table[i].kLevel, pkhist_table[i].victim->name,
				pkhist_table[i].vClan ? end_string(
						pkhist_table[i].vClan->c_name, MAX_CLAN_WHO_LENGTH )
						: end_string("Unknown", MAX_CLAN_WHO_LENGTH ),
				pkhist_table[i].vLevel, ctime(&(pkhist_table[i].time)));
		add_buf(output, buf);
	}

	add_buf(
			output,
			"{B+------------------------------------------------------------------------------------------------------------------+{x\n\r");
	page_to_char(output->string, ch);
	free_buf(output);
}

/*************************************** CHECKS ***************************************/

bool is_enemy(CHAR_DATA *ch, CHAR_DATA *vch)
{
	CLAN_DATA *chClan, *vchClan;

	chClan = get_clan_by_ch(ch);
	vchClan = get_clan_by_ch(vch);

	if (chClan == NULL || vchClan == NULL)
		return FALSE;

	if (chClan->independent || vchClan->independent)
		return FALSE;

	return chClan->enemy == vchClan;
}

bool clan_can_use(CHAR_DATA *ch, OBJ_DATA *obj)
{
	return can_wear_clan_eq(ch, obj, FALSE );
}

bool is_clan_obj(OBJ_DATA *obj)
{
	return is_full_clan_obj(obj, NULL, FALSE );
}

bool is_clan_obj_ind(OBJ_INDEX_DATA *pObj)
{
	return is_full_clan_obj_index(pObj, NULL, FALSE );
}

bool is_clead(CHAR_DATA *ch)
{
	ROSTER_DATA *roster;
	GET_ROSTER( ch, roster );

	if (roster == NULL)
		return TRUE;
	return roster->trust == TRUST_LDR;
}

bool is_clan(CHAR_DATA *ch)
{
	return (!is_indep(ch));
}

bool is_pkill(CHAR_DATA *ch)
{
	CLAN_DATA *clan;

	if ((clan = get_clan_by_ch(ch)) == NULL)
		return FALSE;
	return clan->pkill;
}

bool is_indep(CHAR_DATA *ch)
{
	CLAN_DATA *clan;

	if ((clan = get_clan_by_ch(ch)) == NULL)
		return TRUE;
	return clan->independent;
}

/* Check if the object index is part of the clan area and whether it is part of the clan specified (if applicable ) */
bool is_full_clan_obj_index(OBJ_INDEX_DATA *pObj, CLAN_DATA *clan,
		bool checkclan)
{
	if (pObj == NULL || pObj->area == NULL)
		return FALSE;

	if (checkclan && clan == NULL)
		return FALSE;

	if (!checkclan && pObj->area == get_clan_area())
		return TRUE;

	if (checkclan && pObj->vnum >= clan->vnum[0] && pObj->vnum <= clan->vnum[1])
		return TRUE;

	return FALSE;
}

/* Check if the object is part of the clan area and whether it is part of the clan specified (if applicable ) */
bool is_full_clan_obj(OBJ_DATA *obj, CLAN_DATA *clan, bool checkclan)
{
	if (obj == NULL || obj->pIndexData == NULL)
		return FALSE;

	return is_full_clan_obj_index(obj->pIndexData, clan, checkclan);
}

/* Check if the mob index is part of the clan area and whether it is part of the clan specified (if applicable ) */
bool is_clan_mob_index(MOB_INDEX_DATA *pMob, CLAN_DATA *clan, bool checkclan)
{
	if (pMob == NULL || pMob->area == NULL)
		return FALSE;

	if (checkclan && clan == NULL)
		return FALSE;

	if (!checkclan && pMob->area == get_clan_area())
		return TRUE;

	if (checkclan && pMob->vnum >= clan->vnum[0] && pMob->vnum <= clan->vnum[1])
		return TRUE;

	return FALSE;
}

/* Check if the mob is part of the clan area and whether it is part of the clan specified (if applicable ) */
bool is_clan_mob(CHAR_DATA *mob, CLAN_DATA *clan, bool checkclan)
{
	if (mob == NULL || mob->pIndexData == NULL)
		return FALSE;

	return is_clan_mob_index(mob->pIndexData, clan, checkclan);
}

bool is_clan_mob_type(CHAR_DATA *mob, CLAN_DATA *clan, int type)
{
	if (mob->pIndexData == NULL || type >= CMOB_MAX)
		return FALSE;
	if (clan == NULL)
		if ((clan = get_clan_by_vnum(mob->pIndexData->vnum)) == NULL)
			return FALSE;

	if (clan->cMob[type] != NULL && clan->cMob[type] == mob->pIndexData)
		return TRUE;

	return FALSE;
}

/* Check if the room index is part of the clan area and whether it is part of the clan specified (if applicable ) */
bool is_clan_room(ROOM_INDEX_DATA *pRoom, CLAN_DATA *clan, bool checkclan)
{
	CLAN_DATA *tclan;

	if (pRoom == NULL)
		return FALSE;

	if (checkclan && clan == NULL)
		return FALSE;

	tclan = get_clan_by_vnum(pRoom->vnum);

	if ((!checkclan && tclan != NULL) || (checkclan && tclan == clan))
		return TRUE;

	return FALSE;
}

/* Check whether the two are in the same clan */
bool is_same_clan(CHAR_DATA *ch, CHAR_DATA *vch)
{
	CLAN_DATA *chClan, *vchClan;
	chClan = get_clan_by_ch(ch);
	vchClan = get_clan_by_ch(vch);
	// Basically an npc check.
	if (chClan == NULL && vchClan == NULL)
		return FALSE;
	if ((chClan && chClan->independent) || (vchClan && vchClan->independent))
		return FALSE;
	return (chClan == vchClan);
}

/* Check whether a person can kill another */
bool can_pkill_other(CHAR_DATA *ch, CHAR_DATA *vch, bool quiet)
{
	ROSTER_DATA *chRoster, *vchRoster;
	int i;

	GET_ROSTER( ch, chRoster );
	GET_ROSTER( vch, vchRoster );

	if (chRoster == NULL || vchRoster == NULL)
		return TRUE;

	if (IS_IMMORTAL( ch ) && get_trust(ch) >= 108)
		return TRUE;

	// Both player killers
	if (chRoster->clan->pkill && vchRoster->clan->pkill)
	{
		if (!chRoster->clan->independent && chRoster->clan == vchRoster->clan)
		{
			if (!quiet)
				send_to_char("You cannot attack clan members.\n\r", ch);
			return FALSE;
		}

		// Newbie flag if they're not both flagged or unflagged, they can't fight
		if (!IS_SET( chRoster->flags, ROST_NEWBIE )
				&& IS_SET( vchRoster->flags, ROST_NEWBIE ))
		{
			if (!quiet)
				send_to_char("Trying to attack newbies, eh?\n\r", ch);
			return FALSE;
		}
		else if (IS_SET( chRoster->flags, ROST_NEWBIE )
				&& !IS_SET( vchRoster->flags, ROST_NEWBIE ))
		{
			if (!quiet)
				send_to_char(
						"Notnewbie yourself to fight with the Big Boys.\n\r",
						ch);
			return FALSE;
		}

		for (i = 0; i < max_pkhist_entries; i++) // Check spamkill
		{
			if (!pkhist_table[i].killer || !pkhist_table[i].victim
					|| (pkhist_table[i].time + TIME_SPAMKILL_LIMIT)
							< current_time
					|| IS_SET( pkhist_table[i].flags, PKHIST_INVALID ))
				continue;

			if (pkhist_table[i].killer == vchRoster && pkhist_table[i].victim
					== chRoster)
			{
				if (!quiet)
					send_to_char(
							"Because you are the initiator, your victim will be able to attack you again.\n\r",
							ch);

				SET_BIT( pkhist_table[i].flags, PKHIST_INVALID );
			}
			if (pkhist_table[i].kClan && pkhist_table[i].vClan && !is_indep(ch)
					&& pkhist_table[i].vClan == chRoster->clan
					&& pkhist_table[i].kClan == vchRoster->clan
					&& !IS_SET( pkhist_table[i].flags, PKHIST_CHINVALID ))
			{
				if (!quiet)
					send_to_char(
							"The clan members of the person you are attacking may now enter your clan hall.\n\r",
							ch);

				SET_BIT( pkhist_table[i].flags, PKHIST_CHINVALID );
			}
			else if (pkhist_table[i].killer == chRoster
					&& pkhist_table[i].victim == vchRoster)
			{
				if (!quiet)
					send_to_char(
							"You have already killed that person in the last hour.\n\r",
							ch);
				return FALSE; // ch has killed vch in the last hour so he can't attack again.
			}
			else if (pkhist_table[i].victim == vchRoster
					&& pkhist_table[i].kClan && pkhist_table[i].kClan
					== chRoster->clan)
			{
				if (!quiet)
					send_to_char(
							"A member of your clan has already killed that person in the last hour.\n\r",
							ch);
				return FALSE; // Someone in ch's clan has killed vict already
			}
		}
		return TRUE;
	}

	//First case: killer=nonpk, vict=nonpk/pk. Second case: killer=pk, vict=nonpk
	if (!quiet && !chRoster->clan->pkill)
		send_to_char(
				"You're non-pk so you cannot fight other players outside the arena.\n\r",
				ch);
	else if (!quiet && !vchRoster->clan->pkill)
		send_to_char(
				"They're non-pk and cannot be fought outside the arena.\n\r",
				ch);

	return FALSE;
}

bool can_pkill(CHAR_DATA *ch, CHAR_DATA *victim)
{
	if (!is_pkill(ch) || !is_pkill(victim))
		return FALSE;

	if (IS_SET (victim->act, PLR_TWIT))
		return TRUE;

	if (victim->pcdata->tier < 2 && ch->pcdata->tier > 2)
	{
		send_to_char("Pick on someone your own size.\n\r", ch);
		return FALSE;
	}

	if (ch->pcdata->tier < 2 && victim->pcdata->tier > 2)
	{
		send_to_char(
				"They would crush you in moments, better advance first.\n\r",
				ch);
		return FALSE;
	}

	if (ch->level > victim->level + 10)
	{
		send_to_char("Pick on someone your own size.\n\r", ch);
		return FALSE;
	}

	if (ch->level < victim->level - 10)
	{
		send_to_char("Pick on someone your own size.\n\r", ch);
		return FALSE;
	}

	return can_pkill_other(ch, victim, FALSE );
}

/* See whether person can wear the obj
 * Remove pieces if over the limit
 */
bool can_wear_clan_eq(CHAR_DATA *ch, OBJ_DATA *obj, bool remove)
{
	ROSTER_DATA *roster;
	OBJ_DATA *checkObj;
	int max, count;

	if (IS_IMMORTAL( ch ) || !is_full_clan_obj(obj, NULL, FALSE ))
		return TRUE;

	GET_ROSTER( ch, roster ); // No need for null checks after because those are handled by get_eq_wear_max

	if ((max = clan_eq_wear_max(ch)) <= 0 || !is_full_clan_obj(obj,
			roster->clan, TRUE ))
		return FALSE;

	for (count = 0, checkObj = ch->carrying; checkObj != NULL; checkObj
			= checkObj->next_content)
	{
		if (!is_full_clan_obj(checkObj, NULL, FALSE ) || checkObj->wear_loc
				== WEAR_NONE)
			continue;

		if (is_full_clan_obj(checkObj, roster->clan, TRUE ))
		{
			count++;
			if (count >= max)
			{
				if (!remove)
					return FALSE;

				unequip_char(ch, checkObj);
				act("$p slips off you.", ch, checkObj, NULL, TO_CHAR );
				act("$p slips off $n.", ch, checkObj, NULL, TO_ROOM );
			}
		}
		else
		{
			unequip_char(ch, checkObj); // Bust the naughty players who keep another clans eq on
			act("$p slips off you.", ch, checkObj, NULL, TO_CHAR );
			act("$p slips off $n.", ch, checkObj, NULL, TO_ROOM );
		}
	}
	return TRUE;
}

/* Update the spamkill/pk history table */
void update_pkhist(ROSTER_DATA *chRoster, ROSTER_DATA *vchRoster, bool arena)
{
	PKHIST_DATA *pkhist;

	max_pkhist_entries++;

	if (pkhist_table == NULL && (pkhist = malloc(sizeof(PKHIST_DATA))) == NULL) // If there aren't any entries in the table
	{
		printf_debug("Error allocating memory for pkhist_table, sizeof %d",
				sizeof(PKHIST_DATA));
		return;
	}
	else if ((pkhist = realloc(pkhist_table, sizeof(PKHIST_DATA)
			* max_pkhist_entries)) == NULL)
	{
		printf_debug("Error reallocating memory for pkhist_table, sizeof %d",
				sizeof(PKHIST_DATA) * max_pkhist_entries);
		return;
	}

	pkhist[max_pkhist_entries - 1].killer = chRoster;
	pkhist[max_pkhist_entries - 1].kClan = chRoster->clan;

	pkhist[max_pkhist_entries - 1].victim = vchRoster;
	pkhist[max_pkhist_entries - 1].vClan = vchRoster->clan;

	pkhist[max_pkhist_entries - 1].time = current_time;
	pkhist[max_pkhist_entries - 1].flags = arena ? PKHIST_ARENA|PKHIST_INVALID : 0;

	if (vchRoster->character) // Should always be true, but just in case
		pkhist[max_pkhist_entries - 1].vLevel = vchRoster->character->level + tier_level_bonus(vchRoster->character);
	else
		pkhist[max_pkhist_entries - 1].vLevel = 0;

	if (chRoster->character) // Should always be true, but just in case
		pkhist[max_pkhist_entries - 1].kLevel = chRoster->character->level + tier_level_bonus(chRoster->character);
	else
		pkhist[max_pkhist_entries - 1].kLevel = 0;

	pkhist_table = pkhist;
	save_pkhist_data();
}

// Helper function.
int rank_kill_ch(CHAR_DATA *ch, CHAR_DATA *vch, bool arena, bool simulate)
{
	if (IS_NPC(ch) || IS_NPC(vch))
		return 0;
	ROSTER_DATA *chRoster, *vchRoster;
	GET_ROSTER(ch, chRoster);
	GET_ROSTER(vch, vchRoster);

	int result = rank_kill(chRoster, vchRoster, arena, simulate);

	if (!simulate && result > 0)
	{
		char buf[MAX_STRING_LENGTH];
		sprintf(buf, "You have won {Y%d{x %s points!\r\n", result,
				arena ? "arena" : "pkill");
		send_to_char(buf, ch);

		// Pkill highlanders gain points only outside arena, non-pkill, only inside arena.
		/* if (ch->class == CLASS_BARBARIAN)
		{
			// OLD
			//ch->pcdata->power[POWER_POINTS] = ((ch->pcdata->arank - 1500) * 100)
			//		+ 5000;
			//NEW - CHECK
			ch->pcdata->power[POWER_POINTS] = chRoster->pkilldata->kPoints;
			update_power(ch);
		}
		if (vch->class == CLASS_BARBARIAN)
		{
			// OLD
			//vch->pcdata->power[POWER_POINTS] = ((vch->pcdata->arank - 1500)
			//		* 100) + 5000;
			//NEW - CHECK
			vch->pcdata->power[POWER_POINTS] = vchRoster->pkilldata->kPoints;
			update_power(vch);
		} */
	}
	return result;
}

/* Reward and return the number of points a player gets for killing someone else */
int rank_kill(ROSTER_DATA *chRoster, ROSTER_DATA *vchRoster, bool arena,
		bool simulate)
{
	DESCRIPTOR_DATA *d;
	int result;//, max = 500;

	if (chRoster == NULL || vchRoster == NULL)
		return 0;

	if (chRoster->clan->id == CLAN_IMMORTAL || vchRoster->clan->id
			== CLAN_IMMORTAL || chRoster == vchRoster)
		return 0;

	if ((chRoster->character && IS_IMMORTAL( chRoster->character ))
			|| (vchRoster->character && IS_IMMORTAL( vchRoster->character )))
		return 0;

	if (chRoster->clan == vchRoster->clan && !chRoster->clan->independent)
		return 0;

	// If we're pkill and in the arena, we don't get points.
	if (chRoster->clan->pkill && arena)
		return 0;

	/* What was i smoking?
	if ((chRoster->clan->pkill && vchRoster->clan->pkill && !arena)
			|| (!chRoster->clan->pkill && !vchRoster->clan->pkill))
		max += 2000;

	result = (((vchRoster->pkilldata->kPoints * 2 + vchRoster->dPoints / 6)
			* 115 / 100) - (chRoster->pkilldata->kPoints * 2
			+ chRoster->dPoints / 6)) / 3;
	 */

	result = UMAX(3, vchRoster->pkilldata->kPoints*4/10);

	if (chRoster->clan->enemy && vchRoster->clan->enemy
			&& chRoster->clan->enemy == vchRoster->clan
			&& vchRoster->clan->enemy == chRoster->clan)
	{
//		max = max * 2;
		result = (result*13) / 100;
	}

//	result = URANGE( 10, result, max );

	if (!simulate && is_main_server())
		update_pkhist(chRoster, vchRoster, arena);

	if (!simulate)
	{
		if (vchRoster->bounty > 0 && chRoster->character
				&& ((!vchRoster->clan->pkill && arena) || !arena))
		{
			char buf[MAX_STRING_LENGTH];
			int len = strlen(vchRoster->name);

			sprintf(
					buf,
					"{CBOUNTY {r>{R>{r> {R%s{W has {GCOLLECTED{W the {Y%ld{W platinum bounty on {R%s'%s{W head!{x\n\r",
					chRoster->name, vchRoster->bounty, vchRoster->name,
					vchRoster->name[len - 1] == 's' ? "" : "s");

			for (d = descriptor_list; d != NULL; d = d->next)
			{
				if (d->connected != CON_PLAYING || d->character == NULL
						|| IS_SET( d->character->comm, COMM_QUIET ))
					continue;

				send_to_char(buf, d->character);
			}
			chRoster->character->platinum += vchRoster->bounty;
			vchRoster->bounty = 0;
		}

		chRoster->pkilldata->kPoints += result;
		chRoster->pkilldata->kills++;
		chRoster->pkilldata->wins++;

		chRoster->clan->pkilldata->kills++;
		chRoster->clan->pkilldata->wins++;

		// Only adjust victim's stuff only if pkill and out of arena or nopkill and in arena.
		if (!vchRoster->clan->pkill || !arena) {
			vchRoster->pkilldata->kPoints = UMAX(0, vchRoster->pkilldata->kPoints - UMAX(3, vchRoster->pkilldata->kPoints*25/100));
			vchRoster->clan->pkilldata->deaths++;
			vchRoster->clan->pkilldata->losses++;

			vchRoster->pkilldata->deaths++;
			vchRoster->pkilldata->losses++;
		}

		if (IS_SET(chRoster->flags, ROST_NEWBIE) && chRoster->pkilldata->kPoints >= 10)
		{
			REMOVE_BIT(chRoster->flags, ROST_NEWBIE);
			if (chRoster->character)
				send_to_char("{RYour newbie flag has expired. You are now a FULL player-killer.\n\r", chRoster->character);
			save_roster_data(chRoster);
		}
	}
	return result;
}

void clan_entry_trigger(CHAR_DATA *ch, bool greet)
{
	ROSTER_DATA *chRoster;
	CLAN_DATA *clan;
	CHAR_DATA *guard;
	char buf[MAX_STRING_LENGTH];
	int i;

	if (IS_NPC( ch ) || ch->in_room == NULL)
		return;

	if ((clan = get_clan_by_vnum(ch->in_room->vnum)) == NULL)
		return;

	GET_ROSTER( ch, chRoster );

	for (i = 0; i < max_pkhist_entries; i++) // Check spamkill
	{
		if (!pkhist_table[i].killer || !pkhist_table[i].victim
				|| !pkhist_table[i].kClan || (pkhist_table[i].time
				+ TIME_SPAMKILL_LIMIT) < current_time
				|| IS_SET( pkhist_table[i].flags, PKHIST_INVALID ))
			continue;

		if (pkhist_table[i].victim == chRoster && pkhist_table[i].kClan == clan)
		{
			sprintf(
					buf,
					"{REven though you were killed by a member of the {x%s{R clan within the hour, for trespassing you can now be killed again.{x\n\r",
					strip_bespaces(clan->c_name));
			send_to_char(buf, ch);

			SET_BIT( pkhist_table[i].flags, PKHIST_INVALID );
		}
	}

	for (guard = ch->in_room->people; guard != NULL; guard = guard->next_in_room)
		if (IS_NPC( guard ) && is_clan_mob_type(guard, clan, CMOB_GUARD ))
			break;

	if (guard == NULL)
		return;

	if (is_same_clan(ch, guard))
	{
		if (greet)
		{
			sprintf(buf, "Greetings, %s", ch->name);
			do_say(guard, buf);
		}
		return;
	}
	if (get_trust(ch) >= SUPREME)
		return;
	if (ch->level <= 75 || IS_AFFECTED( ch, AFF_CHARM ))
	{
		switch (number_range(0, 5))
		{
		case 0:
			strcpy(buf, "$n{x says '{mThis area is restricted friend.{x'");
			break;
		case 1:
			strcpy(buf, "$n{x looks at you and gestures silently.");
			break;
		case 2:
			strcpy(buf,
					"$n{x asks '{mEver dance with the devil in the pale moon light?{x'");
			break;
		case 3:
			strcpy(buf, "$n{x says '{mBegone from this place.{x'");
			break;
		case 4:
			strcpy(buf, "$n{x says '{mLeave now or suffer.{x'");
			break;
		case 5:
			strcpy(buf, "$n{x says '{mWelcome, too bad you cannot stay.{x'");
			break;
		default:
			strcpy(buf, "$n{x says '{mAdios.{x'");
			break;
		}
		act(buf, guard, NULL, ch, TO_ROOM );
		char_from_room(ch);
		char_to_room(ch, get_room_index(VNUM_MARKET_SQUARE));
		do_look(ch, "auto");
		return;
	}

	sprintf(buf, "Help! I'm being attacked by %s!", guard->short_descr);
	do_yell(ch, buf);

	do_ctalk(guard, "Intruder! Intruder!");
	multi_hit(guard, ch, TYPE_UNDEFINED );

	if (IS_SET( ch->plyr,PLAYER_RUNNING ))
		REMOVE_BIT( ch->plyr,PLAYER_RUNNING );
}

/************************************* CLAN SKILLS ************************************/

/* Interpreter for clan skill commands, called at the end of interpret
 * if ch isn't an NPC, is in a clan and the command they entered wasn't
 * picked up by interpret. Returns true if valid clanskill found.
 */
bool interpret_clan_cmd(CHAR_DATA *ch, char *command, char *argument)
{
	ROSTER_DATA *roster;
	int cmd, total;

	GET_ROSTER( ch, roster );

	if (!roster || roster->pkilldata == NULL)
		return FALSE;

	total = get_total_points(roster);

	for (cmd = 0; clan_skill_table[cmd].func != NULL; cmd++)
	{
		if (get_trust(ch) < SUPREME
		&& (!clan_has_skill(roster->clan, cmd)
		|| total < clan_skill_table[cmd].points))
			continue;
		if (str_prefix(command, clan_skill_table[cmd].cmd))
			continue;
		if (ch->position < clan_skill_table[cmd].position)
		{
			switch (ch->position)
			{
			case POS_DEAD:
				send_to_char("Lie still; you are DEAD.\n\r", ch);
				break;

			case POS_MORTAL:
			case POS_INCAP:
				send_to_char("You are hurt far too bad for that.\n\r", ch);
				break;

			case POS_STUNNED:
				send_to_char("You are too stunned to do that.\n\r", ch);
				break;

			case POS_SLEEPING:
				send_to_char("In your dreams, or what?\n\r", ch);
				break;

			case POS_RESTING:
				send_to_char("Nah... You feel too relaxed...\n\r", ch);
				break;

			case POS_SITTING:
				send_to_char("Better stand up first.\n\r", ch);
				break;

			case POS_FIGHTING:
				send_to_char("No way!  You are still fighting!\n\r", ch);
				break;
			}
			return FALSE;
		}

		(*clan_skill_table[cmd].func)(ch, argument);
		return TRUE;
	}
	return FALSE;
}

/* The following clan skills were written originally by Gabe Volker and rewritten by Deryck Arnold */
void do_adrenaline(CHAR_DATA *ch, char *argument)
{
	AFFECT_DATA af;

	if (is_affected(ch, gsn_adrenaline))
	{
		affect_strip(ch, gsn_adrenaline);
		send_to_char("Your fury subsides.\n\r", ch);
		return;
	}

	af.where = TO_AFFECTS;
	af.type = gsn_adrenaline;
	af.level = ch->level;
	af.duration = -1;
	af.bitvector = 0;
	af.location = APPLY_DAMROLL;
	af.modifier = ch->level * 6 / 10;
	affect_to_char(ch, &af);

	send_to_char(
			"You feel the adrenaline being pumped through your veins.\n\r", ch);
	act("$n's eyes turn red and roll back into $n's skull!", ch, NULL, NULL,
			TO_ROOM );
}

void do_battle_fury(CHAR_DATA *ch, char *argument)
{
	AFFECT_DATA af;

	if (is_affected(ch, gsn_battle_fury))
	{
		affect_strip(ch, gsn_battle_fury);
		send_to_char("Your battle fury subsides.\n\r", ch);
		return;
	}

	af.where = TO_AFFECTS;
	af.type = gsn_battle_fury;
	af.level = ch->level;
	af.bitvector = 0;
	af.duration = -1;
	af.modifier = ch->level / 7;
	af.location = APPLY_DAMROLL;
	affect_to_char(ch, &af);

	af.location = APPLY_HITROLL;
	affect_to_char(ch, &af);

	af.location = APPLY_AC;
	af.bitvector = SHD_STEEL;
	af.modifier = -ch->level / 7;
	affect_to_char(ch, &af);

	send_to_char(
			"Your eyes invert for a moment as you are consumed by fury!\n\r",
			ch);
	act("$n gets a wicked look in $s eyes!", ch, NULL, NULL, TO_ROOM );
}

void do_bloodlust(CHAR_DATA *ch, char *argument)
{
	AFFECT_DATA af;

	if (is_affected(ch, gsn_bloodlust))
	{
		affect_strip(ch, gsn_bloodlust);
		send_to_char("Your lust for blood subsides.\n\r", ch);
		return;
	}

	af.where = TO_AFFECTS;
	af.type = gsn_bloodlust;
	af.level = ch->level;
	af.duration = -1;
	af.bitvector = 0;
	af.location = APPLY_DAMROLL;
	af.modifier = ch->level / 6;
	affect_to_char(ch, &af);

	af.location = APPLY_HITROLL;
	affect_to_char(ch, &af);

	af.location = APPLY_AC;
	af.modifier = -ch->level / 6;
	affect_to_char(ch, &af);

	send_to_char("A terrible lust for blood begins to take hold.\n\r", ch);
	send_to_char("Your eyes burn red as the lust overwhelms you!\n\r", ch);
	act("$n's eyes turn red as the bloodlust overwhelms them!", ch, NULL, NULL,
			TO_ROOM );
	act("$n's eyes turn bloodshot!", ch, NULL, NULL, TO_ROOM );
}

/* Hides the GreenBlade from mortal eyes
 * GreenBlade appears as a green cloaked figure
 * Only Immortals and Clan Members see the real name when concealed
 * Restricts:
 * -None
 * Benefits:
 * - General, lower Ac, small hit/dam, bit of regen.
 * - Overall vanishment
 * Functionality:
 * - Employ Identity or Morph
 * - Use gsn_concealClan1
 */
void do_conceal(CHAR_DATA *ch, char *argument)
{
	AFFECT_DATA af;

	/*if (ch->morph_form[0] && ch->morph_form[0] != MORPH_CONCEAL)
	{
		send_to_char(
				"You cannot conceal yourself while in another morphed form.\n\r",
				ch);
		return;
	}*/

	if (is_affected(ch, gsn_conceal))
		affect_strip(ch, gsn_conceal);
	if (is_affected(ch, gsn_conceal2))
		affect_strip(ch, gsn_conceal2);
	if (is_affected(ch, gsn_concealClan2))
		affect_strip(ch, gsn_concealClan2);

	if (ch->morph_form[0] == MORPH_CONCEAL || is_affected(ch, gsn_concealClan1))
	{
		affect_strip(ch, gsn_concealClan1);

		if (ch->long_descr != NULL && ch->long_descr[0] != '\0')
		{
			free_string(ch->long_descr);
			ch->long_descr = str_dup("");
		}
		ch->morph_form[0] = 0;
		send_to_char("You expose yourself to the lands.\n\r", ch);
		return;
	}

	send_to_char("You conceal your presence.\n\r", ch);
	act("$n wraps a cloak around $mself.", ch, NULL, NULL, TO_ROOM ); // Put up here to make sure $n is a name, not a morphed name

	af.where = TO_AFFECTS;
	af.type = gsn_concealClan1;
	af.level = ch->level;
	af.location = APPLY_MORPH_FORM;
	af.modifier = MORPH_CONCEAL;
	af.bitvector = AFF_SNEAK;
	af.duration = -1;
	affect_to_char(ch, &af);

	af.bitvector = 0;
	af.location = APPLY_HITROLL;
	af.modifier = ch->level / 8;
	affect_to_char(ch, &af);

	af.location = APPLY_DAMROLL;
	affect_to_char(ch, &af);
}

void do_divide(CHAR_DATA *ch, char *argument)
{
	AFFECT_DATA af;

	if (is_affected(ch, gsn_divide))
	{
		affect_strip(ch, gsn_divide);
		send_to_char("The divide closes slowly.\n\r", ch);
		return;
	}

	af.where = TO_AFFECTS;
	af.type = gsn_divide;
	af.level = ch->level;
	af.duration = -1;
	af.bitvector = 0;
	af.location = APPLY_DAMROLL;
	af.modifier = ch->level / 7;
	affect_to_char(ch, &af);

	af.location = APPLY_HITROLL;
	affect_to_char(ch, &af);

	af.location = APPLY_AC;
	af.bitvector = SHD_STONE;
	af.modifier = -ch->level / 7;
	affect_to_char(ch, &af);

	act("$n's body becomes one with the divide!", ch, NULL, NULL, TO_ROOM);
	send_to_char("A great divide engulfs you!\n\r", ch);
}

void do_faith(CHAR_DATA *ch, char *argument)
{
	CHAR_DATA *victim;
	int chance = number_range(75, 100), dam = 0;

	if (ch->stunned > 0)
	{
		send_to_char("You're far too stunned to proclaim your faith!\n\r", ch);
		return;
	}

	if ((victim = ch->fighting) == NULL)
	{
		if (argument[0] == '\0')
		{
			send_to_char(
					"Who would you like to proclaim your blind faith towards?\n\r",
					ch);
			return;
		}
		if ((victim = get_char_room(ch, argument)) == NULL)
		{
			send_to_char("You proclaim your faith aloud to the room.\n\r", ch);
			return;
		}
	}
	if (victim == ch)
	{
		send_to_char(
				"You have already proven your faith towards yourself.\n\r", ch);
		return;
	}
	if (ch->mana < 100)
	{
		send_to_char("You don't have enough energy to muster.\n\r", ch);
		return;
	}

	if (is_safe(ch, victim))
		return;

	if (number_percent() < chance)
	{
		dam = dice(ch->level, 13);

		if (saves_spell(ch->level, victim, DAM_LIGHT))
			dam = dam * 3 / 4;

		ch->mana -= 100;
		send_to_char(
				"You call upon your faith and suddenly the room erupts in blinding light!\n\r",
				ch);
		act(
				"$n utters words of his faith and suddenly the room erupts in blinding light!",
				ch, NULL, NULL, TO_ROOM );
		spell_blindness(gsn_blindness, ch->level * 3 / 2, ch, (void *) victim,
				TARGET_CHAR );
	}
	else
	{
		ch->mana -= 50;
		send_to_char("You momentarily lose faith.\n\r", ch);
		act("$n utters words of his faith but falters halfway!", ch, NULL,
				NULL, TO_ROOM );
	}

	damage(ch, victim, dam, gsn_blind_faith, DAM_LIGHT, TRUE, 0);
	WAIT_STATE( ch, PULSE_VIOLENCE )
;}

void do_modi(CHAR_DATA *ch, char *argument)
{
	AFFECT_DATA af;

	if (argument[0] == '\0')
	{
		send_to_char("Syntax: modi anger         - damroll boost\n\r"
			"        modi concentration - hitroll boost\n\r"
			"        modi calm          - removes both effects\n\r", ch);
		return;
	}

	if (!str_prefix(argument, "anger"))
	{
		if (is_affected(ch, gsn_concentration))
		{
			affect_strip(ch, gsn_concentration);
			send_to_char("Your concentration diminishes.\n\r", ch);
		}
		if (is_affected(ch, gsn_modis_anger))
			affect_strip(ch, gsn_modis_anger);

		af.where = TO_AFFECTS;
		af.type = gsn_modis_anger;
		af.level = ch->level;
		af.duration = -1;
		af.bitvector = 0;
		af.location = APPLY_DAMROLL;
		af.modifier = ch->level * 4 / 10;
		affect_to_char(ch, &af);

		send_to_char(
				"You release your stored energy and bellow a mighty battlecry!\n\r",
				ch);
		send_to_char("You feel the Wrath of Modi enter your body!\n\r", ch);
		act("$n bellows a mighty battlecry and begins frothing at the mouth.",
				ch, NULL, NULL, TO_ROOM );
		act("$n's eyes turn blood red.", ch, NULL, NULL, TO_ROOM );
	}
	else if (!str_prefix(argument, "calm"))
	{
		if (is_affected(ch, gsn_modis_anger))
		{
			affect_strip(ch, gsn_modis_anger);
			send_to_char("Your anger subsides.\n\r", ch);
		}
		if (is_affected(ch, gsn_concentration)) // Not using else/if in case somehow someone has both affects.
		{
			affect_strip(ch, gsn_concentration);
			send_to_char("Your concentration diminishes.\n\r", ch);
		}
		send_to_char("You feel calm and collected.\n\r", ch);
	}
	else if (!str_prefix(argument, "concentration"))
	{
		if (is_affected(ch, gsn_modis_anger))
		{
			affect_strip(ch, gsn_modis_anger);
			send_to_char("Your anger subsides.\n\r", ch);
		}
		if (is_affected(ch, gsn_concentration))
			affect_strip(ch, gsn_concentration);

		af.where = TO_AFFECTS;
		af.type = gsn_concentration;
		af.level = ch->level;
		af.duration = -1;
		af.bitvector = 0;
		af.location = APPLY_HITROLL;
		af.modifier = ch->level * 4 / 10;
		affect_to_char(ch, &af);

		send_to_char(
				"You concentrate on the determination of Modi and your aim improves!\n\r",
				ch);
		act("$n's eyes turn light blue with white steaks.", ch, NULL, NULL,
				TO_ROOM );
	}
	else
		do_modi(ch, "");
}

void do_shadow_form(CHAR_DATA *ch, char *argument)
{
	AFFECT_DATA af;

	if (is_affected(ch, gsn_shadow_form))
	{
		affect_strip(ch, gsn_shadow_form);
		send_to_char("The shadows begin to subside.\n\r", ch);
		return;
	}

	af.where = TO_AFFECTS;
	af.type = gsn_shadow_form;
	af.level = ch->level;
	af.duration = -1;
	af.location = APPLY_AC;
	af.modifier = -ch->level * 6 / 10;
	af.bitvector = 0;
	affect_to_char(ch, &af);

	send_to_char("The shadows form all around you!\n\r", ch);
	act("The shadows form all around $n!", ch, NULL, NULL, TO_ROOM);
}

void do_solar_flare(CHAR_DATA *ch, char *argument)
{
	CHAR_DATA *victim, *victim_next;

	if (ch->in_room == NULL)
		return;

	if (ch->stunned > 0)
	{
		send_to_char("You're far too stunned to hurl anything!\n\r", ch);
		return;
	}

	act("$n hurls a solar flare into the room! {Y{zF{W{zL{Y{zA{W{zS{Y{zH{x!!!",
			ch, NULL, NULL, TO_ROOM );
	act("You hurl a solar flare into the room! {Y{zF{W{zL{Y{zA{W{zS{Y{zH{x!!!",
			ch, NULL, NULL, TO_CHAR );

	if (number_percent() < 2)
	{
		act("You accidentally stare at the flare as it explodes!", ch, NULL,
				NULL, TO_CHAR );
		spell_blindness(gsn_blindness, ch->level * 3 / 4, ch, (void *) ch,
				TARGET_CHAR );
	}

	for (victim = ch->in_room->people; victim != NULL; victim = victim_next)
	{
		victim_next = victim->next_in_room;
		if (is_safe_quiet(ch, victim))
			continue;

		if (number_percent() < 80)
			spell_blindness(gsn_blindness, ch->level * 3 / 4, ch,
					(void *) victim, TARGET_CHAR );
		if (victim->fighting == NULL)
			multi_hit(victim, ch, TYPE_UNDEFINED );
	}
	WAIT_STATE( ch, PULSE_VIOLENCE )
;}

void do_vmight(CHAR_DATA *ch, char *argument)
{
	AFFECT_DATA af;

	if (is_affected(ch, gsn_vmight))
	{
		affect_strip(ch, gsn_vmight);
		send_to_char("The Might of the Vanir fades from your mind.\n\r", ch);
		return;
	}

	af.where = TO_AFFECTS;
	af.type = gsn_vmight;
	af.level = ch->level;
	af.duration = -1;
	af.bitvector = 0;
	af.location = APPLY_DAMROLL;
	af.modifier = ch->level / 5;
	affect_to_char(ch, &af);

	af.bitvector = SHD_STONE;
	af.location = APPLY_AC;
	af.modifier = -ch->level / 5;
	affect_to_char(ch, &af);

	send_to_char(
			"You begin to feel the might of the Vanir all around you!\n\r", ch);
	act("$n begins channeling the Might of the Vanir!", ch, NULL, NULL, TO_ROOM );
	act("$n's eyes fade to black!", ch, NULL, NULL, TO_ROOM );
}

void do_wform(CHAR_DATA *ch, char *argument)
{
	AFFECT_DATA af;

	/*if (ch->morph_form[0] && ch->morph_form[0] != MORPH_CONCEAL2)
	{
		send_to_char(
				"You cannot transform yourself while in another morphed form.\n\r",
				ch);
		return;
	}*/

	if (is_affected(ch, gsn_conceal))
		affect_strip(ch, gsn_conceal);
	if (is_affected(ch, gsn_conceal2))
		affect_strip(ch, gsn_conceal2);
	if (is_affected(ch, gsn_concealClan1))
		affect_strip(ch, gsn_concealClan1);

	if (ch->morph_form[0] == MORPH_CONCEAL2 || is_affected(ch, gsn_concealClan2))
	{
		affect_strip(ch, gsn_concealClan2);

		if (ch->long_descr != NULL && ch->long_descr[0] != '\0')
		{
			free_string(ch->long_descr);
			ch->long_descr = str_dup("");
		}
		ch->morph_form[0] = 0;
		send_to_char("You transform back into a human.\n\r", ch);
		return;
	}

	send_to_char("You transform into a wolf.\n\r", ch);
	act("$n's body spasms slightly as $e turns into a wolf!", ch, NULL, NULL,
			TO_ROOM ); // Put up here to make sure $n is a name, not a morphed name

	af.where = TO_AFFECTS;
	af.type = gsn_concealClan2;
	af.level = ch->level;
	af.location = APPLY_MORPH_FORM;
	af.modifier = MORPH_CONCEAL2;
	af.bitvector = AFF_SNEAK;
	af.duration = -1;
	affect_to_char(ch, &af);

	af.bitvector = 0;
	af.location = APPLY_HITROLL;
	af.modifier = ch->level / 8;
	affect_to_char(ch, &af);

	af.location = APPLY_DAMROLL;
	affect_to_char(ch, &af);
}

/******************************** INTEGRATION FUNCTIONS *******************************/

void make_rosters(CHAR_DATA *ch, char *argument)
{
	ROSTER_DATA *roster;
	FILE *fp;
	char name[MAX_STRING_LENGTH];
	int played;

	if (str_cmp(ch->name, "Nico"))
	{
		send_to_char("Restricted command.\n\r", ch);
		return;
	}

	if ((fp = fopen("rostlist.txt", "r")) == NULL)
	{
		send_to_char("No rostlist.\n\r", ch);
		return;
	}

	while (!feof(fp))
	{
		strcpy(name, fread_word(fp));

		if (name[0] == '#')
			break;

		roster = new_roster_data(TRUE);

		free_string(roster->name);
		roster->name = str_dup(capitalize(name));
		roster->pkilldata = new_pkill_data();
		roster->last_login = current_time;

		if (fread_number(fp) > 0)
			roster->clan = get_clan_by_id(CLAN_LONER);

		played = fread_number(fp);

		if (fread_number(fp) > 0)
		{
			SET_BIT( roster->flags, ROST_NEWBIE );
		//	roster->newbie_time = current_time + (TIME_NEWBIE_GRACE * played
		//			/TIME_HOUR);
		}

		if (fread_number(fp) > 0)
			roster->clan = get_clan_by_id(CLAN_IMMORTAL);
	}
	update_all_rosters();
	fclose(fp);
}

/* Required to make sure everyone stays pk/nonpk after change
 void prep_rosters( CHAR_DATA *ch, char *argument )
 {
 CHAR_DATA *vch;
 FILE *fp, *fpList, *fpOut;
 char name[MAX_STRING_LENGTH];

 if ( str_cmp( ch->name, "Nico" ) )
 {
 send_to_char( "Restricted command.\n\r", ch );
 return;
 }

 *///    system( "dir -1 ../player/*/* > ../player/names.txt" );
/*
 if ( (fpList = fopen( "../player/names.txt", "a" )) == NULL )
 {
 send_to_char( "Error reading names.txt for writing", ch );
 return;
 }
 fprintf( fpList, "#\n" );
 fclose( fpList );

 if ( (fpList = fopen( "../player/names.txt", "r" )) == NULL )
 {
 send_to_char( "Error reading names.txt for reading", ch );
 return;
 }
 if ( (fpOut = fopen( "rostlist.txt", "w" )) == NULL )
 {
 send_to_char( "Error opening rostlist.txt for writing", ch );
 fclose( fpList );
 return;
 }

 while ( !feof( fpList ) )
 {
 strcpy( name, fread_word( fpList ) );
 if ( name[0] == '#' )
 break;

 vch = new_char( );
 vch->pcdata = new_pcdata( );

 if ( ( fp = fopen( name, "r" ) ) != NULL )
 {
 char *word = fread_word( fp );

 if ( !str_cmp( word, "#PLAYER" ) )
 fread_char( vch, fp);

 fclose( fp );
 }

 if ( vch->name[0] == '\0' || !str_cmp( vch->name, "Nico" ) )	// Stops dual rosters for me
 {
 free_pcdata( vch->pcdata );
 free_char( vch );
 continue;
 }

 fprintf( fpOut, "%s %d %d %d %d\n",
 vch->name, clan_table[vch->clan].pkill,
 vch->played,
 IS_SET( vch->plyr, PLAYER_NEWBIE ) ? 1 : 0,
 IS_IMMORTAL( vch ) ? 1 : 0 );

 free_pcdata( vch->pcdata );
 free_char( vch );
 }
 fclose( fpList );
 fprintf( fpOut, "#\n" );
 fclose( fpOut );
 unlink( "../player/names.txt" );
 }*/

