/*
 * expmod.c
 *
 *  Created on: 4 Feb 2011
 *      Author: Nico
 *
 *  Variable experience modifier.
 */

#include <stdlib.h>
#include <stdio.h>
#include "merc.h"

#define EXPMOD_FILE		"expmod.txt"

void do_announce(CHAR_DATA *ch, char *argument);

static int expmod_num = 1;				// Exp modifier numerator.
static int expmod_denom = 1;			// Exp modifier denominator.
static int expmod_min_level = 1;		// Minimum level of players affected.
static int expmod_max_level = LEVEL_HERO;		// Maximum level of players affected.
static int expmod_timer = -1;			// How long left on the timer in minutes.
static int expmod_last_update = 0;		// Last update for expmod.

// We will assume that any change to the exp_mod_denom field will handle zeroes.

bool expmod_is_enabled() {
	return expmod_timer != 0
		&& expmod_num != expmod_denom;
}

char *expmod_multiplier() {
	static char buf[MAX_STRING_LENGTH];

	if (expmod_denom == 1)
		sprintf(buf, "%d", expmod_num);
	else
		sprintf(buf, "%.3f", ((float)expmod_num)/((float)expmod_denom));

	return buf;
}

char *expmod_level_range() {
	static char buf[MAX_STRING_LENGTH];

	if (expmod_min_level == expmod_max_level)
		sprintf(buf, "%d", expmod_min_level);
	else
		sprintf(buf, "%d-%d", expmod_min_level, expmod_max_level);

	return buf;
}

char *expmod_duration() {
	static char buf[MAX_STRING_LENGTH];

	if (expmod_timer < 0)
		return "forever";

	sprintf(buf, "%d minute%s", expmod_timer, expmod_timer != 1 ? "s" : "");
	return buf;
}

bool expmod_applies(CHAR_DATA *ch) {
	return expmod_is_enabled()
		&& ch->level >= expmod_min_level
		&& ch->level <= expmod_max_level;
}

int expmod_calc(int exp) {
	return exp * expmod_num / expmod_denom;
}

void expmod_save() {
	FILE *fp;

	if ((fp = get_temp_file()) == NULL) {
		printf_debug("expmod_save: could not open temporary file.");
		return;
	}

	fprintf(fp, "%d %d %d %d %d",
		expmod_num, expmod_denom, expmod_min_level, expmod_max_level, expmod_timer);

	if (!close_write_file(EXPMOD_FILE))
		printf_debug("expmod_save: could not save file as "EXPMOD_FILE".");
}

void expmod_load() {
	FILE *fp;

	if ((fp = fopen(EXPMOD_FILE, "r")) == NULL) {
		printf_debug("expmod_load: could not open "EXPMOD_FILE".");
		return;
	}

	expmod_num = fread_number(fp);
	expmod_num = UMAX(expmod_num, 1);

	expmod_denom = fread_number(fp);
	if (expmod_denom == 0)
		expmod_denom = 1;

	expmod_min_level = fread_number(fp);
	expmod_min_level = UMAX(expmod_min_level, 1);

	expmod_max_level = fread_number(fp);
	expmod_max_level = UMIN(expmod_max_level, LEVEL_HERO);

	expmod_timer = fread_number(fp);
	fclose(fp);
}

