/***************************************************************************
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

//#include "port_include"

/*
 * Accommodate old non-Ansi compilers.
 */
#if defined(TRADITIONAL)
#define const
#define args( list )			( )
#define DECLARE_DO_FUN( fun )		void fun( )
#define DECLARE_SPEC_FUN( fun )		bool fun( )
#define DECLARE_SPELL_FUN( fun )	void fun( )
#else
#define args( list )			list
#define DECLARE_DO_FUN( fun )		DO_FUN    fun
#define DECLARE_SPEC_FUN( fun )		SPEC_FUN  fun
#define DECLARE_SPELL_FUN( fun )	SPELL_FUN fun
#endif

/* system calls */
int unlink();
int system();

/* mccp : support bits */
#include <zlib.h>
//#include <../../../home/bree3693/rot/src/zlib.h>
#define TELOPT_COMPRESS 85
#define COMPRESS_BUF_SIZE 16384

/* SQL
 #include <mysql.h>
 */

// #define SKYN_DEBUG

/*
 * Short scalar types.
 * Diavolo reports AIX compiler has bugs with short types.
 */
#if	!defined(FALSE)
#define FALSE	 0
#endif

#if	!defined(TRUE)
#define TRUE	 1
#endif

#if	defined(_AIX)
#if	!defined(const)
#define const
#endif
typedef int sh_int;
typedef int bool;
#define unix
#else
typedef int sh_int;
/* typedef short   int			sh_int; */
typedef unsigned char bool;
#endif

#define WWW_WHO		"????"
#define pktimer 22  /* Default one easier to change in future */

/*
 * Colour stuff by Lope of Loping Through The MUD
 */
#define CLEAR           "[0m"		/* Resets Colour        */
#define FAINT		"[2m"
#define ITALIC		"[3m"
#define UNDERLINE	"[4m"		/* Underline		*/
#define BLINK           "[5m"		/* Blink                */
#define NEGATIVE	"[7m"
#define B_BLACK         "[40m"	/* Background Colors - Tas */
#define B_RED           "[41m"
#define B_GREEN         "[42m"
#define B_YELLOW        "[43m"
#define B_BLUE          "[44m"
#define B_MAGENTA       "[45m"
#define B_CYAN          "[46m"
#define B_WHITE         "[47m"
#define B_D_GREY        "[7;1;30m"	/* Bright Background Colors - Tas */
#define B_B_RED         "[7;1;31m"
#define B_B_GREEN       "[7;1;32m"
#define B_B_YELLOW      "[7;1;33m"
#define B_B_BLUE        "[7;1;34m"
#define B_B_MAGENTA     "[7;1;35m"
#define B_B_CYAN        "[7;1;36m"
#define B_B_WHITE       "[7;1;37m"
#define C_BLACK		"[0m[30m"	/* Normal Colours       */
#define C_RED           "[0m[31m"
#define C_GREEN         "[0m[32m"
#define C_YELLOW        "[0m[33m"
#define C_BLUE          "[0m[34m"
#define C_MAGENTA       "[0m[35m"
#define C_CYAN          "[0m[36m"
#define C_WHITE         "[0m[37m"
#define C_D_GREY        "[1m[30m"	/* Light Colors         */
#define C_B_RED         "[1m[31m"
#define C_B_GREEN       "[1m[32m"
#define C_B_YELLOW      "[1m[33m"
#define C_B_BLUE        "[1m[34m"
#define C_B_MAGENTA     "[1m[35m"
#define C_B_CYAN        "[1m[36m"
#define C_B_WHITE       "[1m[37m"
#define R_BLACK		"[0m[0;30m"	/* Reset Colours       */
#define R_RED           "[0m[0;31m"
#define R_GREEN         "[0m[0;32m"
#define R_YELLOW        "[0m[0;33m"
#define R_BLUE          "[0m[0;34m"
#define R_MAGENTA       "[0m[0;35m"
#define R_CYAN          "[0m[0;36m"
#define R_WHITE         "[0m[0;37m"
#define R_D_GREY        "[0m[1;30m"	/* Reset Light         */
#define R_B_RED         "[0m[1;31m"
#define R_B_GREEN       "[0m[1;32m"
#define R_B_YELLOW      "[0m[1;33m"
#define R_B_BLUE        "[0m[1;34m"
#define R_B_MAGENTA     "[0m[1;35m"
#define R_B_CYAN        "[0m[1;36m"
#define R_B_WHITE       "[0m[1;37m"
#define BACKSPACE       "[1D"
#define BACKSPACEB      "[2D"
#define BACKSPACEC      "[3D"
#define BACKSPACED      "[4D"
#define BACKSPACEE      "[5D"
#define BACKSPACEF      "[6D"
#define BACKSPACEG      "[7D"
#define BACKSPACEH      "[8D"
#define BACKSPACEI      "[9D"
#define BACKSPACEJ      "[10D"

/*
 * Structure types.
 */
typedef struct affect_data AFFECT_DATA;
//typedef struct cooldown_data COOLDOWN_DATA;
typedef struct area_data AREA_DATA;
typedef struct disabled_data DISABLED_DATA;
typedef struct ban_data BAN_DATA;
typedef struct wiz_data WIZ_DATA;
typedef struct buf_type BUFFER;
typedef struct char_data CHAR_DATA;
typedef struct descriptor_data DESCRIPTOR_DATA;
typedef struct exit_data EXIT_DATA;
typedef struct extra_descr_data EXTRA_DESCR_DATA;
typedef struct help_data HELP_DATA;
typedef struct help_area_data HELP_AREA;
typedef struct kill_data KILL_DATA;
typedef struct mem_data MEM_DATA;
typedef struct mob_index_data MOB_INDEX_DATA;
typedef struct note_data NOTE_DATA;
typedef struct obj_data OBJ_DATA;
typedef struct obj_index_data OBJ_INDEX_DATA;
typedef struct pc_data PC_DATA;
typedef struct gen_data GEN_DATA;
typedef struct reset_data RESET_DATA;
typedef struct room_index_data ROOM_INDEX_DATA;
typedef struct shop_data SHOP_DATA;
typedef struct time_info_data TIME_INFO_DATA;
typedef struct weather_data WEATHER_DATA;
typedef struct mprog_list MPROG_LIST;
typedef struct mprog_code MPROG_CODE;
typedef struct arank_data ARANK_DATA;
typedef struct prank_data PRANK_DATA;

/* New Definitions */
typedef struct cln_data CLN_DATA;
typedef struct mbr_data MBR_DATA;

typedef struct grant_data GRANT_DATA;
typedef	struct	clan_data		CLAN_DATA;
typedef	struct	roster_data		ROSTER_DATA;

/*
 * Function types.
 */
typedef void DO_FUN( CHAR_DATA *ch, char *argument );
typedef bool SPEC_FUN( CHAR_DATA *ch );
typedef void SPELL_FUN( int sn, int level, CHAR_DATA *ch, void *vo, int target );

/*
 * String and memory management parameters.
 */
#define	MAX_KEY_HASH		 1024
#define MAX_STRING_LENGTH	 9216
#define MSL	MAX_STRING_LENGTH
#define MAX_INPUT_LENGTH	  256
#define MIL	MAX_INPUT_LENGTH
#define MAX_CHANNEL_LENGTH	 9216 /* added to support
					Long-discussion in 1 message 
						-Tas */
#define PAGELEN			   22
#define STRING( string )     char string[MSL/4];

/*
 * Game parameters.
 * Increase the max'es if you add more of something.
 * Adjust the pulse numbers to suit yourself.
 */
#define MAX_VNUM                  max_int
#define MAX_SOCIALS                1024
// I don't believe this number is accurate anymore.. Just giving some extras..
// Error checking should allow it to be okay - Marduk
//#define MAX_SKILL                  457
#define MAX_SKILL                  857
#define MAX_GROUP                  90
#define MAX_IN_GROUP		   20
#define MAX_TRACK		   20
#define MAX_LIQUID		   36
#define MAX_DAMAGE_MESSAGE         47   // 6 New added
#define MAX_ALIAS		   50 
#define MAX_FORGET		   15
#define MAX_DUPES		   20
#define MAX_CLASS                  35
#define MAX_PC_RACE                66
#define MAX_RANK                   8
#define MAX_LEVEL                  110
#define MAX_GRANT		   110
#define MAX_STAT		   31
#define MAX_STAT_NEW		   51
#define MAX_GOD                    23     /* Max GODS + 2 */
#define MAX_RARES                  7
#define LEVEL_HERO		   (MAX_LEVEL - 9)
#define LEVEL_IMMORTAL		   (MAX_LEVEL - 8)
#define ED_CLASS                   9

#define PULSE_PER_SECOND	    4
#define PULSE_VIOLENCE		  ( 4 * PULSE_PER_SECOND)
#define PULSE_MOBILE		  ( 6 * PULSE_PER_SECOND)
#define PULSE_MUSIC		  (15 * PULSE_PER_SECOND)
#define PULSE_TICK		  (30 * PULSE_PER_SECOND)
#define PULSE_AREA		  (45 * PULSE_PER_SECOND)
#define PULSE_QUEST               (30 * PULSE_PER_SECOND)
#define PULSE_MARKET              (500 * PULSE_PER_SECOND) //Bree
#define PULSE_TELEPORT		  (20 * PULSE_PER_SECOND) /* Froth */

#define IMPLEMENTOR		MAX_LEVEL
#define	CREATOR			(MAX_LEVEL - 1)
#define SUPREME			(MAX_LEVEL - 2)
#define DEITY			(MAX_LEVEL - 3)
#define GOD			(MAX_LEVEL - 4)
#define IMMORTAL		(MAX_LEVEL - 5)
#define DEMI			(MAX_LEVEL - 6)
#define KNIGHT			(MAX_LEVEL - 7)
#define SQUIRE			(MAX_LEVEL - 8)
#define HERO			LEVEL_HERO

#define VALUE_SILVER		0
#define VALUE_GOLD		1
#define VALUE_PLATINUM		2
#define VALUE_QUEST_POINT	3
#define MAX_AUCTION_PAYMENT	5

#define GAME_COL_NAME		"{rA{Rs{yg{Ya{Wr{Yd{yi{Ra{rn {8N{wig{Whtm{war{8e{x"
#define GAME_NAME			"Asgardian Nightmare"
#define GAME_VERSION		"3.1"
#define GAME_EXE			"asgard"
#define VALGRIND1		"./valgrind --tool=callgrind --instr-atstart=no"

/*
 * Site ban structure.
 */

#define BAN_SUFFIX		A
#define BAN_PREFIX		B
#define BAN_NEWBIES		C
#define BAN_ALL			D	
#define BAN_PERMIT		E
#define BAN_PERMANENT		F

//#define AFFECT_NONE         0
//#define AFFECT_SHOCK        1
//#define AFFECT_TELE         1
//#define AFFECT_SLOW         2
//#define AFFECT_POISON       3
//#define AFFECT_SLEEP        4
//#define AFFECT_CURSE        5
//#define AFFECT_BLIND        6
//#define AFFECT_PLAGUE       7
//#define AFFECT_WEAKEN       8
//#define AFFECT_BERSERK      9

struct ban_data
{
	BAN_DATA * next;
	bool valid;
	sh_int ban_flags;
	sh_int level;
	char * name;
};

struct disabled_data
{
	DISABLED_DATA *next; /* pointer to next node */
	struct cmd_type const *command; /* pointer to the command struct*/
	char *disabled_by; /* name of disabler */
	sh_int level; /* level of disabler */
};

struct wiz_data
{
	WIZ_DATA * next;
	bool valid;
	sh_int level;
	char * name;
	char * desc;
};

struct buf_type
{
	BUFFER * next;
	bool valid;
	sh_int state; /* error state of the buffer */
	sh_int size; /* size in k */
	char * string; /* buffer's string */
};

/*
 * Time and weather stuff.
 */
#define SUN_DARK		    0
#define SUN_RISE		    1
#define SUN_LIGHT		    2
#define SUN_SET			    3

#define SKY_CLOUDLESS		    0
#define SKY_CLOUDY		    1
#define SKY_RAINING		    2
#define SKY_LIGHTNING		    3

struct time_info_data
{
	int hour;
	int day;
	int month;
	int year;
};

struct weather_data
{
	int mmhg;
	int change;
	int sky;
	int sunlight;
};

/*
 * Structure for a command in the command lookup table.
 */
struct cmd_type
{
	char * const name;
	DO_FUN * do_fun;
	sh_int position;
	sh_int level;
	sh_int tier;
	sh_int log;
	sh_int show;
};

/*
 * Connected state for a channel.
 */
#define CON_PLAYING			 0
#define CON_GET_NAME			 1
#define CON_GET_OLD_PASSWORD		 2
#define CON_CONFIRM_NEW_NAME		 3
#define CON_GET_NEW_PASSWORD		 4
#define CON_CONFIRM_NEW_PASSWORD	 5
#define CON_GET_NEW_RACE		 6
#define CON_GET_NEW_SEX			 7
#define CON_GET_NEW_CLASS		 8
#define CON_GET_ALIGNMENT		 9
#define CON_GET_GOD                     10
#define CON_DEFAULT_CHOICE		11 
#define CON_GEN_GROUPS			12 
#define CON_PICK_WEAPON			13
#define CON_READ_IMOTD			14
#define CON_READ_MOTD			15
#define CON_BREAK_CONNECT		16
#define CON_COPYOVER_RECOVER            17

/*
 * Descriptor (channel) structure.
 */
struct descriptor_data
{
	DESCRIPTOR_DATA * next;
	DESCRIPTOR_DATA * snoop_by;
	CHAR_DATA * character;
	CHAR_DATA * original;
	bool valid;
	char * host;
	sh_int descriptor;
	sh_int connected;
	bool fcommand;
	char inbuf[4 * MAX_INPUT_LENGTH];
	char incomm[MAX_INPUT_LENGTH];
	char inlast[MAX_INPUT_LENGTH];
	int repeat;
	char * outbuf;
	int outsize;
	int outtop;
	char * showstr_head;
	char * showstr_point;
	z_stream * out_compress; /* mccp : support data */
	unsigned char * out_compress_buf;
	void * pEdit; /* OLC */
	void *		pCedit;
	char ** pString; /* OLC */
	int editor; /* OLC */
	char * clanEdit;
	char * hostip;
	bool wasCompressed; // Hack for copyover
};

/*
 * Attribute bonus structures.
 */
struct str_app_type
{
	sh_int tohit;
	sh_int todam;
	sh_int carry;
	sh_int wield;
};

struct int_app_type
{
	sh_int learn;
};

struct wis_app_type
{
	sh_int practice;
};

// Add tododge and to counterdodge
struct dex_app_type
{
	sh_int defensive;
//    sh_int      tospeed;
};

struct con_app_type
{
	sh_int hitp;
	sh_int shock;
};

struct grant_data
{
	GRANT_DATA * next;
	DO_FUN * do_fun;
	char * name;
	int duration;
	int level;
};

/*
 * Drunk struct
 */
struct struckdrunk
{
	int min_drunk_level;
	int number_of_rep;
	char *replacement[11];
};

/*
 * TO types for act.
 */
#define TO_ROOM		    0
#define TO_NOTVICT	    1
#define TO_VICT		    2
#define TO_CHAR		    3
#define TO_ALL		    4
#define TO_WORLD            5
#define TO_NONSHORT	    6

/*
 *  Types of Magic
 */
#define MAGIC_NONE  -1
#define MAGIC_WHITE  0   // Healing/Protection
#define MAGIC_RED    1   // Combat
#define MAGIC_BLUE   2   // Detection/Transportation/Creation
#define MAGIC_GREEN  3   // Enhance/Enchant
#define MAGIC_BLACK  4   // Evil--Take a guess
#define MAGIC_ORANGE 5   // Illusion/Benediction/Miscellaneous
#define IS_SPELL_WHITE(sn) (magic_table[sn] == MAGIC_WHITE) ? TRUE : FALSE
#define IS_SPELL_RED(sn) (magic_table[sn] == MAGIC_RED) ? TRUE : FALSE
#define IS_SPELL_BLUE(sn) (magic_table[sn] == MAGIC_BLUE) ? TRUE : FALSE
#define IS_SPELL_BLACK(sn) (magic_table[sn] == MAGIC_BLACK) ? TRUE : FALSE
#define IS_SPELL_GREEN(sn) (magic_table[sn] == MAGIC_GREEN) ? TRUE : FALSE
#define IS_SPELL_ORANGE(sn) (magic_table[sn] == MAGIC_ORANGE) ? TRUE : FALSE

/*
 * Help table types.
 
 struct	help_data
 {
 HELP_DATA *	next;
 sh_int	level;
 char *	keyword;
 char *	text;
 bool  delete;
 }; */

/*
 * New Help_Data for OLC
 */
struct help_data
{
	HELP_DATA * next;
	HELP_DATA * next_area;
	sh_int level;
	char * keyword;
	char * text;
};

struct help_area_data
{
	HELP_AREA * next;
	HELP_DATA * first;
	HELP_DATA * last;
	AREA_DATA * area;
	char * filename;
	bool changed;
};

/*
 * Shop types.
 */
#define MAX_TRADE	 5

struct shop_data
{
	SHOP_DATA * next; /* Next shop in list		*/
	sh_int keeper; /* Vnum of shop keeper mob	*/
	sh_int buy_type[MAX_TRADE]; /* Item types shop will buy	*/
	sh_int profit_buy; /* Cost multiplier for buying	*/
	sh_int profit_sell; /* Cost multiplier for selling	*/
	sh_int open_hour; /* First opening hour		*/
	sh_int close_hour; /* First closing hour		*/
};

