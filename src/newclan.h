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

/* CLAN CODE HEADERS by Nico/Zarko(Deryck Arnold) - if it breaks, you know who to blame!
 * Written exclusively for Asgardian Nightmares - point system idea by Solphor.
 * Definitions for newclan.c.
 * 
 * Put #include "newclan.h" in all files bar merc.h that are listed here:
 
   act_wiz.c:
   In do_copyover:
    save_all_clans( );
    save_all_rosters( );
   
   comm.c:
   In do_crashsave:
    save_all_clans( );
    save_all_rosters( );
   In nanny:
    update_roster( d->character, FALSE );
   In copyover_recover for each pc character:
    update_roster( d->character, FALSE );
   In close_socket:
    update_roster( d->character, TRUE );
   In bust_a_prompt:
    case ED_CEDIT:
        sprintf( buf, "{B[ {GCEDIT {B- {WNo. {R%d {B- {x%s {B]{x\n\r",
            ((CLAN_DATA *) ch->desc->pCedit)->id,
            ((CLAN_DATA *) ch->desc->pCedit)->c_name );
        break;
    case ED_MCEDIT:
        sprintf( buf, "{B[ {MM{GCEDIT {B- {x%s {B-{w %s {B]{x\n\r",
            ((CLAN_DATA *) ch->desc->pCedit)->c_name,
            ((MOB_INDEX_DATA *) ch->desc->pEdit)->short_descr );
        break;
    case ED_OCEDIT:
        sprintf( buf, "{B[ {CO{GCEDIT {B- {x%s {B-{w %s {B]{x\n\r",
            ((CLAN_DATA *) ch->desc->pCedit)->c_name,
            ((OBJ_INDEX_DATA *) ch->desc->pEdit)->short_descr );
        break;
    case ED_RCEDIT:
        sprintf( buf, "{B[ {RR{GCEDIT {B- {x%s {B-{w %s {B]{x\n\r",
            ((CLAN_DATA *) ch->desc->pCedit)->c_name,
            ((ROOM_INDEX_DATA *) ch->desc->pEdit)->name );
        break;
    case ED_SCEDIT:
        sprintf( buf, "{B[ {YS{GCEDIT {B- {WNo. {R%d {B- {x%s {B]{x\n\r",
            ((CLAN_DATA *) ch->desc->pCedit)->id,
            ((CLAN_DATA *) ch->desc->pCedit)->c_name );
        break;
    
   db.c:
   In boot_db:
    log_string( "Loading Clans." );
    load_all_clans( );
    log_string( "Loading Player Rosters." );
    load_all_rosters( );
    log_string( "Loading PK History Table." );
    load_pkhist_data( );
    log_string( "Fixing Clan Data." );
    fix_clan_boot( );
   
   fight.c:
   In is_safe_new:
    return !can_pkill_other( ch, victim, !show );
    
   handler.c:
   In can_pkill:
    return can_pkill_other( ch, victim, TRUE );
   In is_pkill replace with:
    return can_pkill_other( ch, victim, FALSE );
   
   interp.c:
   Command table entries:
    { "bounty",		do_bounty,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "cedit",		do_cedit,	POS_RESTING,	 0,  1,  LOG_ALWAYS, 1 },
    { "clan",		do_ctalk,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "clist",		do_clist,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "loner",		do_loner,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "notnewbie",	do_notnewbie,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "petition",	do_petition,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "pkhist",		do_pk_history,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "pkill",		do_go_pkill,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "rank",		do_rank,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "roster",		do_clan_roster,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
   In interpret, alter dispatching of normal command and disabled check to fit and add:
    if ( !found )
        cSkillFound = interpret_clan_cmd( ch, command );
    
   merc.h
   This needs to go in MOB/OBJ/ROOM_INDEX_DATA
    union
    {
    	CLAN_DATA *	data;
    	int		id;
    } clan_data; ********************************* NO NEED FOR THIS UNION ANYMORE!!!
    
   In typedef statements:
    typedef	struct	clan_data		CLAN_DATA;
    typedef	struct	roster_data		ROSTER_DATA;
    
   Add to the rename function something to rename roster data.
   Add free_roster_data( roster ) to the delete command and save_all_rosters( ).

   Stuff to test:
   - PK checks
   - PK rank checks
   - Clan creation/deletion
   - Clan depositing
   - Clan member management/exile
   - Clan equipment wearing
   - Clan skills
   - Clan purchases/upgrades
   - Mob/obj/room clan checks
   - Reboots/copyovers/crashes
   
   While wizlocked for testing, put in comm.c =~ 2692:
   write_to_buffer( d, "The game is wizlocked at the moment.\n\r" );
   write_to_buffer( d, "The new clan system is being tested.\n\r" );
   write_to_buffer( d, "Thank you for your patience.\n\r" );
 *
 *
 */

/*********************************** DEFINED VALUES ***********************************/

/* Time defines in time_t */
#define TIME_SECOND		1			// Duh
#define TIME_MINUTE		(60 * TIME_SECOND)
#define TIME_HOUR		(60 * TIME_MINUTE)
#define TIME_DAY		(24 * TIME_HOUR)
#define TIME_WEEK		(7  * TIME_DAY)
#define TIME_ROSTER_DELETE	(8  * TIME_WEEK)
#define TIME_ROSTER_UPD_TICK	(10 * TIME_MINUTE)
#define TIME_NEWBIE_GRACE	(4  * TIME_WEEK)
#define TIME_ROSTER_PEN_TICK	(TIME_HOUR)
#define TIME_SPAMKILL_LIMIT	(TIME_HOUR)

/* Area vnum defines */
#define VNUM_CLAN_LOWER			60100	// Lower clan area vnum
#define VNUM_CLAN_UPPER			70000	// Upper clan area vnum
#define VNUM_CLAN_BLOCK			50	// Number of vnums per clan
#define VNUM_STOCK_LOWER		60000	// Lower stock vnum
#define VNUM_STOCK_UPPER		60099	// Upper stock vnum
#define VNUM_MARKET_SQUARE		3014	// Vnum of the market square
#define VNUM_CLAN_PET_ROOM		70000

