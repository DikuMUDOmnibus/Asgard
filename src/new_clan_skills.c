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

int focus_dam(CHAR_DATA *ch);

/* New Swashbuckler Skill
 *Head Cut by Bree
 */

void do_head_cut(CHAR_DATA *ch, char *argument)
{
	CHAR_DATA *victim;
	int chance, dam;
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *wield, *second;
	int multiplier;
	bool DUAL = FALSE;
	int hroll = 0;
	dam = 0;
	AFFECT_DATA af;
	one_argument(argument, arg);

	if ((chance = get_skill(ch, gsn_head_cut)) == 0 || !has_skill(ch,
			gsn_head_cut))
	{
		send_to_char("You do not know how to head cut\n\r", ch);
		return;
	}

	if (arg[0] == '\0')
	{
		victim = ch->fighting;

		if (victim == NULL)
		{
			send_to_char("But you aren't fighting anyone!\n\r", ch);
			return;
		}
	}
	else if ((victim = get_char_room(ch, arg)) == NULL)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if (victim == ch)
	{
		send_to_char("Cut at your own head? Cute.\n\r", ch);
		return;
	}

	if (IS_AFFECTED(ch,AFF_FEAR) && (number_percent() <= 25))
	{
		act("You attempt a head cut, but begin crying instead.", ch, NULL,
				NULL, TO_CHAR);
		act("$n attempts a head cut, but begins crying instead.", ch, NULL,
				NULL, TO_ROOM);
		WAIT_STATE(ch,DEF_FEAR_WAIT)
;		return;
	}

	if (ch->stunned)
	{
		send_to_char("You are still a bit woozy.\n\r",ch);
		return;
	}

	wield = get_eq_char(ch,WEAR_WIELD);
	second = get_eq_char(ch,WEAR_SECONDARY);

	if ((wield == NULL)
			|| (( wield->value[0] != WEAPON_SWORD)
					&& ( wield->value[0] != WEAPON_AXE)
					&& ( wield->value[0] != WEAPON_DAGGER) ))
	{
		if ((second == NULL)
				|| (( second->value[0] != WEAPON_SWORD)
						&& ( second->value[0] != WEAPON_AXE)
						&& ( second->value[0] != WEAPON_DAGGER) ))
		{
			send_to_char("You must be wielding a sword or axe or dagger to attempt a head cut.\n\r",ch);
			return;
		}
		else
		{
			DUAL = TRUE;
		}
	}

	if (is_safe(ch,victim) )
	return;

	chance += ch->carry_weight/25;
	chance -= victim->carry_weight/20;
	chance += (ch->size - victim->size)*20;
	chance -= get_curr_stat(victim,STAT_DEX);
	chance += get_curr_stat(ch,STAT_STR)/3;
	chance += get_curr_stat(ch,STAT_DEX)/2;

	if (IS_AFFECTED(ch,AFF_HASTE) )
	chance += 10;

	if (IS_AFFECTED(victim,AFF_HASTE) )
	chance -= 15;

	chance += (ch->level - victim->level);

	act("$n attempts a cut at $N's head!",ch,0,victim,TO_NOTVICT);
	act("You attempt a head cut on $N",ch,0,victim,TO_CHAR);
	act("$n attempts to cut at your head!",ch,0,victim,TO_VICT);

	if (number_percent() < chance)
	{
		check_improve(ch,gsn_head_cut,TRUE,1);
		WAIT_STATE(ch,skill_table[gsn_head_cut].beats + 4);

		if (DUAL)
		wield = second;

		if (wield->pIndexData->new_format)
		dam = dice(wield->value[1],wield->value[2]);
		else
		dam = number_range(wield->value[1],wield->value[2]);

/*
		if (get_skill(ch,g sn_enhanced_damage) > 0 )
		{
			if (number_percent() <= get_skill(ch,g sn_enhanced_damage) )
			{
				check_improve(ch,g sn_enhanced_damage,TRUE,1);
				dam += dam * (number_range(25,75)/100) * ch->pcdata->learned[g sn_enhanced_damage]/100;
			}
		}
*/

		dam += GET_DAMROLL(ch)/2;
		dam *= ch->pcdata->learned[gsn_head_cut];
		dam /= 100;
		multiplier = number_range(ch->level/3, ch->level/2);
		multiplier /= 10;
		multiplier += 5/4;
		dam *= multiplier;

		/*    spell_blind(gsn_blind, 2 * level / 6, ch, (void *) victim,TARGET_CHAR); */

		if (!DUAL && IS_WEAPON_STAT(wield,WEAPON_TWO_HANDS))
		dam *= 1.25;

		hroll = ch->hitroll;

		/* if ((hroll) >= 250)
		 dam += dam*2;

		 if ((hroll) >= 350)
		 dam += dam*4/2.3; -took out for balancing issues Dusk- */

		if (!IS_NPC(victim))
		dam = dam*2/3;

		if (dam <= 0)
		dam = 1;

		damage(ch,victim,dam,gsn_head_cut,DAM_PIERCE,TRUE,0);
	}
	else if (number_range(1, 10) >= 8)
	{
		af.where = TO_AFFECTS;
		af.type = gsn_head_cut;
		af.level = ch->level;
		af.bitvector = AFF_BLIND;
		af.duration = 1;
		af.modifier = -4;
		af.location = APPLY_DEX;

		affect_to_char( victim,&af);

		act( "$n scores a cut above the eye and {Rblood{x starts to pour into their eyes!", victim, NULL, NULL, TO_ROOM );
		damage(ch,victim,dam,gsn_head_cut,DAM_PIERCE,TRUE,0);
		check_improve(ch,gsn_head_cut,FALSE,1);
		WAIT_STATE(ch,skill_table[gsn_head_cut].beats + 4);
	}
	else
	{
		damage(ch,victim,dam,gsn_head_cut,DAM_PIERCE,TRUE,0);
		check_improve(ch,gsn_head_cut,FALSE,1);
		WAIT_STATE(ch,skill_table[gsn_head_cut].beats + 4);
	}
	return;
}