/*
 * Per-class stuff.
 */

#define MAX_GUILD 	3
#define MAX_STATS 	5
#define STAT_STR 	0
#define STAT_INT	1
#define STAT_WIS	2
#define STAT_DEX	3
#define STAT_CON	4

struct class_type
{
	char * name; /* the full name of the class */
	char who_name[4]; /* Three-letter name for 'who'  */
	sh_int attr_prime; /* Prime attribute		*/
	sh_int weapon; /* First weapon			*/
	sh_int guild[MAX_GUILD]; /* Vnum of guild rooms		*/
	sh_int skill_adept; /* Maximum skill level		*/
	sh_int thac0_00; /* Thac0 for level  0		*/
	sh_int thac0_32; /* Thac0 for level 32		*/
	sh_int hp_min; /* Min hp gained on leveling	*/
	sh_int hp_max; /* Max hp gained on leveling	*/
	bool fMana; /* Class gains mana on level	*/
	char * base_group; /* base skills gained		*/
	char * default_group; /* default skills gained	*/
	sh_int finished; /* Finished Coding? */
};

/* For Class Finished */
#define NOT_STARTED   (A)
#define BASIC_START   (B)
#define SEMI_FINISHED (C)
#define FINISHED      (D)
#define REVAMPING     (E)

struct item_type
{
	int type;
	char * name;
};

struct weapon_type
{
	char * name;
	sh_int vnum;
	sh_int type;
	sh_int *gsn;
};

struct wiznet_type
{
	char * name;
	long flag;
	int level;
};

struct attack_type
{
	char * name; /* name */
	char * noun; /* message */
	int damage; /* damage class */
};

struct race_type
{
	char * name; /* call name of the race */
	bool pc_race; /* can be chosen by pcs */
	long act; /* act bits for the race */
	long aff; /* aff bits for the race */
	long off; /* off bits for the race */
	long imm; /* imm bits for the race */
	long res; /* res bits for the race */
	long vuln; /* vuln bits for the race */
	long shd; /* shd bits for the race */
	long form; /* default form flag for the race */
	long parts; /* default parts for the race */
	sh_int mod_true_stats[11]; /* Stat Add/Subt */
};

struct pc_race_type /* additional data for pc races */
{
	char * name; /* MUST be in race_type */
	char who_name[12];
	char * rare_race[MAX_RARES]; /* possible derived races */
	sh_int points; /* cost in points of the race */
	sh_int class_mult[MAX_CLASS]; /* exp multiplier for class, * 100 */
	char * skills[5]; /* bonus skills for the race */
	sh_int stats[MAX_STATS]; /* starting stats */
	sh_int max_stats[MAX_STATS]; /* maximum stats */
	sh_int size; /* size */
};

struct spec_type
{
	char * name; /* special function name */
	SPEC_FUN * function; /* the function */
};

/*
 * Data structure for notes.
 */

#define NOTE_NOTE	0
#define NOTE_IDEA	1
#define NOTE_PENALTY	2
#define NOTE_NEWS	3
#define NOTE_CHANGES	4
#define NOTE_SIGN	5
#define NOTE_WEDDINGS	6
#define NOTE_QUOTES	7

struct note_data
{
	NOTE_DATA * next;
	bool valid;
	sh_int type;
	char * sender;
	char * date;
	char * to_list;
	char * subject;
	char * text;
	time_t date_stamp;
};

/*
 * An affect.
 */
struct affect_data
{
	AFFECT_DATA * next;
	bool valid;
	sh_int where;
	sh_int type;
	sh_int level;
	sh_int duration;
	sh_int location;
	sh_int modifier;
	int bitvector;
};

/*
 * A cooldown.
 */
/*
struct cooldown_data
{
	COOLDOWN_DATA * next;
	bool valid;
	sh_int type;
	sh_int duration;
	int bitvector;
};
*/

/* where definitions */
#define TO_AFFECTS	0
#define TO_OBJECT	1
#define TO_IMMUNE	2
#define TO_RESIST	3
#define TO_VULN		4
#define TO_WEAPON	5
#define TO_SHIELDS	6
#define TO_ROOM_AFF     7
#define TO_AFFECTS2   8

/*
 * A kill structure (indexed by level).
 */
struct kill_data
{
	sh_int number;
	sh_int killed;
};

/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (Start of section ... start here)                     *
 *                                                                         *
 ***************************************************************************/

/*
 * Well known mob virtual numbers.
 * Defined in #MOBILES.
 */
#define MOB_VNUM_SAGE              3188
#define MOB_VNUM_FALCON		   3192
#define MOB_VNUM_FIDO		   3090
#define MOB_VNUM_CITYGUARD	   3060
#define MOB_VNUM_VAMPIRE	   3404
#define MOB_VNUM_GUARDIAN          3060

#define MOB_VNUM_PATROLMAN	   2106
#define MOB_VNUM_CORPSE		   3174
#define MOB_VNUM_ANIMATE	   3175
#define MOB_VNUM_FGOLEM            3193
#define MOB_VNUM_IGOLEM            3194
#define MOB_VNUM_DEMON		   3181
#define MOB_VNUM_CLAYTON	   10009
#define GROUP_VNUM_TROLLS	   2100
#define GROUP_VNUM_OGRES	   2101

/* RT ASCII conversions -- used so we can have letters in this file */

#define A		  	0x0001
#define B			0x0002
#define C			0x0004
#define D			0x0008
#define E			0x0010
#define F			0x0020
#define G			0x0040
#define H			0x0080

#define I			0x0100
#define J			0x0200
#define K			0x0400
#define L		 	0x0800
#define M			0x1000
#define N		 	0x2000
#define O			0x4000
#define P			0x8000

#define Q			0x00010000
#define R			0x00020000
#define S			0x00040000
#define T			0x00080000
#define U			0x00100000
#define V			0x00200000
#define W			0x00400000
#define X			0x00800000

#define Y			0x01000000
#define Z			0x02000000
#define aa			0x04000000
#define bb			0x08000000
#define cc			0x10000000
#define dd			0x20000000
#define ee             0x40000000
#define ff             0x80000000
#define gg             0x100000000

/* Stance defines */

#define STANCE_NONE            -1
#define STANCE_NORMAL           0
#define STANCE_VIPER            1
#define STANCE_CRANE            2
#define STANCE_CRAB             3
#define STANCE_MONGOOSE         4
#define STANCE_BULL             5
#define STANCE_MANTIS           6
#define STANCE_DRAGON           7
#define STANCE_TIGER            8
#define STANCE_MONKEY           9
#define STANCE_SWALLOW          10

/* Stance Flags */

#define STANCE_AFF_SPEED            (A)
#define STANCE_AFF_IMPROVED_BLOCK   (B)
#define STANCE_AFF_ENHANCED_BLOCK   (C)
#define STANCE_AFF_IMPROVED_DODGE   (D)
#define STANCE_AFF_ROLL             (E)
#define STANCE_AFF_IMPROVED_DEFENSE (F)
#define STANCE_AFF_ACROBATICS       (G)
#define STANCE_AFF_AGGRESSIVE       (H)
#define STANCE_AFF_COUNTER          (I)
#define STANCE_AFF_BYPASS           (J)
#define STANCE_AFF_BLOCK            (K)
#define STANCE_AFF_DOUBLE           (M)
#define STANCE_AFF_DOUBLE_SPEED     (N)
#define STANCE_AFF_DOUBLE_POWER     (O)

/*
 * ACT bits for mobs.
 * Used in #MOBILES.
 */
#define ACT_IS_NPC		(A)		/* Auto set for mobs	*/
#define ACT_SENTINEL	    	(B)		/* Stays in one room	*/
#define ACT_SCAVENGER	      	(C)		/* Picks up objects	*/
#define ACT_KEY			(D)
#define ACT_RANGER		(E)
#define ACT_AGGRESSIVE		(F)    		/* Attacks PC's		*/
#define ACT_STAY_AREA		(G)		/* Won't leave area	*/
#define ACT_WIMPY		(H)
#define ACT_PET			(I)		/* Auto set for pets	*/
#define ACT_TRAIN		(J)		/* Can train PC's	*/
#define ACT_PRACTICE		(K)		/* Can practice PC's	*/
#define ACT_DRUID		(L)
#define ACT_NO_BODY		(M)		/* Will not leave a corpse */
#define ACT_NB_DROP		(N)		/* Corpseless will drop all */
#define ACT_UNDEAD		(O)	
#define ACT_VAMPIRE		(P)
#define ACT_CLERIC		(Q)
#define ACT_MAGE		(R)
#define ACT_THIEF		(S)
#define ACT_WARRIOR		(T)
#define ACT_NOALIGN		(U)
#define ACT_NOPURGE		(V)
#define ACT_OUTDOORS		(W)
#define ACT_IS_SATAN		(X)
#define ACT_INDOORS		(Y)
#define ACT_IS_PRIEST		(Z)
#define ACT_IS_HEALER		(aa)
#define ACT_GAIN		(bb)
#define ACT_UPDATE_ALWAYS	(cc)
#define ACT_IS_BANKER		(dd)
#define ACT_CHALLENGER          (ee)
#define ACT_CHALLENGED          (ff)

/* damage classes */
#define DAM_NONE                0
#define DAM_BASH                1
#define DAM_PIERCE              2
#define DAM_SLASH               3
#define DAM_FIRE                4
#define DAM_COLD                5
#define DAM_LIGHTNING           6
#define DAM_ACID                7
#define DAM_POISON              8
#define DAM_NEGATIVE            9
#define DAM_HOLY                10
#define DAM_ENERGY              11
#define DAM_MENTAL              12
#define DAM_DISEASE             13
#define DAM_DROWNING            14
#define DAM_LIGHT		15
#define DAM_OTHER               16
#define DAM_HARM		17
#define DAM_CHARM		18
#define DAM_SOUND		19
#define DAM_ELEMENTAL           20 //new elemental dam type for shamans = pious
/* damage() function flags */
#define DF_NOREDUCE		0x0001
#define DF_NODODGE		0x0002
#define DF_NOPARRY		0x0004
#define DF_NOBLOCK		0x0008
#define DF_RESONATE		0x0010

/* dam_message_new() function flags -- Skyntil */
#define DMF_NONE     (A)
#define DMF_WHIP     (B)
#define DMF_DECAP    (C)
#define DMF_RESONATE (D)

/* OFF bits for mobiles */
#define OFF_AREA_ATTACK         (A)
#define OFF_BACKSTAB            (B)
#define OFF_BASH                (C)
#define OFF_BERSERK             (D)
#define OFF_DISARM              (E)
#define OFF_DODGE               (F)
#define OFF_FADE                (G)
#define OFF_FAST                (H)
#define OFF_KICK                (I)
#define OFF_KICK_DIRT           (J)
#define OFF_PARRY               (K)
#define OFF_RESCUE              (L)
#define OFF_TAIL                (M)
#define OFF_TRIP                (N)
#define OFF_CRUSH		(O)
#define ASSIST_ALL       	(P)
#define ASSIST_ALIGN	        (Q)
#define ASSIST_RACE    	     	(R)
#define ASSIST_PLAYERS      	(S)
#define ASSIST_GUARD        	(T)
#define ASSIST_VNUM		(U)
#define OFF_FEED		(V)
#define OFF_CLAN_GUARD		(W)
#define OFF_PHASE               (X)

/* return values for check_imm */
#define IS_NORMAL		0
#define IS_IMMUNE		1
#define IS_RESISTANT		2
#define IS_VULNERABLE		3

/* IMM bits for mobs */
#define IMM_SUMMON              (A)
#define IMM_CHARM               (B)
#define IMM_MAGIC               (C)
#define IMM_WEAPON              (D)
#define IMM_BASH                (E)
#define IMM_PIERCE              (F)
#define IMM_SLASH               (G)
#define IMM_FIRE                (H)
#define IMM_COLD                (I)
#define IMM_LIGHTNING           (J)
#define IMM_ACID                (K)
#define IMM_POISON              (L)
#define IMM_NEGATIVE            (M)
#define IMM_HOLY                (N)
#define IMM_ENERGY              (O)
#define IMM_MENTAL              (P)
#define IMM_DISEASE             (Q)
#define IMM_DROWNING            (R)
#define IMM_LIGHT		(S)
#define IMM_SOUND		(T)
#define IMM_WOOD                (X)
#define IMM_SILVER              (Y)
#define IMM_IRON                (Z)

/* RES bits for mobs */
#define RES_SUMMON		(A)
#define RES_CHARM		(B)
#define RES_MAGIC               (C)
#define RES_WEAPON              (D)
#define RES_BASH                (E)
#define RES_PIERCE              (F)
#define RES_SLASH               (G)
#define RES_FIRE                (H)
#define RES_COLD                (I)
#define RES_LIGHTNING           (J)
#define RES_ACID                (K)
#define RES_POISON              (L)
#define RES_NEGATIVE            (M)
#define RES_HOLY                (N)
#define RES_ENERGY              (O)
#define RES_MENTAL              (P)
#define RES_DISEASE             (Q)
#define RES_DROWNING            (R)
#define RES_LIGHT		(S)
#define RES_SOUND		(T)
#define RES_WOOD                (X)
#define RES_SILVER              (Y)
#define RES_IRON                (Z)

/* VULN bits for mobs */
#define VULN_SUMMON		(A)
#define VULN_CHARM		(B)
#define VULN_MAGIC              (C)
#define VULN_WEAPON             (D)
#define VULN_BASH               (E)
#define VULN_PIERCE             (F)
#define VULN_SLASH              (G)
#define VULN_FIRE               (H)
#define VULN_COLD               (I)
#define VULN_LIGHTNING          (J)
#define VULN_ACID               (K)
#define VULN_POISON             (L)
#define VULN_NEGATIVE           (M)
#define VULN_HOLY               (N)
#define VULN_ENERGY             (O)
#define VULN_MENTAL             (P)
#define VULN_DISEASE            (Q)
#define VULN_DROWNING           (R)
#define VULN_LIGHT		(S)
#define VULN_SOUND		(T)
#define VULN_WOOD               (X)
#define VULN_SILVER             (Y)
#define VULN_IRON		(Z)

/* body form */
#define FORM_EDIBLE             (A)
#define FORM_POISON             (B)
#define FORM_MAGICAL            (C)
#define FORM_INSTANT_DECAY      (D)
#define FORM_OTHER              (E)  /* defined by material bit */

/* actual form */
#define FORM_ANIMAL             (G)
#define FORM_SENTIENT           (H)
#define FORM_UNDEAD             (I)
#define FORM_CONSTRUCT          (J)
#define FORM_MIST               (K)
#define FORM_INTANGIBLE         (L)

#define FORM_BIPED              (M)
#define FORM_CENTAUR            (N)
#define FORM_INSECT             (O)
#define FORM_SPIDER             (P)
#define FORM_CRUSTACEAN         (Q)
#define FORM_WORM               (R)
#define FORM_BLOB		(S)

#define FORM_MAMMAL             (V)
#define FORM_BIRD               (W)
#define FORM_REPTILE            (X)
#define FORM_SNAKE              (Y)
#define FORM_DRAGON             (Z)
#define FORM_AMPHIBIAN          (aa)
#define FORM_FISH               (bb)
#define FORM_COLD_BLOOD		(cc)	

/* body parts */
#define PART_HEAD               (A)
#define PART_ARMS               (B)
#define PART_LEGS               (C)
#define PART_HEART              (D)
#define PART_BRAINS             (E)
#define PART_GUTS               (F)
#define PART_HANDS              (G)
#define PART_FEET               (H)
#define PART_FINGERS            (I)
#define PART_EAR                (J)
#define PART_EYE		(K)
#define PART_LONG_TONGUE        (L)
#define PART_EYESTALKS          (M)
#define PART_TENTACLES          (N)
#define PART_FINS               (O)
#define PART_WINGS              (P)
#define PART_TAIL               (Q)
/* for combat */
#define PART_CLAWS              (U)
#define PART_FANGS              (V)
#define PART_HORNS              (W)
#define PART_SCALES             (X)
#define PART_TUSKS		(Y)

/*
 * Bits for 'affected_by'.
 * Used in #MOBILES.
 */
#define AFF_BLIND		(A)
#define AFF_DETECT_LOCATION	(B)
#define AFF_DETECT_EVIL		(C)
#define AFF_DETECT_INVIS	(D)
#define AFF_DETECT_MAGIC	(E)
#define AFF_DETECT_MOTION	(F)
#define AFF_DETECT_GOOD		(G)
#define AFF_DISPLACE		(H)
#define AFF_FAERIE_FIRE		(I)
#define AFF_INFRARED		(J)
#define AFF_CURSE		(K)
#define AFF_FARSIGHT		(L)
#define AFF_POISON		(M)
#define AFF_DEAF		(N)
#define AFF_SNEAK		(P)
#define AFF_HIDE		(Q)
#define AFF_SLEEP		(R)
#define AFF_CHARM		(S)
#define AFF_FLYING		(T)
#define AFF_PASS_DOOR		(U)
#define AFF_HASTE		(V)
#define AFF_CALM		(W)
#define AFF_PLAGUE		(X)
#define AFF_WEAKEN		(Y)
#define AFF_DARK_VISION		(Z)
#define AFF_BERSERK		(aa)
#define AFF_SWIM		(bb)
#define AFF_REGENERATION        (cc)
#define AFF_SLOW		(dd)
#define AFF_REMORT              (ee)
#define AFF_FEAR		(ff)
//#define AFF_INSOMNIA            (gg)
/*
 *affects2 for additional affects
 */
