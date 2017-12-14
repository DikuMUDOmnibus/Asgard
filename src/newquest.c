/*
 * newquest.c
 *
 *  Created on: 6 Jan 2011
 *      Author: Nico
 */
#include <stdlib.h>
#include <stdio.h>

#include "merc.h"
#include "recycle.h"
#include "string.h"
#include "newclan.h"
#include "newquest.h"
#include "pathfind.h"
#include "global_mods.h"
#include "achievements.h"

QITEM_FN_DEF(next_items);

QITEM_FN_DEF(quest_request);
QITEM_FN_DEF(quest_complete);
QITEM_FN_DEF(quest_quit);
QITEM_FN_DEF(quest_info);

QITEM_FN_DEF(item);
QITEM_FN_DEF(gmod);
QITEM_FN_DEF(corpse);

QITEM_FN_DEF(quest_put);
QITEM_FN_DEF(quest_take);
QITEM_FN_DEF(locker);

QITEM_FN_DEF(imm_hunger);
QITEM_FN_DEF(imm_thirst);

QITEM_FN_DEF(wpnflag);
QITEM_FN_DEF(aesirupgrade);

QITEM_FN_DEF(restring_item);

QITEM_FN_DEF(title_pretitle);
QITEM_FN_DEF(title_whotitle);

void do_say(CHAR_DATA *ch, char *argument);
void do_put(CHAR_DATA *ch, char *argument);
void do_get(CHAR_DATA *ch, char *argument);
bool is_safe_quiet(CHAR_DATA * ch, CHAR_DATA * victim);
BUFFER * display_stats( OBJ_DATA *obj, CHAR_DATA *ch, bool contents );
BUFFER *show_list_to_char(OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing);
void do_announce(CHAR_DATA *ch, char *argument);

QUEST_ITEM quest_item_title[] = {
	{	"pretitle",	0,	COST_QUEST,	7500,	QITEM_FN_NAME(title_pretitle),	NULL,	"change your pretitle"	},
	{	"whotitle",	0,	COST_QUEST,	10000,	QITEM_FN_NAME(title_whotitle),	NULL,	"change your pretitle"	},
	{	NULL,		0,	0,			0,		NULL,							NULL,	NULL					}
};

QUEST_ITEM quest_item_restring[] = {
	{	"short",	0,	COST_QUEST,	300,	QITEM_FN_NAME(restring_item),	NULL,	"restring the short name"	},
	{	"long",		0,	COST_QUEST,	300,	QITEM_FN_NAME(restring_item),	NULL,	"restring the long name"	},
	{	NULL,		0,	0,			0,		NULL,							NULL,	NULL						}
};

QUEST_ITEM quest_item_corpse[] = {
	{	"quest",	0,	COST_QUEST,	300,	QITEM_FN_NAME(corpse),	NULL,	"corpse retrieval with questpoints"	},
	{	"platinum",	0,	COST_PLAT,	600,	QITEM_FN_NAME(corpse),	NULL,	"corpse retrieval with platinum"	},
	{	NULL,		0,	0,			0,		NULL,					NULL,	NULL								}
};

QUEST_ITEM quest_item_vinir[] = {
	{	"dagger",		9921,	COST_QUEST,	3000,	QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"mace",			9918,	COST_QUEST,	3000,	QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"flail",		9922,	COST_QUEST,	3000,	QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"polearm",		9920,	COST_QUEST,	3000,	QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"spear",		9924,	COST_QUEST,	3000,	QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"axe",			9919,	COST_QUEST,	3000,	QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"sword",		9917,	COST_QUEST,	3000,	QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"whip",			9923,	COST_QUEST,	3000,	QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"platemail",	9912,	COST_QUEST,	3000,	QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"helmet",		9913,	COST_QUEST,	3000,	QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"legplates",	9907,	COST_QUEST,	3000,	QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"boots",		9911,	COST_QUEST,	3000,	QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"gauntlets",	9909,	COST_QUEST,	3000,	QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"armplates",	9908,	COST_QUEST,	3000,	QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"shield",		9910,	COST_QUEST,	3000,	QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"cape cloak",	9914,	COST_QUEST,	3000,	QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"bracer",		9906,	COST_QUEST,	3000,	QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"girth",		9916,	COST_QUEST,	3000,	QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"exotic",		9925,	COST_QUEST,	3000,	QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"mask",			9915,	COST_QUEST,	3000,	QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	NULL,			0,		0,			0,		NULL,					NULL,	NULL						}
};