/* Mob vnum defines */
#define VNUM_STOCK_MOB_GUARD		60000	// Stock clan guard vnum
#define VNUM_STOCK_MOB_MAGE		60001	// Stock clan mage vnum
#define VNUM_STOCK_MOB_MASON		60002	// Stock clan mason vnum
#define VNUM_STOCK_MOB_TRAINER		60003	// Stock clan trainer vnum
#define VNUM_STOCK_MOB_SMITH		60004	// Stock clan blacksmith vnum
#define VNUM_STOCK_MOB_BREWER		60005	// Stock clan brewer vnum
#define VNUM_STOCK_MOB_BAKER		60006	// Stock clan baker vnum
#define VNUM_STOCK_MOB_GPRIEST		60007	// Stock clan good priest vnum
#define VNUM_STOCK_MOB_EPRIEST		60008	// Stock clan evil priest vnum
#define VNUM_STOCK_MOB_HEALER		60009	// Stock clan healer vnum
#define VNUM_STOCK_MOB_SHOPKEEP		60010	// Stock clan shopkeep vnum
#define VNUM_STOCK_MOB_PET		60011	// Stock clan pet vnum
#define VNUM_STOCK_MOB_TANNER		60012	// Stock clan tanner vnum
#define VNUM_STOCK_MOB_QUESTOR		60013	// Stock clan quest master vnum

/* Obj vnum defines */
#define VNUM_STOCK_OBJ_FURNITURE	60000	// Stock clan furniture vnum
#define VNUM_STOCK_OBJ_PORTAL		60001	// Stock clan portal vnum
#define VNUM_STOCK_OBJ_SCROLL		60002	// Stock clan scroll vnum
#define VNUM_STOCK_OBJ_WAND		60003	// Stock clan wand vnum
#define VNUM_STOCK_OBJ_STAFF		60004	// Stock clan staff vnum
#define VNUM_STOCK_OBJ_WEAPON		60005	// Stock clan weapon vnum
#define VNUM_STOCK_OBJ_ARMOUR		60006	// Stock clan armour vnum
#define VNUM_STOCK_OBJ_POTION		60007	// Stock clan potion vnum
#define VNUM_STOCK_OBJ_CONTAINER	60008	// Stock clan container vnum
#define VNUM_STOCK_OBJ_DCONTAINER	60009	// Stock clan drink container vnum
#define VNUM_STOCK_OBJ_FOOD		60010	// Stock clan food vnum
#define VNUM_STOCK_OBJ_BOAT		60011	// Stock clan board vnum
#define VNUM_STOCK_OBJ_FOUNTAIN		60012	// Stock clan fountain vnum
#define VNUM_STOCK_OBJ_SLOT_MACHINE	60013	// Stock clan slot machine vnum
#define VNUM_STOCK_OBJ_PILL		60014	// Stock clan pill vnum
#define VNUM_STOCK_OBJ_WARP_STONE	60015	// Stock clan warp stone vnum
#define VNUM_STOCK_OBJ_DEMON_STONE	60016	// Stock clan demon stone vnum
#define VNUM_STOCK_OBJ_LIGHT		60017	// Stock clan light vnum

/* Room vnum defines */
#define VNUM_STOCK_ROOM_RECALL		60000	// Stock main clan hall
#define VNUM_STOCK_ROOM_SMITHY		60001	// Stock smithy
#define VNUM_STOCK_ROOM_WORKSHOP	60002	// Stock mage's workshop
#define VNUM_STOCK_ROOM_GSHRINE		60003	// Stock good priest's shrine
#define VNUM_STOCK_ROOM_ESHRINE		60004	// Stock evil priest's shrine
#define VNUM_STOCK_ROOM_SHOP		60005	// Stock clan shop
#define VNUM_STOCK_ROOM_TROOM		60006	// Stock clan trainers room
#define VNUM_STOCK_ROOM_STABLE		60007	// Stock clan stable
#define VNUM_STOCK_ROOM_BAKERY		60008	// Stock clan bakery
#define VNUM_STOCK_ROOM_BREWERY		60009	// Stock clan brewery
#define VNUM_STOCK_ROOM_TANNERY		60010	// Stock clan tannery

/* Special Clan indices */
#define CLAN_NONE		0		// Non-clanned nonpk clan
#define CLAN_LONER		1		// Non-clanned pk clan
#define CLAN_PK_OUTCAST		2		// Exiled pk players go here
#define CLAN_NONPK_OUTCAST	3		// Exiled non-pk players go here
#define CLAN_IMMORTAL		4		// Immortal clan

/* Clan mob types - don't forget cmob_name table */
#define CMOB_GUARD		0		// Clan guard
#define CMOB_MAGE		1		// Clan mage
#define CMOB_MASON		2		// Clan mason
#define CMOB_TRAINER		3		// Clan trainer
#define CMOB_SMITH		4		// Clan blacksmith
#define CMOB_BREWER		5		// Clan brewer
#define CMOB_BAKER		6		// Clan baker
#define CMOB_GPRIEST		7		// Clan good priest -> has to be next to and before epriest
#define CMOB_EPRIEST		8		// Clan evil priest -> has to be directly after gpriest
#define CMOB_HEALER		9		// Clan healer
#define CMOB_SHOPKEEP		10		// Clan shopkeep
#define CMOB_PET		11		// Clan pet
#define CMOB_TANNER		12		// Clan tanner
#define CMOB_QUESTOR		13		// Clan questor
#define CMOB_MAX		14		// Number of (referenced) clan mobs

/* Clan room types */
#define CROOM_RECALL		0		// Recall room
#define CROOM_SMITHY		1		// Smithy room
#define CROOM_WORKSHOP		2		// Workshop room
#define CROOM_GSHRINE		3		// Good shrine room -> has to be next to and before eshrine
#define CROOM_ESHRINE		4		// Evil shrine room -> has to be directly after gshrine
#define CROOM_SHOP		5		// Shop room
#define CROOM_TROOM		6		// Trainers room
#define CROOM_STABLE		7		// Stable for a pet
#define CROOM_BAKERY		8		// Bakery for a baker
#define CROOM_BREWERY		9		// Brewery for a brewer
#define CROOM_TANNERY		10		// Tannery for a tanner
#define CROOM_MAX		11		// Number of (referenced) clan rooms