#define AFF2_GARROTE (A) 

/*
 * More Bits for 'shielded_by'.
 * Used in #MOBILES.
 */
#define SHD_PROTECT_VOODOO	(A)
#define SHD_INVISIBLE		(B)
#define SHD_ICE			(C)
#define SHD_FIRE		(D)
#define SHD_SHOCK		(E)
#define SHD_DEATH		(F)
#define SHD_SANCTUARY		(H)
#define SHD_NOPRAY              (K)
#define SHD_PROTECT_EVIL	(N)
#define SHD_PROTECT_GOOD	(O)
#define SHD_PROTECT_AURA        (P)
#define SHD_STONE               (Q)
#define SHD_STEEL               (R)
#define SHD_INSOMNIA            (S)
/*
 * Sex.
 * Used in #MOBILES.
 */
#define SEX_NEUTRAL		      0
#define SEX_MALE		      1
#define SEX_FEMALE		      2

/* AC types */
#define AC_PIERCE			0
#define AC_BASH				1
#define AC_SLASH			2
#define AC_EXOTIC			3

/* dice */
#define DICE_NUMBER			0
#define DICE_TYPE			1
#define DICE_BONUS			2

/* size */
#define SIZE_TINY			0
#define SIZE_SMALL			1
#define SIZE_MEDIUM			2
#define SIZE_LARGE			3
#define SIZE_HUGE			4
#define SIZE_GIANT			5

/*
 * Well known object virtual numbers.
 * Defined in #OBJECTS.
 */
#define OBJ_VNUM_SILVER_ONE	      1
#define OBJ_VNUM_GOLD_ONE	      2
#define OBJ_VNUM_GOLD_SOME	      3
#define OBJ_VNUM_SILVER_SOME	      4
#define OBJ_VNUM_COINS		      5
#define OBJ_VNUM_PLATINUM_ONE	      6
#define OBJ_VNUM_PLATINUM_SOME	      7

#define OBJ_VNUM_CORPSE_NPC	     10
#define OBJ_VNUM_CORPSE_PC	     11

/* Body Parts */
#define OBJ_VNUM_SEVERED_HEAD	     12
#define OBJ_VNUM_TORN_HEART	     13
#define OBJ_VNUM_SLICED_ARM	     14
#define OBJ_VNUM_SLICED_LEG	     15
#define OBJ_VNUM_GUTS		     16
#define OBJ_VNUM_BRAINS		     17
#define OBJ_VNUM_BLOOD		     18
#define OBJ_VNUM_EYEBALL             30

#define OBJ_VNUM_MUSHROOM	     20
#define OBJ_VNUM_LIGHT_BALL	     21
#define OBJ_VNUM_SPRING		     22
#define OBJ_VNUM_DISC		     23
#define OBJ_VNUM_PORTAL		     25
#define OBJ_VNUM_VOODOO		     51
#define OBJ_VNUM_EXIT		     52
#define OBJ_VNUM_CHAIN		     53
#define OBJ_VNUM_BAG		     54
#define OBJ_VNUM_PASSBOOK	     55
#define OBJ_VNUM_STEAK		     56
#define OBJ_VNUM_GENERIC             67
#define OBJ_VNUM_ROSE		   1001

#define OBJ_VNUM_PIT		   3010

#define OBJ_VNUM_SCHOOL_MACE	   3700
#define OBJ_VNUM_SCHOOL_DAGGER	   3701
#define OBJ_VNUM_SCHOOL_SWORD	   3702
#define OBJ_VNUM_SCHOOL_SPEAR	   3717
#define OBJ_VNUM_SCHOOL_STAFF	   3718
#define OBJ_VNUM_SCHOOL_AXE	   3719
#define OBJ_VNUM_SCHOOL_FLAIL	   3720
#define OBJ_VNUM_SCHOOL_WHIP	   3721
#define OBJ_VNUM_SCHOOL_POLEARM    3722

#define OBJ_VNUM_SCHOOL_VEST	   3703
#define OBJ_VNUM_SCHOOL_SHIELD	   3704
#define OBJ_VNUM_SCHOOL_BANNER     3716
#define OBJ_VNUM_MAP		   3162
#define OBJ_VNUM_WMAP		   3163
#define OBJ_VNUM_EMAP		   3169

#define OBJ_VNUM_WHISTLE	   2116

#define OBJ_VNUM_QUEST_SIGN	   1118
#define	OBJ_VNUM_POTION		   3384
#define	OBJ_VNUM_SCROLL		   3385

#define	OBJ_VNUM_QDIAMOND	   3388
#define	OBJ_VNUM_CUBIC		   3386
#define	OBJ_VNUM_DPOUCH		   3383
#define	OBJ_VNUM_CPOUCH		   3387
#define OBJ_VNUM_DIAMOND	   3377

#define OBJ_VNUM_SURVIVAL_PACK	   1203
#define OBJ_VNUM_SURVIVAL_A	   1304
#define OBJ_VNUM_SURVIVAL_B	   1507
#define OBJ_VNUM_SURVIVAL_C	   3023
#define OBJ_VNUM_SURVIVAL_D	   3072
#define OBJ_VNUM_SURVIVAL_E	   3076
#define OBJ_VNUM_SURVIVAL_F	   3077
#define OBJ_VNUM_SURVIVAL_G	   3097
#define OBJ_VNUM_SURVIVAL_H	   3111
#define OBJ_VNUM_SURVIVAL_I	   3112
#define OBJ_VNUM_SURVIVAL_J	   3113
#define OBJ_VNUM_SURVIVAL_K	   3162
#define OBJ_VNUM_SURVIVAL_L	   3351
#define OBJ_VNUM_SURVIVAL_M	   3357
#define OBJ_VNUM_SURVIVAL_N	   3362
#define OBJ_VNUM_SURVIVAL_O	   3504
#define OBJ_VNUM_SURVIVAL_P	   5232
#define OBJ_VNUM_SURVIVAL_Q	   5318
#define OBJ_VNUM_SURVIVAL_R	   6601
#define OBJ_VNUM_SURVIVAL_S	   7800
#define OBJ_VNUM_SURVIVAL_T	   7801
#define OBJ_VNUM_SURVIVAL_U	   7802
#define OBJ_VNUM_SURVIVAL_V	   7803
#define OBJ_VNUM_SURVIVAL_W	   9222
#define OBJ_VNUM_SURVIVAL_X	   9224
/*Stock Certificates -Bree
 #define OBJ_VNUM_STOCK_A           47501 //Bree Inc.
 #define OBJ_VNUM_STOCK_B           47502 //Fesdor Unlimited
 #define OBJ_VNUM_STOCK_C           47504 //Grandiuer
 #define OBJ_VNUM_STOCK_D           47503 //Asgard Pharmy
 #define OBJ_VNUM_STOCK_E           47505 //bob's babbles
 #define OBJ_VNUM_STOCK_F           47506 //aesir
 #define OBJ_VNUM_STOCK_G           47507 //conquest
 #define OBJ_VNUM_STOCK_H           47508 //kyuubi
 #define OBJ_VNUM_STOCK_I           47509 //Enforcer
 #define OBJ_VNUM_STOCK_J           47510 //aduentia
 */
/*
 * Item types.
 * Used in #OBJECTS.
 */
#define ITEM_LIGHT		      1
#define ITEM_SCROLL		      2
#define ITEM_WAND		      3
#define ITEM_STAFF		      4
#define ITEM_WEAPON		      5
#define ITEM_TREASURE		      8
#define ITEM_ARMOR		      9
#define ITEM_POTION		     10
#define ITEM_CLOTHING		     11
#define ITEM_FURNITURE		     12
#define ITEM_TRASH		     13
#define ITEM_CONTAINER		     15
#define ITEM_DRINK_CON		     17
#define ITEM_KEY		     18
#define ITEM_FOOD		     19
#define ITEM_MONEY		     20
#define ITEM_BOAT		     22
#define ITEM_CORPSE_NPC		     23
#define ITEM_CORPSE_PC		     24
#define ITEM_FOUNTAIN		     25
#define ITEM_PILL		     26
#define ITEM_PROTECT		     27
#define ITEM_MAP		     28
#define ITEM_PORTAL		     29
#define ITEM_WARP_STONE		     30
#define ITEM_ROOM_KEY		     31
#define ITEM_GEM		     32
#define ITEM_JEWELRY		     33
#define ITEM_JUKEBOX		     34
#define ITEM_DEMON_STONE	     35
#define ITEM_EXIT		     36
#define ITEM_PIT		     37
#define ITEM_PASSBOOK		     38
#define ITEM_SKELETON                39 /* for necros */
#define ITEM_SLOT_MACHINE	     40

/*
 * Extra flags.
 * Used in #OBJECTS.
 */
#define ITEM_GLOW		(A)
#define ITEM_HUM		(B)
#define ITEM_DARK		(C)
#define ITEM_LOCK		(D)
#define ITEM_EVIL		(E)
#define ITEM_INVIS		(F)
#define ITEM_MAGIC		(G)
#define ITEM_NODROP		(H)
#define ITEM_BLESS		(I)
#define ITEM_ANTI_GOOD		(J)
#define ITEM_ANTI_EVIL		(K)
#define ITEM_ANTI_NEUTRAL	(L)
#define ITEM_NOREMOVE		(M)
#define ITEM_INVENTORY		(N)
#define ITEM_NOPURGE		(O)
#define ITEM_ROT_DEATH		(P)
#define ITEM_VIS_DEATH		(Q)
#define ITEM_NOSAC		(R)
#define ITEM_NONMETAL		(S)
#define ITEM_NOLOCATE		(T)
#define ITEM_MELT_DROP		(U)
#define ITEM_HAD_TIMER		(V)
#define ITEM_SELL_EXTRACT	(W)
#define ITEM_BURN_PROOF		(Y)
#define ITEM_NOUNCURSE		(Z)
#define ITEM_QUEST		(aa)
#define ITEM_FORCED		(bb)
#define ITEM_QUESTPOINT		(cc)
#define ITEM_NORESTRING		(dd)
#define ITEM_CLAN               (ee) /* Remort Items */
#define ITEM_NOENCHANT          (ff) /* no enchant */
/*
 * Wear flags.
 * Used in #OBJECTS.
 */
#define ITEM_TAKE		(A)
#define ITEM_WEAR_FINGER	(B)
#define ITEM_WEAR_NECK		(C)
#define ITEM_WEAR_BODY		(D)
#define ITEM_WEAR_HEAD		(E)
#define ITEM_WEAR_LEGS		(F)
#define ITEM_WEAR_FEET		(G)
#define ITEM_WEAR_HANDS		(H)
#define ITEM_WEAR_ARMS		(I)
#define ITEM_WEAR_SHIELD	(J)
#define ITEM_WEAR_ABOUT		(K)
#define ITEM_WEAR_WAIST		(L)
#define ITEM_WEAR_WRIST		(M)
#define ITEM_WIELD		(N)
#define ITEM_HOLD		(O)
#define ITEM_RESERVED		(P)
#define ITEM_WEAR_FLOAT		(Q)
#define ITEM_WEAR_FACE		(R)
#define ITEM_WEAR_EAR           (S)
#define ITEM_WEAR_ANKLE         (T)
#define ITEM_WEAR_CLAN          (U)
#define ITEM_WEAR_RELIG         (V)
#define ITEM_WEAR_PATCH         (W)
#define ITEM_WEAR_BACK          (X)
#define ITEM_WEAR_RNOSE          (Y) \\new right nose ring item Pious
#define ITEM_WEAR_LNOSE          (Z) \\new left nose ring item  Pious
/* weapon class */
#define WEAPON_EXOTIC		0
#define WEAPON_SWORD		1
#define WEAPON_DAGGER		2
#define WEAPON_SPEAR		3
#define WEAPON_MACE		4
#define WEAPON_AXE		5
#define WEAPON_FLAIL		6
#define WEAPON_WHIP		7	
#define WEAPON_POLEARM		8

#define MAX_WEAPON              8

/* weapon types */
#define WEAPON_FLAMING		(A)
#define WEAPON_FROST		(B)
#define WEAPON_VAMPIRIC		(C)
#define WEAPON_SHARP		(D)
#define WEAPON_VORPAL		(E)
#define WEAPON_TWO_HANDS	(F)
#define WEAPON_SHOCKING		(G)
#define WEAPON_POISON		(H)
#define WEAPON_ACIDIC           (I)
#define WEAPON_OSMOSIS          (J)
#define WEAPON_LEGENDARY        (k) //Weapon type legendary Pious @ new lgdary gear
#define WEAPON_MYTHIC           (l)
#define WEAPON_ELEMENTAL        (m) // shit here
#define WEAPON_ENERGYDRAIN      (N) 

/* gate flags */
#define GATE_NORMAL_EXIT	(A)
#define GATE_NOCURSE		(B)
#define GATE_GOWITH		(C)
#define GATE_BUGGY		(D)
#define GATE_RANDOM		(E)

/* furniture flags */
#define STAND_AT		(A)
#define STAND_ON		(B)
#define STAND_IN		(C)
#define SIT_AT			(D)
#define SIT_ON			(E)
#define SIT_IN			(F)
#define REST_AT			(G)
#define REST_ON			(H)
#define REST_IN			(I)
#define SLEEP_AT		(J)
#define SLEEP_ON		(K)
#define SLEEP_IN		(L)
#define PUT_AT			(M)
#define PUT_ON			(N)
#define PUT_IN			(O)
#define PUT_INSIDE		(P)

/*
 * Apply types (for affects).
 * Used in #OBJECTS.
 */
#define APPLY_NONE		      0
#define APPLY_STR		      1
#define APPLY_DEX		      2
#define APPLY_INT		      3
#define APPLY_WIS		      4
#define APPLY_CON		      5
#define APPLY_SEX		      6
#define APPLY_CLASS		      7
#define APPLY_LEVEL		      8
#define APPLY_AGE		      9
#define APPLY_HEIGHT		     10
#define APPLY_WEIGHT		     11
#define APPLY_MANA		     12
#define APPLY_HIT		     13
#define APPLY_MOVE		     14
#define APPLY_GOLD		     15
#define APPLY_EXP		     16
#define APPLY_AC		     17
#define APPLY_HITROLL		     18
#define APPLY_DAMROLL		     19
#define APPLY_SAVES		     20
#define APPLY_SAVING_PARA	     20
#define APPLY_SAVING_ROD	     21
#define APPLY_SAVING_PETRI	     22
#define APPLY_SAVING_BREATH	     23
#define APPLY_SAVING_SPELL	     24
#define APPLY_SPELL_AFFECT	     25
#define APPLY_REGEN                  26
#define APPLY_MORPH_FORM             27
#define APPLY_MANA_REGEN	     28
#define APPLY_CRIPPLE_HEAD           29
#define APPLY_CRIPPLE_ARM            30
#define APPLY_CRIPPLE_LEG            31
#define APPLY_MYTHIC                 32
//#define APPLY_INSOMNIA               33
/*
 * Values for containers (value[1]).
 * Used in #OBJECTS.
 */
#define CONT_CLOSEABLE		      1
#define CONT_PICKPROOF		      2
#define CONT_CLOSED		      4
#define CONT_LOCKED		      8
#define CONT_PUT_ON		     16

/*
 * Well known room virtual numbers.
 * Defined in #ROOMS.
 */
#define ROOM_VNUM_LIMBO		      2
#define ROOM_VNUM_CORNER	      3
#define ROOM_VNUM_CHAT		   1200
#define ROOM_VNUM_TEMPLE	   3001
#define ROOM_VNUM_TEMPLEB	   3365
#define ROOM_VNUM_PIT		   3367
#define ROOM_VNUM_ALTAR		   3054
#define ROOM_VNUM_ALTARB	   3366
#define ROOM_VNUM_SCHOOL	   3700
#define ROOM_VNUM_BALANCE	   4500
#define ROOM_VNUM_CIRCLE	   4400
#define ROOM_VNUM_DEMISE	   4201
#define ROOM_VNUM_HONOR		   4300
#define ROOM_VNUM_CHAIN		   7914
#define ROOM_VNUM_AWINNER          109
#define ROOM_VNUM_ALOSER           110
#define ROOM_VNUM_CLANS		  20000
#define ROOM_VNUM_MORGUE         30201
#define ROOM_VNUM_GODROOM        9804
#define SINGLE_WAR_WAITING_ROOM  9804
#define ROOM_ARENA_LO		100
#define ROOM_ARENA_HI	 	129	
#define ROOM_ARENA_MORGUE 	131	
#define ROOM_ARENA_LOUNGE 	130	

/*
 * Clan Rooms
 */
#define ROOM_VNUM_GUARDIANS	   20050
#define ROOM_VNUM_UNFORGIV	   20100
#define ROOM_VNUM_DARKMIST	   20150
#define ROOM_VNUM_KANE		   20200
#define ROOM_VNUM_JEDI		   20250
#define ROOM_VNUM_MIDNIGHT	   20300
#define ROOM_VNUM_JUDGES	   20350
#define ROOM_VNUM_ARCANES	   20400
#define ROOM_VNUM_ANGELS	   20450
#define ROOM_VNUM_KINDRED	   20500
#define ROOM_VNUM_HAWX		   20550
#define ROOM_MORGUE                30201