void do_expmod(CHAR_DATA *ch, char *argument) {
	char buf[MAX_STRING_LENGTH];

	if (argument[0] == '\0') {
		send_to_char(
			"Syntax: {wexpmod level <min level> <max level>\r\n"
			"        {y - Set the level range affected by the modifier\r\n"
			"        {wexpmod duration <minutes>|forever\r\n"
			"        {y - Set the duration for the modifier to last\r\n"
			"        {wexpmod multiplier <numerator> [denominator]\r\n"
			"        {y - Set the modifier multiplier (numerator/denominator fraction)\r\n"
			"        {wexpmod default level|all [duration]\r\n"
			"        {y - Set the default expmod for levellers or everyone with an optional duration\r\n"
			"        {wexpmod status\r\n"
			"        {y - See the current status of the experience modifier.\r\n\r\n"
			"        {wThe experience modifier only takes effect if the duration != 0\r\n"
			"        and the multiplier != 0\r\n{x",
			ch);
		return;
	}

	argument = one_argument(argument, buf);

	bool pre_change = expmod_is_enabled();

	if (!str_prefix(buf, "level")) {
		argument = one_argument(argument, buf);

		if (buf[0] == '\0' || argument[0] == '\0') {
			send_to_char("Please specify both the minimum level and maximum level.\r\n", ch);
			return;
		}

		int min_level, max_level;
		if ((min_level = atoi(buf)) < 1
		||	(max_level = atoi(argument)) < 1
		||	min_level > max_level
		||  max_level > LEVEL_HERO) {
			sprintf(buf, "Please specify the levels as numbers between 1 and %d, with the minimum level first.\r\n", LEVEL_HERO);
			send_to_char(buf, ch);
			return;
		}

		sprintf(buf, "Level range set to %d-%d.\r\n", min_level, max_level);
		send_to_char(buf, ch);
		expmod_min_level = min_level;
		expmod_max_level = max_level;
	} else if (!str_prefix(buf, "duration")) {
		if (argument[0] == '\0') {
			send_to_char("Please specify a length in minutes or \"forever\".\r\n", ch);
			return;
		}

		if (is_number(argument)) {
			int duration = atoi(argument);
			if (duration <= 0)
				send_to_char("The length must be a positive number.\r\n", ch);
			else {
				sprintf(buf, "Duration set to %d minutes.\r\n", duration);
				send_to_char(buf, ch);
				expmod_timer = duration;
			}
		} else if (!str_cmp(argument, "forever")) {
			expmod_timer = -1;
			send_to_char("Timer set to forever.\r\n", ch);
		} else
			send_to_char("Invalid length of time.\r\n", ch);
	} else if (!str_prefix(buf, "multiplier")) {
		int numerator, denominator = 1;

		argument = one_argument(argument, buf);

		if (buf[0] == '\0' || (numerator = atoi(buf)) < 1) {
			send_to_char("Please specify a multiplier greater than zero.\r\n", ch);
			return;
		}

		if (argument[0] != '\0' && (denominator = atoi(argument)) < 1) {
			send_to_char("Please specify a divisor greater than zero.\r\n", ch);
			return;
		}

		if (denominator > numerator) {
			send_to_char("The divisor must be less than the multiplier.\r\n", ch);
			return;
		}

		if (numerator / denominator > 10) {
			send_to_char("No more than 10x multiplier, please.\r\n", ch);
			return;
		}

		// TODO, reduce fraction using gcf, rather than just this.
		if (numerator % denominator == 0) {
			numerator = numerator/denominator;
			denominator = 1;
		}

		expmod_num = numerator;
		expmod_denom = denominator;
		sprintf(buf, "Experience multiplier set to %s.\r\n", expmod_multiplier());
		send_to_char(buf, ch);
	} else if (!str_prefix(buf, "default")) {
		argument = one_argument(argument, buf);

		if (buf[0] == '\0') {
			send_to_char("Please specify \"level\" or \"all\".\r\n", ch);
			return;
		}

		int duration = -1;

		if (argument[0] != '\0') {
			if ((duration = atoi(argument)) <= 0) {
				send_to_char("Either don't specify a duration for forever, or specify a positive duration in minutes.\r\n", ch);
				return;
			}
		}

		if (!str_cmp(buf, "level")) {
			send_to_char("Enabling leveller experience mod.\r\n", ch);
			expmod_max_level = LEVEL_HERO-1;
			expmod_num = 5;
			expmod_denom = 4;
		} else if (!str_prefix(buf, "all")) {
			send_to_char("Enabling global 2x experience mod.\r\n", ch);
			expmod_max_level = LEVEL_HERO;
			expmod_num = 2;
			expmod_denom = 1;
		} else {
			do_expmod(ch, "default");
			return;
		}

		if (duration > 0) {
			sprintf(buf, "This will last for %d minutes.\r\n", duration);
			send_to_char(buf, ch);
		}

		expmod_min_level = 1;
		expmod_timer = duration;

	} else if (!str_prefix(buf, "status")) {
		sprintf(buf,
			"Experience modifier status:\r\n"
			" Multiplier:  %s\r\n"
			" Level range: %s\r\n"
			" Duration:    %s\r\n"
			" Enabled:     %s\r\n",
			expmod_multiplier(), expmod_level_range(),
			expmod_duration(), expmod_is_enabled() ? "yes" : "no");
		send_to_char(buf, ch);
	} else
		do_expmod(ch, "");

	// Change enabled experience modifier.
	if (!pre_change && expmod_is_enabled()) {
		sprintf(buf, "An experience modifier of %s has been enabled for levels %s lasting %s!",
			expmod_multiplier(), expmod_level_range(), expmod_duration());
		do_announce(ch, buf);
	} else if (pre_change) {
		if (expmod_is_enabled()) {
			sprintf(buf, "Experience modifier changed to %s for levels %s, lasting %s.",
				expmod_multiplier(), expmod_level_range(), expmod_duration());
			do_announce(ch, buf);
		} else
			do_announce(ch, "Experience modifier returned to normal.");
	}

	expmod_save();
}

void expmod_update() {
	if (!expmod_is_enabled()) {
		expmod_last_update = current_time;
		return;
	}
	if (expmod_timer > 0 && current_time - expmod_last_update >= 60) {
		expmod_last_update = current_time;
		if (--expmod_timer == 0) {
//			do_announce(NULL, "Experience modifier returned to normal.");
			do_expmod(NULL, "default level");
		}
		expmod_save();
	}
}