QUEST_ITEM quest_item_aesir[] = {
	{	"sword",	9873,	COST_QUEST,	13500,		QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"greatsword",	9904,	COST_QUEST,	13500,		QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"dagger",	9874,	COST_QUEST,	13500,		QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
//	{	"mace",		9875,	COST_QUEST,	13500,		QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"mattock",	9929,	COST_QUEST,	13500,		QITEM_FN_NAME(item),	NULL,	"YOU SHUOLD NOT SEE THIS"	},
	{	"hammer",	9876,	COST_QUEST,	13500,		QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"flail",	9931,	COST_QUEST,	13500,		QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"ranseur",	9877,	COST_QUEST,	13500,		QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"staff",	9878,	COST_QUEST,	13500,		QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"spear",	9930,	COST_QUEST,	13500,		QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"axe",		9879,	COST_QUEST,	13500,		QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"adze",		9932,	COST_QUEST,	13500,		QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"whip",		9880,	COST_QUEST,	13500,		QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"platemail",	9881,	COST_QUEST,	13500,		QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"helmet",	9882,	COST_QUEST,	13500,		QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"legplates",	9891,	COST_QUEST,	13500,		QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"boots",	9883,	COST_QUEST,	13500,		QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"gauntlets",	9884,	COST_QUEST,	13500,		QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"armplates",	9885,	COST_QUEST,	13500,		QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"shield",	9886,	COST_QUEST,	13500,		QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"cape cloak",	9887,	COST_QUEST,	13500,		QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"bracer",	9888,	COST_QUEST,	13500,		QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"girth",	9889,	COST_QUEST,	13500,		QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"mask",		9890,	COST_QUEST,	13500,		QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"pack",		9905,	COST_QUEST,	3400,		QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	NULL,		0,	0,		0,		NULL,			NULL,	NULL			}
};

QUEST_ITEM quest_item_qitems[] = {
	{	"mask",		9853,	COST_QUEST,	2650,	QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"pod",		9851,	COST_QUEST,	2800,	QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"stone",	9926,	COST_QUEST,	3000,	QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	"light",	9852,	COST_QUEST,	3400,	QITEM_FN_NAME(item),	NULL,	"YOU SHOULD NOT SEE THIS"	},
	{	NULL,		0,		0,			0,		NULL,					NULL,	NULL						}
};

QUEST_ITEM quest_item_immunity[] = {
	{	"hunger",	0,	COST_QUEST,	1500,	QITEM_FN_NAME(imm_hunger),	NULL,	"immunity to hunger"	},
	{	"thirst",	0,	COST_QUEST,	1500,	QITEM_FN_NAME(imm_thirst),	NULL,	"immunity to thirst"	},
	{	NULL,		0,	0,			0,		NULL,						NULL,	NULL					}
};

// Hack to put in weapon flag bits.
QUEST_ITEM quest_item_wpnflags[] = {
	{	"flaming",	0,	COST_NOTAPPL,	WEAPON_FLAMING,		QITEM_FN_NAME(wpnflag),	NULL,	"flame weapon flag"		},
	{	"frosting",	0,	COST_NOTAPPL,	WEAPON_FROST,		QITEM_FN_NAME(wpnflag),	NULL,	"frost weapon flag"		},
	{	"shocking",	0,	COST_NOTAPPL,	WEAPON_SHOCKING,	QITEM_FN_NAME(wpnflag),	NULL,	"shock weapon flag"		},
	{	"sharp",	0,	COST_NOTAPPL,	WEAPON_SHARP,		QITEM_FN_NAME(wpnflag),	NULL,	"sharp weapon flag"		},
	{	"vorpal",	0,	COST_NOTAPPL,	WEAPON_VORPAL,		QITEM_FN_NAME(wpnflag),	NULL,	"vorpal weapon flag"	},
	{	"poisoned",	0,	COST_NOTAPPL,	WEAPON_POISON,		QITEM_FN_NAME(wpnflag),	NULL,	"poison weapon flag"	},
	{	"vampiric",	0,	COST_NOTAPPL,	WEAPON_VAMPIRIC,	QITEM_FN_NAME(wpnflag),	NULL,	"vampiric weapon flag"	},
	{	"osmotic",	0,	COST_NOTAPPL,	WEAPON_OSMOSIS,		QITEM_FN_NAME(wpnflag),	NULL,	"osmosis weapon flag"	},
	{	"draining",	0,	COST_NOTAPPL,	WEAPON_ENERGYDRAIN,	QITEM_FN_NAME(wpnflag), NULL,	"energydrain weapon flag"},
	{	NULL,		0,	0,				0,					NULL,					NULL,	NULL					}
};

QUEST_ITEM quest_item_gmods[] = {
	{	"xp0.25",		VNUM_XPVOUCHER_025,	COST_NOTAPPL,	25,		QITEM_FN_NAME(gmod),	NULL,	"+0.25 xpmod voucher"	},
	{	"xp0.50",		VNUM_XPVOUCHER_050,	COST_NOTAPPL,	50,		QITEM_FN_NAME(gmod),	NULL,	"+0.25 xpmod voucher"	},
	{	"xp0.75",		VNUM_XPVOUCHER_075,	COST_NOTAPPL,	75,		QITEM_FN_NAME(gmod),	NULL,	"+0.25 xpmod voucher"	},
	{	"xp1.00",		VNUM_XPVOUCHER_100,	COST_NOTAPPL,	100,	QITEM_FN_NAME(gmod),	NULL,	"+0.25 xpmod voucher"	},
	{	"xp1.25",		VNUM_XPVOUCHER_125,	COST_NOTAPPL,	125,	QITEM_FN_NAME(gmod),	NULL,	"+0.25 xpmod voucher"	},
	{	"xp1.50",		VNUM_XPVOUCHER_150,	COST_NOTAPPL,	150,	QITEM_FN_NAME(gmod),	NULL,	"+0.25 xpmod voucher"	},
	{	"xp1.75",		VNUM_XPVOUCHER_175,	COST_NOTAPPL,	175,	QITEM_FN_NAME(gmod),	NULL,	"+0.25 xpmod voucher"	},
	{	"xp2.00",		VNUM_XPVOUCHER_200,	COST_NOTAPPL,	200,	QITEM_FN_NAME(gmod),	NULL,	"+0.25 xpmod voucher"	},

	{	"quest0.25",	VNUM_QPVOUCHER_025,	COST_NOTAPPL,	25,		QITEM_FN_NAME(gmod),	NULL,	"+0.25 qpmod voucher"	},
	{	"quest0.50",	VNUM_QPVOUCHER_050,	COST_NOTAPPL,	50,		QITEM_FN_NAME(gmod),	NULL,	"+0.50 qpmod voucher"	},
	{	"quest0.75",	VNUM_QPVOUCHER_075,	COST_NOTAPPL,	75,		QITEM_FN_NAME(gmod),	NULL,	"+0.75 qpmod voucher"	},
	{	"quest1.00",	VNUM_QPVOUCHER_100,	COST_NOTAPPL,	100,	QITEM_FN_NAME(gmod),	NULL,	"+1.00 qpmod voucher"	},
	{	"quest1.25",	VNUM_QPVOUCHER_125,	COST_NOTAPPL,	125,	QITEM_FN_NAME(gmod),	NULL,	"+0.25 qpmod voucher"	},
	{	"quest1.50",	VNUM_QPVOUCHER_150,	COST_NOTAPPL,	150,	QITEM_FN_NAME(gmod),	NULL,	"+0.50 qpmod voucher"	},
	{	"quest1.75",	VNUM_QPVOUCHER_175,	COST_NOTAPPL,	175,	QITEM_FN_NAME(gmod),	NULL,	"+0.75 qpmod voucher"	},
	{	"quest2.00",	VNUM_QPVOUCHER_200,	COST_NOTAPPL,	200,	QITEM_FN_NAME(gmod),	NULL,	"+1.00 qpmod voucher"	},
	{	NULL,			0,	0,				0,				NULL,					NULL,	NULL,							}
};

QUEST_ITEM quest_items[] = {
	{	"title",	0,	COST_NOTAPPL,	0,		QITEM_FN_NAME(next_items),	quest_item_title,		"your whotitle or pretitle"	},
	{	"restring",	0,	COST_NOTAPPL,	0,		QITEM_FN_NAME(next_items),	quest_item_restring,	"item restringing"			},
	{	"corpse",	0,	COST_NOTAPPL,	0,		QITEM_FN_NAME(next_items),	quest_item_corpse,		"corpse retrieval"			},
	{	"vinir",	0,	COST_NOTAPPL,	0,		QITEM_FN_NAME(next_items),	quest_item_vinir,		"vinir items"				},
	{	"aesir",	0,	COST_NOTAPPL,	0,		QITEM_FN_NAME(next_items),	quest_item_aesir,		"aesir items"				},
	{	"upgrade",	0,	COST_QUEST,	10000,		QITEM_FN_NAME(aesirupgrade),	NULL,		"aesir upgrade"				},
	{	"qitems",	0,	COST_NOTAPPL,	0,		QITEM_FN_NAME(next_items),	quest_item_qitems,		"general quest items"		},
	{	"immunity",	0,	COST_NOTAPPL,	0,		QITEM_FN_NAME(next_items),	quest_item_immunity,	"immunity"					},
	{	"wpnflag",	0,	COST_NOTAPPL,	0,		QITEM_FN_NAME(next_items),	quest_item_wpnflags,	"weapon flags"				},
	{	"voucher",	0,	COST_NOTAPPL,	0,		QITEM_FN_NAME(next_items),	quest_item_gmods,		"redeem a xp/qpmod voucher"	},
	{	"locker",	0,	COST_QUEST,		2000,	QITEM_FN_NAME(locker),		NULL,					"your locker"				},
	{	"pill",		50,	COST_QUEST,		300,	QITEM_FN_NAME(item),		NULL,					"YOU SHOULD NOT SEE THIS"	},
	{	NULL,		0,	0,				0,		NULL,						NULL,					NULL						}
};

QUEST_ITEM quest_item_request = {
	"", 0, COST_NOTAPPL, 0, QITEM_FN_NAME(quest_request), NULL, NULL
};
QUEST_ITEM quest_item_complete = {
	"", 0, COST_NOTAPPL, 0, QITEM_FN_NAME(quest_complete), NULL, NULL
};
QUEST_ITEM quest_item_quit = {
	"", 0, COST_NOTAPPL, 0, QITEM_FN_NAME(quest_quit), NULL, NULL
};
QUEST_ITEM quest_item_info = {
	"", 0, COST_NOTAPPL, 0, QITEM_FN_NAME(quest_info), NULL, NULL
};
QUEST_ITEM quest_item_locker = {
	"", 0, COST_NOTAPPL, 0, QITEM_FN_NAME(locker), NULL, NULL
};

QUEST_VERB quest_verbs[] = {
	// Items.
#define QVERB_BUY		&quest_verbs[0]
	{	"buy",		TRUE,	quest_items,			"buy a quest item/service"						},
#define QVERB_SELL		&quest_verbs[1]
	{	"sell",		TRUE,	quest_items,			"sell back a quest item"						},
#define QVERB_BROWSE	&quest_verbs[2]
	{	"browse",	TRUE,	quest_items,			"browse or get info on items/services"			},
#define QVERB_LIST		&quest_verbs[3]
	{	"list",		TRUE,	quest_items,			"same as browse"								},

	// Quest control.
#define QVERB_REQUEST	&quest_verbs[4]
	{	"request",	TRUE,	&quest_item_request,	"request a quest"								},
#define QVERB_COMPLETE	&quest_verbs[5]
	{	"complete",	TRUE,	&quest_item_complete,	"hand in a quest"								},
#define QVERB_QUIT		&quest_verbs[6]
	{	"quit",		TRUE,	&quest_item_quit,		"quit a quest"									},
#define QVERB_INFO		&quest_verbs[7]
	{	"info",		FALSE,	&quest_item_info,		"get information on your current/pending quest"	},

	// Lockers.
#define QVERB_PUT		&quest_verbs[8]
	{	"put",		TRUE,	&quest_item_locker,		"put something in your locker"					},
#define QVERB_TAKE		&quest_verbs[9]
	{	"get",		TRUE,	&quest_item_locker,		"get something from your locker"				},
#define QVERB_INVEN		&quest_verbs[10]
	{	"inventory",TRUE,	&quest_item_locker,		"show whats in your locker"						},

	{	NULL,		FALSE,	NULL,					NULL											}
};

char *price_types[] = {
	"ERROR",
	"qps",
	"platinum",
	"n/a"
};

CHAR_DATA *get_qm(CHAR_DATA *ch) {
	CHAR_DATA *qm;

	for (qm = ch->in_room->people; qm != NULL; qm = qm->next_in_room)
		if (IS_NPC(qm) && can_see(ch, qm) && qm->spec_fun == spec_lookup("spec_questmaster"))
			return qm;

	return qm;
}

QUEST_VERB *get_verb(char *argument) {
	QUEST_VERB *verb;

	for (verb = quest_verbs; verb->name != NULL; verb++)
		if (!str_prefix(argument, verb->name))
			return verb;
	return NULL;
}

QUEST_ITEM *get_item(char *argument, QUEST_ITEM *table) {
	for (; table->name != NULL; table++)
		if (!str_prefix(argument, table->name))
			return table;
	return NULL;
}

bool check_price(CHAR_DATA *ch, CHAR_DATA *qm, QUEST_ITEM *item) {
	char buf[MAX_STRING_LENGTH];
	if (item == NULL)
		return FALSE;

	switch (item->cost_type) {
	case COST_QUEST:
		if (ch->questpoints >= item->cost)
			return TRUE;
		break;
	case COST_PLAT:
		if (ch->platinum >= item->cost)
			return TRUE;
		break;
	case COST_NOTAPPL:
		return TRUE;
	}

	if (qm) {
		sprintf(buf, "$n{x says '{SSorry, $N{S, you need %d %s for that.{x'",
			item->cost, price_types[item->cost_type]);
		act(buf, qm, NULL, ch, TO_VICT);
	} else {
		sprintf(buf, "You need %d %s for that.",
			item->cost, price_types[item->cost_type]);
		send_to_char(buf, ch);
	}
	return FALSE;
}

void deduct_price(CHAR_DATA *ch, QUEST_ITEM *item) {
	if (item == NULL)
		return;

	switch (item->cost_type) {
	case COST_QUEST:
		ch->questpoints -= item->cost;
		break;
	case COST_PLAT:
		ch->platinum -= item->cost;
		break;
	}
}
void add_price(CHAR_DATA *ch, QUEST_ITEM *item, float modifier) {
	if (item == NULL)
		return;
	int oldCost = item->cost;
	item->cost = (int)(-((float)item->cost)*modifier);
	deduct_price(ch, item);
	item->cost = oldCost;
}

void display_items(QUEST_ITEM *table, BUFFER *buffer) {
	char buf[MAX_STRING_LENGTH], price[MAX_STRING_LENGTH];
	OBJ_INDEX_DATA *obj;

	add_buf(buffer, "{GName       {B-{G Description                         {B-{G Cost{x\r\n");
	for (; table->name != NULL; table++) {
		if (table->cost_type == COST_NOTAPPL)
			sprintf(price, "{yvaries");
		else
			sprintf(price, "{R%d {w%.8s", table->cost, price_types[table->cost_type]);

		if (table->obj_vnum > 0)
			obj = get_obj_index(table->obj_vnum);
		else
			obj = NULL;

		// Prioritise object description over the table description.
		sprintf(buf, "{w%-10s {B- {x%s {B- %s{x\r\n",
			table->name, end_string(obj != NULL ? obj->short_descr : table->description, 35), price);
		add_buf(buffer, buf);
	}
}

void do_quest(CHAR_DATA *ch, char *argument) {
	char buf[MAX_STRING_LENGTH];
	QUEST_VERB *verb;
	QUEST_ITEM *item;
	CHAR_DATA *qm = NULL;

	if (IS_NPC(ch)) {
		send_to_char("NPCs can't quest.\r\n", ch);
		return;
	}

	if (argument[0] == '\0') {
		BUFFER *buffer = new_buf();

		add_buf(buffer, "Available commands ({R*{x indicates you need a QM):\r\n");
		for (verb = quest_verbs; verb->name != NULL; verb++) {
			sprintf(buf, "{R%c{w%-9s{B -{x %s\r\n",
				verb->need_qm ? '*' : ' ', verb->name, verb->description);
			add_buf(buffer, buf);
		}

		page_to_char(buffer->string, ch);
		free_buf(buffer);
		return;
	}

	argument = one_argument(argument, buf);

	if ((verb = get_verb(buf)) == NULL) {
		send_to_char("Invalid command. Use \"{Gquest{x\" without any arguments to see a list of commands.\r\n", ch);
		return;
	}

	if (verb->need_qm && (qm = get_qm(ch)) == NULL) {
		send_to_char("You need a quest master to do that.\r\n", ch);
		return;
	}

	if (verb->items == NULL) {
		printf_debug("do_quest: null handler for verb %s, ch %s", verb->name, ch->name);
		send_to_char("An error has occurred! Please tell an immortal.\r\n", ch);
		return;
	}

	// Hack for no-item lists.
	if (verb->items->name[0] == '\0')
		item = verb->items;
	else {
		if (argument[0] == '\0') {
			BUFFER *buffer = new_buf();

			add_buf(buffer, "Please specify an item type from the following:\r\n");
			display_items(verb->items, buffer);

			sprintf(buf, "\r\nSyntax: {wquest %s {R<item-type>{x\r\n", verb->name);
			add_buf(buffer, buf);

			page_to_char(buffer->string, ch);
			free_buf(buffer);
			return;
		}

		argument = one_argument(argument, buf);
		if ((item = get_item(buf, verb->items)) == NULL) {
			send_to_char("Invalid item choice.\r\n", ch);
			return;
		}
	}

	// Perform the work.
	(*item->func)(ch, qm, verb, item, argument);
}

void end_quest(CHAR_DATA *ch, int timer) {
	// Remove tokens.
	if (ch->qstate == QSTATE_OBJ) {
		ROOM_INDEX_DATA *room = get_room_index(ch->qroom);
		if (room != NULL) {
			OBJ_DATA *obj;
			for (obj = room->contents; obj != NULL; obj = obj->next_content) {
				if (obj->pIndexData->vnum == ch->qtarget && obj->owner != NULL && !str_cmp(obj->owner, ch->name)) {
					extract_obj(obj);
					break;
				}
			}
		}
	}
    ch->qstate  = QSTATE_NONE;
    ch->qtarget = 0;
    ch->qtimer  = timer;
    ch->qmvnum  = 0;
    ch->qroom   = 0;
}

QITEM_FN_DEF(next_items) {
	char buf[MAX_STRING_LENGTH];

	if (item->items == NULL) {
		printf_debug( "next_items: No items for verb %s on item %s for character %s and extraargs %s.",
			verb->name, item->name, ch->name, argument);
		send_to_char("Something has gone wrong! Please tell an immortal.\r\n", ch);
		return;
	}
	if (argument[0] == '\0') {
		BUFFER *buffer = new_buf();

		add_buf(buffer, "Please specify an item from the following:\r\n");
		display_items(item->items, buffer);

		sprintf(buf, "\r\nSyntax: {wquest %s %s {R<item>{x\r\n", verb->name, item->name);
		add_buf(buffer, buf);

		page_to_char(buffer->string, ch);
		free_buf(buffer);
		return;
	}
	argument = one_argument(argument, buf);
	if ((item = get_item(buf, item->items)) == NULL) {
		send_to_char("Invalid item choice.\r\n", ch);
		return;
	}

	(*item->func)(ch, qm, verb, item, argument);
}

QITEM_FN_DEF(quest_request) {
	char buf[MAX_STRING_LENGTH];
	AREA_DATA *pArea;
	ROOM_INDEX_DATA *room;
	CHAR_DATA *mob;
	OBJ_DATA *obj;
	int type = number_range(QTYPE_MIN, QTYPE_MAX), vnum = 0, count;
	bool found = FALSE;

	if (ch->qstate == QSTATE_COMPLETE) {
		act("$n{x says '{SWhy don't you tell me about how you completed you last quest first, $N{S?{x'", qm, NULL, ch, TO_VICT);
		return;
	}
	if (ch->qstate != QSTATE_NONE) {
		act( "$n{x says '{SYou're still on a quest, $N{S.{x'", qm, NULL, ch, TO_VICT );
		return;
	}
	if (ch->qtimer > 0) {
		act( "$n{x says '{SYou can't quest yet, $N{S. Give others a chance.{x'", qm, NULL, ch, TO_VICT );
		return;
	}

	// 10000 tries to get a target.
	for (count = 0; count < 10000; room = NULL, count++) {
		vnum = number_range(300, 60000);
		if ((room = get_room_index(vnum)) == NULL || (pArea = room->area) == NULL)
			continue;

		if (pArea->continent != ch->in_room->area->continent
		|| 	strstr(pArea->builders, "Unlinked")
		|| 	strstr(pArea->builders, "Noquest")
		||	IS_SET(room->room_flags, ROOM_NO_MOB)
		||	IS_SET(room->room_flags, ROOM_SAFE)
		||	IS_SET(room->room_flags, ROOM_NO_MAGIC)
		||	IS_SET(room->room_flags, ROOM_NO_RECALL)
		||	IS_SET(room->room_flags, ROOM_IMP_ONLY)
		||	IS_SET(room->room_flags, ROOM_GODS_ONLY)
		||	IS_SET(room->room_flags, ROOM_HEROES_ONLY)
		||	IS_SET(room->room_flags, ROOM_NEWBIES_ONLY)
		||	IS_SET(room->room_flags, ROOM_PET_SHOP)
		||	IS_SET(room->room_flags, ROOM_SHOP)
		||	IS_SET(room->room_flags, ROOM_PRIVATE)
		||	IS_SET(room->room_flags, ROOM_SOLITARY)
		||	IS_SET(room->room_flags, ROOM_ARENA)
		||  bfs_pathfind(ch->in_room, pf_end_at_room(room), pf_filt_normal(ch), PF_MAX_DEPTH, FALSE) < 0)
			continue;

		switch (type) {
		case QSTATE_MOB:
			for (mob = room->people; mob != NULL; mob = mob->next_in_room) {
				int lvl_diff;
				if (!IS_NPC(mob) || is_safe_quiet(ch, mob))
					continue; // Deal with the basics first.

				// Alignment must be different.
				if (IS_EVIL(ch) == IS_EVIL(mob))
					continue;

				lvl_diff = mob->level - ch->level;
				// < LGD: -20 < lvl < 20, LGD: -20 < lvl < 50
				if (abs(lvl_diff) < 20 || (ch->level == LEVEL_HERO && lvl_diff > 0 && lvl_diff < 50))
					break;
			}
			if (mob != NULL) {
				sprintf( buf, "$n{x says '{SAsgard's most heinous criminal, %s{S has escaped from the dungeons!{x'",
						mob->short_descr );
				act( buf, qm, NULL, ch, TO_VICT );
				sprintf( buf, "$n{x says '{SOur court wizardess has located it somewhere in the region of %s{S in %s{S.{x'",
						room->name, pArea->name );
				act( buf, qm, NULL, ch, TO_VICT );
				act( "$n{x says '{SThe penalty of this crime is DEATH.{x'", qm, NULL, ch, TO_VICT );
				act( "$n{x says '{SWe're counting on you, $N{S, to carry out the sentence.{x'", qm, NULL, ch, TO_VICT );
				vnum = mob->pIndexData->vnum;
				found = TRUE;
			}
			break;
		case QSTATE_OBJ:
			if ((obj = create_object(get_obj_index(number_range(QTOKEN_LOWER, QTOKEN_UPPER)), 0)) != NULL) {
				free_string(obj->owner);
				obj->owner = str_dup(ch->name);
				obj_to_room(obj, room);
				act("$n{x says '{SA vile fiend has stolen a $p{S from the town treasury!{x'", qm, obj, ch, TO_VICT );
				sprintf(buf, "$n{x says '{SOur court wizardess has located it somewhere in the region of %s{S in %s{S.{x'",
						room->name, pArea->name);
				act(buf, qm, NULL, ch, TO_VICT);
				act("$n{x says '{SWe're counting on you, $N{S, to recover it. May the Gods be with you!{x'", qm, NULL, ch, TO_VICT);
				vnum = obj->pIndexData->vnum;
				found = TRUE;
			}
			break;
		}
		if (found) {
			ch->qstate	= type;
			ch->qtarget	= vnum;
			ch->qmvnum	= qm->pIndexData->vnum;
			ch->qtimer	= number_range(10, 30);
			ch->qroom	= room->vnum;
			sprintf(buf, "$n{x says '{SYou have %d ticks to complete this quest.{x'", ch->qtimer);
			act( buf, qm, NULL, ch, TO_VICT );
			act( "$n{x says '{SMay the Gods be with you!{x'", qm, NULL, ch, TO_VICT );
			return;
		}
	}
	act("$n{x says '{SI'm sorry, $N{S, but I can't find a quest for you at the moment.{x'", qm, NULL, ch, TO_VICT);
	ch->qtimer	= 3;
}

QITEM_FN_DEF(quest_complete) {
	act("$n informs $N $e has completed $s quest.", ch, NULL, qm, TO_ROOM);
	act("You inform $N you have completed $s quest.", ch, NULL, qm, TO_CHAR);

	if (ch->qstate == QSTATE_NONE) {
		act("$n{x says '{SBut you're not on a quest, $N{S.{x'", qm, NULL, ch, TO_VICT);
		return;
	}

	if (ch->qmvnum != qm->pIndexData->vnum) {
		act("$n{x says '{SI think you may have got the wrong person, $N{S.{x'", qm, NULL, ch, TO_VICT);
		return;
	}

	if (ch->qstate == QSTATE_OBJ) {
		OBJ_DATA *obj;
		for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
			if (obj->pIndexData->vnum == ch->qtarget) {
				act( "You hand $p{x over to $N{x, who accepts it graciously.", ch, obj, qm, TO_CHAR );
				act( "$n{x hands $p{x over to $N{x, who accepts it graciously.", ch, obj, qm, TO_ROOM );
				extract_obj(obj);
				// Yes, messy.
				goto questcomplete;
			}
		}
	}

	if (ch->qstate != QSTATE_COMPLETE) {
		act("$n{x says '{SBut you haven't finished your quest yet, $N{S.{x'", qm, NULL, ch, TO_VICT);
		return;
	}

questcomplete:
	act("$n{x says '{SThank you for completing the quest I gave you, $N{S!{x'", qm, NULL, ch, TO_VICT);

	char buf[MAX_STRING_LENGTH];
	int rewardQuest	= number_range( 15, 40 );
	int rewardPlat	= number_range( 5, 15 );
	int rewardExp	= number_range( 50, 350 );
	gmod_apply(GMOD_QP, &rewardQuest);
	gmod_apply(GMOD_QP, &rewardExp);
	gmod_apply(GMOD_QP, &rewardPlat);

	sprintf(buf, "$n{x says '{SAs a reward, I am giving you %d quest points, %d platinum and %d experience points! (%.2fx mod){x'",
			rewardQuest, rewardPlat, rewardExp, gmod_get_mult(GMOD_QP));
	act(buf, qm, NULL, ch, TO_VICT);

	ch->questpoints	+= rewardQuest;
	ch->platinum	+= rewardPlat;
	gain_exp(ch, rewardExp, TRUE, TRUE);

	if ( number_percent() < 5 ) {
		int rewardPrac	= number_range( 2, 5 );
		sprintf(buf, "$n{x says '{SAlso, for being such a good sport, I am awarding you %d practice points!{x'",
				rewardPrac);
		act(buf, qm, NULL, ch, TO_VICT);
		ch->practice	+= rewardPrac;
	}

	// Super-bonus prize (like the iquest pill before).
	if (number_percent() < 3) {
		rewardQuest	= number_range(300, 350);
		gmod_apply(GMOD_QP, &rewardQuest);
		sprintf(buf, "$n{x says '{SDING DING DING! YOU'VE WON THE JACKPOT OF %d QUESTPOINTS!{x'",
			rewardQuest);
		act(buf, qm, NULL, ch, TO_VICT);
		ch->questpoints	+= rewardQuest;
	}

	// Another super-bonus prize - a voucher to increase rewards.
	if (number_percent() < 3) {
		OBJ_INDEX_DATA *voucherIndex = get_obj_index(number_range(907, 914));
		OBJ_DATA *voucher = voucherIndex != NULL ? create_object(voucherIndex, voucherIndex->level) : NULL;
		if (voucher != NULL) {
			sprintf(buf, "$n{x says '{SJust as another little bonus, I'm giving you a %s{S!{x'",
       			voucher->short_descr);
			act(buf, qm, NULL, ch, TO_VICT);
			obj_to_char(voucher, ch);
		}
	}

	// Yet another bonus.
	if (number_percent() < 3 && gmod_add(GMOD_XP, 0.50f)) {
		act("$n{x says '{SI'm so impressed, I've increased the experience modifier for everyone!{x'", qm, NULL, ch, TO_VICT);
		sprintf(buf, "%s{Y has increased the experience modifier by {G+0.50x{Y to a total of {G%.2fx{Y!",
			qm->short_descr, gmod_get_mult(GMOD_XP));
		do_announce(ch, buf);
	}

	// Chance to quest again straight away.
	if (number_percent() < 2) {
		act("$n{x nudges you and says '{STell you what, $N{S, I'm going to let you quest again straight away if you want!{x'", qm, NULL, ch, TO_VICT);
		end_quest(ch, 0);
	} else
		end_quest(ch, 10);

	// Achievement monitoring.
	ach_increment(ch, ACH_QUESTS, 1);
}

