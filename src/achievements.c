/*
 * achievements.c
 *
 *  Created on: 22 Jun 2011
 *      Author: Nico
 *  Achievements system.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "merc.h"
#include "recycle.h"
#include "achievements.h"

bool pach_learned1(CHAR_DATA *ch);
bool pach_learned2(CHAR_DATA *ch);
bool pach_stance1(CHAR_DATA *ch);
bool pach_stance2(CHAR_DATA *ch);

// Table of achievements.
const ACH_DEFINITION achievements_table[] = {
	{ "lgd", 	ACH_LGD,		1,		"I Am Legend",					1000,	"reach level 101 at tier 3"					},

	{ "kNPC1",	ACH_NPCKILLS,	1,		"You murderer!",				100,	"kill an NPC"								},
	{ "kNPC2",	ACH_NPCKILLS,	2000,	"Seasoned killer",				2000,	"kill %lu NPCs"								},
	{ "kNPC3",	ACH_NPCKILLS,	50000,	"Bathed in blood",				5000,	"kill %lu NPCs"								},
	{ "kNPC4",	ACH_NPCKILLS,	100000,	"NPC GENOCIDE!!",				9000,	"kill %lu NPCs"								},

	{ "kPK1",	ACH_PKKILLS,	1,		"Antisocial",					1000,	"kill a player outside the arena"			},
	{ "kPK2",	ACH_PKKILLS,	50,		"Player-slayer",				5000,	"kill %lu players outside the arena"		},
	{ "kPK3",	ACH_PKKILLS,	200,	"Unstoppable",					10000,	"kill %lu players outside the arena"		},

	{ "kAre1",	ACH_AKILLS,		1,		"Ring fighter",					500,	"kill a player in the arena"				},
	{ "kAre2",	ACH_AKILLS,		100,	"Gladiator",					2000,	"kill %lu players in the arena"				},
	{ "kAre3",	ACH_AKILLS,		500,	"A sure thing",					5000,	"kill %lu players in the arena"				},

	{ "kBab1",	ACH_BABKILLS,	5,		"Babblicious",					500,	"kill %lu babbles"							},
	{ "kBab2",	ACH_BABKILLS,	100,	"The Bane of the Babbles",		1000,	"kill %lu babbles"							},
	{ "kBab3",	ACH_BABKILLS,	500,	"Kiss my ass, NINJA BABBLE",	5000,	"kill %lu babbles"							},

	{ "ques1",	ACH_QUESTS,		1,		"Q-q-q-quest!",					100,	"complete a quest"							},
	{ "ques2",	ACH_QUESTS,		500,	"Compulsive questor",			3000,	"complete %lu quests"						},
	{ "ques3",	ACH_QUESTS,		5000,	"*COUGH*Shryp*COUGH*",			9000,	"complete %lu quests"						},

	{ "hour1",	ACH_HOURS,		1,		"This MUD is OK, I guess...",	500,	"play over an hour total"					},
	{ "hour2",	ACH_HOURS,		500,	"Zeit meister",					4000,	"play for a total of %lu hours"				},
	{ "hour3",	ACH_HOURS,		1500,	"This is SO FUN!",				8000,	"play for a total of %lu hours"				},
	{ "hour4",	ACH_HOURS,		2500,	"I MUD, therefore I am",		15000,	"play for a total of %lu hours"				},

	{ "run1",	ACH_RUNS,		1,		"Sprinter",						100,	"run somewhere"								},
	{ "run2",	ACH_RUNS,		1000,	"Regular exerciser",			1000,	"run anywhere %lu times"					},
	{ "run3",	ACH_RUNS,		3000,	"Marathon runner",				2000,	"run anywhere %lu times"					},

	{ "vchr1",	ACH_VOUCHERS,	1,		"Modified",						100,	"redeem a modifier voucher"					},
	{ "vchr2",	ACH_VOUCHERS,	10,		"Redeemer",						1000,	"redeem %lu modifier vouchers"				},
	{ "vchr3",	ACH_VOUCHERS,	100,	"Voucher-mania!",				5000,	"redeem %lu modifier vouchers"				},

	{ "rand1",	ACH_RANDOMS,	1,		"WTF?! This piece sucks!",		200,	"pop a random item"							},
	{ "rand2",	ACH_RANDOMS,	100,	"Randomiser",					750,	"pop %lu random items"						},
	{ "rand3",	ACH_RANDOMS,	1000,	"Sir Pop-A-Lot",				1300,	"pop %lu random items"						},
	{ "rand4",	ACH_RANDOMS,	5000,	"BOTTER!",						2000,	"pop %lu random items"						},

	{ "quaf1",	ACH_QUAFFS,		10,		"I don't drink, I quaff!",		100,	"quaff %lu potions"							},
	{ "quaf2",	ACH_QUAFFS,		250,	"Self-medicator",				1500,	"quaff %lu potions"							},
	{ "quaf3",	ACH_QUAFFS,		1500,	"Quaffoholic",					2000,	"quaff %lu potions"							},

	{ "bank1",	ACH_BALANCE,	100,	"I can buy something!",			500,	"have %lu platinum in the bank"				},
	{ "bank2",	ACH_BALANCE,	5000,	"Plat-runner",					1000,	"have %lu platinum in the bank"				},
	{ "bank3",	ACH_BALANCE,	100000,	"I'm rich...RICH!",				5000,	"have %lu platinum in the bank"				},
	{ "bank4",	ACH_BALANCE,	1000000,"Millionaires are overrated",	10000,	"have %lu platinum in the bank"				},

	{ "stab1",	ACH_BACKSTABS,	1,		"Backstabber",					100,	"backstab an NPC or player"					},
	{ "stab2",	ACH_BACKSTABS,	50,		"Pain-in-the-back(side)",		500,	"backstab %lu NPCs or players"				},
	{ "stab3",	ACH_BACKSTABS,	2000,	"Bastard",						2000,	"backstab %lu NPCs or players"				},

	{ "hari1",	ACH_HARAKIRI,	1,		"OMG! WTF Are You Doing?!",		100,	"perform hara kiri"							},
	{ "hari2",	ACH_HARAKIRI,	100,	"Masochist",					7500,	"perform hara kiri %lu times"				},
	{ "hari3",	ACH_HARAKIRI,	1000,	"Its only a flesh wound...",	2000,	"perform hara kiri %lu times"				},
	{ "hari4",	ACH_HARAKIRI,	5000,	"Seriously...you need help",	5000,	"perform hara kiri %lu times"				},

	{ "cast1",	ACH_CASTS,		1000,	"Spell spammer",				2000,	"successfully cast %lu spells"				},
	{ "cast2",	ACH_CASTS,		10000,	"Can I OD on magic?",			5000,	"successfully cast %lu spells"				},

	{ "lght1",	ACH_LIGHTNING,	1,		"You've been...Thunderstruck!",	100,	"experience the lightning dream"			},
	{ "lght2",	ACH_LIGHTNING,	5,		"Zzzzizzling",					500,	"experience the lightning dream %lu times"	},
	{ "lght3",	ACH_LIGHTNING,	20,		"Ride The Lightning",			2000,	"experience the lightning dream %lu times"	},

	{ "pill1",	ACH_PILLS,		1,		"One won't hurt, will it?",		100,	"eat a shimmering white pill"				},
	{ "pill2",	ACH_PILLS,		10,		"Taste the rainbow",			500,	"eat %lu shimmering white pills"			},
	{ "pill3",	ACH_PILLS,		100,	"Addict!",						800,	"eat %lu shimmering white pills"			},
	{ "pill4",	ACH_PILLS,		500,	"Pill popper",					1500,	"eat %lu shimmering white pills"			},
	{ "pill5",	ACH_PILLS,		10000,	"*COUGH*Bree*COUGH*",			3000,	"eat %lu shimmering white pills"			},

	// Start of psuedo achievements
	{ "learn1",	ACH_PSEUDO,	(unsigned long)&pach_learned1,	"I'm learnding",				1000,	"Reach 50 in one magic type"	},
	{ "learn2",	ACH_PSEUDO,	(unsigned long)&pach_learned2,	"I'm learning!",				5000,	"Reach 50 in all magic types"	},

	{ "stanc1",	ACH_PSEUDO,	(unsigned long)&pach_stance1,	"Patience, young grasshopper",	1000,	"Reach 200 in one stance"		},
	{ "stanc2",	ACH_PSEUDO,	(unsigned long)&pach_stance2,	"I know kung fu!",				5000,	"Reach 200 in all stances"		},

	// End marker.
	{ NULL,		-1,				-1,		NULL,							-1,		NULL										}
};

// Memory management for achievment done structure.
static ACH_DONE ach_done_zero, *ach_done_free;
ACH_DONE *new_ach_done(const ACH_DEFINITION *def) {
	ACH_DONE *done;
	if (ach_done_free == NULL)
		done = malloc(sizeof(ACH_DONE));
	else {
		done = ach_done_free;
		ach_done_free = done->next;
	}
	*done = ach_done_zero;
	done->type = def;
	return done;
}
void free_ach_done(ACH_DONE *done) {
	done->next = ach_done_free;
	ach_done_free = done;
}

void ach_increment(CHAR_DATA *ch, ACH_METRIC metric, unsigned long amount) {
	// Safety checks.
	if (ch == NULL || ch->pcdata == NULL)
		return;

	// Cap amount, to prevent overflow.
	amount = UMIN(amount, 294967294UL);
	// Update the metric on the player record.
	ch->pcdata->achievement_metrics[metric]
		= UMIN(ch->pcdata->achievement_metrics[metric]+amount, 4000000000UL);
	// Update character.
	ach_update(ch);
}

bool ach_unlocked(CHAR_DATA *ch, const ACH_DEFINITION *achievement) {
	ACH_DONE *done;
	for (done = ch->pcdata->achievements_done; done != NULL; done = done->next)
		if (done->type == achievement)
			return TRUE;
	return FALSE;
}

// Unlock an achievement.
static void unlock_achievement(CHAR_DATA *ch, const ACH_DEFINITION *achievement) {
	char buf[MAX_STRING_LENGTH];
	ACH_DONE *done;

	// Break out if we need to.
	if (ach_unlocked(ch, achievement))
		return;

	// Tell the player.
	sprintf(buf, "{D<< {YYou have unlocked the {D\"{G%s{D\"{Y achievement for {G%d{Y experience! {D>>{x\r\n",
		achievement->name, achievement->reward);
	send_to_char(buf, ch);

	// Now tell everyone in the room.
	sprintf(buf, "{D<< {Y$n has unlocked the {D\"{G%s{D\"{Y achievement! {D>>{x\r\n",
		achievement->name);
	act(buf, ch, NULL, NULL, TO_ROOM);

	// Gain experience quietly, since we say how much they get already.
	gain_exp(ch, achievement->reward, TRUE, TRUE);

	// Add the achievement to the player.
	done = new_ach_done(achievement);
	done->next = ch->pcdata->achievements_done;
	ch->pcdata->achievements_done = done;
}

void ach_update(CHAR_DATA *ch) {
	// Safety checks.
	if (ch == NULL || ch->pcdata == NULL)
		return;

	const ACH_DEFINITION *def;
	for (def = achievements_table; def->name != NULL; ++def) {
		if (def->metric > ACH_MAX) {
			if (((bool(*)(CHAR_DATA*))def->threshold)(ch))
				unlock_achievement(ch, def);
		} else if (ch->pcdata->achievement_metrics[def->metric] >= def->threshold) {
			// Achievement!
			unlock_achievement(ch, def);
		}
	}
}

static char *progress_bar(CHAR_DATA *ch, const ACH_DEFINITION *achievement) {
	static char buf[MAX_STRING_LENGTH];
	char percentage_buf[MAX_STRING_LENGTH];
	unsigned long percentage = UMIN(100, ch->pcdata->achievement_metrics[achievement->metric]*100/achievement->threshold);

	if (ach_unlocked(ch, achievement))	// If its 100%, show it.
		sprintf(buf, "{B[{G*UNLOCKED*{B]");
	else if (achievement->metric > ACH_MAX) {
		sprintf(buf, "{B[{y----------{B]");
	} else {
		if (percentage < 10)
			sprintf(percentage_buf, " {r%lu{W%% ",
				percentage);
		else
			sprintf(percentage_buf, "%c{%c%2.2lu{W%%",
				percentage >= 40 ? '=' : ' ',
				percentage < 50 ? 'y' : 'g', percentage);

		sprintf(buf, "{B[{D%c%c%c%s{D%c%c%c{B]",
			percentage >= 10 ? '=' : ' ', percentage >= 20 ? '=' : ' ',
			percentage >= 30 ? '=' : ' ', percentage_buf,
			percentage >= 80 ? '=' : ' ', percentage >= 90 ? '=' : ' ',
			percentage >= 100 ? '=' : ' ');
	}
	return buf;
}

static char *description_format(const ACH_DEFINITION *def) {
	static char buf[MAX_STRING_LENGTH];

	sprintf(buf, def->description, def->threshold);
	return buf;
}

void do_achievements(CHAR_DATA *ch, char *argument) {
	BUFFER *buffer;
	char buf[MAX_STRING_LENGTH];

	buffer = new_buf();
	add_buf(buffer, "{B+--------------------------------------- {GYour Achievements{B ---------------------------------------+\r\n");

	// Loop through achievements_table, printing progress.
	const ACH_DEFINITION *def;
	for (def = achievements_table; def->name != NULL; ++def) {
		sprintf(buf, "| %s {w%5d{Wxp {B- {w%-29s {B- {w%-40s {B|\r\n",
			progress_bar(ch, def),
			def->reward,
			def->name,
			description_format(def));
		add_buf(buffer, buf);
	}

	add_buf(buffer, "+-------------------------------------------------------------------------------------------------+{x\r\n");
	page_to_char(buffer->string, ch);
	free_buf(buffer);
}


// Start of psuedo-achievement functions.
bool pach_learned1(CHAR_DATA *ch) {
	int i;
	for (i = 0; i < 6; ++i)
		if (ch->magic[i] >= 50)
			return TRUE;
	return FALSE;
}
bool pach_learned2(CHAR_DATA *ch) {
	int i;
	for (i = 0; i < 6; ++i)
		if (ch->magic[i] < 50)
			return FALSE;
	return TRUE;
}

bool pach_stance1(CHAR_DATA *ch) {
	int i;
	for (i = 1; i < 11; ++i)
		if (ch->stance[i] >= 200)
			return TRUE;
	return FALSE;
}
bool pach_stance2(CHAR_DATA *ch) {
	int i;
	for (i = 1; i < 11; ++i)
		if (ch->stance[i] < 200)
			return FALSE;
	return TRUE;
}