/* Rank stuff */
#define RANK_MEMBER		0		// <- Lowest intra-clan rank
#define RANK_RECRUIT		4000		// There will be no leader or vice-leader
#define RANK_SOLDIER		16000		// ranks. There will only be one designated
#define RANK_LIEUTENANT		48000		// leader who will make the decisions, but
#define RANK_COMMANDER		128000		// still has to gain points for skills/eq
#define RANK_ECAPTAIN		512000		// <- Highest intra-clan rank

/* Clan limits */
#define	TOTAL_CLAN_RANK		6		// Total intra-clan ranks (expandable)
#define MAX_PK_CLAN_MEMBERS	4		// Clan limit for pk clans
#define MAX_NONPK_CLAN_MEMBERS	6		// Clan limit for non-pk clans
#define MAX_CLAN_WHO_LENGTH	12		// Max length of colour name
#define MAX_CLAN_RANK_LENGTH	17		// Max length of clan rank names

/* Player flags in ROSTER_DATA */
#define ROST_AWAY		(B)		// Stops player from being penalised if inactive
#define ROST_REROLL		(C)		// Deletes roster data if person doesn't finish for an extended period of time
#define ROST_INACTIVE		(D)		// Activates after a certain amount of time, earlier for leaders
#define ROST_CONFIRM_NEW	(E)		// Confirm creation of clan
#define ROST_CONFIRM_PK		(F)		// Confirm choice to go pk
#define ROST_CONFIRM_LONER	(G)		// Confirm choice to loner
#define ROST_NEWBIE		(H)		// Marks player as a newbie
#define ROST_EXILE_GRACE	(I)		// Grace period where player can leave clan without losing too much
#define ROST_DELETED		(J)		// Marks the roster as deleted
#define ROST_IMMORTAL		(K)		// Immortals

/* PKHIST_DATA flags */
#define PKHIST_INVALID		(A)		// If the victim attacks the killer within the time limit
#define PKHIST_CHINVALID	(B)		// If a member of the victim's clan attacks the killer, this gets flagged allowing entry into the victim's clan hall
#define PKHIST_ARENA		(C)		// Arena or not.

/* Log entry types */
#define ENTRY_DEPOSIT		0		// Clan deposits
#define ENTRY_DELETE		1		// Clan deletion
#define ENTRY_UPGRADE		2		// Clan upgrades
#define ENTRY_CREATE		3		// Clan creation
#define ENTRY_ACCEPT		4		// Petition accepted
#define ENTRY_GUILD		5		// Remove/add via the guild command
#define ENTRY_TRUST		6		// Trusting a member
#define ENTRY_EXILE		7		// Clan exiles 
#define ENTRY_WAR		8		// Clan wars
#define ENTRY_CHANGED		9		// Clan data changes

/* Log filter types */
#define FILTER_NONE		0		// No filter
#define FILTER_NAME		1		// Filter by name
#define FILTER_TYPE		2		// Filter by type of entry
#define FILTER_TIME		3		// Filter by time (everything starting from time specified)
#define FILTER_KEYWORD		4		// Filter by keyword in entry string

/* Editor types */
#if !defined(ED_NONE)
#define ED_NONE			0		// Not editing anything
#endif
#define ED_CEDIT		8		// Main clan editor
#define ED_MCEDIT		9		// Clan mob editor
#define ED_OCEDIT		10		// Clan object editor
#define ED_RCEDIT		11		// Clan room editor
#define ED_SCEDIT		12		// Clan skill editor

/* CEDIT trusts - note these are different to clan ranks */
#define TRUST_NONE		-1		// No one can use the command
#define TRUST_ALL		0		// Anyone, including those not in the clan can use the command
#define TRUST_MEMBER		1		// Anyone in the clan
#define TRUST_VLR		4		// Vice-leaders up
#define TRUST_LDR		5		// Only leaders

/* Point values for aqp and platinum */
#define POINT_VALUE_QP		4		// Aqp:point ratio
#define POINT_VALUE_PLATINUM	1		// Platinum:point ratio

/* Upgrade return states */
#define UPGRADE_SUCCESS		0		// Clan upgrade was a success - deduct money
#define UPGRADE_FAIL		1		// Clan upgrade failed - report it
#define UPGRADE_REQ_ARG		2		// Clan upgrade needs more arguments to execute

/* COST_DATA flags */
#define UPGD_FLAG		(A)		// Clan upgrade is a flag type of upgrade
#define UPGD_NO_BULK		(B)		// Clan upgrade cannot be bought in bulk
#define UPGD_ROOM_OBJ		(C)		// Clan object isn't sold, but is placed in a room
#define UPGD_XCOST		(D)		// Upgrade has a variable price

/* XCOST_DATA flags */
#define XUPGD_IMM_SPECIAL	(A)		// For the only-one-allowed immunities
#define XUPGD_MOB		(B)		// Can be applied to mobs
#define XUPGD_OBJ		(C)		// Can be applied to objects

#define BIT_AFFECT		0		// Affect bit
#define BIT_SHIELD		1		// Shield bit

/* Spell xcost types */
#define SPELL_DEFENSIVE		0		// Defensive spells like armour
#define SPELL_OFFENSIVE		1		// Offensive spells like magic missile
#define SPELL_MISC		2		// Misc spells like animate

/* COST_DATA max value stuff */
#define MAX_NONE		-1		// No max value
#define MAX_SPECIAL		-2		// Use max_func to calc special max value

/* Misc prices */
#define PRICE_CLAN_QP		0		// Aqp price for a clan - need to have platinum too
#define PRICE_CLAN_PLATINUM	30000		// Platinum price for a clan - need to have aquest points too
#define PRICE_SKILL_MINOR_QP	2500		// Aqp price for a minor clan skill
#define PRICE_SKILL_MINOR_PLAT	10000		// Platinum price for a minor clan skill
#define PRICE_SKILL_MAJOR_QP	7500		// Aqp price for a major clan skill
#define PRICE_SKILL_MAJOR_PLAT	15000		// Platinum price for a major clan skill