QITEM_FN_DEF(quest_quit) {
	act("$n informs $N that he is giving up on $S quest.", ch, NULL, qm, TO_ROOM);
	act("You inform $N that you are giving up on your quest.", ch, NULL, qm, TO_CHAR);

	if (ch->qstate == QSTATE_NONE) {
		act("$n{x says '{SBut you're not on a quest, $N{S.{x'", qm, NULL, ch, TO_VICT);
		return;
	}
	if (ch->qmvnum != qm->pIndexData->vnum) {
		act("$n{x says '{SI think you may have got the wrong person, $N{S.{x'", qm, NULL, ch, TO_VICT);
		return;
	}
	if (ch->qstate == QSTATE_COMPLETE) {
		act("$n{x says '{SBut you've already completed your quest, $N{S!{x'", qm, NULL, ch, TO_VICT);
		return;
	}

	act("$n{x says '{SI'm sorry to hear that, $N{S. You'll have to wait now.{x'", qm, NULL, ch, TO_VICT);
	end_quest(ch, 15);
}

QITEM_FN_DEF(quest_info) {
	char buf[MAX_STRING_LENGTH];

	BUFFER *buffer = new_buf();
	add_buf(buffer, "{B+- {GQuest Info{B ---------------------------+\r\n");
	if (ch->qstate != QSTATE_NONE) {
		sprintf(buf, "{B| {wState:  {y%-30.30s {B|\r\n",
			ch->qstate == QSTATE_COMPLETE ? "Complete" : "Ongoing" );
		add_buf(buffer, buf);

		if (ch->qstate == QSTATE_MOB) {
			MOB_INDEX_DATA *mob = get_mob_index(ch->qtarget);
			sprintf(buf, "{B| {wTarget: {B({wMOB{B){x %s {B|\r\n",
				end_string(mob ? mob->short_descr : "NULL", 24));
			add_buf(buffer, buf);
		} else if (ch->qstate == QSTATE_OBJ) {
			OBJ_INDEX_DATA *obj = get_obj_index(ch->qtarget);
			sprintf(buf, "{B| {wTarget: {B({wOBJ{B){x %s {B|\r\n",
				end_string(obj ? obj->short_descr : "NULL", 24));
			add_buf(buffer, buf);
		}

		ROOM_INDEX_DATA *room = get_room_index(ch->qroom);
		sprintf(buf, "{B| {wRoom:   {y%s {B|\r\n",
			end_string(room ? room->name : "NULL", 30) );
		add_buf(buffer, buf);
		sprintf(buf, "{B| {wArea:   {y%s {B|\r\n",
			end_string(room ? room->area->name : "NULL", 30) );
		add_buf(buffer, buf);
	}

	sprintf(buf, "{B|{w Time:   {y%-12d %17s {B|\r\n",
		ch->qtimer, ch->qstate == QSTATE_NONE ? "(till next quest)" : "");
	add_buf(buffer, buf);

	add_buf(buffer, "{B+----------------------------------------+{x\r\n");
	page_to_char(buffer->string, ch);
	free_buf(buffer);
}

