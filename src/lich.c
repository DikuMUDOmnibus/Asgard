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

int focus_level(long total);
int cast_level;


void do_slash(CHAR_DATA * ch, char *argument) {
        CHAR_DATA *victim;
        OBJ_DATA *wield;		
        int slashdbase = 0;
        int slashbonus = 0;
        int slashdam = 0;
        int chance = 0;

        if (get_skill(ch, gsn_slash) == 0 || (!IS_NPC (ch) && ch->level < skill_table[gsn_slash].skill_level[ch->class])) {
                send_to_char("You are not skilled enough to perform that strike.\n\r", ch);
                return;
        }

        if ((victim = ch->fighting) == NULL) {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }

        if ((wield = get_eq_char(ch, WEAR_WIELD)) == NULL) {
                send_to_char("You are not wielding a weapon.\n\r", ch);
                return;
        }

        //sword req
        if (wield->value[0] != WEAPON_SWORD) {
                send_to_char("You need a sword to slash!n\r", ch);
                return;
        }
        
        //stun
        if (ch->stunned) {
                send_to_char("You're still a little woozy.\n\r", ch);
                return;
        }

	//no moves!
                if (ch->move < 50) {
                send_to_char("You're too tired.\n\r", ch);
                return;
        }

       /* chance to hit, dex/10 is +50 bonus at 500 + 20 from dex sets       up 70% chance to hit?  */
        chance += get_curr_stat(ch, STAT_DEX);
        chance = (chance + ch->hitroll) / 5; 
        if (IS_AFFECTED(ch, AFF_HASTE))
                chance += 5;
        if (IS_AFFECTED(victim, AFF_HASTE))
                chance -= 5;
//messaging for whiff
        if (number_percent() > chance) {
                act("$n thrashes about wildly hitting nothing but air!", ch, 0, 0, TO_ROOM);
                send_to_char("You flail about missing your intended target!\n\r", ch);

                ch->move -= 25;
                check_improve(ch, gsn_slash, FALSE, 2);
                WAIT_STATE(ch, 12);
                return;
        }

        //messaging for hit check 
        act("$n slashes their target with a quick strike!", ch, 0, 0, TO_ROOM); 
        send_to_char("You slash your opponent with a quick strike!\n\r", ch);
       
        ch->move -= 50;
        check_improve(ch, gsn_slash, TRUE, 2);

        /* damage…ot oh this probably needs some work */

       
       	  slashdbase = ch->damroll;
 	  
/* hm I think this works better at 1200 droll doing 1500 damage befor hroll and random bonus, 800 droll would be 1000 damage, 2400 droll for 3k damage */
	
	  //slashbonus = (slashbonus + slashdbase) / 1200 * 1500;
	  slashbonus = (slashbonus + slashdbase) / 4 * 5;
	

/* slight bonus factor for hroll plus last 2 lines with a % multiplier between 75-115% to make damage more random*/

        slashdam += slashbonus;
        slashdam = slashdam + ( ch->hitroll / 5); 
        slashdam = (slashdam * number_percent()) / 10;  

	

        damage(ch, victim, slashdam, gsn_slash, DAM_SLASH, TRUE, 0);
               check_improve(ch, gsn_slash, FALSE, 1);
                WAIT_STATE(ch, skill_table[gsn_slash].beats);

	AFFECT_DATA af;

				af.where = TO_AFFECTS;
				af.type = gsn_slash;
				af.level = ch->level;			
			
			//attempt to remove stacking
			if (IS_AFFECTED(victim,AFF_REGENERATION))
			{ 
				AFFECT_DATA *paf, *paf_next;
			for(paf = victim->affected; paf != NULL; paf = paf_next)
				{
					paf_next = paf->next;
				if(paf->type == gsn_slash)
				{
					affect_remove (victim, paf);
				}
			}
			}
/* Affects set based on weapon type, i changed it so both impact regen if your wielding a poisoned weapon they are likely already poisoned from the flag new affect gives great penalty for a poison flag over flaming flaming -101 regen poison -135 regen combined -235 regen over 4 ticks */


	
				if (IS_WEAPON_STAT(wield, WEAPON_FLAMING))
				{
				af.bitvector = AFF_REGENERATION;
				af.duration = af.level / 25; //4 ticks
				af.modifier = (ch->level+tier_spell_bonus(ch)*3 /2) / -1;//-100 regen + 1.5x spell bonus level
				af.location = APPLY_REGEN;
				affect_join(victim, &af);
				send_to_char("You cry out in anguish as a burning blade sears your wounds.\n\r", victim);
				act("$n screams in anguish as $p sears $m.", victim, wield, NULL, TO_ROOM);
				}

		if (IS_WEAPON_STAT(wield, WEAPON_POISON))
				{
				af.bitvector = AFF_REGENERATION;
				af.duration = af.level / 25; //4 ticks
				af.modifier = (ch->level+tier_spell_bonus(ch)*2) * -1;//-100 regen + 2x spell bonu level
				af.location = APPLY_REGEN;
				affect_join(victim, &af);
				send_to_char("You cringe in pain as a poisoned blade slices your body.\n\r", victim);
				act("$n cringes as $p poisons $m.", victim, wield, NULL, TO_ROOM);

				}
                return;
                     
        }


//minion summon skill for fades
void spell_minion(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
	char buf[MAX_STRING_LENGTH];
	MOB_INDEX_DATA *pMobIndex;
	
	if (ch->pet != NULL)
	{
		send_to_char("You failed.\n\r", ch);
		return;
	}
if (ch->class == CLASS_FADE)
{

	cast_level = ch->level+tier_spell_bonus(ch);
		CHAR_DATA *pet;

//level check for type of pet

	if (cast_level > 184)
{
		pMobIndex = get_mob_index(3328);
		pet = create_mobile(pMobIndex);
	}
	
	
	else if (cast_level > 159)
{
		pMobIndex = get_mob_index(3327);
		pet = create_mobile(pMobIndex);
	}

	else if (cast_level > 144)
{
		pMobIndex = get_mob_index(3326);
		pet = create_mobile(pMobIndex);
	}
	
	else if (cast_level > 129)
{
		pMobIndex = get_mob_index(3325);
		pet = create_mobile(pMobIndex);
	}
	
	else if (cast_level > 114)
{
		pMobIndex = get_mob_index(3324);
		pet = create_mobile(pMobIndex);
	}

	else
{
		pMobIndex = get_mob_index(3323);
		pet = create_mobile(pMobIndex);
	}
	




		if (!IS_SET(pet->act, ACT_PET))
			SET_BIT(pet->act, ACT_PET);
		if (!IS_SET(pet->affected_by, AFF_CHARM))
			SET_BIT(pet->affected_by, AFF_CHARM);
		pet->comm = COMM_NOTELL | COMM_NOSHOUT | COMM_NOCHANNELS;
		sprintf(buf, "%s{GThe mark of %s is on it's forehead.{x.\n\r",
		pet->description, ch->name);
		free_string(pet->description);
		pet->description = str_dup(buf);
		char_to_room(pet, ch->in_room);
		add_follower(pet, ch);
		pet->leader = ch;
		ch->pet = pet;
		pet->alignment = ch->alignment; 
		}
	return;	
	}