/*
 * Clan Room Entrances
 */
#define ENT_VNUM_GUARDIANS	   20051
#define ENT_VNUM_UNFORGIV	   20101
#define ENT_VNUM_DARKMIST	   20151
#define ENT_VNUM_KANE		   20201
#define ENT_VNUM_JEDI		   20251
#define ENT_VNUM_MIDNIGHT	   20301
#define ENT_VNUM_JUDGES		   20351
#define ENT_VNUM_ARCANES	   20401
#define ENT_VNUM_ANGELS		   20451
#define ENT_VNUM_KINDRED	   20501
#define ENT_VNUM_HAWX		   20551

/*
 * Clan Pits
 */
#define OBJ_PIT_GUARDIANS	   20099
#define OBJ_PIT_UNFORGIV	   20149
#define OBJ_PIT_DARKMIST	   20199
#define OBJ_PIT_KANE		   20249
#define OBJ_PIT_JEDI		   20299
#define OBJ_PIT_MIDNIGHT	   20349
#define OBJ_PIT_JUDGES		   20399
#define OBJ_PIT_ARCANES		   20449
#define OBJ_PIT_ANGELS		   20499
#define OBJ_PIT_KINDRED		   20549
#define OBJ_PIT_HAWX		   20599

/*
 * Room flags.
 * Used in #ROOMS.
 */
#define ROOM_DARK		(A)
#define ROOM_NO_MOB		(C)
#define ROOM_INDOORS		(D)

#define ROOM_TELEPORT   (G)

#define ROOM_SHOP               (H)
#define ROOM_NO_MAGIC           (I)
#define ROOM_PRIVATE		(J)
#define ROOM_SAFE		(K)
#define ROOM_SOLITARY		(L)
#define ROOM_PET_SHOP		(M)
#define ROOM_NO_RECALL		(N)
#define ROOM_IMP_ONLY		(O)
#define ROOM_GODS_ONLY		(P)
#define ROOM_HEROES_ONLY	(Q)
#define ROOM_NEWBIES_ONLY	(R)
#define ROOM_LAW		(S)
#define ROOM_NOWHERE		(T)
#define ROOM_CLAN_ENT		(U)
#define ROOM_LOCKED		(X)
#define ROOM_TRAP               (Y)
#define ROOM_ARENA              (Z)
#define ROOM_REMORT             (aa)
#define ROOM_SILENCED           (bb)
#define ROOM_ON_FIRE		(cc)

/*
 * Room Affected_By Flags  -- New -- Skyntil
 */
#define ROOM_AFF_SAFE           (A)  /* For the Sanctuary Spell */
/* Below this line not coded */
#define ROOM_AFF_DARKNESS       (B)  /* Gotta have this */
#define ROOM_AFF_PROTECT        (C)  /* Globe for all? */
#define ROOM_AFF_SLOW           (D)  /* Lethargy */
#define ROOM_AFF_SPEED          (E)  /* Haste */
#define ROOM_AFF_DAMAGE         (F)  /* Goes by Room Skill */
#define ROOM_AFF_FAERIE         (G)  /* Faerie Fog */
#define ROOM_AFF_SMOKE          (H)  /* Smokebomb */
#define ROOM_AFF_FIRES          (I)  /* Fires - Torment */
#define ROOM_AFF_OVERGROWN		(J)	 /* Overgrowth - Fesdor */
/* Length of wait for fear -- Sorn */
#define DEF_FEAR_WAIT		5

/*
 * Directions.
 * Used in #ROOMS.
 */
#define DIR_NORTH		      0
#define DIR_EAST		      1
#define DIR_SOUTH		      2
#define DIR_WEST		      3
#define DIR_UP			      4
#define DIR_DOWN		      5

/*
 * Exit flags.
 * Used in #ROOMS.
 */
#define EX_ISDOOR		      (A)
#define EX_CLOSED		      (B)
#define EX_LOCKED		      (C)
#define EX_PICKPROOF		      (F)
#define EX_NOPASS		      (G)
#define EX_EASY			      (H)
#define EX_HARD			      (I)
#define EX_INFURIATING		      (J)
#define EX_NOCLOSE		      (K)
#define EX_NOLOCK		      (L)
#define EX_HIDDEN                     (M)

/*
 * Sector types.
 * Used in #ROOMS.
 */
#define SECT_INSIDE		      0
#define SECT_CITY		      1
#define SECT_FIELD		      2
#define SECT_FOREST		      3
#define SECT_HILLS		      4
#define SECT_MOUNTAIN		      5
#define SECT_WATER_SWIM		      6
#define SECT_WATER_NOSWIM	      7
#define SECT_UNUSED		      8
#define SECT_AIR		      9
#define SECT_DESERT		     10
#define SECT_MAX		     11

/*
 * Equpiment wear locations.
 * Used in #RESETS.
 */
#define WEAR_NONE		     -1
#define WEAR_LIGHT		      0
#define WEAR_FINGER_L		      1
#define WEAR_FINGER_R		      2
#define WEAR_NECK_1		      3
#define WEAR_NECK_2		      4
#define WEAR_BODY		      5
#define WEAR_HEAD		      6
#define WEAR_LEGS		      7
#define WEAR_FEET		      8
#define WEAR_HANDS		      9
#define WEAR_ARMS		     10
#define WEAR_SHIELD		     11
#define WEAR_ABOUT		     12
#define WEAR_WAIST		     13
#define WEAR_WRIST_L		     14
#define WEAR_WRIST_R		     15
#define WEAR_WIELD		     16
#define WEAR_HOLD		     17
#define WEAR_FLOAT		     18
#define WEAR_SECONDARY		     19
#define WEAR_FACE		     20
#define WEAR_EAR                     21
#define WEAR_RESERVED                22
#define WEAR_ANKLE_L                 23
#define WEAR_ANKLE_R                 24
#define WEAR_CLAN                    25
#define WEAR_RELIG                   26
#define WEAR_PATCH                   27
#define WEAR_BACK                    28
#define MAX_WEAR		     29
#define WEAR_NOSE_R                  30
#define WEAR_NOSE_L                  31
#define WEAR_CURSED                  32

/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (End of this section ... stop here)                   *
 *                                                                         *
 ***************************************************************************/

/*
 * Conditions.
 */
#define COND_DRUNK		      0
#define COND_FULL		      1
#define COND_THIRST		      2
#define COND_HUNGER		      3

/*
 * Positions.
 */
#define POS_DEAD		      0
#define POS_MORTAL		      1
#define POS_INCAP		      2
#define POS_STUNNED		      3
#define POS_SLEEPING		      4
#define POS_RESTING		      5
#define POS_SITTING		      6
#define POS_FIGHTING		      7
#define POS_STANDING		      8

#define FIGHT_OPEN                    0
#define FIGHT_START                   1
#define FIGHT_BUSY                    2
#define FIGHT_LOCK                    3

/*
 * ACT bits for players.
 */
#define PLR_IS_NPC		(A)		/* Don't EVER set.	*/

/*
 * Colour stuff by Lope of Loping Through The MUD
 */
#define PLR_COLOUR		(B)

/* RT auto flags */
#define PLR_AUTOASSIST		(C)
#define PLR_AUTOEXIT		(D)
#define PLR_AUTOLOOT		(E)
#define PLR_AUTOSAC             (F)
#define PLR_AUTOGOLD		(G)
#define PLR_AUTOSPLIT		(H)

/* RT personal flags */
#define PLR_NOTRAN		(I)
#define PLR_AUTOPEEK		(J)
#define PLR_NOCANCEL		(K)
#define PLR_NORESTORE		(L)
#define PLR_NOTITLE		(M)
#define PLR_HOLYLIGHT		(N)
#define PLR_BRIEF_COMBAT        (O)
#define PLR_CANLOOT		(P)
#define PLR_NOSUMMON		(Q)
#define PLR_NOFOLLOW		(R)
#define PLR_QUESTOR             (S)
#define PLR_NOVIOLATE           (T)
#define PLR_NOEVOLVE		(V)
/* penalty flags */
#define PLR_PERMIT		(U)
#define PLR_LOG			(W)
#define PLR_DENY		(X)
#define PLR_FREEZE		(Y)
#define PLR_TWIT		(Z)
#define PLR_SHORT_COMBAT	(aa)
#define PLR_KEY			(bb)
#define PLR_NOCLAN		(cc)
#define PLR_REROLL		(dd)

#define PLR_NOCGOSSIP		(ee)
#define PLR_ARENA		(ff)
#define PLR_NOGOTO              (ff)
#define PLR_IS_BUILDER          (gg)

/* PLAYER flags - Skyntil */
#define PLAYER_GHOST            (A)    /* After a PKILL */
#define PLAYER_REMORT           (B)    /* During REMORT */
#define PLAYER_LOG_WATCH        (C)    /* See DEBUG and LOG messages */
#define PLAYER_RUNNING          (D)    /* During Run */
#define PLAYER_MORPHED          (E)    /* When Morphed */
#define PLAYER_ANCIENT_ENEMY    (F)
#define PLAYER_WARRIOR          (G)
#define PLAYER_THIEF            (H)
#define PLAYER_SUBDUE           (I)    /* This for Subdue Code */
#define PLAYER_STAT_FOCUS_FIXED (J)    /* After fixed focus    */
//#define PLAYER_NEWBIE		(K)
#define PLAYER_NOFALCON		(L)
#define PLAYER_NEWLOOK		(M)

#define ABILITY_CRUSADER_SUNDER   (A)

#define ABILITY_LICH_NIGHTSTALKER  (A)
/* RT comm flags -- may be used on both mobs and chars */
#define COMM_QUIET              (A)
#define COMM_DEAF            	(B)
#define COMM_NOWIZ              (C)
#define COMM_NOAUCTION          (D)
#define COMM_NOPUBCHAN          (E)
#define COMM_NOASK              (F)
#define COMM_NOMUSIC            (G)
#define COMM_NOCLAN		(H)
#define COMM_NOQUOTE		(I)
#define COMM_SHOUTSOFF		(J)
/* NO PRAY IS IN SHD, NOCGOSSIP IS IN PLR */
/* display flags */
#define COMM_TRUE_TRUST		(K)
#define COMM_COMPACT		(L)
#define COMM_BRIEF		(M)
#define COMM_PROMPT		(N)
#define COMM_COMBINE		(O)
#define COMM_TELNET_GA		(P)
#define COMM_SHOW_AFFECTS	(Q)
#define COMM_NOGRATS		(R)
#define COMM_WIPED		(S)

/* penalties */
#define COMM_NOEMOTE		(T)
#define COMM_NOSHOUT		(U)
#define COMM_NOTELL		(V)
#define COMM_NOCHANNELS		(W) 
#define COMM_NO_CLAN          (X)
#define COMM_SNOOP_PROOF	(Y)
#define COMM_AFK		(Z)
#define COMM_LONG		(aa)
#define COMM_STORE		(bb)
#define COMM_NOQGOSSIP		(cc)
#define COMM_NOSOCIAL           (dd)                                 
#define COMM_NOCHAT             (ee)
#define COMM_NOARENA            (ff)
/*#define COMM_SILENCE			(gg)*/

/* WIZnet flags */
#define WIZ_ON			(A)
#define WIZ_TICKS		(B)
#define WIZ_LOGINS		(C)
#define WIZ_SITES		(D)
#define WIZ_LINKS		(E)
#define WIZ_DEATHS		(F)
#define WIZ_RESETS		(G)
#define WIZ_MOBDEATHS	(H)
#define WIZ_FLAGS		(I)
#define WIZ_PENALTIES	(J)
#define WIZ_SACCING		(K)
#define WIZ_LEVELS		(L)
#define WIZ_SECURE		(M)
#define WIZ_SWITCHES	(N)
#define WIZ_SNOOPS		(O)
#define WIZ_RESTORE		(P)
#define WIZ_LOAD		(Q)
#define WIZ_NEWBIE		(R)
#define WIZ_PREFIX		(S)
#define WIZ_SPAM		(T)
#define WIZ_DEBUG               (U)   // Skyntil
/* Highlander Quickening */
#define POWER_KILLS  0
#define POWER_POINTS 1
#define POWER_LEVEL  2

#define COMBAT_POWER		0
#define COMBAT_DEFENSE		1
#define COMBAT_AGILITY		2
#define COMBAT_TOUGHNESS	3
#define MAGIC_POWER		4
#define MAGIC_ABILITY		5
#define MAGIC_DEFENSE		6
#define CURRENT_FOCUS		7
#define MAGIC_RESONANCE		8

/* power settings -- Set for each class
 #define POWER_1      0
 #define POWER_2      1
 #define POWER_3      2
 */

//Mob states.
typedef struct mpstate MPSTATE;

struct mpstate {
	MPSTATE *next;
	int mobvnum;
	int state;
};

/*
 * Prototype for a mob.
 * This is the in-memory version of #MOBILES.
 */
struct mob_index_data
{
	MOB_INDEX_DATA * next;
	SPEC_FUN * spec_fun;
	SHOP_DATA * pShop;
	MPROG_LIST * mprogs;
	AREA_DATA * area; /* OLC */
	sh_int vnum;
	sh_int group;
	bool new_format;
	sh_int count;
	sh_int killed;
	char * player_name;
	char * short_descr;
	char * long_descr;
	char * description;
	unsigned long act;
	long affected_by;
	long affected2_by;
	long shielded_by;
	sh_int alignment;
	sh_int level;
	sh_int hitroll;
	sh_int hit[3];
	sh_int mana[3];
	sh_int damage[3];
	sh_int ac[4];
	sh_int dam_type;
	sh_int attacks;
	long off_flags;
	long imm_flags;
	long res_flags;
	long vuln_flags;
	sh_int start_pos;
	sh_int default_pos;
	sh_int sex;
	sh_int race;
	long wealth;
	long form;
	long parts;
	sh_int size;
	char * material;
	long mprog_flags;
	char * die_descr;
	char * say_descr;
	int round_dam;
	bool see_all_mortals;
	int mob_tier;
};

/* memory settings */
#define MEM_CUSTOMER	A	
#define MEM_SELLER	B
#define MEM_HOSTILE	C
#define MEM_AFRAID	D

/* memory for mobs */
struct mem_data
{
	MEM_DATA *next;
	bool valid;
	int id;
	int reaction;
	time_t when;
};

/* arena and pk rank data */
struct arank_data
{
	int num;
	char * name;
	int rank;
};
struct prank_data
{
	int num;
	char * name;
	int rank;
};

/*
 * One character (PC or NPC).
 */
struct char_data
{
	CHAR_DATA * next;
	CHAR_DATA * next_in_room;
	CHAR_DATA * master;
	CHAR_DATA * leader;
	CHAR_DATA * fighting;
	CHAR_DATA * reply;
	CHAR_DATA * pet;
	CHAR_DATA * mprog_target;
	CHAR_DATA * challenger; /* person who challenged you */
	CHAR_DATA * challenged; /* person who you challenged */
	CHAR_DATA * gladiator; /* ARENA player wagered on */
	MEM_DATA * memory;
	SPEC_FUN * spec_fun;
	MOB_INDEX_DATA * pIndexData;
	DESCRIPTOR_DATA * desc;
	AFFECT_DATA * affected;
//	COOLDOWN_DATA * cooldowns;
	NOTE_DATA * pnote;
	OBJ_DATA * carrying;
	OBJ_DATA * on;
	ROOM_INDEX_DATA * in_room;
	ROOM_INDEX_DATA * was_in_room;
	AREA_DATA * zone;
	PC_DATA * pcdata;
	GEN_DATA * gen_data;
	int questpoints;

	sh_int questroom;
	sh_int followers;
	sh_int stance[11];
	sh_int eq_age;
	bool valid;
	bool hastimer;
	bool attacker;
	bool on_quest;
	bool wedpost;
	char * name;
	long id;
	sh_int version;
	char * short_descr;
	char * long_descr;
	char * die_descr;
	char * say_descr;
	char * description;
	char * prompt;
	char * prefix;
	sh_int group;

	sh_int sex;
	sh_int class;
	sh_int race;
	sh_int level;
	sh_int trust;
	int color;
	int color_auc;
	int color_cgo;
	int color_cla;
	int color_con;
	int color_dis;
	int color_fig;
	int color_gos;
	int color_gra;
	int color_chat;
	int color_gte;
	int color_imm;
	int color_mob;
	int color_mus;
	int color_opp;
	int color_qgo;
	int color_que;
	int color_quo;
	int color_roo;
	int color_say;
	int color_sho;
	int color_tel;
	int color_wit;
	int color_wiz;
	int played;
	int lines; /* for the pager */
	int stunned;
	time_t llogoff;
	time_t logon;
	sh_int timer;
	sh_int wait;
	sh_int daze;
	int hit;
	int max_hit;
	int mana;
	int max_mana;
	int move;
	int max_move;
	long platinum;
	long gold;
	long silver;
	long balance[4];
	long exp;
	int tells;
	long ability_crusader;
	long ability_lich;
	long act;
	long comm; /* RT added to pad the vector */
	long wiznet; /* wiz stuff */
	long imm_flags;
	long res_flags;
	long vuln_flags;
	sh_int invis_level;
	sh_int incog_level;
	sh_int ghost_level;
	long affected_by;
	long affected2_by;
	long shielded_by;
	sh_int position;
	sh_int practice;
	sh_int train;
	sh_int carry_weight;
	sh_int carry_number;
	sh_int saving_throw;
	sh_int alignment;
	sh_int hitroll;
	sh_int damroll;
	sh_int armor[4];
	sh_int wimpy;
	/* tracking */
	sh_int track_to[MAX_TRACK];
	sh_int track_from[MAX_TRACK];
	/* stats */
	sh_int perm_stat[MAX_STATS];
	sh_int mod_stat[MAX_STATS];
	sh_int true_stat[11];
	sh_int mod_true_stat[11];
	/* parts stuff */
	long form;
	long parts;
	sh_int size;
	char* material;
	/* mobile stuff */
	long off_flags;
	sh_int damage[3];
	sh_int dam_type;
	sh_int start_pos;
	sh_int default_pos;
	sh_int mprog_delay;
	sh_int god;
	char *pload;