QITEM_FN_DEF(item) {
	char buf[MAX_STRING_LENGTH];
	OBJ_INDEX_DATA *objIndex = get_obj_index(item->obj_vnum);
	OBJ_DATA *obj = objIndex ? create_object(objIndex, objIndex->level) : NULL;

	if (obj == NULL)
		sprintf(buf, "quest item: NULL object for item %s vnum %d.",
			item->name, item->obj_vnum);
	else if (verb == QVERB_BUY) {
		if (!check_price(ch, qm, item))
			return;

		deduct_price(ch, item);
		obj_to_char(obj, ch);
		act("$n{x digs around in $s cupboard for something, emerging with $p{x.", qm, obj, NULL, TO_ROOM);
		act("$N{x gives you $p{x.", ch, obj, qm, TO_CHAR);
		act("$N{x gives $n{x $p{x.", ch, obj, qm, TO_ROOM);
		do_say(qm, "Pleasure doing business with you!");
		return;
	} else if (verb == QVERB_SELL) {
		extract_obj(obj);
		for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
			if (obj->pIndexData->vnum == item->obj_vnum)
				break;
		if (obj == NULL)
			act("$n{x says '{SYou don't have it, $N{S.{x'", qm, NULL, ch, TO_VICT);
		else {
			float mod = 0.80f;
			// Nasty hack for aesir junk.
			if ((item->obj_vnum >= 9873 && item->obj_vnum <= 9905) || (item->obj_vnum >= 9929 && item->obj_vnum <= 9932))
				mod = 0.869565f;
			add_price(ch, item, mod);
	        act("$n{x takes $p{x, dusts it off and puts it into $s cupboard.", qm, obj, NULL, TO_ROOM);
			do_say(qm, "Pleasure doing business with you!");
            extract_obj(obj);
		}
		return;
	} else if (verb == QVERB_BROWSE || verb == QVERB_LIST) {
		BUFFER *stats = display_stats(obj, ch, FALSE);
		page_to_char(stats->string, ch);
		free_buf(stats);
		extract_obj(obj);
		return;
	} else
		sprintf(buf, "quest item: Invalid verb %s on item %s for character %s and extraargs %s.",
			verb->name, item->name, ch->name, argument);
	printf_debug( buf);
	send_to_char("Something has gone wrong! Please tell an immortal.\r\n", ch);
}

