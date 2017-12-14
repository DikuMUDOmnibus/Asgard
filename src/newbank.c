/*
 * newbank.c
 *
 *  Created on: 26 May 2011
 *      Author: Nico
 *  Cleaner version of bank code.
 */

#include <stdlib.h>
#include <stdio.h>
#include "merc.h"
#include "achievements.h"

#define BANK_MAX	4

struct bank_type {
	char * name;
	sh_int vnum;
};

static const struct bank_type bank_table[BANK_MAX] = {
	{ "First National Bank of Midgard", 3334 },
	{ "The Mercenary's Vault", 17350 },
	{ "Solace Federal Reserve", 10267 },
	{ "New Thalos Savings and Trust", 9800 },
};

static int get_bank_id(CHAR_DATA *ch) {
	int bank;

	if (ch != NULL || ch->in_room != NULL)
		for (bank = 0; bank < BANK_MAX; bank++)
			if (bank_table[bank].vnum == ch->in_room->vnum)
				return bank;

	return -1;
}

static CHAR_DATA *get_banker(CHAR_DATA *ch) {
	CHAR_DATA *banker;

	for (banker = ch->in_room->people; banker != NULL; banker = banker->next_in_room)
		if (IS_NPC(banker) && IS_SET(banker->act, ACT_IS_BANKER))
			return banker;

	return NULL;
}

void do_withdraw(CHAR_DATA *ch, char *argument) {
	char buf[MAX_STRING_LENGTH];
	int bank = get_bank_id(ch);
	CHAR_DATA *banker;
	long amount;

	if (bank < 0) {
		send_to_char("You can only withdraw money at a bank.\r\n", ch);
		return;
	}

	if ((banker = get_banker(ch)) == NULL) {
		send_to_char("The banker appears to be on their break.\r\n", ch);
		return;
	}

	if (argument[0] == '\0' || !is_number(argument) || (amount = atol(argument)) <= 0) {
		act("$n{x says '{STell me an amount greater than zero to withdraw, $N{S.{x'", banker, NULL, ch, TO_VICT);
		return;
	}

	if (ch->balance[bank] < amount) {
		act("$n{x says '{SYou don't have enough money in your account, $N{S.{x'", banker, NULL, ch, TO_VICT);
		return;
	}

	ch->balance[bank]	-= amount;
	ch->platinum		+= amount;
	sprintf(buf, "$N{x reluctantly gives %ld platinum coins to you.", amount);
	act(buf, ch, NULL, banker, TO_CHAR);
	act("$N{x reluctantly hands a pile of coins to $n{x.", ch, NULL, banker, TO_ROOM);
}

void do_deposit(CHAR_DATA *ch, char *argument) {
	char buf[MAX_STRING_LENGTH];
	int bank = get_bank_id(ch);
	CHAR_DATA *banker;
	long amount;

	if (bank < 0) {
		send_to_char("You can only deposit money at a bank.\r\n", ch);
		return;
	}

	if ((banker = get_banker(ch)) == NULL) {
		send_to_char("The banker appears to be on their break.\r\n", ch);
		return;
	}

	if (argument[0] == '\0' || !is_number(argument) || (amount = atol(argument)) <= 0) {
		act("$n{x says '{STell me an amount greater than zero to deposit, $N{S.{x'", banker, NULL, ch, TO_VICT);
		return;
	}

	if (amount > ch->platinum) {
		act("$n{x says '{SYou're too POOR to cover that amount, $N{S!!!{x'", banker, NULL, ch, TO_VICT);
		return;
	}

	// Check for negatives in case of overflow.
	if (ch->balance[bank] + amount > 2000000000L || ch->balance[bank] + amount < 0) {
		act("$n{x says '{SOur vaults can't handle that much money, $N{S.{x'", banker, NULL, ch, TO_VICT);
		return;
	}

	ch->platinum		-= amount;
	ch->balance[bank]	+= amount;
	sprintf(buf, "You hand %ld platinum coins to $N{x, who accepts them greedily.", amount);
	act(buf, ch, NULL, banker, TO_CHAR);
	act("$n{x hands a pile of coins to $N{x, who accepts them greedily.", ch, NULL, banker, TO_ROOM);

	// Achievement stuff.
	ch->pcdata->achievement_metrics[ACH_BALANCE] = UMAX(ch->pcdata->achievement_metrics[ACH_BALANCE], ch->balance[bank]);
	ach_update(ch);
}

void do_balance(CHAR_DATA *ch, char *argument) {
	char buf[MAX_STRING_LENGTH];
	int bank = get_bank_id(ch), otherbank;
	CHAR_DATA *banker;

	if (bank < 0) {
		send_to_char("You can only check your balance at a bank.\r\n", ch);
		return;
	}

	if ((banker = get_banker(ch)) == NULL) {
		send_to_char("The banker appears to be on their break.\r\n", ch);
		return;
	}

	sprintf(buf, "$n{x says '{SYou have %ld platinum in our bank, $N{S.{x'", ch->balance[bank]);
	act(buf, banker, NULL, ch, TO_VICT);

	bool first = TRUE;
	for (otherbank = 0; otherbank < BANK_MAX; otherbank++) {
		if (otherbank != bank && ch->balance[otherbank] > 0) {
			if (first)
				act("$n{x says '{SYou also have money in our other branches. I'll read the balances out to you.{x'", banker, NULL, ch, TO_VICT);
			sprintf(buf, "$n{x says '{SYou have %ld platinum at %s{S.{x'", ch->balance[otherbank], bank_table[otherbank].name);
			act(buf, banker, NULL, ch, TO_VICT);
			first = FALSE;
		}
	}
}