void do_grenado(CHAR_DATA *ch, char *argument)
{
	CHAR_DATA *victim;
	int chance, dam;
	char arg[MAX_INPUT_LENGTH];
	int multiplier = 3;
	//int hroll = 0;
	dam = 0;
	AFFECT_DATA af;

	one_argument(argument, arg);

	if ((chance = get_skill(ch, gsn_grenado)) == 0 || !has_skill(ch,
			gsn_grenado))
	{
		send_to_char("You do not know how to make or use a grenado!\n\r", ch);
		return;
	}

	if (arg[0] == '\0')
	{
		victim = ch->fighting;

		if (victim == NULL)
		{
			send_to_char("But you aren't fighting anyone!\n\r", ch);
			return;
		}
	}
	else if ((victim = get_char_room(ch, arg)) == NULL)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if (victim == ch)
	{
		send_to_char("You can't grenado yourself!!\n\r", ch);
		return;
	}

	if (IS_AFFECTED(ch,AFF_FEAR) && (number_percent() <= 10))
	{
		act("You try to toss a grenado, but begin crying instead.", ch, NULL,
				NULL, TO_CHAR);
		act("$n tries to toss a grenado, but begins crying instead.", ch, NULL,
				NULL, TO_ROOM);
		WAIT_STATE(ch,DEF_FEAR_WAIT)
;		return;}

	if(ch->stunned)
	{
		send_to_char("You are still a bit woozy.\n\r",ch);
		return;
	}

	if (is_safe(ch,victim) )
	return;

	// Hitroll bonus..?
	// We may need to check by NPC ... 
//	if IS_NPC(victim)
//		chance += ((ch->hitroll / 20) - (victim->hitroll / 40));
//	else
		chance += ((ch->hitroll / 20) - (victim->hitroll / 40));

	// 20 dex vs 25? lowers chance by 35 (or increases!)!
	chance += (get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX))*7;
	// giant vs tiny? -24% chance!?! tiny vs giant? +24%!
	chance += (victim->size - ch->size) * 4;
	chance += get_curr_stat(ch,STAT_STR)/3;
	if IS_NPC(victim) //Silly mobs with stats that aren't set!
		chance += 15;

	// This is mostly taken care of in DEX comparison.
	if (IS_AFFECTED(ch,AFF_HASTE) )
		chance += 10;
	if (IS_AFFECTED(victim,AFF_HASTE) )
		chance -= 10;

	if IS_NPC(victim)
		chance += (ch->level - victim->level)/20;
	else
		chance += (ch->level - victim->level)/2;
	
	if (ch->class == CLASS_ALCHEMIST)
		chance += 10;

	act("$n tosses a {gg{Gr{Rena{Gd{go{x at $N!",ch,0,victim,TO_NOTVICT);
	act("You toss a {gg{Gr{Rena{Gd{go{x at $N",ch,0,victim,TO_CHAR);
	act("$n tosses a {gg{Gr{Rena{Gd{go{x at you!",ch,0,victim,TO_VICT);

	if (number_percent() < chance)
	{
		check_improve(ch,gsn_grenado,TRUE,1);
		WAIT_STATE(ch,skill_table[gsn_grenado].beats + 8);

		dam = (ch->level * 2 * ch->pcdata->learned[gsn_grenado] / 120) + 10;

/*
		if (get_skill(ch,g sn_enhanced_damage) > 0 )
		{
			if (number_percent() <= get_skill(ch,g sn_enhanced_damage) )
			{
				check_improve(ch,g sn_enhanced_damage,TRUE,1);
				dam *= ch->pcdata->learned[g sn_enhanced_damage]/100;
			}
		}
*/

		multiplier =  number_range((chance/2),chance) / 8; 

		if (multiplier > 15)
			multiplier -= 1;
		if (multiplier > 17)
			multiplier -= 1;	
		if (multiplier > 19)
			multiplier -= 1;
		if (multiplier > 21)
			multiplier -= 1;
		if (multiplier > 22)
			multiplier -= 1;

		if (multiplier < 6) {
			act("$N dives to the side avoiding the main blast!",ch,0,victim,TO_CHAR);
			act("$N dives to the side, avoiding the main blast of $n's grenado!",ch,0,victim,TO_NOTVICT);
			act("You dive to the side, avoiding the main blast of $n's grenado!",ch,0,victim,TO_VICT);
			dam *= (multiplier / 2);
		}
		else if (multiplier > 14) {
			act("$N tries to dodge, but moves directly into the grenado's path! They're blind!",ch,0,victim,TO_NOTVICT);
			act("$N walks straight into your grenado! They're blind!",ch,0,victim,TO_CHAR);
			act("You attempt to dodge, but walk directly into $n's grenado! You're blind!",ch,0,victim,TO_VICT);
			dam *= (multiplier * 6/5);
			
			af.where = TO_AFFECTS;
			af.type = gsn_grenado;
			af.level = ch->level;
			af.bitvector = AFF_BLIND;
			af.duration = 1;
			af.modifier = -3;
			af.location = APPLY_DEX;
			affect_to_char( victim, &af);
		}
		else {
			if (number_range(1, 4) == 1) {
				af.where = TO_AFFECTS;
				af.type = gsn_grenado;
				af.level = ch->level;
				af.bitvector = AFF_BLIND;
				af.duration = 0;
				af.modifier = -2;
				af.location = APPLY_DEX;
				affect_to_char( victim,&af );
			}
			dam *= multiplier;
		}
		act( "$n's {gg{Gr{Rena{Gd{go{x explodes!", victim, NULL, NULL, TO_ROOM );
		damage(ch,victim,dam,gsn_grenado,DAM_SOUND,TRUE,0);
		check_improve(ch,gsn_grenado,FALSE,1);
		WAIT_STATE(ch,skill_table[gsn_grenado].beats + 10);
	}
	else
	{
		damage(ch,victim,0,gsn_grenado,DAM_SOUND,TRUE,0);
		check_improve(ch,gsn_grenado,FALSE,1);
		WAIT_STATE(ch,skill_table[gsn_grenado].beats + 10);
	}
	if (!IS_SET(ch->in_room->affected_by,ROOM_AFF_FAERIE))
		if ((number_percent() > 80) || ((number_percent() > 70) && (ch->class == CLASS_ALCHEMIST))) {
			af.where = TO_ROOM_AFF;
			af.type = gsn_faerie_fog;
			af.duration = multiplier/3;
			af.location = APPLY_NONE;
			af.level = chance;
			af.bitvector = ROOM_AFF_FAERIE;
			affect_to_room(ch->in_room, &af);
			act("{MA cloud of purple smoke spreads across the room.", ch, NULL, NULL, TO_ROOM);
			act("{MA cloud of purple smoke spreads from the grenado.",ch,NULL,NULL,TO_CHAR);
		} 

	return;
}