QITEM_FN_DEF(gmod) {
	if (verb != QVERB_SELL) {
		send_to_char("You can only {Rsell{x a voucher.\r\n", ch);
		return;
	}

	OBJ_DATA *voucher;
	for (voucher = ch->carrying; voucher != NULL; voucher = voucher->next_content) {
		if (voucher->wear_loc == WEAR_NONE && voucher->pIndexData->vnum == item->obj_vnum)
			break;
	}

	if (voucher == NULL) {
		act( "$n{x says '{SYou're not carrying the right voucher, $N{S.{x'", qm, NULL, ch, TO_VICT );
		return;
	}

	// Hacky, but meh.
	GMOD_TYPE type;
	if (!str_prefix("xp", item->name))
		type = GMOD_XP;
	else if (!str_prefix("quest", item->name))
		type = GMOD_QP;
	else {
		printf_debug("Unknown modifier type: %s.", item->name);
		act( "$n{x says '{SI seem to have broken, please tell an immortal, $N{S.{x'", qm, NULL, ch, TO_VICT );
		return;
	}

	if (gmod_add(type, ((float)item->cost)/100.0f)) {
		char buf[MAX_STRING_LENGTH];
		act("$N{x takes $p{x from you.", ch, voucher, qm, TO_CHAR);
		extract_obj(voucher);
		sprintf(buf, "A {G+%0.2fx{Y %s voucher has just been redeemed, increasing the modifier to {G%.2fx{Y!",
			((float)item->cost)/100.0f, gmod_get_name(type), gmod_get_mult(type));
		do_announce(ch, buf);
		act("$n{x says '{SPleasure doing business with you, $N{S.{x'", qm, NULL, ch, TO_VICT);
		ach_increment(ch, ACH_VOUCHERS, 1);
	} else {
		act("$n{x says '{SSorry, $N{S, the modifier can't be increased at the moment.{x'", qm, NULL, ch, TO_VICT);
	}
}