	int plyr; /* PLAYER FLAGS */
	char *ghost; /* Ghost Desc */
	int ghost_timer; /* 5 Min to re-equip */
	int regen_rate;
	int mana_regen_rate;
	sh_int magic[6]; /* New Magic System */
	int fight_timer;
	int shd_aura_timer;

	OBJ_DATA *morphobj; // Object Morphs into
	char *morph; // The name that is seen
	int morph_form[3]; /* form, sn, charges */

	int stance_aff;
	sh_int toughness; // Sanc Replace for Warriors
	//int					assisting;	//For fixing autoassist problem
	int special_skill[6]; /* For Future Development */

	int qstate;		// Quest state
	int qtarget;	// Quest target (vnum)
	int qtimer;		// Quest timer (for quest and between quests)
	int qmvnum;		// Questmaster vnum
	int qroom;		// Quest room

	OBJ_DATA *locker;	// Locker to store spare stuff.

	MPSTATE *mpstates;
};

// Enum of achievement metrics.
// Note: to add a metric, insert it *JUST* BEFORE ACH_MAX ONLY, otherwise
// you will get very weird, possibly broken, results.
typedef enum achievement_metric {
	ACH_NPCKILLS = 0,
	ACH_PKKILLS,
	ACH_CASTS,
	ACH_QUESTS,
	ACH_AKILLS,
	ACH_BABKILLS,
	ACH_HOURS,
	ACH_RUNS,
	ACH_VOUCHERS,
	ACH_BACKSTABS,
	ACH_QUAFFS,
	ACH_LGD,
	ACH_RANDOMS,
	ACH_BALANCE,
	ACH_HARAKIRI,
	ACH_LIGHTNING,
	ACH_PILLS,

	// Only add new metrics before ACH_MAX.
	ACH_MAX,

	// These are pseudo-metrics, i.e. function pointers
	// determine the achievement get thing.
	ACH_PSEUDO
} ACH_METRIC;

// Types of achievements out there.
typedef struct achievement_definition {
	char *save_name;
	ACH_METRIC metric;
	unsigned long threshold;
	char *name;
	int reward;
	char *description;
} ACH_DEFINITION;

typedef struct achievement_done ACH_DONE;
struct achievement_done {
	ACH_DONE *next;
	const ACH_DEFINITION *type;
};

// FOCUS_TIER
struct tier_exp_type {
	int exp_bonus;
};

#define MAX_TIERS 100
#define TERT 0
#define SEC 1
#define PRI 2

struct class_focus_mod {
	sh_int combat_power;
	sh_int combat_defense;
	sh_int combat_agility;
	sh_int combat_toughness;
	sh_int magic_power;
	sh_int magic_resonance;
	sh_int magic_ability;
	sh_int magic_defense;
};

/*
 * Data which only PC's have.
 */
struct pc_data
{
	PC_DATA * next;
	BUFFER * buffer;
	bool valid;
	char * pwd;
	char * bamfin;
	char * bamfout;
	char * identity;
	char * immskll;
	char * title;
	bool usr_ttl;
	char * who_descr;
	char * socket;
	time_t last_note;
	time_t last_idea;
	time_t last_penalty;
	time_t last_quotes;
	time_t last_news;
	time_t last_changes;
	time_t last_weddings;
	int perm_hit;
	int perm_mana;
	int perm_move;
	int base_hit;
	int base_mana;
	int base_move;
	sh_int true_sex;
	int last_level;
	int recall;
	int tier;
	sh_int fake_stance;
	sh_int condition[4];
	sh_int learned[MAX_SKILL];
	sh_int learnlvl[MAX_SKILL];
	bool group_known[MAX_GROUP];
	sh_int points;
	bool confirm_delete;
	bool confirm_reroll;
	char * forget[MAX_FORGET];
	char * dupes[MAX_DUPES];
	char * alias[MAX_ALIAS];
	char * alias_sub[MAX_ALIAS];
	int security; /* OLC builder security */
	int plr_wager; /* ARENA amount wagered */
	sh_int confirm_loner;
	char * pretit;
	char * colour_name;
	sh_int round_dam; /* Short combat */
	int power[3]; /* kills,points,level */
	long focus[9]; /* ComStren, ComDef, ComAgil, ComTough, MagPow, MagAbil, MagDef, MagRes */
	int status;
	float edam;
	/* Grant */
	GRANT_DATA * granted;
	ROSTER_DATA *roster;

	// Achievements
	unsigned long achievement_metrics[ACH_MAX];
	ACH_DONE *achievements_done;
};

/* Data for generating characters -- only used during generation */
struct gen_data
{
	GEN_DATA *next;
	bool valid;
	bool skill_chosen[MAX_SKILL];
	bool group_chosen[MAX_GROUP];
	int points_chosen;
};

/*
 * Liquids.
 */
#define LIQ_WATER        0

struct liq_type
{
	char * liq_name;
	char * liq_color;
	sh_int liq_affect[5];
};

/*
 * Extra description data for a room or object.
 */
struct extra_descr_data
{
	EXTRA_DESCR_DATA *next; /* Next in list                     */
	bool valid;
	char *keyword; /* Keyword in look/examine          */
	char *description; /* What to see                      */
};

/*
 * Prototype for an object.
 */
struct obj_index_data
{
	OBJ_INDEX_DATA * next;
	EXTRA_DESCR_DATA * extra_descr;
	AFFECT_DATA * affected;
	AREA_DATA * area; /* OLC */
	bool new_format;
	char * name;
	char * short_descr;
	char * description;
	sh_int vnum;
	sh_int reset_num;
	char * material;
	sh_int item_type;
	int extra_flags;
	int wear_flags;
	sh_int level;
	sh_int condition;
	sh_int count;
	sh_int weight;
	int cost;
	int value[5];
	sh_int class;
	sh_int timer;
	int class_restrict_flags;
};

/*
 * One object.
 */
struct obj_data
{
	OBJ_DATA * next;
	OBJ_DATA * next_content;
	OBJ_DATA * contains;
	OBJ_DATA * in_obj;
	OBJ_DATA * on;
	CHAR_DATA * carried_by;
	EXTRA_DESCR_DATA * extra_descr;
	AFFECT_DATA * affected;
	OBJ_INDEX_DATA * pIndexData;
	ROOM_INDEX_DATA * in_room;
	bool valid;
	bool enchanted;
	char * owner;
	char * killer;
	char * name;
	char * short_descr;
	char * description;
	sh_int item_type;
	int extra_flags;
	int wear_flags;
	sh_int wear_loc;
	sh_int weight;
	int cost;
	sh_int level;
	sh_int tier_level;
	sh_int condition;
	char * material;
	sh_int timer;
	int value[5];
	sh_int class;
	sh_int odds;
	int class_restrict_flags;
	char * maker;
	char * made;
	char * looted_by; /* PK LOOTING */
	char * looted_from; /* PK LOOTING */
	int loot_timer; /* PK LOOTING */
};

/*
 * Exit data.
 */
struct exit_data
{
	union
	{
		ROOM_INDEX_DATA * to_room;
		sh_int vnum;
	} u1;
	sh_int exit_info;
	sh_int key;
	char * keyword;
	char * description;
	EXIT_DATA * next; /* OLC */
	int rs_flags; /* OLC */
	int orig_door; /* OLC */
};

/*
 * Reset commands:
 *   '*': comment
 *   'M': read a mobile 
 *   'O': read an object
 *   'P': put object in object
 *   'G': give object to mobile
 *   'E': equip object to mobile
 *   'D': set state of door
 *   'R': randomize room exits
 *   'S': stop (end of list)
 */

/*
 * Area-reset definition.
 */
struct reset_data
{
	RESET_DATA * next;
	char command;
	sh_int arg1;
	sh_int arg2;
	sh_int arg3;
	sh_int arg4;
};

/*
 * Area definition.
 */
struct area_data
{
	AREA_DATA * next;
	RESET_DATA * reset_first;
	RESET_DATA * reset_last;
	char * file_name;
	char * name;
	char * credits;
	sh_int age;
	sh_int nplayer;
	sh_int low_range;
	sh_int high_range;
	sh_int min_vnum;
	sh_int max_vnum;
	bool empty;
	char * builders; /* OLC  Listing of */
	int vnum; /* OLC Area Vnum */
	int area_flags; /* OLC */
	int security; /* OLC - Value 1 - 9 */
	int continent; /* OLC - Defined Values */
	int mob_tier;
	HELP_AREA * helps; /* Hedit */
};

/*
 * Room type.
 */
// bree_room
struct room_index_data
{
	sh_int affect_room_type;
	ROOM_INDEX_DATA * next;
	CHAR_DATA * people;
	OBJ_DATA * contents;
	EXTRA_DESCR_DATA * extra_descr;
	AREA_DATA * area;
	EXIT_DATA * exit[12];
	EXIT_DATA * old_exit[12];
	RESET_DATA * reset_first; /* OLC */
	RESET_DATA * reset_last; /* OLC */
	char * name;
	char * description;
	char * owner;
	sh_int vnum;
	int room_flags;
	sh_int light;
	sh_int sector_type;
	sh_int heal_rate;
	sh_int mana_rate;
	int tele_dest;
	AFFECT_DATA * affected;
	int affected_by;

	// Pathfinding stuff.
	ROOM_INDEX_DATA * pf_next;
	ROOM_INDEX_DATA * pf_prev;
	long pf_visited;
	int pf_depth;
	char pf_direction;
};

/*
 * MOBprog definitions
 */
#define TRIG_ACT	(A)
#define TRIG_BRIBE	(B)
#define TRIG_DEATH	(C)
#define TRIG_ENTRY	(D)
#define TRIG_FIGHT	(E)
#define TRIG_GIVE	(F)
#define TRIG_GREET	(G)
#define TRIG_GRALL	(H)
#define TRIG_KILL	(I)
#define TRIG_HPCNT	(J)
#define TRIG_RANDOM	(K)
#define TRIG_SPEECH	(L)
#define TRIG_EXIT	(M)
#define TRIG_EXALL	(N)
#define TRIG_DELAY	(O)
#define TRIG_SURR	(P)

struct mprog_list
{
	int trig_type;
	char * trig_phrase;
	sh_int vnum;
	char * code;
	MPROG_LIST * next;
	bool valid;
};

struct mprog_code
{
	sh_int vnum;
	char * code;
	MPROG_CODE * next;
};

/*
 * Types of attacks.
 * Must be non-overlapping with spell/skill types,
 * but may be arbitrary beyond that.
 */
#define TYPE_UNDEFINED               -1
#define TYPE_HIT                     1000

/*
 *  Target types.
 */
#define TAR_IGNORE		    0
#define TAR_CHAR_OFFENSIVE	    1
#define TAR_CHAR_DEFENSIVE	    2
#define TAR_CHAR_SELF		    3
#define TAR_OBJ_INV		    4
#define TAR_OBJ_CHAR_DEF	    5
#define TAR_OBJ_CHAR_OFF	    6
#define TAR_OBJ_TRAN		    7

#define TARGET_CHAR		    0
#define TARGET_OBJ		    1
#define TARGET_ROOM		    2
#define TARGET_NONE		    3

/*
 * Skills include spells as a particular case.
 */
struct skill_type
{
	char * name; /* Name of skill		*/
	sh_int skill_level[MAX_CLASS]; /* Level needed by class	*/
	sh_int rating[MAX_CLASS]; /* How hard it is to learn	*/
	SPELL_FUN * spell_fun; /* Spell pointer (for spells)	*/
	sh_int target; /* Legal targets		*/
	sh_int minimum_position; /* Position for caster / user	*/
	sh_int * pgsn; /* Pointer to associated gsn	*/
	bool socket; /* Allow same socket use?	*/
	sh_int slot; /* Slot for #OBJECT loading	*/
	sh_int min_mana; /* Minimum mana used		*/
	sh_int beats; /* Waiting time after use	*/
	sh_int cooldown; /* Skill cooldown  */
// Leaving cooldown in here because it will be tedious to remove them all...
	char * noun_damage; /* Damage message		*/
	char * msg_off; /* Wear off message		*/
	char * msg_obj; /* Wear off message for obects	*/
	sh_int * sgsn; /* Second Gsn */
	sh_int * tgsn; /* Third Gsn */
};

extern int chain;
extern int maxSocial;

/*
 * These are skill_lookup return values for common skills and spells.
 */
extern sh_int gsn_backstab;
extern sh_int gsn_circle;
extern sh_int gsn_dodge;
extern sh_int gsn_phase;
extern sh_int gsn_blood_phase;
extern sh_int gsn_envenom;
extern sh_int gsn_feed;
extern sh_int gsn_hide;
extern sh_int gsn_peek;
extern sh_int gsn_pick_lock;
extern sh_int gsn_sneak;
extern sh_int gsn_steal;
extern sh_int gsn_assassinate;
extern sh_int gsn_strangle;
extern sh_int gsn_smokebomb;
extern sh_int gsn_layhands;
extern sh_int gsn_lead;
extern sh_int gsn_riposte;
extern sh_int gsn_coup_de_coup;
//extern sh_int   gsn_song_protection;

extern sh_int gsn_disarm;
extern sh_int gsn_dual_wield;
extern sh_int gsn_enhanced_damage;
extern sh_int gsn_kick;
extern sh_int gsn_acute_vision;
extern sh_int gsn_acute_hearing;
extern sh_int gsn_skin;
extern sh_int gsn_demand;
extern sh_int gsn_parry;
extern sh_int gsn_rescue;
extern sh_int gsn_second_attack;
extern sh_int gsn_third_attack;
extern sh_int gsn_multistrike;
extern sh_int gsn_fourth_attack;
extern sh_int gsn_fifth_attack;
extern sh_int gsn_sixth_attack;

extern sh_int gsn_blindness;
extern sh_int gsn_charm_person;
extern sh_int gsn_curse;
extern sh_int gsn_invis;
extern sh_int gsn_mass_invis;
extern sh_int gsn_plague;
extern sh_int gsn_poison;
extern sh_int gsn_voodan_curse;
extern sh_int gsn_sleep;
extern sh_int gsn_fly;
extern sh_int gsn_sanctuary;
extern sh_int gsn_sharpen;
extern sh_int gsn_room_sanctuary;
extern sh_int gsn_darkshield;
extern sh_int gsn_mana_shield;
extern sh_int gsn_siphon;
extern sh_int gsn_sense;
extern sh_int gsn_blood_shield;
/* new gsns */
extern sh_int gsn_axe;
extern sh_int gsn_dagger;
extern sh_int gsn_flail;
extern sh_int gsn_mace;
extern sh_int gsn_polearm;
extern sh_int gsn_shield_block;
extern sh_int gsn_shield_wall;
extern sh_int gsn_spear;
extern sh_int gsn_sword;
extern sh_int gsn_whip;

extern sh_int gsn_bash;
extern sh_int gsn_berserk;
extern sh_int gsn_dirt;
extern sh_int gsn_feed;
extern sh_int gsn_fear;
extern sh_int gsn_hand_to_hand;
extern sh_int gsn_trip;

extern sh_int gsn_fast_healing;
extern sh_int gsn_haggle;
extern sh_int gsn_lore;
extern sh_int gsn_meditation;

extern sh_int gsn_scrolls;
extern sh_int gsn_staves;
extern sh_int gsn_wands;
extern sh_int gsn_recall;
extern sh_int gsn_stun;
extern sh_int gsn_track;
extern sh_int gsn_gouge;
extern sh_int gsn_grip;
extern sh_int gsn_blind_fighting;
extern sh_int gsn_ambush;
extern sh_int gsn_butcher;
extern sh_int gsn_chameleon;
extern sh_int gsn_domination;
extern sh_int gsn_heighten;
extern sh_int gsn_shadow;
extern sh_int gsn_greed;

extern sh_int gsn_faerie_fire;
extern sh_int gsn_divine_protection;
extern sh_int gsn_protective_aura;
extern sh_int gsn_turn_undead;
extern sh_int gsn_hold_align;
extern sh_int gsn_wrath_of_god;

extern sh_int gsn_quickening;
extern sh_int gsn_decapitate;
extern sh_int gsn_blademastery;

extern sh_int gsn_shriek;
extern sh_int gsn_on_fire;
extern sh_int gsn_twirl;
extern sh_int gsn_god_favor;
extern sh_int gsn_garrote;

/* Ranger/Strider/Hunter */
extern sh_int gsn_herb;
extern sh_int gsn_entangle;
extern sh_int gsn_crevice;
extern sh_int gsn_barkskin;
extern sh_int gsn_forest_blend;
extern sh_int gsn_shadow_blend;
extern sh_int gsn_detect_terrain;