/* File/Folder definitions */
#define CLAN_DATA_FOLDER	"../data/clans/"		// Clan data folder
#define CLAN_HISTORY_FOLDER	"../data/clans/history/"	// Clan transactions (donations/punishments/upgrades)
#define CLAN_DATA_LIST		"../data/clans/clan.lst"	// Clan data list
#define TEST_CLAN_DATA_FOLDER	"../data/clanstest/"		// Clan data folder
#define TEST_CLAN_HISTORY_FOLDER	"../data/clanstest/history/"	// Clan transactions (donations/punishments/upgrades)
#define TEST_CLAN_DATA_LIST		"../data/clanstest/clan.lst"	// Clan data list
#define ROSTER_DATA_FOLDER	"../data/rosters/"		// Roster data folder
#define ROSTER_DATA_LIST	"../data/rosters/roster.lst"	// Roster data list
#define TEST_ROSTER_DATA_FOLDER	"../data/rosterstest/"		// Roster data folder
#define TEST_ROSTER_DATA_LIST	"../data/rosterstest/roster.lst"	// Roster data list
#define PKHIST_DATA_FILE	"../data/spamkill.dat"		// Spam kill data file
#if !defined(TEMP_FILE)						// Define a temp file if not already defined
#define TEMP_FILE		"../player/romtmp"
#endif


/* Macros */
// Check whether the person has roster data
#define HAS_ROSTER( Ch )	( ((Ch) && (Ch)->pcdata && (Ch)->pcdata->roster	\
				   && (Ch)->pcdata->roster->clan) ? TRUE : FALSE )

// Retrieve the person's roster data
#define GET_ROSTER( Ch, Roster )			\
	if ((Ch) && (Ch)->pcdata) {				\
		if ((Ch)->pcdata->roster == NULL)	\
			update_roster((Ch), FALSE);		\
		(Roster) = (Ch)->pcdata->roster;	\
		(Roster)->character = (Ch);			\
	} else									\
		(Roster) = NULL;

#define SET_ROSTER( Ch, Roster ) ( (Roster) ?					\
				    ((Ch)->pcdata->roster = (Roster)) :		\
				    ((Ch)->pcdata->roster = NULL) )

#define EDIT_CLAN( Ch, Clan )	( !((Ch)->desc->pCedit) ?			\
				    ((Clan) = NULL) :				\
				    ((Clan) = (CLAN_DATA *) (Ch)->desc->pCedit) )

#define IS_EDITING_CLAN( Ch, Clan ) ( (Ch)->desc->editor >= ED_CEDIT		\
				   && (Ch)->desc->editor <= ED_SCEDIT		\
				   && (Ch)->desc->pCedit == (void *) (Clan) )

// Macroise these so that the locations can be changed easily
#define SET_EDIT_CLAN( Ch, Clan ) ( (Ch)->desc->pCedit = (void *) (Clan) )	// Sets what clan the person is editing
#define SET_EDIT_ITEM( Ch, Item ) ( (Ch)->desc->pEdit  = (void *) (Item) )	// Sets what item(e.g piece of eq) the person is editing
#define SET_EDIT_TYPE( Ch, Type ) ( (Ch)->desc->editor = (Type)		 )	// Sets the type of editor(e.g. obj cedit)


/************************************** TYPEDEFS **************************************/

//typedef	struct	clan_data	CLAN_DATA;
typedef	const	struct	cost_data	COST_DATA;
typedef		struct	pkill_data	PKILL_DATA;
//typedef	struct	roster_data	ROSTER_DATA;
typedef		struct	pkhist_data	PKHIST_DATA;
typedef		struct	xcost_data	XCOST_DATA;
	
typedef int	CALC		( CHAR_DATA *ch, COST_DATA *upgrade );	// Value retrieval function for price stuff
#define CALC_FUN( fun )		int fun ( CHAR_DATA *ch, COST_DATA *upgrade )
#define DECLARE_CALC_FUN( fun ) CALC		fun

typedef	bool	CEDIT		( CHAR_DATA *ch, char *argument ); // Cedit olc type
#define CEDIT_FUN( fun )	bool fun ( CHAR_DATA *ch, char *argument )
#define DECLARE_CEDIT_FUN( fun ) CEDIT		fun

typedef int	UPGD		( CHAR_DATA *ch, char *argument, COST_DATA *upgrade, XCOST_DATA *xupgrade );	// Object upgrade function
#define UPGD_FUN( fun )		int fun ( CHAR_DATA *ch, char *argument, COST_DATA *upgrade, XCOST_DATA *xupgrade )
#define DECLARE_UPGD_FUN( fun )  UPGD		fun

/************************************* STRUCTURES *************************************/

/* PKILL_DATA structure:
 *  Usage: CLAN_DATA as *pkilldata
 *	   ROSTER_DATA as *pkilldata
 *
 * - For both pk data and arena data.
 *
 * - Records kills, deaths, wins and losses.
 *
 * - Differences between wins and kills / deaths and losses will
 *   become more apparent in arena matches(team efforts).
 *
 * - Records overall points gained from pk/arena fights.
 *
 * - Points determine overall clan rank in CLAN_DATA context
 *   and individual rank in ROSTER_DATA context.
 *
 */
struct pkill_data
{
    PKILL_DATA *	next;			// For free list
    long		kPoints;		// Points (based on gains from kills)
    int			kills;			// Number of kills
    int			deaths;			// Number of deaths
    int			wins;			// Number of wins
    int			losses;			// Number of losses
    bool		valid;			// Memory management - indicates whether freed or not
};


/* COST_DATA structure:
 * - Stores costs of clan items, rooms, mobs, upgrades.
 *
 * - Level requirements of mobs are also there.
 *
 * - Max overall limit only - too hard to keep track of max-per-mob
 *   without making things too overly complex, so the 6-portal rule
 *   will no longer be applicable.
 *
 * - Specific: Mobiles - 0=CMOB index,  1=CROOM index of reset room.
 *	       Objects - 0=stock vnum.
 *	       Rooms   - 0=CROOM index.
 *             MUpgrd  - 0=CMOB index for specific upgrades.
 *	       OUpgrd  - 0=item type for specific upgrades.
 *	       RUpgrd  - 0=room flags if applicable.
 */
