/* This file is the Clan-Skills of the PK Clan DarkSide && Renshai */
/* Copyright 2000, Gabe Volker */
/* To be used only on Asgardian Nightmare unless permission is granted */
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
DECLARE_DO_FUN( do_announce );
int focus_dam(CHAR_DATA *ch);
int focus_hit(CHAR_DATA *ch);
int focus_ac(CHAR_DATA *ch);


/* Increased Accuracy resulting in Double Strike */
void do_concentration(CHAR_DATA *ch, char *argument)
{
	int chance;

	if ((chance = get_skill(ch, gsn_concentration)) == 0 || !has_skill(ch,
			gsn_concentration))
	{
		send_to_char("{.You know not the way of the Rensahi.{x\n\r", ch);
		return;
	}

	if (is_affected(ch, gsn_concentration))
	{
		send_to_char("{.You are already concentrating on your accuracy!{x\n\r",
				ch);
		return;
	}

	if (ch->move < 251)
	{
		send_to_char(
				"You do not have the stamina to do this. Gather your energy.\n\r",
				ch);
		return;
	}

	if (is_affected(ch, gsn_modis_anger))
	{
		send_to_char("{.Your anger fades, you start to cool down.{x\n\r", ch);
		affect_strip(ch, gsn_modis_anger);
	}

	if (number_percent() < chance)
	{
		AFFECT_DATA af;

		WAIT_STATE (ch, PULSE_VIOLENCE)
;		ch->move -= 250;

		send_to_char ("{.You concentrate on the determination of Modi and your aim improves!{x\n\r", ch);
		act ("{($n's eyes turn light blue with white steaks.{x", ch, NULL, NULL, TO_ROOM);

		/* check_improve (ch, gsn_concentration, TRUE, 2); */

		af.where = TO_AFFECTS;
		af.type = gsn_concentration;
		af.level = ch->level;
		af.duration = -1;
		af.bitvector = 0;

		af.location = APPLY_HITROLL;
		af.modifier = UMAX (1, ch->level / 3);
		affect_to_char (ch, &af);
	}

	return;

}

void do_calm(CHAR_DATA *ch, char *argument)
{
	/* Tien
	 if(ch->clan != clan_lookup("renshai"))
	 {
	 send_to_char("Huh?\n\r",ch);
	 return;
	 }
	 */
	if (is_affected(ch, gsn_concentration))
	{
		send_to_char(
				"{.Your concentration fades, your temperature starts to rise.{x\n\r",
				ch);
		affect_strip(ch, gsn_concentration);
		return;
	}

	if (is_affected(ch, gsn_modis_anger))
	{
		send_to_char("{.Your anger fades, you start to cool down.{x\n\r", ch);
		affect_strip(ch, gsn_modis_anger);
		return;
	}
	send_to_char("{.You feel cool and collected.{x\n\r", ch);
	return;
}


// void do_crit_strike(CHAR_DATA *ch, char *argument)
//{
//      int chance;
//
//       if ((chance = get_skill (ch, gsn_crit_strike)) == 0
//        || !has_skill(ch,gsn_crit_strike))
//	{
//               send_to_char ("{.You know not the way of the Storm.{x\n\r", ch);
//		return;
//	}
//       if (is_affected (ch, gsn_crit_strike))
//	{
//              send_to_char ("{.Your fury begins to subside.{x\n\r", ch);
//		affect_strip(ch, gsn_crit_strike);
//              return;
//        }
//
//      if( ch->mana < 100 )
//    {
//     send_to_char("You do not have the willpower to do this. Gather your energy.\n\r",ch);
//         return;
//        }
//
//       if (is_affected (ch, gsn_crit_strike))
//        {
//               send_to_char("{.Your fury begins to subside.{x\n\r",ch);
//              affect_strip(ch, gsn_crit_strike);
//        }
//
//	if (number_percent () < chance)
//	{
//		AFFECT_DATA af;
//
//		WAIT_STATE (ch, PULSE_VIOLENCE);
//              ch->mana -= 100;
//
//              send_to_char ("{.You focus on crital striking!{x\n\r",ch);
//              send_to_char ("{.You feel the {!Wrath {xof The {&Storm{x enter your body!{x\n\r", ch);
//              act ("{($n concentration increases{x", ch, NULL, NULL, TO_ROOM);
//              act ("{($n's eyes turn into shadows.{x", ch, NULL, NULL, TO_ROOM);
//
//               /* check_improve (ch, gsn_crit_strike, TRUE, 2); */
//
//		af.where = TO_AFFECTS;
//               af.type = gsn_crit_strike;
//		af.level = ch->level;
//              af.duration = -1;
//                af.bitvector = 0;
//
//		af.location = APPLY_DAMROLL;
//               af.modifier = UMAX (1, ch->level / 5);
//		affect_to_char (ch, &af);
//	}
//
//        return;
//}