/* Druid/Sage */
extern sh_int gsn_magma_stream;
extern sh_int gsn_steam_blast;
extern sh_int gsn_detect_efade;
extern sh_int gsn_fist_earth;
extern sh_int gsn_monsoon;
extern sh_int gsn_earthmeld;

/* Warlock */
extern sh_int gsn_channel;
extern sh_int gsn_headache;
extern sh_int gsn_domineer;
extern sh_int gsn_soulbind;
extern sh_int gsn_wither;

/*spartan skills */
extern sh_int gsn_spartan_bash;
extern sh_int gsn_spear_jab;
extern sh_int gsn_spirit_sparta;
extern sh_int gsn_rally;

//ish skills
extern sh_int gsn_slash;
extern sh_int gsn_2hand_mast;
extern sh_int gsn_fury;
extern sh_int gsn_whirlwind;
extern sh_int gsn_hammerblow;
//end ish skills


extern sh_int gsn_coule;
/* blade dancer */
extern sh_int gsn_grenado;
extern sh_int gsn_blood_dance;
extern sh_int gsn_bladedance;

/* Monk */
extern sh_int gsn_palm;
extern sh_int gsn_martial_arts;

extern sh_int gsn_blindness_dust;
extern sh_int gsn_solar_flare;
extern sh_int gsn_ground_stomp;
extern sh_int gsn_healing_trance;
extern sh_int gsn_meditation;
extern sh_int gsn_purify_body;
extern sh_int gsn_will_of_iron;
extern sh_int gsn_sidekick;
extern sh_int gsn_spin_kick;
extern sh_int gsn_vital_hit;
extern sh_int gsn_follow_through;
extern sh_int gsn_quick_step;

extern sh_int gsn_throw;
extern sh_int gsn_prayer;
extern sh_int gsn_lunge;
extern sh_int gsn_nerve;
extern sh_int gsn_roll;
extern sh_int gsn_chop;
extern sh_int gsn_head_cut;

extern sh_int gsn_punch;
extern sh_int gsn_elbow;
extern sh_int gsn_knee;
extern sh_int gsn_headbutt;
extern sh_int gsn_gut;
extern sh_int gsn_shoulder;
extern sh_int gsn_spinkick;

extern sh_int gsn_weaponbash;
extern sh_int gsn_hurl;
extern sh_int gsn_hara_kiri;
extern sh_int gsn_hari;
extern sh_int gsn_conceal;
extern sh_int gsn_shadow_dance;
extern sh_int gsn_snattack;
extern sh_int gsn_snattacktwo;
extern sh_int gsn_divide;
extern sh_int gsn_bloodlust;
extern sh_int gsn_vmight;
extern sh_int gsn_phsyco;
extern sh_int gsn_conceal2;
extern sh_int gsn_concealClan1;
extern sh_int gsn_concealClan2;
extern sh_int gsn_shadow_form;
extern sh_int gsn_target;
extern sh_int gsn_battle_fury;
extern sh_int gsn_shapeshift;
extern sh_int gsn_cripple;
extern sh_int gsn_cripple_head;
extern sh_int gsn_cripple_leg;
extern sh_int gsn_blind_faith;
extern sh_int gsn_faerie_fog;
extern sh_int gsn_fires;
extern sh_int gsn_axe2;
extern sh_int gsn_dagger2;
extern sh_int gsn_flail2;
extern sh_int gsn_mace2;
extern sh_int gsn_polearm2;
extern sh_int gsn_spear2;
extern sh_int gsn_sword2;
extern sh_int gsn_whip2;
extern sh_int gsn_hand2;

extern sh_int gsn_fists_fury; /* Monk */
extern sh_int gsn_fists_flame; /* Monk */
extern sh_int gsn_fists_ice; /* Monk */
extern sh_int gsn_fists_darkness; /* Ninja/Voodan */
extern sh_int gsn_fists_acid; /* Ninja */
extern sh_int gsn_fists_liquid; /* Monk */
extern sh_int gsn_fists_fang; /* Bard */
extern sh_int gsn_fists_claw; /* Bard */
extern sh_int gsn_fists_divinity; /* Monk */
extern sh_int gsn_adamantium_palm; /* Monk */
extern sh_int gsn_deflect; /* Monk */
extern sh_int gsn_counter_defense; /* Not set yet */
extern sh_int gsn_roundhouse; /* Monk */
extern sh_int gsn_spinkick; /* Ninja */
extern sh_int gsn_harmony; /* Monk */
extern sh_int gsn_chi; /* Monk */
extern sh_int gsn_michi; /* Ninja */
extern sh_int gsn_inner_str; /* ninja */

/* Necromancers */
extern sh_int gsn_animate_skeleton;
extern sh_int gsn_animate_dead;
extern sh_int gsn_mummify;
extern sh_int gsn_embalm;
extern sh_int gsn_lesser_golem;
extern sh_int gsn_greater_golem;
extern sh_int gsn_preserve_limb;
extern sh_int gsn_cremate;
extern sh_int gsn_fear_word;

/* New Stuff -- Paladin/DP/Warrior Classes */
extern sh_int gsn_shield_bash;
extern sh_int gsn_cleave;
extern sh_int gsn_dual_wield2;
extern sh_int gsn_imbue_weapon;
extern sh_int gsn_restore;

extern sh_int gsn_sunder;

/* Renshai */
extern sh_int gsn_modis_anger;
extern sh_int gsn_nightwish;
extern sh_int gsn_concentration;
extern sh_int gsn_crit_strike;
extern sh_int gsn_vengeance;

/* Gypsy */
extern sh_int gsn_vanish;
extern sh_int gsn_wander;

extern sh_int gsn_impair;
extern sh_int gsn_disguise;

extern sh_int gsn_adrenaline;

/*
 * Utility macros.
 */
#define IS_VALID(data)		((data) != NULL && (data)->valid)
#define VALIDATE(data)		((data)->valid = TRUE)
#define INVALIDATE(data)	((data)->valid = FALSE)
#define UMIN(a, b)		((a) < (b) ? (a) : (b))
#define UMAX(a, b)		((a) > (b) ? (a) : (b))
#define URANGE(a, b, c)		((b) < (a) ? (a) : ((b) > (c) ? (c) : (b)))
#define LOWER(c)		((c) >= 'A' && (c) <= 'Z' ? (c)+'a'-'A' : (c))
#define UPPER(c)		((c) >= 'a' && (c) <= 'z' ? (c)+'A'-'a' : (c))
#define IS_SET(flag, bit)	((flag) & (bit))
#define SET_BIT(var, bit)	((var) |= (bit))
#define REMOVE_BIT(var, bit)	((var) &= ~(bit))
#define TOGGLE_BIT(var, bit)	((var) ^= (bit))
#define IS_NULLSTR(str)		((str) == NULL || (str)[0] == '\0')
#define ENTRE(min,num,max)	( ((min) < (num)) && ((num) < (max)) )
#define IN_ARENA(ch)		( IS_NPC(ch) ? FALSE : \
			IS_SET(ch->in_room->room_flags,ROOM_ARENA))

/*
 * Character macros.
 */
#define IS_NPC(ch)		(IS_SET((ch)->act, ACT_IS_NPC))
#define IS_IMMORTAL(ch)		((ch)->level >= LEVEL_IMMORTAL)
#define IS_HERO(ch)		((ch)->level >= LEVEL_HERO)
#define IS_TRUSTED(ch,level)	(get_trust((ch)) >= (level))
#define IS_AFFECTED(ch, sn)	(IS_SET((ch)->affected_by, (sn)))
#define IS_AFFECTED2(ch, sn)    (IS_SET((ch)->affected2_by, (sn)))
#define IS_SHIELDED(ch, sn)	(IS_SET((ch)->shielded_by, (sn)))
#define IS_QUESTOR(ch)          (IS_SET((ch)->act, PLR_QUESTOR))
#define GET_AGE(ch)		((int) (17 + ((ch)->played \
				    + current_time - (ch)->logon )/72000))

#define IS_GOOD(ch)		(ch->alignment >= 350)
#define IS_EVIL(ch)		(ch->alignment <= -350)
#define IS_NEUTRAL(ch)		(!IS_GOOD(ch) && !IS_EVIL(ch))

#define IS_AWAKE(ch)		(ch->position > POS_SLEEPING)
#define IS_CRUSADER(ch)		(ch->class == 25)
#define GET_AC(ch,type)		((ch)->armor[type] \
			+ (focus_ac(ch)) \
		        + ( IS_AWAKE(ch) \
			? dex_app[get_curr_stat(ch,STAT_DEX)].defensive : 0 ) \
			)
#define GET_HITROLL(ch) ( \
		((ch)->hitroll) \
		+ (str_app[get_curr_stat(ch,STAT_STR)].tohit) \
		+ (focus_hit(ch)) \
		+ (IS_CRUSADER(ch) ? ((ch->alignment*ch->level)/2000) : 0 ) \
		)
#define GET_TRUHITROLL(ch) ( \
		((ch)->hitroll) \
		+ (str_app[get_curr_stat(ch,STAT_STR)].tohit) \
		+ (focus_hit(ch)) \
		+ (IS_CRUSADER(ch) ? ((ch->alignment*ch->level)/2000) : 0 ) \
		)		
#define GET_DAMROLL(ch) ( \
		((ch)->damroll) \
		+ (str_app[get_curr_stat(ch,STAT_STR)].todam) \
		+ (focus_dam(ch)) \
		+ (ch->class == CLASS_BARBARIAN \
		? ((ch)->hitroll/6) : 0) \
		+ (IS_CRUSADER(ch) ? ((ch->alignment*ch->level)/2000) : 0 ) \
		+ (ch->class == CLASS_MONK \
		? (ch->pcdata->power[0]/10+ch->pcdata->power[1]/10) : 0) \
		) 
#define GET_MOD_DAMROLL2(ch) ( \
		((ch)->damroll < 1450) ? \
		( \
		(((ch)->damroll)-((ch)->damroll*(ch)->damroll*2.0)/12100.0) \
		+ (str_app[get_curr_stat(ch,STAT_STR)].todam) \
		+ (focus_dam(ch)) \
		+ (ch->class == CLASS_BARBARIAN \
		? ((ch)->hitroll/6) : 0) \
		+ (IS_CRUSADER(ch) ? ((ch->alignment*ch->level)/2000) : 0 ) \
		+ (ch->class == CLASS_MONK \
		? (ch->pcdata->power[0]/10+ch->pcdata->power[1]/10) : 0) \
		) : \
		( \
		((((ch)->damroll)*1.004)-426) \
		+ (str_app[get_curr_stat(ch,STAT_STR)].todam) \
		+ (focus_dam(ch)) \
		+ (ch->class == CLASS_BARBARIAN \
		? ((ch)->hitroll/6) : 0) \
		+ (IS_CRUSADER(ch) ? ((ch->alignment*ch->level)/2000) : 0 ) \
		+ (ch->class == CLASS_MONK \
		? (ch->pcdata->power[0]/10+ch->pcdata->power[1]/10) : 0) \
		) \
		) 
#define GET_MOD_DAMROLL(ch) ( fn_GET_MOD_DAMROLL(ch) )
		

// New tier focus bonuses - Marduk
#define tier_spell_bonus(ch) (IS_NPC(ch) ? 0 : ( focus_level((ch)->pcdata->focus[MAGIC_POWER]) \
				+ focus_level((ch)->pcdata->focus[MAGIC_RESONANCE]) \
				+ focus_level((ch)->pcdata->focus[MAGIC_ABILITY]) \
				+ focus_level((ch)->pcdata->focus[MAGIC_DEFENSE])) / 4 )
#define tier_level_bonus(ch) ( focus_level_bonus(ch) )
//#define tier_level_mob_bonus(ch) ( focus_level_mob_bonus(ch) )

#define IS_OUTSIDE(ch)		(!IS_SET(				    \
				    (ch)->in_room->room_flags,		    \
				    ROOM_INDOORS))

#define WAIT_STATE(ch, npulse)	if (IS_IMMORTAL(ch))  ch->wait = 0;	    \
				else					    \
				((ch)->wait = UMAX((ch)->wait, (npulse)))
#define DAZE_STATE(ch, npulse)  ((ch)->daze = UMAX((ch)->daze, (npulse)))
#define get_carry_weight(ch)	((ch)->carry_weight + ((ch)->silver/10) +  \
				((ch)->gold * 2 / 5) + ((ch)->platinum * 3 / 5))
#define HAS_TRIGGER(ch,trig)    (IS_SET((ch)->pIndexData->mprog_flags,(trig)))
#define IS_SWITCHED( ch )       ( ch->desc && ch->desc->original )
#define IS_BUILDER(ch, Area)    ( !IS_NPC(ch) && !IS_SWITCHED( ch ) &&    \
                                ( ch->pcdata->security >= Area->security  \
                                || strstr( Area->builders, ch->name )     \
                                || strstr( Area->builders, "All" ) ) )

#define IS_STANCE(ch, sn)       (ch->stance[0] == sn)

/*
 * Object macros.
 */
#define CAN_WEAR(obj, part)	(IS_SET((obj)->wear_flags,  (part)))
#define IS_OBJ_STAT(obj, stat)	(IS_SET((obj)->extra_flags, (stat)))
#define IS_WEAPON_STAT(obj,stat)(IS_SET((obj)->value[4],(stat)))
#define WEIGHT_MULT(obj)	((obj)->item_type == ITEM_CONTAINER ? \
	(obj)->value[4] : 100)
#define IS_MORPHED( ch )        (IS_SET((ch)->plyr, PLAYER_MORPHED))

/*
 * Description macros.
 */

/* Handler.C -- Important that this is here  
 --- Eternal being crap, replaced with identity code

 #define PERS(ch, looker)        ( get_pers((ch),(looker)) )

 Old PERS
 #define PERS(ch, looker)	( can_see( looker, (ch) ) ?		\
				( IS_NPC(ch) ? (ch)->short_descr	\
                                : (ch)->name ) : "someone" )
 */

/* #define PERS(ch, looker)        ( can_see( looker, (ch) ) ?             \
                                ( IS_NPC(ch) ? (ch)->short_descr        \
                                : (ch)->name ) :                        \
                                ( IS_NPC(ch) ? "someone" :              \
                                ( IS_IMMORTAL(ch) ?                     \
                                ( (ch)->pcdata->identity[0] == '\0' ?   \
                                "someone" : (ch)->pcdata->identity )    \
                                : "someone" ))) */

/* #define PERS(ch, looker)        ( can_see( looker, (ch) ) ?             \
				( IS_NPC(ch) ? (ch)->short_descr	\
                                : get_pers(ch,looker) ) :               \
                                ( IS_IMMORTAL(ch) && !IS_NPC(ch) ?      \
                                ( (ch)->pcdata->identity[0] == '\0' ?   \
                                "someone" : (ch)->pcdata->identity )    \
                                : "someone" ) : "someone" ) */

#define PERS(ch,looker)         show_pers(ch,looker)

/* Handler.C -- Pers for UnSeen Mortals */
char *get_pers(CHAR_DATA *ch, CHAR_DATA *looker);
char *get_name(CHAR_DATA *ch, CHAR_DATA *victim, char buf[]);
char *show_pers(CHAR_DATA *ch, CHAR_DATA *looker);
void send_to_world(char *argument);
/*
 * Structure for a social in the socials table.
 */
struct social_type
{
	char name[20];
	char * char_no_arg;
	char * others_no_arg;
	char * char_found;
	char * others_found;
	char * vict_found;
	char * char_not_found;
	char * char_auto;
	char * others_auto;
};

/*
 * Global constants.
 */
extern const struct str_app_type str_app[MAX_STAT];
extern const struct int_app_type int_app[MAX_STAT];
extern const struct wis_app_type wis_app[MAX_STAT];
extern const struct dex_app_type dex_app[MAX_STAT];
extern const struct con_app_type con_app[MAX_STAT];

extern const struct class_type class_table[MAX_CLASS];
extern const struct weapon_type weapon_table[];
extern const struct item_type item_table[];
extern const struct wiznet_type wiznet_table[];
extern const struct attack_type attack_table[];
extern const struct race_type race_table[];
//extern const    struct  flag_type       affect_room_flags[];
extern const struct pc_race_type pc_race_table[];
extern const struct spec_type spec_table[];
extern const struct liq_type liq_table[];
extern struct skill_type skill_table[MAX_SKILL];
extern struct social_type social_table[MAX_SOCIALS];
extern struct tier_exp_type tier_exp_bonus[MAX_TIERS];
extern char * const title_table[MAX_CLASS][MAX_LEVEL + 1][2];
extern char * const dir_name[];
extern const sh_int rev_dir[];
extern const sh_int movement_loss[SECT_MAX];
extern struct struckdrunk drunk[];
extern const struct cmd_type cmd_table[];


/*
 * Global variables.
 */
extern HELP_DATA * help_first;
extern SHOP_DATA * shop_first;
extern DISABLED_DATA * disabled_first;
extern CHAR_DATA * char_list;
extern DESCRIPTOR_DATA * descriptor_list;
extern OBJ_DATA * object_list;
extern MPROG_CODE * mprog_list;

extern char bug_buf[];
extern time_t current_time;
extern bool fLogAll;
extern KILL_DATA kill_table[];
extern char log_buf[];
extern TIME_INFO_DATA time_info;
extern WEATHER_DATA weather_info;
extern char last_command[MAX_STRING_LENGTH]; //Last command
extern bool MOBtrigger;

extern AREA_DATA * area_first;
extern AREA_DATA * area_last;
extern int top_area;