struct cost_data
{
    char *		name;			// Name of item/upgrade
    char *		descr;			// Short description of the upgrade
    UPGD *		func;			// Pointer to function of upgrade/whatever
    CALC *		curr_value;		// Pointer to function to retrieve current value of clan's upgrade
    CALC *		max_func;		// Pointer to function to retrieve special max values
    XCOST_DATA *	xcost;			// Any extra tables
    long		flags;			// Any flags
    int			quantity;		// Quantity of items or modifier for effects
    int			max;			// Max limit(-1 for none)
    int			specific[2];		// Specific type, i.e. item type (-1 for none)
    int			qps;			// Cost in questpoints
    int			platinum;		// Cost in platinum
    sh_int		xreq1;			// Mob index of req 1 for xcost
    sh_int		xreq2;			// Ditto for req 2
    sh_int		cMob[CMOB_MAX];		// Clan mob levels required
    bool		cRoom[CROOM_MAX];	// Clan rooms required
};

/* XCOST_DATA structure:
 * - Extra prices, e.g. spell types for scrolls
 *
 * - Bit member will hold skill gsns, wpn flags, wpn damtypes
 */
struct xcost_data
{
    char *		name;			// Name of item
    long		bit;			// Loaded in at boot(if required) to speed up lookups (e.g. skill gsn, wpn flag..)
    int			flags;			// Special flags
    int			qps;			// Cost in questpoints
    int			platinum;		// Cost in platinum
    sh_int		type;			// Spell defensive, etc.
    sh_int		req1;			// Requirement one (e.g mage level)
    sh_int		req2;			// Requirement two (e.g smithy level)
};

/* CLAN_DATA structure:
 *  Usage: PCDATA *clan_data
 *
 * - Linked list design
 *
 * - Holds all the info on a clan.
 */
struct clan_data
{
    CLAN_DATA *		next;			// Linked-list design for easier memory management
    CLAN_DATA *		enemy;			// Clan war enemy - more points for kills
    union
    {
    	CLAN_DATA *	data;			// Clan data of petitioned enemy
    	int		id;			// Id of clan petitioned for war
    } Penemy;
    PKILL_DATA *	pkilldata;		// PK data or arena data(in the case of nonpk clan)
    COST_DATA *		upgrade;		// Next upgrade pending confirmation
    MOB_INDEX_DATA *	cMob[CMOB_MAX];		// Pointers to clan mob id data
    ROOM_INDEX_DATA *	cRoom[CROOM_MAX];	// Pointers to clan room id data
    XCOST_DATA *	xupgrade;		// Points to selected xcost table
    ROSTER_DATA *	u_rost;			// Points to the person selecting the upgrade
    char *		name;			// Name of clan to be used as keyword
    char *		c_name;			// Coloured name of clan (No reference to black people here!)
    char *		r_name[TOTAL_CLAN_RANK];// Rank names
    char *		r_who[TOTAL_CLAN_RANK];	// Rank names on who
    char *		description;		// Clan description
    long		platinum;		// Platinum in clan account
    long		qps;			// Quest points in clan account
    time_t		exit_changed;		// Last time entrance/exit of clan hall was changed
    time_t		edit_time;		// Time period in which clan data can be edited.
    int			u_quantity;		// Quantity of pending upgrade desired
    int			id;			// Reference number
    int			members;		// Number of members in the clan	
    int			vnum[2];		// Lower and upper clan area vnums for the specific clan
    int			entrance_vnum;		// Room outside the entrance
    bool		pkill;			// TRUE for pkill clans, FALSE for nonpkill clans
    bool		independent;		// For clans like loner or no clan
    bool		valid;			// Memory management - indicates whether freed or not

    int			skills[2]; // Major and minor clanskills.
};

/* ROSTER_DATA structure:
 *  Usage: PCDATA as *roster
 *
 * - Linked-list design.
 *
 * - Provides the roster for clans.
 *
 * - Individual player scores are stored here.
 *
 */
struct roster_data
{
    ROSTER_DATA *	next;			// Linked-list design for easier memory management
    CHAR_DATA *		character;		// Pointer back to character
    CLAN_DATA *		clan;			// Player clan (even if none)
    CLAN_DATA *		petition;		// Clan petitioned for membership
    PKILL_DATA *	pkilldata;		// Individual scores
    char *		name;			// Name of player
    time_t		last_login;		// Last login time - inactive people will be booted
    time_t		penalty_time;		// Clan punishments - will work with flags
    long		dPoints;		// Points based on donations
    long		flags;			// Flags e.g CLAN_LEADER for leader
    long		bounty;			// Bounty on head
    int			id;			// Unique id number
    int			trust;			// Clan trust - assignable by clan leader
    bool 		valid;			// Memory management - indicates whether freed or not
};

/* PKHIST_DATA structure
 * - Table design
 *
 * - Records pkills - prevents spamkilling by recording time
 *
 * - Players can view whos killed who
 *
 */
struct pkhist_data
{
    ROSTER_DATA *	victim;			// Data of victim
    ROSTER_DATA *	killer;			// Data of killer
    CLAN_DATA *		vClan;			// Current clan of vicitm
    CLAN_DATA *		kClan;			// Current clan of killer
    time_t		time;			// Time of death
    long		flags;			// Flags like invalid flags
    int			vLevel;			// Level of victim
    int			kLevel;			// Level of killer
};

/* struct log_type
 * - For clan logs
 *
 * - Display name and keyword
 */
struct log_type
{
    char *		keyword;		// Keyword of the item
    char *		display;		// Display name of the item
};

/* struct vnum_table
 * - Stores names/vnums of referenced rooms/mobs
 */
struct vnum_table
{
    char *		name;			// Name of type
    int			vnum;			// Vnum of type
};

/* struct clan_ranks:
 * - Contains the default clan ranks and points required for each.
 * - Might enable it to be changable via OLC later.
 * - Clan leaders will be able to customise their clan's ranks.
 */
struct clan_ranks
{
    char *	name;				// DEFAULT name of rank
    char *	who_name;			// Three letter(minus colours) name that shows on who
    int		points;				// Number of points to reach rank
};

/* struct cedit_cmd_type:
 * - New OLC command table for mortals
 * - Allows implementation of DIY clans   
 */
