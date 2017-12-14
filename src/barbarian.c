/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,	   *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *									   *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael	   *
 *  Chastain, Michael Quan, and Mitchell Tse.				   *
 *									   *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc	   *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.						   *
 *									   *
 *  Much time and thought has gone into this software and you are	   *
 *  benefitting.  We hope that you share your changes too.  What goes	   *
 *  around, comes around.						   *
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
#if defined( macintosh )
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <errno.h>		
#include <unistd.h>		
#include <sys/time.h>
#endif
#include <ctype.h>		
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "merc.h"
#include "newclan.h"
#include "magic.h"




void do_fury(CHAR_DATA *ch, char *argument)
{
	AFFECT_DATA af;

	if ((get_skill(ch, gsn_fury) <= 0) || (ch->level
			< skill_table[gsn_fury].skill_level[ch->class]))
	{
		send_to_char("You are not a true barbarian.\n\r", ch);
		return;
	}

	if (is_affected(ch, gsn_fury) || is_affected(ch, gsn_berserk))
	{
		send_to_char("{RYou are already in a rage.{x\n\r", ch);
		return;
	}

	if (ch->mana < 100 || ch->move < 100)
	{
		send_to_char("{WYou are inner fire is too weak to fly into a fury.{x\n\r",
				ch);
		return;
	}

	if (number_percent() > get_skill(ch, gsn_fury))
	{
		send_to_char(
				"{YYou let out a weak howl as nothing seems to happen.{x\n\r",
				ch);
		check_improve(ch, gsn_fury, FALSE, 1);
		ch->mana -= 50;
		ch->move -= 50;
		return;
	}

	af.where = TO_AFFECTS;
	af.type = gsn_fury;
	af.level = (ch->level + tier_level_bonus(ch));
	af.location = APPLY_HITROLL;
	af.modifier = (ch->level + (tier_level_bonus(ch)*1.5));
	af.duration = (ch->level + tier_level_bonus(ch)) / 10;
	af.bitvector = 0;
	affect_to_char(ch, &af);
	af.location = APPLY_DAMROLL;
	affect_to_char(ch, &af);

	
	af.modifier = UMAX(10, 10 + tier_level_bonus(ch));
	af.location = APPLY_REGEN;	
	affect_to_char(ch, &af);

	af.modifier = UMAX (10, ((ch->level + tier_level_bonus(ch)) * 3 / 4));
	af.location = APPLY_AC;
	affect_to_char (ch, &af);

	ch->mana -= 100;
	ch->move -= 100;

	act("{R$n roars loudly and flies into a fit of rage.{x", ch, NULL, NULL, TO_ROOM);
	send_to_char("{RYou unleash a loud roar as your fly into a fit of rage.{x\n\r", ch);
	check_improve(ch, gsn_fury, TRUE, 1);
	return;
}

void do_whirlwind(CHAR_DATA * ch, char *argument) {
       	CHAR_DATA *vch, *vch_next;
        OBJ_DATA *wield;		
        int dam_type = 0;
	int whirld = 0;
        int chance = 0;
	OBJ_DATA *wobj = get_eq_char(ch, WEAR_WIELD);
	
        if (get_skill(ch, gsn_whirlwind) == 0 || (!IS_NPC (ch) && ch->level < skill_table[gsn_whirlwind].skill_level[ch->class])) {
                send_to_char("You are not skilled enough to perform that attack.\n\r", ch);
                return;
 }

        if ((wield = get_eq_char(ch, WEAR_WIELD)) == NULL) {
                send_to_char("You are not wielding a weapon.\n\r", ch);
                return;
        }
        
        //stun
        if (ch->stunned) {
                send_to_char("You're still a little woozy.\n\r", ch);
                return;
        }

	//no moves!
        if (ch->move < 100) {
                send_to_char("You're too tired.\n\r", ch);
                return;
        }


	if (IS_AFFECTED(ch,AFF_FEAR) && (number_percent() <= 50))
	{
		act("You go to whirlwind, but begin crying instead.", ch, NULL, NULL,
				TO_CHAR);
		act("$n goes to whirlwind, but begins crying instead.", ch, NULL, NULL,
				TO_ROOM);
		WAIT_STATE(ch,DEF_FEAR_WAIT);
		return;}

       /* hitroll over 10 is around 70% chance to land around 500 hitroll w/str in  */
        chance += get_curr_stat(ch, STAT_STR);
        chance = chance + (ch->hitroll / 10); 
        if (IS_AFFECTED(ch, AFF_HASTE))
                chance += 5;

	//messaging for whiff
        if (number_percent() > chance) {
                act("$n spins in a circle missing all targets!", ch, 0, 0, TO_ROOM);
                send_to_char("You whirl about in a circle missing all targets!\n\r", ch);

                ch->move -= 50;
                check_improve(ch, gsn_whirlwind, FALSE, 2);
                WAIT_STATE(ch, 12);
                return;
        }

        //messaging for hit 
        act("$n whirls about attacking all targets!", ch, 0, 0, TO_ROOM); 
        send_to_char("You whirl about attacking your foes!\n\r", ch);
       
        ch->move -= 100;
        check_improve(ch, gsn_whirlwind, TRUE, 2);
	dam_type = attack_table[wield->value[3]].damage;	
	WAIT_STATE(ch, 12);



//start loop to check pcs/mobs in room
for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
	{
		vch_next = vch->next_in_room;
		
		//elimates grouped chars/pks and self from damage
		if ((is_safe_spell(ch, vch, TRUE) || (IS_NPC(vch) && IS_NPC(ch))
				|| (is_same_group(ch, vch))))
		continue;
	

		
	if (IS_WEAPON_STAT(wield, WEAPON_TWO_HANDS)) {	

//attack 1
		whirld = dice(wobj->value[1], wobj->value[2]) + ch->damroll;		
		whirld = whirld * 5;
		damage(ch, vch, whirld, gsn_whirlwind, dam_type, TRUE, 0);
//attack 2		
		whirld = dice(wobj->value[1], wobj->value[2]) + ch->damroll;		
		whirld = whirld * 7 / 2;
		damage(ch, vch, whirld, gsn_whirlwind, dam_type, TRUE, 0);
	}
	else
	{ 

//attack 1
		whirld = dice(wobj->value[1], wobj->value[2]) + ch->damroll;		
		whirld = whirld * 7 / 4;
		damage(ch, vch, whirld, gsn_whirlwind, dam_type, TRUE, 0);
//attack 2
		whirld = dice(wobj->value[1], wobj->value[2]) + ch->damroll;		
		whirld = whirld * 7 / 4;
		damage(ch, vch, whirld, gsn_whirlwind, dam_type, TRUE, 0);
//attack 3
		whirld = dice(wobj->value[1], wobj->value[2]) + ch->damroll;		
		whirld = whirld * 7 / 4;
		damage(ch, vch, whirld, gsn_whirlwind, dam_type, TRUE, 0);


	
	}			
			
	
	}
	
	
	
                return;
                     
        }