extern bool global_quest;
extern bool global_quest_closed;
extern int arena;
extern int reboot_counter;
extern int shutdown_counter;
extern int auction_ticket;

/*
 * OS-dependent declarations.
 * These are all very standard library functions,
 *   but some systems have incomplete or non-ansi header files.
 */
#if	defined(_AIX)
char * crypt ( const char *key, const char *salt );
#endif

#if	defined(apollo)
int atoi ( const char *string );
void * calloc ( unsigned nelem, size_t size );
char * crypt ( const char *key, const char *salt );
#endif

#if	defined(hpux)
char * crypt ( const char *key, const char *salt );
#endif

#if	defined(linux)
char * crypt( const char *key, const char *salt );
#endif

#if	defined(macintosh)
#define NOCRYPT
#if	defined(unix)
#undef	unix
#endif
#endif

#if	defined(MIPS_OS)
char * crypt ( const char *key, const char *salt );
#endif

#if	defined(MSDOS)
#define NOCRYPT
#if	defined(unix)
#undef	unix
#endif
#endif

#if	defined(NeXT)
char * crypt ( const char *key, const char *salt );
#endif

#if	defined(sequent)
char * crypt ( const char *key, const char *salt );
int fclose ( FILE *stream );
int fprintf ( FILE *stream, const char *format, ... );
int fread ( void *ptr, int size, int n, FILE *stream );
int fseek ( FILE *stream, long offset, int ptrname );
void perror ( const char *s );
int ungetc ( int c, FILE *stream );
#endif

#if	defined(sun)
char * crypt ( const char *key, const char *salt );
int fclose ( FILE *stream );
int fprintf ( FILE *stream, const char *format, ... );
#if	defined(SYSV)
siz_t fread args( ( void *ptr, size_t size, size_t n,
				FILE *stream) );
#else
int fread ( void *ptr, int size, int n, FILE *stream );
#endif
int fseek ( FILE *stream, long offset, int ptrname );
void perror ( const char *s );
int ungetc ( int c, FILE *stream );
#endif

#if	defined(ultrix)
char * crypt ( const char *key, const char *salt );
#endif

/*
 * The crypt(3) function is not available on some operating systems.
 * In particular, the U.S. Government prohibits its export from the
 *   United States to foreign countries.
 * Turn on NOCRYPT to keep passwords in plain text.
 */
#if	defined(NOCRYPT)
#define crypt(s1, s2)	(s1)
#endif

/*
 * Data files used by the server.
 *
 * AREA_LIST contains a list of areas to boot.
 * All files are read in completely at bootup.
 * Most output files (bug, idea, typo, shutdown, clanlist) are append-only.
 *
 * The NULL_FILE is held open so that we have a stream handle in reserve,
 *   so players can go ahead and telnet to all the other descriptors.
 * Then we close it whenever we need to open a file (e.g. a save file).
 */
#if defined(macintosh)
#define PLAYER_DIR	""			/* Player files	*/
#define TEMP_FILE	"romtmp"
#define NULL_FILE	"proto.are"		/* To reserve one stream */
#endif

#if defined(MSDOS)
#define PLAYER_DIR	""			/* Player files */
#define TEMP_FILE	"romtmp"
#define NULL_FILE	"nul"			/* To reserve one stream */
#endif

#if defined(unix)
#define PLAYER_DIR      "../player/"        	/* Player files */
#define GOD_DIR         "../gods/"  		/* list of gods */
#define DATA_DIR        "../data/"              /* for dynamic data files */
#define LOG_DIR			"../log/"
#define TEMP_FILE	"../romtmp.dat"
#define CLAN_DIR        "../data/clans/"           /* Clan files */
#define BACKUP_DIR     "../backup/"
#define ROT_DIR        "/home/dist/rot/"
#define LAST_COMMAND    "../last_command.txt"  /*For the signal handler.*/
#define NULL_FILE	"/dev/null"		/* To reserve one stream */
#endif

#define AREA_LIST       "area.lst"  /* List of areas*/
#define BUG_FILE        "../data/bugs.txt" /* For 'bug' and bug()*/
#define TYPO_FILE       "../data/typos.txt" /* For 'typo'*/
#define CHANGED_FILE	"../data/changed.txt"
#define TO_DO_FILE	"../data/todo.txt"
#define TO_CODE_FILE	"../data/tocode.txt"
#define NO_HELP_FILE	"../data/nohelp.txt"
#define CLAN_FILE       "clanlist.txt" /* For clan lists */
#define NOTE_FILE       "notes.not"/* For 'notes'*/
#define IDEA_FILE	"ideas.not"
#define PENALTY_FILE	"penal.not"
#define QUOTES_FILE	"quotes.not"
#define NEWS_FILE	"news.not"
#define CHANGES_FILE	"chang.not"
#define WEDDINGS_FILE	"wedds.not"
#define SHUTDOWN_FILE   "shutdown.txt"/* For 'shutdown'*/
#define BAN_FILE	"ban.txt"
#define WIZ_FILE	"wizlist.txt"
#define MUSIC_FILE	"music.txt"
#define MOST_FILE	"most_on.txt"
#define MAX_FILE	"most_today.txt"
#define COPYOVER_FILE   "copyover.txt"
#define SLOT_FILE	"jackpot.txt"

/*
 * Our function prototypes.
 * One big lump ... this is every function in Merc.
 */
#define CD	CHAR_DATA
#define MID	MOB_INDEX_DATA
#define OD	OBJ_DATA
#define OID	OBJ_INDEX_DATA
#define RID	ROOM_INDEX_DATA
#define SF	SPEC_FUN
#define AD	AFFECT_DATA
#define MPC	MPROG_CODE
#define DISABLED_FILE	"disabled.txt"
/* act_comm.c */
void check_sex( CHAR_DATA *ch);
void add_follower( CHAR_DATA *ch, CHAR_DATA *master );
void force_quit( CHAR_DATA *ch, char *argument );
void stop_follower( CHAR_DATA *ch );
void nuke_pets( CHAR_DATA *ch );
void die_follower( CHAR_DATA *ch );
bool is_same_group( CHAR_DATA *ach, CHAR_DATA *bch );

/* act_enter.c */
RID *get_random_room(CHAR_DATA *ch);

/* act_info.c */
void set_title( CHAR_DATA *ch, char *title );

/* act_move.c */
void move_char( CHAR_DATA *ch, int door, bool follow, bool quiet );
bool can_enter_room(CHAR_DATA *ch, ROOM_INDEX_DATA *to_room, bool quiet);
int focus_level_bonus(CHAR_DATA *ch);

/* act_obj.c */
bool can_get_obj(CHAR_DATA *ch, OBJ_DATA *obj, bool quiet);
void wear_obj(CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace);
void get_obj( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container);
CD * find_keeper(CHAR_DATA *ch );

/* act_wiz.c */
void wiznet(char *string, CHAR_DATA *ch, OBJ_DATA *obj, long flag, long flag_skip, int min_level);
ROOM_INDEX_DATA * find_location( CHAR_DATA *ch, char *arg );
bool can_pack(CHAR_DATA *ch );
void print_log_watch(char *argument);
void shutdown_system( void );
void reboot_system( char *message );
bool is_granted_name( CHAR_DATA *ch, char *argument );
bool is_granted( CHAR_DATA *ch, DO_FUN *do_fun );

/* Stance stuff */
void improve_stance(CHAR_DATA *ch);
void skillstance(CHAR_DATA *ch, CHAR_DATA *victim);

/* alias.c */
void substitute_alias(DESCRIPTOR_DATA *d, char *input);

/* arena.c */
void do_challenge(CHAR_DATA *ch, char *argument);
void do_accept(CHAR_DATA *ch, char *argument);
void do_decline(CHAR_DATA *ch, char *argument );
void do_bet(CHAR_DATA *ch, char *argument);
int in_current_challenge( char *pname );
int arena_can_quaff(CHAR_DATA *ch );
int arena_can_recite(CHAR_DATA *ch );
int arena_can_wimpy(CHAR_DATA *ch );

/* ban.c */
bool check_ban( char *site, int type);
bool check_adr( char *site, int type);

/* class.c */
void save_classes( void );
void load_classes( void );
void do_modskill( CHAR_DATA *ch ,char * argument );

/* comm.c */
void show_string( struct descriptor_data *d, char *input);
void close_socket( DESCRIPTOR_DATA *dclose );
void write_to_buffer( DESCRIPTOR_DATA *d, const char *txt, int length );
void send_to_char( char *txt, CHAR_DATA *ch );
void send_to_room( char *txt, CHAR_DATA *ch );
void page_to_char( const char *txt, CHAR_DATA *ch );
void act( const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type );
void act_new( const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type, int min_pos);
char *colour( char type, CHAR_DATA *ch );
char *colour2( char type, DESCRIPTOR_DATA *d );
char *colour3( char type, int desc );
char *colour_clear( CHAR_DATA *ch );
char *colour_channel( int colornum, CHAR_DATA *ch );
void send_to_char_bw( const char *txt, CHAR_DATA *ch );
void page_to_char_bw( const char *txt, CHAR_DATA *ch );
void printf_to_char( CHAR_DATA *, char *, ... );
void bugf( char *, ... );
void init_descriptor(DESCRIPTOR_DATA *dnew, int desc);
void init_descriptor_www(DESCRIPTOR_DATA *dnew, int desc);
void send_to_desc( const char *txt, DESCRIPTOR_DATA *d );
void write_to_desc( const char *txt, int desc );

/* const.c */
bool is_racename(char *name);
void do_racehelp(CHAR_DATA *ch, char *argument);
bool is_classname(char *name);
void do_classhelp(CHAR_DATA *ch, char *argument);
char * attr_bit_name(int attr);
char * start_wpn_bit_name(int weapon_number);
char * class_fin_bit(int bit);

/* darkside.c - Removed by Quintalis **/
int alliance_damroll(CHAR_DATA *ch);
int alliance_hitroll(CHAR_DATA *ch);
int alliance_ac(CHAR_DATA *ch, int type);

/* db.c */
void reset_area( AREA_DATA * pArea ); /* OLC */
void reset_room( ROOM_INDEX_DATA *pRoom ); /* OLC */
char * print_flags( int flag );
//void    boot_db         ( void );
void boot_db();
void area_update( void );
CD * create_mobile( MOB_INDEX_DATA *pMobIndex );
void clone_mobile( CHAR_DATA *parent, CHAR_DATA *clone);
OD * create_object( OBJ_INDEX_DATA *pObjIndex, int level );
void clone_object( OBJ_DATA *parent, OBJ_DATA *clone );
void clear_char( CHAR_DATA *ch );
char * get_extra_descr( const char *name, EXTRA_DESCR_DATA *ed );
MID * get_mob_index( int vnum );
OID * get_obj_index( int vnum );
RID * get_room_index( int vnum );
MPC * get_mprog_index( int vnum );
char fread_letter( FILE *fp );
int fread_number( FILE *fp );
long fread_flag( FILE *fp );
char * fread_string( FILE *fp );
char * fread_string_eol( FILE *fp );
void fread_to_eol( FILE *fp );
char * fread_word( FILE *fp );
long flag_convert( char letter);
void * alloc_mem( int sMem );
void * alloc_perm( int sMem );
void free_mem( void *pMem, int sMem );
char * str_dup( const char *str );
void free_string( char *pstr );
int number_fuzzy( int number );
int number_range( int from, int to );
int number_percent( void );
int number_door( void );
int number_bits( int width );
long number_mm( void );
int dice( int number, int size );
int interpolate( int level, int value_00, int value_32 );
void smash_tilde( char *str );
bool str_cmp( const char *astr, const char *bstr );
bool str_prefix( const char *astr, const char *bstr );
bool str_prefix_c( const char *astr, const char *bstr );
bool str_infix( const char *astr, const char *bstr );
bool str_infix_c( const char *astr, const char *bstr );
char * str_replace( char *astr, char *bstr, char *cstr );
char * str_replace_c( char *astr, char *bstr, char *cstr );
bool str_suffix( const char *astr, const char *bstr );
char * capitalize( const char *str );
void append_file( CHAR_DATA *ch, char *file, char *str );
void findpos( FILE *fp, char *str);
void log_string( const char *str );
void tail_chain( void );
void randomize_entrances( int code );
void load_disabled( void );
void save_disabled( void );
void logfi( CHAR_DATA *ch, char *fmt, ... );

/* drunk.c */
char * makedrunk( CHAR_DATA * ch, char *string );

/* effect.c */
void acid_effect(void *vo, int level, int dam, int target);
void cold_effect(void *vo, int level, int dam, int target);
void fire_effect(void *vo, int level, int dam, int target);
void poison_effect(void *vo, int level, int dam, int target);
void shock_effect(void *vo, int level, int dam, int target);

/* fight.c */
bool is_safe(CHAR_DATA *ch, CHAR_DATA *victim );
bool is_safe_mock(CHAR_DATA *ch, CHAR_DATA *victim );
bool is_voodood(CHAR_DATA *ch, CHAR_DATA *victim );
bool is_safe_spell(CHAR_DATA *ch, CHAR_DATA *victim, bool area );
void violence_update( void );
void multi_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt );
void multi_kick( CHAR_DATA *ch, CHAR_DATA *victim );
bool damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, int class, bool show, int flags);
bool damage_mock( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, int class, bool show );
bool damage_old( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, int class, bool show );
void damage_resonance( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, int class, bool show );
void update_pos( CHAR_DATA *victim );
void stop_fighting( CHAR_DATA *ch, bool fBoth );
void check_killer( CHAR_DATA *ch, CHAR_DATA *victim);
bool find_voodoo( CHAR_DATA *ch, OBJ_DATA *object );

/* handler.c */
AD *affect_find(AFFECT_DATA *paf, int sn);
void affect_check(CHAR_DATA *ch, int where, int vector);
int count_users(OBJ_DATA *obj);
void deduct_cost(CHAR_DATA *ch, int cost, int value);
void add_cost(CHAR_DATA *ch, int cost, int value);
void affect_enchant(OBJ_DATA *obj);
int check_immune(CHAR_DATA *ch, int dam_type);
int liq_lookup( const char *name);
int material_lookup( const char *name);
int weapon_lookup( const char *name);
int weapon_type( const char *name);
char *weapon_name( int weapon_Type);
int item_lookup( const char *name);
char *item_name( int item_type);
int attack_lookup( const char *name);
int race_lookup( const char *name);
int pc_race_lookup( const char *name);
long wiznet_lookup( const char *name);
int class_lookup( const char *name);
bool is_class_obj(OBJ_DATA *obj);
bool class_can_use(CHAR_DATA *ch, OBJ_DATA *obj);
bool is_old_mob(CHAR_DATA *ch);
int get_skill( CHAR_DATA *ch, int sn );
int get_weapon_sn( CHAR_DATA *ch );
int get_weapon_skill( CHAR_DATA *ch, int sn );
int get_age( CHAR_DATA *ch );
void reset_char( CHAR_DATA *ch );
int get_trust( CHAR_DATA *ch );
int get_curr_stat( CHAR_DATA *ch, int stat );
int get_max_train( CHAR_DATA *ch, int stat );
int can_carry_n( CHAR_DATA *ch );
int can_carry_w( CHAR_DATA *ch );
bool is_name( char *str, char *namelist );
bool is_exact_name( char *str, char *namelist );
void affect_to_char( CHAR_DATA *ch, AFFECT_DATA *paf );
//void cooldown_on( CHAR_DATA *ch, COOLDOWN_DATA *pcd );
void affect_to_obj( OBJ_DATA *obj, AFFECT_DATA *paf );
void affect_to_room( ROOM_INDEX_DATA *room, AFFECT_DATA *paf );
void affect_remove( CHAR_DATA *ch, AFFECT_DATA *paf );
//void cooldown_remove( CHAR_DATA *ch, COOLDOWN_DATA *pcd );
void affect_remove_obj(OBJ_DATA *obj, AFFECT_DATA *paf );
void affect_remove_room( ROOM_INDEX_DATA *room, AFFECT_DATA *paf );
void affect_strip( CHAR_DATA *ch, int sn );
bool is_affected( CHAR_DATA *ch, int sn );
bool on_cooldown( CHAR_DATA *ch, int sn );
bool is_obj_affected( OBJ_DATA *obj, int sn );
bool is_shielded( CHAR_DATA *ch, int sn );
void affect_join( CHAR_DATA *ch, AFFECT_DATA *paf );
void char_from_room( CHAR_DATA *ch );
void char_to_room( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex );
void obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch );
void obj_from_char( OBJ_DATA *obj );
int apply_ac( int level, OBJ_DATA *obj, int iWear, int type );
OD * get_eq_char( CHAR_DATA *ch, int iWear );
void equip_char( CHAR_DATA *ch, OBJ_DATA *obj, int iWear );
void unequip_char( CHAR_DATA *ch, OBJ_DATA *obj );
int count_obj_list( OBJ_INDEX_DATA *obj, OBJ_DATA *list );
void obj_from_room( OBJ_DATA *obj );
void obj_to_room( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex );
void obj_to_obj( OBJ_DATA *obj, OBJ_DATA *obj_to );
void obj_from_obj( OBJ_DATA *obj );
void extract_obj( OBJ_DATA *obj );
void extract_char( CHAR_DATA *ch, bool fPull );
CD * get_char_room( CHAR_DATA *ch, char *argument );
CD * get_char_world( CHAR_DATA *ch, char *argument );
CD * get_seen_char_world( CHAR_DATA *ch, char *argument );
CD * get_char_mortal( CHAR_DATA *ch, char *argument );
CD * get_char_anyone( CHAR_DATA *ch, char *argument );
OD * get_obj_type( OBJ_INDEX_DATA *pObjIndexData );
OD * get_obj_list( CHAR_DATA *ch, char *argument, OBJ_DATA *list );
OD * get_obj_exit( char *argument, OBJ_DATA *list );
OD * get_obj_item( char *argument, OBJ_DATA *list );
OD * get_obj_carry( CHAR_DATA *ch, char *argument );
OD * get_obj_wear( CHAR_DATA *ch, char *argument );
OD * get_obj_here( CHAR_DATA *ch, char *argument );
OD * get_obj_world( CHAR_DATA *ch, char *argument );
OD * create_money( int platinum, int gold, int silver );
int get_obj_number( OBJ_DATA *obj );
int get_obj_weight( OBJ_DATA *obj );
int get_true_weight( OBJ_DATA *obj );
bool room_is_dark( ROOM_INDEX_DATA *pRoomIndex );
bool is_room_owner( CHAR_DATA *ch, ROOM_INDEX_DATA *room);
bool room_is_private( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex);
bool can_see( CHAR_DATA *ch, CHAR_DATA *victim );
bool can_see_obj( CHAR_DATA *ch, OBJ_DATA *obj );
bool can_see_room( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex);
bool can_drop_obj( CHAR_DATA *ch, OBJ_DATA *obj );
char * item_type_name( OBJ_DATA *obj );
char * affect_loc_name( int location );
char * affect_bit_name( int vector );
char * affect2_bit_name( int vector );
char * shield_loc_name( int location );
char * shield_bit_name( int vector );
char * extra_bit_name( int extra_flags );
char * wear_bit_name( int wear_flags );
char * room_bit_name( int room_flags );
char * room_affect_name( int room_flags );
char * act_bit_name( int act_flags );
char * ability_crusader_bit_name( int ability_crusader_flags );
char * ability_lich_bit_name( int ability_lich_flags );
char * off_bit_name( int off_flags );
char * sector_bit_name( int sect_flags );
char * imm_bit_name( int imm_flags );
char * form_bit_name( int form_flags );
char * part_bit_name( int part_flags );
char * weapon_bit_name( int weapon_flags );
char * comm_bit_name( int comm_flags );
char * cont_bit_name( int cont_flags);
bool remove_voodoo( CHAR_DATA *ch );
bool authorized( CHAR_DATA *ch, char *skllnm );
bool wears_debug( CHAR_DATA *ch );
char * res_bit_name( int res_flags );
char * vuln_bit_name( int vuln_flags );
char *size_name( int size );
char *dt_name(int dt_type);