/* Holy Aura spell by Bree.
 This spell gives a Crusader roughly -38 ac at
 101 by praying to their god for protection. */

void spell_holy_aura(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;

	if (is_affected(ch, sn))
	{
		if (victim == ch)
			send_to_char("You are already protected.\n\r", ch);
		else
			act("$N is already protected.", ch, NULL, victim, TO_CHAR);
		return;
	}

	af.where = TO_SHIELDS;
	af.type = sn;
	af.level = level;
	af.duration = level;
	af.location = APPLY_AC;
	af.modifier = -level / 4;
	af.bitvector = SHD_STONE;
	affect_to_char(victim, &af);
	act("$n is protected by a holy aura", victim, NULL, NULL, TO_ROOM );
	send_to_char("You are surrounded by a holy aura.\n\r", victim);

	return;

}

/* New Shaman Spell By Bree 
 This spell allows the Shaman to channel their
 ancestors, thus giving them more knowledge which
 translates into -ac and small hitroll */

void spell_generations(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;

	if (is_affected(ch, sn))
	{
		if (victim == ch)
			send_to_char(
					"You are already channeling the knowledge of your ancestors.\n\r",
					ch);
		else
			act("$N is already channeling the knowledge of their ancestors.",
					ch, NULL, victim, TO_CHAR);
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 30;
	af.location = APPLY_AC;
	af.modifier = UMAX( -100, -2*level ); // UMAX required in the case of mobs.
	af.bitvector = SHD_STONE;
	affect_to_char(victim, &af);

	af.location = APPLY_HITROLL;
	af.modifier = 20;
	af.duration = 30;
	affect_to_char(victim, &af);

	act("$n looks more knowledgable.", victim, NULL, NULL, TO_ROOM );
	send_to_char("You begin channeling your ancestors.\n\r", victim);

	return;
}

/* New Ninja michi/fast heal combination By Bree */
void do_inner_str(CHAR_DATA *ch, char *argument)
{
	AFFECT_DATA af;

	if ((get_skill(ch, gsn_inner_str) <= 0) || (ch->level
			< skill_table[gsn_inner_str].skill_level[ch->class]))
	{
		send_to_char("Do what now?\n\r", ch);
		return;
	}

	if (is_affected(ch, gsn_inner_str))
	{
		send_to_char("You are already channeling your inner strength\n\r", ch);
		return;
	}

	if (ch->mana < 200 || ch->move < 200)
	{
		send_to_char("You do not have enough mental power to concentrate.\n\r",
				ch);
		return;
	}

	if (number_percent() > get_skill(ch, gsn_inner_str))
	{
		send_to_char(
				"You concentrate on channeling your inner strength, but cannot acheive it.\n\r",
				ch);
		check_improve(ch, gsn_inner_str, FALSE, 1);
		ch->mana -= 100;
		ch->move -= 100;
		return;
	}

	af.where = TO_AFFECTS;
	af.type = gsn_inner_str;
	af.level = ch->level;
	af.location = APPLY_HITROLL;
	af.modifier = ch->level * get_skill(ch, gsn_inner_str) / 200;
	af.duration = ch->level / 6;
	af.bitvector = 0;
	affect_to_char(ch, &af);
	af.location = APPLY_DAMROLL;
	affect_to_char(ch, &af);

	af.location = APPLY_REGEN;
	af.modifier = 100;
	af.duration = ch->level / 6;
	af.bitvector = 0;
	affect_to_char(ch, &af);

	ch->mana -= 200;
	ch->move -= 200;

	act("$n's begins channeling their inner strength.", ch, NULL, NULL, TO_ROOM);
	send_to_char("You focus on your inner strength.\n\r", ch);
	check_improve(ch, gsn_inner_str, TRUE, 1);

	return;
}


	/* Prayer (New Saint Skill) By Bree
	 Prayer basiaclly allows for the saint to pray for
	 the intervention of the Gods. Since you know, they
	 spend most of their time praying and shit.
	 This spell will have a less chance then most of actually
	 landing. Damage is extremely spikey, from 150-2k, but
	 mostly avgs around 600-900 */

void do_prayer(CHAR_DATA *ch, char *argument)
{
	CHAR_DATA *victim;
	int dam, chance, percent;
	char arg[MAX_INPUT_LENGTH];

	if ((chance = get_skill(ch, gsn_prayer)) == 0 || !has_skill(ch, gsn_prayer))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	one_argument(argument, arg);
	if (arg[0] == '\0')
		victim = ch->fighting;
	else
		victim = get_char_room(ch, arg);

	if (victim == NULL)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}
	if (victim == ch)
	{
		send_to_char("You can't pray for the gods to smite YOU, dummy!\n\r", ch);
		return;
	}

	if (ch->stunned > 0)
	{
		send_to_char("You're still a little woozy.\n\r", ch);
		return;
	}

	if ((victim->fighting != ch) && (ch->fighting != victim))
	{
		send_to_char("But you aren't engaged in combat with them.\n\r", ch);
		return;
	}

	/*    if (get_eq_char(ch,WEAR_WIELD) != NULL ||
	 get_eq_char(ch,WEAR_SECONDARY) != NULL)
	 {
	 send_to_char("You cannot pray with items in your hands.\n\r",ch);
	 return;
	 }*/

	if (IS_NPC(victim))
	{
		chance += (ch->level - (victim->level - 12 * 2));
		chance -= get_curr_stat(victim, STAT_DEX) * 2 / 3;
	}
	else
	{
		chance += (ch->level - victim->level * 4);
		chance -= get_curr_stat(victim, STAT_DEX);
	}

	chance += get_curr_stat(ch, STAT_DEX) * 3 / 4;
	chance = URANGE(5, chance, 90);

	if (number_percent() > chance)
	{
		dam = 0;
		check_improve(ch, gsn_prayer, FALSE, 3);
	}

	else
	{

		dam = (ch->level * number_range(8, 18));

		act("$n calls down the power of the Gods upon $N!", ch, 0, victim,
				TO_NOTVICT);
		act("You pray to the Gods for help!", ch, 0, victim, TO_CHAR);
		act("$n calls down the power of the Gods to smite you!", ch, 0, victim,
				TO_VICT);

		dam += (get_curr_stat(ch, STAT_DEX) + str_app[get_curr_stat(ch,
				STAT_STR)].todam) * 8;
		check_improve(ch, gsn_prayer, TRUE, 3);
		WAIT_STATE(victim,PULSE_VIOLENCE)
;		/* A little extra kick */
		if ((percent = number_percent()) <= (get_skill(ch,gsn_prayer) / 3)) // changed from divided by 5

		{
			dam = 2 * dam + (dam * 6 * percent / 40); //changed from 100
			victim->stunned = 1;
			act ("{)You are stunned, and have trouble getting back up!{x", ch, NULL, victim, TO_VICT);
			act ("{.$N is stunned by the gods!{x", ch, NULL, victim, TO_CHAR);
			act ("{($N is having trouble getting back up.{x", ch, NULL, victim, TO_NOTVICT);
		}

	}

	damage(ch,victim,dam,gsn_prayer,DAM_HOLY,TRUE,0);
	WAIT_STATE(ch,PULSE_VIOLENCE);

	return;

}
/* Insomnia Spell By Bree
 Basically this spell makes it more difficult for a
 magic users to land the sleep spell on someone. Very
 few classes should get this spell, i am designing it
 for 2 classes, Archmage and Saint. Check for sleep and affect_strip it*/