QITEM_FN_DEF(corpse) {
	OBJ_DATA *obj;

	if (verb != QVERB_BUY) {
		send_to_char("You can only {Rbuy{x corpse retrieval.\r\n", ch);
		return;
	}

	for (obj = object_list; obj != NULL; obj = obj->next)
		if (obj->in_room != NULL
		&&  obj->item_type == ITEM_CORPSE_PC
		&& !str_cmp(obj->owner, ch->name))
			break;

	if (obj == NULL) {
		act( "$n{x says '{SYou don't have any corpses out there, $N{S.{x'", qm, NULL, ch, TO_VICT );
		return;
	}

	if (!check_price(ch, qm, item))
		return;

	deduct_price(ch, item);
	for (obj = object_list; obj != NULL; obj = obj->next) {
		if (obj->in_room != NULL
		&&  obj->item_type == ITEM_CORPSE_PC
		&& !str_cmp(obj->owner, ch->name)) {
			obj_from_room(obj);
			obj_to_char(obj, ch);
		}
	}
	send_to_char("You belongings appear in your hands!", ch);
}

void set_locker_name(CHAR_DATA *ch) {
	char buf[MAX_STRING_LENGTH];
	sprintf(buf, "locker%s", ch->name);
	free_string(ch->locker->name);
	ch->locker->name = str_dup(buf);
	sprintf(buf, "a locker belonging to %s", ch->name);
	free_string(ch->locker->short_descr);
	ch->locker->short_descr = str_dup(buf);
	free_string(ch->locker->description);
	ch->locker->description = str_dup(buf);
}

QITEM_FN_DEF(locker) {
	char buf[MAX_STRING_LENGTH];
	if (verb == QVERB_BUY) {
		if (!check_price(ch, qm, item))
			return;

		OBJ_INDEX_DATA *lockerIndex = get_obj_index(LOCKER_VNUM);

		if (lockerIndex == NULL) {
			send_to_char("BUG! No lockers available. Please tell an immortal.\r\n", ch);
			return;
		}

		if (ch->locker == NULL) {
			if ((ch->locker = create_object(lockerIndex, lockerIndex->level)) == NULL) {
				send_to_char("BUG! Could not create your locker. Please tell an immortal.\r\n", ch);
				return;
			}
			set_locker_name(ch);
		} else {
			// Expand.
			ch->locker->value[0] += lockerIndex->value[0];
			ch->locker->value[3] += lockerIndex->value[3];
		}
		deduct_price(ch, item);
		act("$n{x says '{SPleasure doing business with you, $N{S!{x'", qm, NULL, ch, TO_VICT);
		sprintf(buf, "$n{x says '{SYour locker now has a capacity of %d, and can hold items that are as heavy as %d.{x'",
			ch->locker->value[3]*10, ch->locker->value[0]*10);
		act(buf, qm, NULL, ch, TO_VICT);
    } else if (ch->locker == NULL)
        act( "$n{x says '{SYou don't have a locker, $N{S.{x'", qm, NULL, ch, TO_VICT );
    else if (verb == QVERB_BROWSE || verb == QVERB_LIST || verb == QVERB_INVEN) {
        BUFFER *buffer = show_list_to_char(ch->locker->contains, ch, TRUE, TRUE);
        send_to_char("Your locker contains:\r\n", ch);
        sprintf(buf, "Your locker capacity is %d with maximum object size of %d.\r\n",
            ch->locker->value[3]*10, ch->locker->value[0]*10);
        add_buf(buffer, buf);
        page_to_char(buffer->string, ch);
        free_buf(buffer);
	} else if (verb == QVERB_SELL)
		act( "$n{x says '{SI don't want your locker space, $N{S!{x'", qm, NULL, ch, TO_VICT );
	else if (verb == QVERB_PUT) {
		// Hack so we can reuse do_put code.
		obj_to_room(ch->locker, ch->in_room);
		argument = one_argument(argument, buf);
		strcat(buf, " locker");
		strcat(buf, ch->name);
		do_put(ch, buf);
		obj_from_room(ch->locker);
	} else if (verb == QVERB_TAKE) {
		// Hack so we can reuse do_get code.
		obj_to_room(ch->locker, ch->in_room);
		argument = one_argument(argument, buf);
		strcat(buf, " locker");
		strcat(buf, ch->name);
		do_get(ch, buf);
		obj_from_room(ch->locker);
	} else {
		printf_debug( "locker: Invalid verb %s for item %s, character %s",
			verb->name, item->name, ch->name);
	}
}