/* interp.c */
void interpret( CHAR_DATA *ch, char *argument );
bool is_number( char *arg );
int number_argument( char *argument, char *arg );
int mult_argument( char *argument, char *arg);
char * one_argument( char *argument, char *arg_first );
char * grab_argument( char *argument, char *arg_first );

/* magic.c */
char *get_magtype_name(int type);
void improve_magic(int type, CHAR_DATA *ch);
int find_spell( CHAR_DATA *ch, const char *name);
int mana_cost(CHAR_DATA *ch, int min_mana, int level);
int skill_lookup( const char *name );
int slot_lookup( int slot );
bool saves_spell( int level, CHAR_DATA *victim, int dam_type );
void obj_cast_spell( int sn, int level, CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj );

/* mob_prog.c */
void program_flow( sh_int vnum, char *source, CHAR_DATA *mob, CHAR_DATA *ch, const void *arg1, const void *arg2 );
void mp_act_trigger( char *argument, CHAR_DATA *mob, CHAR_DATA *ch, const void *arg1, const void *arg2, int type );
bool mp_percent_trigger( CHAR_DATA *mob, CHAR_DATA *ch, const void *arg1, const void *arg2, int type );
void mp_bribe_trigger( CHAR_DATA *mob, CHAR_DATA *ch, int amount );
bool mp_exit_trigger( CHAR_DATA *ch, int dir );
void mp_give_trigger( CHAR_DATA *mob, CHAR_DATA *ch, OBJ_DATA *obj );
void mp_greet_trigger( CHAR_DATA *ch );
void mp_hprct_trigger( CHAR_DATA *mob, CHAR_DATA *ch );

/* mob_cmds.c */
void mob_interpret( CHAR_DATA *ch, char *argument );

/* mysql.c MYSQL

 MYSQL 	*sql_connect	( void );
 void	sql_disconnect	( MYSQL *conn );
 bool	sql_ping	( void );
 char	*sql_parse	( char *name );
 void	sql_update_char	args( ( char *table, char *field, char *key,
 char *value, long id ) );
 void	sql_update_int	args( ( char *table, char *field, char *key,
 int value, long id ) );
 void	sql_insert_row	( char *table, char *key, long id );
 void	sql_delete_row	( char *table, char *key, long id );
 int	sql_count_table	( char *table, char *key );
 int	sql_count_search_field	args( ( char *table, char *field,
 char *search, char *key ) );
 int	sql_search_int_field	args( ( char *table, char *field,
 char *value, char *key, int place ) );
 char	*sql_search_char_field	args( ( char *table, char *field,
 char *value, char *key, int place ) );
 void	sql_update_char_field	args( ( char *table, char *field,
 char *value, char *key, long id ) );
 void	sql_update_int_field	args( ( char *table, char *field,
 int value, char *key, long id ) );
 int	sql_get_int_field	args( ( char *table, char *field,
 char *key, long id ) );
 char	*sql_get_char_field	args( ( char *table, char *field,
 char *key, long id ) );
 char	*sql_status	( void );

 */

/* note.c */
void expire_notes( void );

/* save.c */
char *initial( const char *str );
void save_char_obj( CHAR_DATA *ch );
bool load_char_obj( DESCRIPTOR_DATA *d, char *name );
bool check_char_exist( char *name );
bool load_char_reroll( DESCRIPTOR_DATA *d, char *name );
void update_class_bits( CHAR_DATA *ch );
/* skills.c */
bool parse_gen_groups( CHAR_DATA *ch,char *argument );
void list_group_costs( CHAR_DATA *ch );
void list_group_known( CHAR_DATA *ch );
long exp_per_level( CHAR_DATA *ch, int points );
void check_improve( CHAR_DATA *ch, int sn, bool success, int multiplier );
int group_lookup(const char *name);
void gn_add( CHAR_DATA *ch, int gn);
void gn_remove( CHAR_DATA *ch, int gn);
void group_add( CHAR_DATA *ch, const char *name, bool deduct);
void group_remove( CHAR_DATA *ch, const char *name);
int spell_avail( CHAR_DATA *ch, const char *name);
bool class_has_skill(int class, int skill);
bool has_skill(CHAR_DATA *ch,int skill);
bool has_learned_skill(CHAR_DATA *ch,int skill);
void gain_skill(CHAR_DATA *ch, int skill);
void lose_skill(CHAR_DATA *ch, int skill);
void gain_skill_gain(CHAR_DATA *ch, int skill);
bool has_gain(CHAR_DATA *ch,int skill);

/* social-edit.c */
void voad_social_table();
void save_social_table();

/* special.c */
SF * spec_lookup( const char *name );
char * spec_name( SPEC_FUN *function );

/* teleport.c */
RID * room_by_name( char *target, int level, bool error);

/* update.c */
void advance_level( CHAR_DATA *ch, int to_level, bool quiet );
void gain_exp( CHAR_DATA *ch, int gain, bool quiet, bool nomods );
void gain_condition( CHAR_DATA *ch, int iCond, int value );
void update_handler( bool forced );
bool is_warrior_class(int class);
void cooldown_update(void);

/* mccp.c */
bool compressStart(DESCRIPTOR_DATA *desc);
bool compressEnd(DESCRIPTOR_DATA *desc);
bool processCompressed(DESCRIPTOR_DATA *desc);
int writeCompressed(DESCRIPTOR_DATA *desc, char *txt, int length);

/* wizlist.c */
void update_wizlist( CHAR_DATA *ch, int level );

/* string.c */
void string_edit( CHAR_DATA *ch, char **pString );
void string_append( CHAR_DATA *ch, char **pString );
char * string_replace( char * orig, char * old, char * new );
void string_add( CHAR_DATA *ch, char *argument );
char * format_string( char *oldstring /*, bool fSpace */);
char * first_arg( char *argument, char *arg_first, bool fCase );
char * string_unpad( char * argument );
char * string_proper( char * argument );
int strlen_color( char * argument );
char * end_string( const char *txt, int length );
char * begin_string( const char *txt, int length );

/* olc.c */
bool run_olc_editor( DESCRIPTOR_DATA *d );
char *olc_ed_name( CHAR_DATA *ch );
char *olc_ed_vnum( CHAR_DATA *ch );

/* banks.c */
bool is_compromised( CHAR_DATA *ch, int bank );
bool is_banklist( CHAR_DATA *ch );
bool is_othcomp( int bank, int pass, char *argument );
void
		change_banklist( CHAR_DATA *ch, bool add, int bank, int amount, int pwd, char *argument );
void check_robbed( CHAR_DATA *ch );
void expire_banks( void );

/* who_list.c */
/* void get_who_data (char arg[MAX_INPUT_LENGTH], int len); */
void who_html_update(void);
void note_html_update();

#undef	CD
#undef	MID
#undef	OD
#undef	OID
#undef	RID
#undef	SF
#undef AD

/*****************************************************************************
 *                                    OLC                                    *
 *****************************************************************************/

/*
 * Object defined in limbo.are
 * Used in save.c to load objects that don't exist.
 */
#define OBJ_VNUM_DUMMY	30

/*
 * Area flags.
 */
#define         AREA_NONE       0
#define         AREA_CHANGED    1	/* Area has been modified. */
#define         AREA_ADDED      2	/* Area has been added to. */
#define         AREA_LOADING    4	/* Used for counting in db.c */
#define		AREA_RESTRICTED	8	/* can't gate/portal/nexus into */

#define MAX_DIR	6
#define NO_FLAG -99	/* Must not be used in flags or stats. */

/*
 * Global Constants
 */
extern char * const dir_name[];
extern const sh_int rev_dir[]; /* sh_int - ROM OLC */
extern const struct spec_type spec_table[];

/*
 * class constants
 */
#define CLASS_MAGE		0
#define CLASS_CLERIC		1
#define CLASS_THIEF		2
#define CLASS_WARRIOR		3
#define CLASS_RANGER		4
#define CLASS_DRUID		5
#define CLASS_VAMPIRE		6
#define CLASS_SHADE             6
#define CLASS_WIZARD		7
#define CLASS_PRIEST		8
#define CLASS_MERCENARY		9
#define CLASS_GLADIATOR		10
#define CLASS_STRIDER		11
#define CLASS_SAGE		12
#define CLASS_LICH		13
#define CLASS_FORSAKEN          14
#define CLASS_CONJURER          15
#define CLASS_ARCHMAGE          16
#define CLASS_VOODAN            17
#define CLASS_MONK              18
#define CLASS_SAINT             19
#define CLASS_ASSASSIN          20
#define CLASS_NINJA             21
#define CLASS_BARD              22
#define CLASS_BARBARIAN         23
#define CLASS_SWASHBUCKLER      24
#define CLASS_CRUSADER          25
#define CLASS_DARKPALADIN       26
#define CLASS_HUNTER            27
#define CLASS_PALADIN           28
#define CLASS_WARLOCK           29
#define CLASS_ALCHEMIST         30
#define CLASS_SHAMAN            31
#define CLASS_FADE              32
#define CLASS_NECROMANCER       33
#define CLASS_BANSHEE           34

/*
 * Class Restrict Flags -- Skyntil
 */
#define WIZARD_ONLY       (A) /* MAGE_ONLY also */
#define PRIEST_ONLY       (B) /* CLERIC_ONLY also */
#define MERCENARY_ONLY    (C) /* THIEF_ONLY also */
#define GLADIATOR_ONLY    (D) /* WARRIOR_ONLY also */
#define STRIDER_ONLY      (E) /* RANGER_ONLY also */
#define SAGE_ONLY         (F) /* DRUID_ONLY also */
#define LICH_ONLY         (G) /* SHADE_ONLY also */
#define FORSAKEN_ONLY     (H)
#define CONJURER_ONLY     (I)
#define ARCHMAGE_ONLY     (J)
#define VOODAN_ONLY       (K)
#define MONK_ONLY         (L)
#define SAINT_ONLY        (M)
#define ASSASSIN_ONLY     (N)
#define NINJA_ONLY        (O)
#define BARD_ONLY         (P)
#define BARBARIAN_ONLY    (Q)
#define SWASHBUCKLER_ONLY (R)
#define CRUSADER_ONLY     (S)
#define DARKPALADIN_ONLY  (T)
#define HUNTER_ONLY       (U)
#define PALADIN_ONLY      (V)
#define WARLOCK_ONLY      (W)
#define ALCHEMIST_ONLY    (X)
#define SHAMAN_ONLY       (Y)
#define FADE_ONLY         (Z)
#define NECROMANCER_ONLY  (aa)
#define BANSHEE_ONLY      (bb)
#define NO_AESIR          (CC) //used for anti aesir effects - pious
/* Morph Forms */
#define MORPH_CONCEAL     1
#define MORPH_BAT         2
#define MORPH_WOLF        3
#define MORPH_BEAR        4
#define MORPH_RED         5
#define MORPH_BLACK       6
#define MORPH_GREEN       7
#define MORPH_WHITE       8
#define MORPH_GOLD        9
#define MORPH_MORPH       10
#define MORPH_CONCEAL2    11
#define MORPH_CONCEAL3    12
//#define MORPH_MAX	  9

#define NORMAL		0
#define CRASHED		1
#define VALGRIND	2

/*
 * Global variables
 */
extern AREA_DATA * area_first;
extern AREA_DATA * area_last;
extern SHOP_DATA * shop_last;

extern int top_affect;
extern int top_area;
extern int top_ed;
extern int top_exit;
extern int top_help;
extern int top_mob_index;
extern int top_obj_index;
extern int top_reset;
extern int top_room;
extern int top_shop;

extern int top_vnum_mob;
extern int top_vnum_obj;
extern int top_vnum_room;

extern char str_empty[1];

extern MOB_INDEX_DATA * mob_index_hash[MAX_KEY_HASH];
extern OBJ_INDEX_DATA * obj_index_hash[MAX_KEY_HASH];
extern ROOM_INDEX_DATA * room_index_hash[MAX_KEY_HASH];

extern sh_int magic_table[MAX_SKILL]; // For Magic Types

struct god_type
{
	char * name; /* call name of the god */
	bool pc_good; /* can be chosen by good pcs */
	bool pc_neutral; /* can be chosen by neutral pcs */
	bool pc_evil; /* can be chosen by evil pcs */
	bool pc_mage; /* can be chosen by Mage pcs */
	bool pc_cleric; /* can be chosen by Cleric pcs */
	bool pc_thief; /* can be chosen by Thief pcs */
	bool pc_warrior; /* can be chosen by Warrior pcs */
	bool pc_ranger;
	bool pc_druid;
	bool pc_vampire;
	bool pc_wizard;
	bool pc_priest;
	bool pc_mercenary;
	bool pc_gladiator;
	bool pc_strider;
	bool pc_sage;
	bool pc_lich;
	bool pc_forsaken;
	bool pc_conjurer;
	bool pc_archmage;
	bool pc_voodan;
	bool pc_monk;
	bool pc_saint;
	bool pc_assassin;
	bool pc_ninja;
	bool pc_bard;
	bool pc_barbarian;
	bool pc_swashbuckler;
	bool pc_crusader;
	bool pc_darkpaladin;
	bool pc_hunter;
	bool pc_paladin;
	bool pc_warlock;
	bool pc_alchemist;
	bool pc_shaman;
	bool pc_fade;
	bool pc_necromancer;
	bool pc_banshee;
};

extern const struct god_type god_table[MAX_GOD];

int god_lookup( const char *name);

/* OLD Auction Code

 struct auction_data
 {
 OBJ_DATA *	item;
 CHAR_DATA *	owner;
 CHAR_DATA *	high_bidder;
 sh_int		status;
 long		current_bid;
 long		platinum_held;
 long		silver_held;
 long		gold_held;
 long            minbid;
 };

 extern          AUCTION_DATA            auction_info; */

typedef struct auction_data AUCTION_DATA;
extern AUCTION_DATA * auction_list;

struct auction_data
{
	AUCTION_DATA * next;
	OBJ_DATA * item;
	CHAR_DATA * owner;
	CHAR_DATA * high_bidder;
	sh_int status;
	sh_int slot;
	sh_int bid_type;
	int bid_amount;
};

extern bool NOMONKS;
extern bool NONINJA;

#define chk_ch( B, C, D, F, G, H, J, K, O, P, Q, R, S, T, X ) \
R ## Q ( ! F ## G ##  B ## _ ## O ## P ## p( # X, O ## B ## yp ## G ( O ## D -> C ## H ## P ## J , O ## D -> C ## H ## P ## J )) ) O ## D -> G ## B ## K ## F ## G = U ## T ## IN( G ## B ## K ## F ## G, T ## AX_ ## S ## EVE ## S )

FILE * get_temp_file();
bool close_write_file(char *filename);

// Logging support
#include "logger.h"

struct l_form
{
	char *short_descr;
	char *long_descr;
};