struct cedit_cmd_type
{
    char *	cmd;				// Command name
    CEDIT *	func;				// Functionimmt the clan code has over 1600 lines of pure tables
    sh_int	trust;				// Trust required for immortals to edit
    sh_int	m_trust;			// Trust required for mortals to edit(clan trust for cedit)
    char *	description;			// Description of the command.
};

/* struct clan_skill_cmds:
 * - For a clan skill command table.
 * - Improves performance by taking load off interpret for non-clanned characters.
 * - Easier administration of clan skills.
 * - Prevents conflicts with existing commands.
 * - Clan data part will need to be dynamic
 */
struct clan_skill_cmds
{
    char *	cmd;				// Command name
    char *	name;				// Fancy name with colour for displays
    char *	description;			// Description of what it does
    int		points;				// Player clan points required to use
    int		qps;				// Aqp cost
    int		platinum;			// Platinum cost
    sh_int *	gsn;				// Affect gsn for auto stripping of skills on sell
    bool	major;				// Major skill
    sh_int	position;			// Min pos required
    DO_FUN *	func;				// Function name
};


/******************************** FUNCTION PROTOTYPES *********************************/

/* Memory management */
PKILL_DATA *	new_pkill_data		( void );
void 		free_pkill_data		( PKILL_DATA *pkilldata );

CLAN_DATA *	new_clan_data		( bool get_id );
void 		free_clan_data		( CLAN_DATA *clan );

ROSTER_DATA *	new_roster_data		( bool get_id );
void 		free_roster_data	( ROSTER_DATA *roster );

/* Helper functions */
char *		vnum_lookup		( const struct vnum_table *table, int vnum );
int		generate_clan_id	( void );
long		get_total_points	( ROSTER_DATA *roster );
int		get_r_rank_index	( ROSTER_DATA *roster );
int		get_p_rank_index	( long points );
CLAN_DATA *	get_clan_by_id		( int id );
CLAN_DATA *	get_clan_by_name	( char *name );
CLAN_DATA *	clan_lookup		( char *name );
CLAN_DATA *	get_clan_by_vnum	( int vnum );
CLAN_DATA *	get_clan_by_ch		( CHAR_DATA *ch );
ROOM_INDEX_DATA * get_clan_deathroom	( CHAR_DATA *ch );
int		generate_roster_id	( void );
ROSTER_DATA *	get_roster_by_id	( int id );
ROSTER_DATA *	get_roster_by_name	( char *name, bool prefix );
void		update_roster		( CHAR_DATA *ch, bool quit );
void		update_all_rosters	( void );
void		mark_delete_roster	( ROSTER_DATA *roster );
void		append_clan_log		( CLAN_DATA *clan, int type, char *name, char *string );
void		parse_clan_log		( CHAR_DATA *ch, CLAN_DATA *clan, void *filter, int ftype );
char *		strip_spaces		( char *str );
char *		strip_bespaces		( char *str );
char *		strip_spec_char_col	( char *str );
char *		process_name		( char *str );
char *		get_pen_code		( ROSTER_DATA *roster );
AREA_DATA *	get_clan_area		( void );
int		get_clan_mvnum		( CLAN_DATA *clan );
int		get_clan_ovnum		( CLAN_DATA *clan );
int		get_clan_rvnum		( CLAN_DATA *clan );
void		assign_clan_vnums	( CLAN_DATA *clan );
int		clan_eq_wear_max	( CHAR_DATA *ch );
bool		check_path		( ROOM_INDEX_DATA *end );
void		reset_clan_mob		( MOB_INDEX_DATA *pMob, bool delete );
void		reset_clan_obj		( OBJ_INDEX_DATA *pObj, bool delete );

/* Saving/Loading */
void		load_clan_data		( FILE *fp );
void		load_all_clans		( void );
void		save_clan_data		( CLAN_DATA *clan );
void		save_all_clans		( void );
void		load_roster_data	( FILE *fp );
void		load_all_rosters	( void );
void		save_roster_data	( ROSTER_DATA *roster );
void		save_all_rosters	( void );
void		load_pkhist_data	( void );
void		save_pkhist_data	( void );
void		fix_clan_boot		( void );

/* Upgrade functions */
bool		upgrade_req_pass	( CHAR_DATA *ch, COST_DATA *table, bool showHave );
bool		upgrade_process		( CHAR_DATA *ch, char *argument, COST_DATA *table, bool buy );
void		upgrade_list_table	( CHAR_DATA *ch, COST_DATA *table );
void		xupgrade_list_table	( CHAR_DATA *ch, COST_DATA *table );
DECLARE_DO_FUN	 (  upgd_handler		);

DECLARE_CALC_FUN (  mval_xcost_count	);

DECLARE_CALC_FUN (  cval_mob_normal		);
DECLARE_UPGD_FUN (  mob_normal			);

DECLARE_CALC_FUN (  cval_obj_normal		);
DECLARE_CALC_FUN (  mval_obj_normal		);
DECLARE_UPGD_FUN (  obj_normal			);

DECLARE_CALC_FUN (  cval_room_normal		);
DECLARE_UPGD_FUN (  room_normal			);
DECLARE_UPGD_FUN (  room_recall			);
DECLARE_CALC_FUN (  cval_room_hall_entrance	);
DECLARE_CALC_FUN (  mval_room_hall_entrance	);
DECLARE_UPGD_FUN (  room_hall_entrance		);

DECLARE_CALC_FUN (  cval_mob_upgd_level		);
DECLARE_CALC_FUN (  mval_mob_upgd_level		);
DECLARE_UPGD_FUN (  mob_upgd_level		);
DECLARE_CALC_FUN (  cval_mob_upgd_hp		);
DECLARE_UPGD_FUN (  mob_upgd_hp			);
DECLARE_CALC_FUN (  cval_mob_upgd_mana		);
DECLARE_UPGD_FUN (  mob_upgd_mana		);
DECLARE_CALC_FUN (  cval_mob_upgd_damage	);
DECLARE_CALC_FUN (  mval_mob_upgd_damage	);
DECLARE_UPGD_FUN (  mob_upgd_damage		);
DECLARE_CALC_FUN (  cval_mob_upgd_imm		);
DECLARE_UPGD_FUN (  mob_upgd_imm		);
DECLARE_CALC_FUN (  cval_mob_upgd_res		);
DECLARE_UPGD_FUN (  mob_upgd_res		);
DECLARE_CALC_FUN (  cval_mob_upgd_off		);
DECLARE_UPGD_FUN (  mob_upgd_off		);
DECLARE_CALC_FUN (  cval_mob_upgd_damtype	);
DECLARE_UPGD_FUN (  mob_upgd_damtype		);
DECLARE_CALC_FUN (  cval_mob_upgd_ac		);
DECLARE_UPGD_FUN (  mob_upgd_ac			);
DECLARE_CALC_FUN (  cval_mob_upgd_perm		);
DECLARE_UPGD_FUN (  mob_upgd_perm		);