QITEM_FN_DEF(imm_hunger) {
	if (verb != QVERB_BUY) {
		send_to_char("You can only {Rbuy{x immunity to hunger.\r\n", ch);
		return;
	}

	if (ch->pcdata->condition[COND_HUNGER] == -1) {
		send_to_char("You are already immune to hunger.\r\n", ch);
		return;
	}

	if (!check_price(ch, qm, item))
		return;

	ch->pcdata->condition[COND_HUNGER] = -1;
	deduct_price(ch, item);

	act("$n{x sticks a knife into $N{x's gut and cuts $s stomach out.", qm, NULL, ch, TO_NOTVICT);
	act("$N{x sticks a knife into your gut and cuts your stomach out.", ch, NULL, qm, TO_CHAR);
	send_to_char("You feel perpetually full.\n\r", ch);
}

QITEM_FN_DEF(imm_thirst) {
	if (verb != QVERB_BUY) {
		send_to_char("You can only {Rbuy{x immunity to thirst.\r\n", ch);
		return;
	}

	if (ch->pcdata->condition[COND_THIRST] == -1) {
		send_to_char("You are already immune to thirst.\r\n", ch);
		return;
	}

	if (!check_price(ch, qm, item))
		return;

	ch->pcdata->condition[COND_THIRST] = -1;
	deduct_price(ch, item);

	act("$n{x sticks a knife into $N{x's head and cuts $s hypothalamus out.", qm, NULL, ch, TO_NOTVICT);
	act("$N{x sticks a knife into your gut and cuts your hypothalamus out.", ch, NULL, qm, TO_CHAR);
	send_to_char("Your thirst has been permanently quenched.\n\r", ch);
}

int calc_wpnflag_cost(OBJ_DATA *weapon, QUEST_ITEM *weapon_flag, CHAR_DATA *ch, CHAR_DATA *qm) {
	QUEST_ITEM *chkflags;
	int cost = 1;

	for (chkflags = quest_item_wpnflags; chkflags->name != NULL; chkflags++) {
		if (IS_WEAPON_STAT(weapon, chkflags->cost)) {
			AFFECT_DATA *wpnaff;
			bool foundtemp = FALSE, foundperm = FALSE;
			for (wpnaff = weapon->affected; wpnaff != NULL; wpnaff = wpnaff->next) {
				if (wpnaff->where == TO_WEAPON
				&&  wpnaff->bitvector == chkflags->cost) {
					if (wpnaff->duration < 0)
						foundperm = TRUE;
					else
						foundtemp = TRUE;
				}
			}
			// Found a matching temporary weapon flag (but no perm flag to stop cheaters from exploiting).
			if (foundtemp && !foundperm)
				continue;
			if (chkflags == weapon_flag) {
				char buf[MAX_STRING_LENGTH];
				sprintf(buf, "$n{x says '{SYour weapon is already %s, $N{S!{x'", weapon_flag->name);
				act(buf, qm, NULL, ch, TO_VICT);
				return -1;
			}
			cost++;
		}
	}

	return cost*cost*1500;
}

QITEM_FN_DEF(aesirupgrade) {
	if (argument[0] == '\0') {
		act("$n{x says '{SWhat item would you like me to upgrade, $N{S?{x'", qm, NULL, ch, TO_VICT);
		act("$n{x says '{SEach upgrade increases an Aesir item's stats by 10\% to a maximum of 100%.{S{x'", qm, NULL, ch, TO_VICT);
		act("$n{x says '{SWeapons increase 20\% per upgrade. Weapon dice however do not change{S.{x'", qm, NULL, ch, TO_VICT);
	//	act("$n{x says '{SOne final note: AC stat increases are added as an affect.{x'", qm, NULL, ch, TO_VICT);
		return;
	}

	OBJ_DATA *aesir;

	if ((aesir = get_obj_carry(ch, argument)) == NULL) {
		act("$n{x says '{SYou don't seem to have that, $N{S.{x'", qm, NULL, ch, TO_VICT);
		return;
	}

	if ((aesir->pIndexData->vnum >= 9873 && aesir->pIndexData->vnum <= 9905) || (aesir->pIndexData->vnum >= 9929 && aesir->pIndexData->vnum <= 9932)) { 
	}
	else {
		act("$n{x says '{SYou don't seem to have an aesir piece, $N{S.{x'", qm, NULL, ch, TO_VICT);
		return;
	}

	int cost = 10000;

	if (verb == QVERB_BROWSE || verb == QVERB_LIST) {
		char buf[MAX_STRING_LENGTH];
		sprintf(buf, "$n{x says '{SIt will cost %d questpoints to upgrade the %s{S, $N{S.{x'", cost, aesir->pIndexData->short_descr);
		act(buf, qm, NULL, ch, TO_VICT);
		return;
	} else if (verb != QVERB_BUY) {
		send_to_char("You can only {Rbrowse{x or {Rbuy{x weapon flags.\r\n", ch);
		return;
	}

	char buf[MAX_STRING_LENGTH];

	if (ch->questpoints < cost) {
		sprintf(buf, "$n{x says '{SSorry, $N{S, you need %d questpoints for that.{x'", cost);
		act(buf, qm, NULL, ch, TO_VICT);
		return;
	}

	if (aesir->tier_level >= 10) {
		sprintf(buf, "$n{x says, '{SI'm sorry $N{S, but your %s{S is already maxed out.{x'", aesir->pIndexData->short_descr);
		act(buf, qm, NULL, ch, TO_VICT);
		return;
	}

	ch->questpoints -= cost;
	OBJ_INDEX_DATA *oldaesir;
	AFFECT_DATA af;

	// Strip any old effects.
	AFFECT_DATA *paf, *paf_next;
	for (paf = aesir->affected; paf != NULL; paf = paf_next) {
		paf_next = paf->next;
		affect_remove_obj(aesir, paf);
	}

	// Grab appropriate aesir piece.	
	oldaesir  = get_obj_index(aesir->pIndexData->vnum);

	if (oldaesir == NULL) {
		ch->questpoints += cost;
		sprintf(buf, "$n{x says '{SSorry $N{S, I appear to be confused!{x'"); 
		act(buf, qm, NULL, ch, TO_VICT);
		return;
	}

	aesir->tier_level += 1;

	// Add upgraded keyword
	if (aesir->tier_level == 1) {
		char buf[MSL];
		sprintf(buf, "%s %s", aesir->name, "upgraded");
		free_string(aesir->name);
		aesir->name = str_dup(buf);
	}

	// Modify AC
	if (aesir->item_type == ITEM_ARMOR) {
		aesir->value[0] = oldaesir->value[0] + (oldaesir->value[0] * aesir->tier_level / 10);
		aesir->value[1] = oldaesir->value[1] + (oldaesir->value[1] * aesir->tier_level / 10);
		aesir->value[2] = oldaesir->value[2] + (oldaesir->value[2] * aesir->tier_level / 10);
		aesir->value[3] = oldaesir->value[3] + (oldaesir->value[3] * aesir->tier_level / 10);
	}
/*	if (aesir->item_type == ITEM_ARMOR) {
		af.where = TO_AFFECTS;
		af.type = 1; 
		af.level = aesir->level;
		af.duration = -1;
		af.location = APPLY_AC;
		af.modifier = ((aesir->value[0] + aesir->value[1] + aesir->value[2] + aesir->value[3]) / 4 * aesir->tier_level / 10) * -1;
		af.bitvector = 0;
		affect_to_obj(aesir, &af);
	}  */
	else if (aesir->item_type == ITEM_WEAPON) {
		aesir->tier_level += 1; // Weapons increase twice as fast.
	}

	// Add in original aesir effects.
	for (paf = oldaesir->affected; paf != NULL; paf = paf_next) {
		paf_next = paf->next;

		if ((paf->modifier * aesir->tier_level / 10) != 0) {
			af.where = paf->where;
			af.type = paf->type;
			af.level = paf->level;
			af.duration = paf->duration;
			af.location = paf->location;
			af.modifier = (paf->modifier * aesir->tier_level / 10);
			af.bitvector = paf->bitvector;
			affect_to_obj(aesir, &af);
		}
	}
	
	sprintf(buf, "$n{x says '{SYour %s{S now has a %d0%% bonus, $N{S!{x'", aesir->pIndexData->short_descr, aesir->tier_level); 
	act(buf, qm, NULL, ch, TO_VICT);
}