void do_hammerblow(CHAR_DATA * ch, char *argument) {
       	CHAR_DATA *vch;
        OBJ_DATA *wield;		
        int dam_type = 0;
	int hammer = 0;
        int chance = 0;
	OBJ_DATA *wobj = get_eq_char(ch, WEAR_WIELD);	

        if (get_skill(ch, gsn_hammerblow) == 0 || (!IS_NPC (ch) && ch->level < skill_table[gsn_hammerblow].skill_level[ch->class])) {
                send_to_char("You are not skilled enough to perform that attack.\n\r", ch);
                return;
 }

        if ((vch = ch->fighting) == NULL) {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }

        if ((wield = get_eq_char(ch, WEAR_WIELD)) == NULL) {
                send_to_char("You are not wielding a weapon.\n\r", ch);
                return;
        }
        
        //stun
        if (ch->stunned) {
                send_to_char("You're still a little woozy.\n\r", ch);
                return;
        }

       if (!can_see (ch, vch)) {
                send_to_char("Land a hammerblow blind?  No way.\n\r", ch);
                return;
        }

	//no moves!
        if (ch->move < 50) {
                send_to_char("You're too tired.\n\r", ch);
                return;
        }

if (IS_WEAPON_STAT(wield, WEAPON_TWO_HANDS))
	{
       /* hitroll over 10 is around 70% chance to land around 500 hitroll w/str in  */
        chance += get_curr_stat(ch, STAT_STR);
        chance = chance + (ch->hitroll / 10);


	if (number_percent() > chance) {
                act("$n takes a massive swing completely missing the intended target!", ch, 0, 0, TO_ROOM);
                send_to_char("You slam your weapon into the ground completely missing your target!\n\r", ch);

                ch->move -= 50;
                check_improve(ch, gsn_hammerblow, FALSE, 2);
                WAIT_STATE(ch, 26);  //big swing big penalty for a miss
                return;
        }

        //messaging for hit 
        act("$n takes an overhanded swing blasting their target!", ch, 0, 0, TO_ROOM); 
        send_to_char("You lift your weapon overhead and swing down in a massive blow!\n\r", ch);
       
        ch->move -= 50;
        check_improve(ch, gsn_hammerblow, TRUE, 2);
	dam_type = attack_table[wield->value[3]].damage;	
	WAIT_STATE(ch, 22); //massive swing massive recovery


		hammer = dice(wobj->value[1], wobj->value[2]) + ch->damroll;		
		hammer = hammer * 10;
		damage(ch, vch, hammer, gsn_hammerblow, dam_type, TRUE, 0);

	//checks for chance to stun opponent

			if ( number_percent() < 6)
				{			
				vch->stunned = 2;
				DAZE_STATE (vch, PULSE_VIOLENCE);
				vch->position = POS_RESTING;
				act ("{)You are dazed from the massive hammerblow!{x",
				ch, NULL, vch, TO_VICT);
				act ("{.$N is stunned by your hammerblow!{x", ch, NULL, vch, TO_CHAR);
				act ("{($N is knocked to the ground and having trouble getting back up.{x",
					ch, NULL, vch, TO_NOTVICT);
				}
			else if ( number_percent() > 5 && number_percent() < 16)
				{
				act ("{)$n sends you sprawling with a massive hammberblow!{x",
				ch, NULL, vch, TO_VICT);
				act ("{.You slam into $N, and knocking $M to the ground!{x", ch, NULL, vch, 					TO_CHAR);
				act ("{($n knocks $N to the ground with a massive hammberblow!.{x",
				ch, NULL, vch, TO_NOTVICT);

				DAZE_STATE (vch, PULSE_VIOLENCE);
				vch->position = POS_RESTING;				
				}
			else
				return;
	}
	else
	{
		                send_to_char("You need a two-handed weapon.\n\r", ch);
                return;
        }
	return;
}