DECLARE_UPGD_FUN (  obj_upgd_furn_capacity	);
DECLARE_CALC_FUN (  cval_obj_upgd_v0		);
DECLARE_UPGD_FUN (  obj_upgd_v0			);
DECLARE_CALC_FUN (  cval_obj_upgd_v2		);
DECLARE_UPGD_FUN (  obj_upgd_v2			);
DECLARE_CALC_FUN (  cval_obj_upgd_v3		);
DECLARE_UPGD_FUN (  obj_upgd_v3			);
DECLARE_CALC_FUN (  cval_obj_upgd_v4		);
DECLARE_UPGD_FUN (  obj_upgd_v4			);
DECLARE_CALC_FUN (  cval_obj_upgd_add1_spell	);
DECLARE_UPGD_FUN (  obj_upgd_add1_spell		);
DECLARE_CALC_FUN (  cval_obj_upgd_add3_spell	);
DECLARE_UPGD_FUN (  obj_upgd_add3_spell		);
DECLARE_UPGD_FUN (  obj_upgd_charges		);
DECLARE_UPGD_FUN (  obj_upgd_damage		);
DECLARE_CALC_FUN (  cval_obj_upgd_damtype	);
DECLARE_UPGD_FUN (  obj_upgd_damtype		);
DECLARE_CALC_FUN (  cval_obj_upgd_wpnflag	);
DECLARE_UPGD_FUN (  obj_upgd_wpnflag		);
DECLARE_UPGD_FUN (  obj_upgd_cont_capacity	);
DECLARE_CALC_FUN (  cval_obj_upgd_affected	);
DECLARE_UPGD_FUN (  obj_upgd_affected		);
DECLARE_CALC_FUN (  cval_obj_upgd_objflag	);
DECLARE_UPGD_FUN (  obj_upgd_objflag		);
DECLARE_CALC_FUN (  cval_obj_upgd_wearloc	);
DECLARE_UPGD_FUN (  obj_upgd_wearloc		);

DECLARE_CALC_FUN (  cval_room_upgd_hpregen	);
DECLARE_UPGD_FUN (  room_upgd_hpregen		);
DECLARE_CALC_FUN (  cval_room_upgd_manaregen	);
DECLARE_UPGD_FUN (  room_upgd_manaregen		);
DECLARE_CALC_FUN (  cval_room_upgd_flag		);
DECLARE_UPGD_FUN (  room_upgd_flag		);

/* OLC */
void		cedit_done		( CHAR_DATA *ch );
void		show_cedit_commands	( CHAR_DATA *ch, char *argument, const struct cedit_cmd_type *table );
MOB_INDEX_DATA * get_cmob_by_type_name	( CHAR_DATA *ch, char *name );

DECLARE_CEDIT_FUN(  cedit_create	);
DECLARE_CEDIT_FUN(  cedit_delete	);
DECLARE_CEDIT_FUN(  cedit_deposit	);
DECLARE_CEDIT_FUN(  cedit_desc		);
DECLARE_CEDIT_FUN(  cedit_guild		);
DECLARE_CEDIT_FUN(  cedit_indep		);
DECLARE_CEDIT_FUN(  cedit_log		);
DECLARE_CEDIT_FUN(  cedit_member	);
DECLARE_CEDIT_FUN(  cedit_mob		);
DECLARE_CEDIT_FUN(  cedit_name		);
DECLARE_CEDIT_FUN(  cedit_obj		);
DECLARE_CEDIT_FUN(  cedit_petition	);
DECLARE_CEDIT_FUN(  cedit_pkill		);
DECLARE_CEDIT_FUN(  cedit_rank		);
DECLARE_CEDIT_FUN(  cedit_room		);
DECLARE_CEDIT_FUN(  cedit_show		);
DECLARE_CEDIT_FUN(  cedit_skill		);
DECLARE_CEDIT_FUN(  cedit_time		);
DECLARE_CEDIT_FUN(  cedit_war		);

DECLARE_CEDIT_FUN(  mcedit_buy		);
DECLARE_CEDIT_FUN(  mcedit_desc		);
DECLARE_CEDIT_FUN(  mcedit_long		);
DECLARE_CEDIT_FUN(  mcedit_short	);
DECLARE_CEDIT_FUN(  mcedit_show		);

DECLARE_CEDIT_FUN(  ocedit_buy		);
DECLARE_CEDIT_FUN(  ocedit_long		);
DECLARE_CEDIT_FUN(  ocedit_sell		);
DECLARE_CEDIT_FUN(  ocedit_short	);
DECLARE_CEDIT_FUN(  ocedit_show		);

DECLARE_CEDIT_FUN(  rcedit_buy		);
DECLARE_CEDIT_FUN(  rcedit_desc		);
DECLARE_CEDIT_FUN(  rcedit_exdesc	);
DECLARE_CEDIT_FUN(  rcedit_name		);
DECLARE_CEDIT_FUN(  rcedit_sell		);
DECLARE_CEDIT_FUN(  rcedit_show		);

DECLARE_CEDIT_FUN(  scedit_buy		);
DECLARE_CEDIT_FUN(  scedit_sell		);
DECLARE_CEDIT_FUN(  scedit_show		);

DECLARE_DO_FUN   (  cedit		);
DECLARE_DO_FUN   (  mcedit		);
DECLARE_DO_FUN   (  ocedit		);
DECLARE_DO_FUN   (  rcedit		);
DECLARE_DO_FUN   (  scedit		);

DECLARE_DO_FUN   (  do_cedit		);