/** Alliance clanskill removed - Quintalis 9-28-04 **

 int alliance_damroll(CHAR_DATA *ch)
 {
 int damroll;
 int dam;
 damroll = 0;
 if (IS_NPC(ch)
 || ch->clan != clan_lookup("alliance") )
 return damroll;
 dam = ch->damroll;
 dam += str_app[get_curr_stat(ch,STAT_STR)].todam;
 dam += focus_dam(ch);
 if (IS_CRUSADER(ch))
 dam += (ch->alignment*ch->level)/2000;

 if (ch->class == CLASS_MONK)
 dam += ch->pcdata->power[0]/10+ch->pcdata->power[1]/10;

 if (ch->fighting == NULL)
 damroll = dam/20;
 else if (IS_NPC(ch->fighting))
 damroll = dam/20;
 else if (IN_ARENA(ch))
 damroll = dam/20;
 else if (number_on(ch->clan) == number_on(ch->fighting->clan))
 damroll = dam/8;
 else if (number_on(ch->clan)+1 < number_on(ch->fighting->clan))
 damroll = dam/4;
 else if (number_on(ch->clan) < number_on(ch->fighting->clan))
 damroll = dam/6;
 else
 damroll = dam/20;

 if (damroll < 0)
 damroll = (damroll*damroll)/2;
 return damroll;
 }
 int alliance_hitroll(CHAR_DATA *ch)
 {
 int hitroll;
 int hit;
 hitroll = 0;
 if (IS_NPC(ch)
 || ch->clan != clan_lookup("alliance") )
 return hitroll;
 hit = ch->hitroll;
 hit += str_app[get_curr_stat(ch,STAT_STR)].tohit;
 hit += focus_hit(ch);
 if (IS_CRUSADER(ch))
 hit += (ch->alignment*ch->level)/2000;

 if (ch->fighting == NULL)
 hitroll = hit/20;
 else if (IS_NPC(ch->fighting))
 hitroll = hit/20;
 else if (IN_ARENA(ch))
 hitroll = hit/20;
 else if (number_on(ch->clan) == number_on(ch->fighting->clan))
 hitroll = hit/8;
 else if (number_on(ch->clan)+1 < number_on(ch->fighting->clan))
 hitroll = hit/4;
 else if (number_on(ch->clan) < number_on(ch->fighting->clan))
 hitroll = hit/6;
 else
 hitroll = hit/20;


 if (hitroll < 0)
 hitroll = (hitroll*hitroll)/2;
 return hitroll;
 }
 int alliance_ac(CHAR_DATA *ch, int type)
 {
 int armor;
 int ac;
 armor = 0;
 if (IS_NPC(ch)
 || ch->clan != clan_lookup("alliance") )
 return armor;
 ac = ch->armor[type];
 if (IS_AWAKE(ch))
 ac += dex_app[get_curr_stat(ch,STAT_DEX)].defensive;
 ac += focus_ac(ch);

 if (ch->fighting == NULL)
 armor = ac/20;
 else if (IS_NPC(ch->fighting))
 armor = ac/20;
 else if (IN_ARENA(ch))
 armor = ac/20;
 else if (number_on(ch->clan) == number_on(ch->fighting->clan))
 armor = ac/8;
 else if (number_on(ch->clan)+1 < number_on(ch->fighting->clan))
 armor = ac/4;
 else if (number_on(ch->clan) < number_on(ch->fighting->clan))
 armor = ac/6;
 else
 armor = ac/20;

 if(armor > 0)
 armor = 0-armor;
 return armor;
 }**/
