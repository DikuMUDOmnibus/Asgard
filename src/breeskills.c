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
#include "magic.h"
#include "interp.h"

DECLARE_DO_FUN(do_help);
DECLARE_DO_FUN(do_say);
DECLARE_DO_FUN(do_handle);
DECLARE_DO_FUN(do_disarm);

int focus_hit(CHAR_DATA *ch);
void disarm(CHAR_DATA * ch, CHAR_DATA * victim);

/********************************************************************
 * Name: Coule                                                       *
 * Class: Swashbuckler                                               *
 * Type: Active                                                      *
 * Damage Range:                                                     *
 * Number Of Attacks: 1                                              *
 * Affected By: Hitroll                                              *
 * Extra Affects: Chance To Disarm or Blind                          *
 * Suggested Gain Cost: 3 Creation Points                            *
 * Cost To Use: 50 Movement                                          *
 * Explaination: An attack or feint that slides along the opponent's * 
 * Blade. In performing a sliding action along the opponent’s blade  *
 *********************************************************************/

void do_coule(CHAR_DATA * ch, char *argument) {
	CHAR_DATA *victim;
	OBJ_DATA *wield;
	int hroll = 0;
	int hbonus = 0;
	int dam = 0;
	int chance = 0;

	/* Error message if players try to use it but don't have it. */
	if (get_skill(ch, gsn_coule) == 0 || (!IS_NPC (ch) && ch->level < skill_table[gsn_coule].skill_level[ch->class])) {
		send_to_char("Preforming a Coule requires more skill than you have.\n\r", ch);
		return;
	}

	/* The usual checks ... */

	/* Need to be fighting to use the skill as it requires sliding past a weapon */
	if ((victim = ch->fighting) == NULL) {
		send_to_char("You aren't fighting anyone.\n\r", ch);
		return;
	}

	/* Of course you must have a weapon of your own... */
	if ((wield = get_eq_char(ch, WEAR_WIELD)) == NULL) {
		send_to_char("You need to wield a primary weapon to preform a coule!.\n\r", ch);
		return;
	}

	/* And it must be a sword or spear.... */
	if (wield->value[0] != WEAPON_SWORD && wield->value[0] != WEAPON_SPEAR) {
		send_to_char("You must be wielding a sword or spear to preform this manuever.\n\r", ch);
		return;
	}
	
	/* Since it is a delicate and quick strike, no two handers either */
	if (IS_WEAPON_STAT(wield, WEAPON_TWO_HANDS)) {
		send_to_char("Two-handed weapons are too clumsy to preform a coule!\n\r", ch);
		return;
	}

	/* Fear check */
	if (IS_AFFECTED(ch, AFF_FEAR) && (number_percent() <= 10)) {
		act("You attempt to preform a coule, but begin crying instead.", ch,
			NULL, NULL, TO_CHAR);
		act("$n sneaks attempts to perform a coule, but begins crying instead.",
			ch, NULL, NULL, TO_ROOM);
		WAIT_STATE(ch, DEF_FEAR_WAIT);
		return;
	}

	/* Stun check */
	if (ch->stunned) {
		send_to_char("You're still a little woozy.\n\r", ch);
		return;
	}

	/* Ok lets get to the chance to hit and damage calculations */
	chance += get_curr_stat(ch, STAT_DEX);
	chance += get_curr_stat(ch, STAT_INT);
	chance += ch->hitroll / 3.5;
	if (IS_AFFECTED(ch, AFF_HASTE))
		chance += 10;
	if (IS_AFFECTED(victim, AFF_HASTE))
		chance -= 5;

	/* Check to see if they fail now that we have the chance to strike calculations */
	if (number_percent() > chance) {
		act("$n tries to perform a quick coule but trips instead!", ch, 0, 0, TO_ROOM);
		send_to_char("You try to perform a quick coule but end up tripping instead!\n\r", ch);
		/* Subtract half the normal cost to use the skill since we failed */
		ch->move -= 25;
		check_improve(ch, gsn_coule, FALSE, 2);
		WAIT_STATE(ch, 12);
		return;
	}

	/* Win! Hit the message now...*/
	act("$n performs a {yli{Yghtni{yng{x fast coule!", ch, 0, 0, TO_ROOM);
	send_to_char("You perform a {yli{Yghtni{yng{x fast coule!\n\r", ch);
	/* subtract the cost of the manuever */
	ch->move -= 50;
	check_improve(ch, gsn_coule, TRUE, 2);

	/* Moving right along to the damage calculations and hitroll bonus...  */

	/* Swashbuckler bonus is based on hitroll obviously..*/
	hroll = ch->hitroll;

	if ((hroll) >= 200)
		hbonus = hroll / 4;

	if ((hroll) >= 300)
		hbonus = hroll / 3.5;

	dam += hbonus;
	dam += GET_HITROLL(ch);
	dam *= ch->pcdata->learned[gsn_coule];
	dam /= 12;

	if (number_range(25, 200) > ch->hitroll / 1.5) {
		disarm(ch, victim);
		damage(ch, victim, dam, gsn_coule, DAM_SLASH, TRUE, 0);
		check_improve(ch, gsn_coule, FALSE, 1);
		WAIT_STATE(ch, skill_table[gsn_coule].beats);
	} else {
		damage(ch, victim, dam, gsn_coule, DAM_SLASH, TRUE, 0);
		check_improve(ch, gsn_coule, FALSE, 1);
		WAIT_STATE(ch, skill_table[gsn_coule].beats);
		return;
	}
}

/*************************************************************
 * Name: 
 * Class: Necromancer                                        *
 * Type:
 * Damage:
 * Number Of Attacks:
 * Affected By:
 * Extra Affects:
 * Suggested Gain Cost:
 * Cost To Use:
 * Explaination: 
 * 
 *************************************************************/