//add to update.c to check for sleep and strip it if insomnia affected.
/* void spell_insomnia( int sn, int level, CHAR_DATA *ch, void *vo,int 
 target )
 {
 CHAR_DATA *victim = (CHAR_DATA *) vo;
 AFFECT_DATA af;

 if ( is_affected( ch, sn ) )
 {
 if (victim == ch)
 send_to_char("You are already wide awake.\n\r",ch);
 else
 act("$N is already wide awake",ch,NULL,victim,TO_CHAR);
 return;
 }

 af.where     = TO_AFFECTS;
 af.type      = sn;
 af.level     = level;
 af.duration  = level;
 af.location  = APPLY_NONE;
 af.modifier  = 0;
 affect_to_char( victim, &af );

 act( "$n looks less sleepy", victim, NULL, NULL, TO_ROOM );
 send_to_char( "You begin to need less sleep.\n\r", victim );
 
 return;

 } */

/***** START OF DREAMS ******************/
//if (!IS_NPC(ch) && ch->position == POS_SLEEPING)
//{
//
//if (dream <=186)
//{
//send_to_char("zzzzzzzzzzzzzzzzzzzzzzzzz.\n\r",ch);
//}
// if (is_affected(ch, gsn_insomnia) && ch != NULL)
//
//{   affect_strip(vch,gsn_sleep); 
//    return;
//}