/* Clan management */
DECLARE_DO_FUN	 (  do_guild		);
DECLARE_DO_FUN   (  do_newbie		);
DECLARE_DO_FUN	 (  do_notnewbie	);
DECLARE_DO_FUN	 (  do_go_pkill		);
DECLARE_DO_FUN	 (  do_petition		);
DECLARE_DO_FUN	 (  do_loner		);
bool		exile_player		( ROSTER_DATA *roster );

/* General commands */
DECLARE_DO_FUN	 (  do_ctalk		);
DECLARE_DO_FUN	 (  do_bounty		);

/* Charts */
DECLARE_DO_FUN	 (  do_clist		);
DECLARE_DO_FUN	 (  do_clan_roster	);
DECLARE_DO_FUN   (  do_rank		);
DECLARE_DO_FUN   (  do_pk_history	);

/* Checks */
bool		is_enemy		( CHAR_DATA *ch, CHAR_DATA *vch );
bool		clan_can_use		( CHAR_DATA *ch, OBJ_DATA *obj );
bool		is_clan_obj		( OBJ_DATA *obj );
bool		is_clan_obj_ind		( OBJ_INDEX_DATA *pObj );
bool		is_clead		( CHAR_DATA *ch );
bool		is_clan			( CHAR_DATA *ch );
bool		is_pkill		( CHAR_DATA *ch );
bool		is_indep		( CHAR_DATA *ch );
bool		is_full_clan_obj_index	( OBJ_INDEX_DATA *pObj, CLAN_DATA *clan, bool checkclan );
bool		is_full_clan_obj	( OBJ_DATA *obj, CLAN_DATA *clan, bool checkclan );
bool		is_clan_mob_index	( MOB_INDEX_DATA *pMob, CLAN_DATA *clan, bool checkclan );
bool		is_clan_mob		( CHAR_DATA *mob, CLAN_DATA *clan, bool checkclan );
bool		is_clan_mob_type	( CHAR_DATA *mob, CLAN_DATA *clan, int type );
bool		is_clan_room		( ROOM_INDEX_DATA *pRoom, CLAN_DATA *clan, bool checkclan );
bool		is_same_clan		( CHAR_DATA *ch, CHAR_DATA *vch );
bool		can_pkill		( CHAR_DATA *ch, CHAR_DATA *vch );
bool		can_wear_clan_eq	( CHAR_DATA *ch, OBJ_DATA *obj, bool remove );
void		update_pkhist		( ROSTER_DATA *chRoster, ROSTER_DATA *vchRoster, bool arena );
int		rank_kill_ch	(CHAR_DATA *ch, CHAR_DATA *vch, bool arena, bool simulate);
int		rank_kill		( ROSTER_DATA *chRoster, ROSTER_DATA *vchRoster, bool arena, bool simulate );
void		clan_entry_trigger	( CHAR_DATA *ch, bool greet );
bool		clan_has_skill(CLAN_DATA *clan, int skill);

/* Clan skills */
bool		interpret_clan_cmd	( CHAR_DATA *ch, char *command, char *argument );
DECLARE_DO_FUN	 (  do_adrenaline	);
DECLARE_DO_FUN	 (  do_battle_fury	);
DECLARE_DO_FUN	 (  do_bloodlust	);
DECLARE_DO_FUN	 (  do_conceal		);
DECLARE_DO_FUN	 (  do_cripple		);
DECLARE_DO_FUN	 (  do_divide		);
DECLARE_DO_FUN	 (  do_faith		);
DECLARE_DO_FUN	 (  do_modi		);
DECLARE_DO_FUN	 (  do_shadow_form	);
DECLARE_DO_FUN	 (  do_solar_flare	);
DECLARE_DO_FUN	 (  do_vmight		);
DECLARE_DO_FUN	 (  do_wform		);

/* Integration functions */
DECLARE_DO_FUN   (  make_rosters	);

/*************************************** TABLES ***************************************/

extern	const	struct log_type			log_entry_string[];
extern	const	struct clan_ranks		clan_rank_table[];
extern	const	struct vnum_table		cmob_table[];
extern	const	struct vnum_table		croom_table[];
extern	const	struct vnum_table		portloc_table[];
extern	const	char *				process_name_word[];
extern	const	struct cedit_cmd_type		main_cedit_table[];
extern	const	struct cedit_cmd_type		mob_cedit_table[];
extern	const	struct cedit_cmd_type		obj_cedit_table[];
extern	const	struct cedit_cmd_type		room_cedit_table[];
extern	const	struct cedit_cmd_type		skill_cedit_table[];
extern		struct clan_skill_cmds		clan_skill_table[];
extern		COST_DATA			mob_prices[];
extern		COST_DATA			obj_prices[];
extern		COST_DATA			room_prices[];
extern		COST_DATA			mob_upgd_prices[];
extern		COST_DATA			obj_upgd_prices[];
extern		COST_DATA			room_upgd_prices[];
extern		XCOST_DATA			spell_prices[];
extern		XCOST_DATA			bitaf_prices[];
extern		XCOST_DATA			wpnflag_prices[];
extern		XCOST_DATA			objflag_prices[];
extern		XCOST_DATA			damtype_prices[];
extern		XCOST_DATA			res_prices[];
extern		XCOST_DATA			imm_prices[];
extern		XCOST_DATA			off_prices[];


/********************************** GLOBAL VARIABLES **********************************/

extern	PKHIST_DATA *	pkhist_table;			// Spamkill data table
extern	int		max_pkhist_entries;		// Number of entries in the spamkill table

extern	PKILL_DATA *	pkill_data_free;		// Pointer to PKILL_DATA free list

extern	CLAN_DATA *	clan_list;			// Pointer to clan list
extern	CLAN_DATA *	clan_free;			// Pointer to CLAN_DATA free list
extern	int		max_clan;			// Number of present clans

extern	ROSTER_DATA *	roster_list;			// Pointer to roster list
extern	ROSTER_DATA *	roster_free;			// Pointer to ROSTER_DATA free list

extern	time_t		roster_next_update;		// Time of next update to make sure rosters update every so often
extern	time_t		roster_next_penalty;		// Time of next penalty check to make sure people are penalised for stuff every so often (heh)