QITEM_FN_DEF(wpnflag) {
	if (argument[0] == '\0') {
		act("$n{x says '{SWhat weapon would you like me to flag, $N{S?{x'", qm, NULL, ch, TO_VICT);
		return;
	}

	OBJ_DATA *weapon;

	if ((weapon = get_obj_carry(ch, argument)) == NULL) {
		act("$n{x says '{SYou don't seem to have that, $N{S.{x'", qm, NULL, ch, TO_VICT);
		return;
	}

	if (weapon->item_type != ITEM_WEAPON) {
		act("$n{x says '{SYoucan only flag weapons, $N{S.{x'", qm, NULL, ch, TO_VICT);
		return;
	}

	if (verb == QVERB_BROWSE || verb == QVERB_LIST) {
		int cost = calc_wpnflag_cost(weapon, item, ch, qm);
		if (cost >= 0) {
			char buf[MAX_STRING_LENGTH];
			sprintf(buf, "$n{x says '{SIt will cost %d questpoints to put a %s flag on that weapon, $N{S.{x'", cost, item->name);
			act(buf, qm, NULL, ch, TO_VICT);
		}
		return;
	} else if (verb != QVERB_BUY) {
		send_to_char("You can only {Rbrowse{x or {Rbuy{x weapon flags.\r\n", ch);
		return;
	}

	char buf[MAX_STRING_LENGTH];
	int cost = calc_wpnflag_cost(weapon, item, ch, qm);

	if (cost < 0)
		return;

	if (ch->questpoints < cost) {
		sprintf(buf, "$n{x says '{SSorry, $N{S, you need %d questpoints for that.{x'", cost);
		act(buf, qm, NULL, ch, TO_VICT);
		return;
	}

	ch->questpoints -= cost;
	SET_BIT(weapon->value[4], item->cost);

	// Strip any old effects.
	AFFECT_DATA *wpnaff, *wpnaff_next;
	for (wpnaff = weapon->affected; wpnaff != NULL; wpnaff = wpnaff->next) {
		wpnaff_next = wpnaff->next;
		if (wpnaff->where == TO_WEAPON
		&&  wpnaff->bitvector == item->cost)
			affect_remove_obj(weapon, wpnaff);
	}
	sprintf(buf, "$n{x says '{SYour weapon is now permanently %s, $N{S!{x'", item->name);
	act(buf, qm, NULL, ch, TO_VICT);
}

QITEM_FN_DEF(restring_item) {
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *obj;

	if (verb != QVERB_BUY) {
		send_to_char("You can only {Rbuy{x restrings.\r\n", ch);
		return;
	}

	if (!check_price(ch, qm, item))
		return;

	if (argument[0] == '\0') {
		act("$n{x says '{SWell what do you want me to restring for you, $N{S?{x'", qm, NULL, ch, TO_VICT);
		return;
	}

	argument = one_argument(argument, buf);
	if ((obj = get_obj_carry(ch, buf)) == NULL)
	{
		act("$n{x says '{SYou can't restring non-existant objects, $N{S.{x'", qm, NULL, ch, TO_VICT);
		return;
	}

	if (IS_OBJ_STAT(obj, ITEM_NORESTRING)) {
		act("$n{x says '{SYou can't restring that, $N{x.{x'", qm, NULL, ch, TO_VICT);
		return;
	}

	if (argument[0] == '\0') {
		act("$n{x says '{SWell what do you want to restring it as, $N{S?{x'", qm, NULL, ch, TO_VICT);
		return;
	}

	if (!str_cmp(item->name, "short")) {
		strcpy(buf, process_name(argument));
		if (buf[0] == '\0') {
			act("$n{x says '{SYou can't string it as that, $N{S.{x'", qm, NULL, ch, TO_VICT);
			return;
		}

		free_string(obj->short_descr);
		obj->short_descr	= str_dup(argument);
		free_string(obj->name);
		obj->name			= str_dup(buf);
	} else if (!str_cmp(item->name, "long")) {
		free_string(obj->description);
		obj->description = str_dup(argument);
	} else {
		send_to_char("BUG!!! Tell an immortal.\r\n", ch);
		printf_debug("restring_item: Invalid type: %s", item->name);
		return;
	}
	deduct_price(ch, item);
	act("$n{x says '{SAll done, $N{S!{x'", qm, NULL, ch, TO_VICT);
}

QITEM_FN_DEF(title_pretitle) {
	if (verb != QVERB_BUY) {
		send_to_char("You can only {Rbuy{x pretitles.\r\n", ch);
		return;
	}

	if (IS_SET (ch->act, PLR_NOTITLE)) {
		act("$n{x says '{SSorry, $N{S, you're not allowed a pretitle.{x'", qm, NULL, ch, TO_VICT);
		return;
	}

	if (!check_price(ch, qm, item))
		return;

	if (argument[0] == '\0') {
		act("$n{x says '{SWell what do you want your pretitle to be, $N{S?{x'", qm, NULL, ch, TO_VICT);
		return;
	}

	char buf[MAX_STRING_LENGTH];
	if (strlen_color(argument) > 10)
		strcpy(buf, end_string(argument, 10));
	else
		strcpy(buf, argument);
	strcat(buf, "{x");

	free_string(ch->pcdata->pretit);
	ch->pcdata->pretit = str_dup(buf);
	deduct_price(ch, item);
	act("$n{x says '{SAll done, $N{S!{x'", qm, NULL, ch, TO_VICT);
}

QITEM_FN_DEF(title_whotitle) {
	if (verb != QVERB_BUY) {
		send_to_char("You can only {Rbuy{x whotitles.\r\n", ch);
		return;
	}

	if (IS_SET (ch->act, PLR_NOTITLE)) {
		act("$n{x says '{SSorry, $N{S, you're not allowed a whotitle.{x'", qm, NULL, ch, TO_VICT);
		return;
	}

	if (!check_price(ch, qm, item))
		return;

	if (argument[0] == '\0') {
		act("$n{x says '{SWell what do you want your whotitle to be, $N{S?{x'", qm, NULL, ch, TO_VICT);
		return;
	}

	char buf[MAX_STRING_LENGTH];
	if (IS_IMMORTAL(ch))
		sprintf(buf, "%s{x", end_string(argument, 16));
	else
		sprintf(buf, "%s{x", end_string(argument, 10));

	free_string(ch->pcdata->who_descr);
	ch->pcdata->who_descr = str_dup(buf);
	deduct_price(ch, item);
	act("$n{x says '{SAll done, $N{S!{x'", qm, NULL, ch, TO_VICT);
}
