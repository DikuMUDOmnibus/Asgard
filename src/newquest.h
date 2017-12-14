/*
 * newquest.h
 *
 *  Created on: 7 Jan 2011
 *      Author: Nico
 */

#define COST_QUEST		1
#define COST_PLAT		2
#define COST_NOTAPPL	3

#define QSTATE_COMPLETE	-1
#define QSTATE_NONE		0
#define QSTATE_MOB		1	// Kill-a-target quest
#define QSTATE_OBJ		2	// Retrieve-a-token quest

#define QTYPE_MIN		QSTATE_MOB
#define QTYPE_MAX		QSTATE_OBJ

#define QTOKEN_LOWER	9825	// Quest tokens for obj quest
#define QTOKEN_UPPER	9829	// Ditto above

#define LOCKER_VNUM		79

#define VNUM_XPVOUCHER_025		858
#define VNUM_XPVOUCHER_050		904
#define VNUM_XPVOUCHER_075		915
#define VNUM_XPVOUCHER_100		906
#define VNUM_XPVOUCHER_125		916
#define VNUM_XPVOUCHER_150		917
#define VNUM_XPVOUCHER_175		918
#define VNUM_XPVOUCHER_200		919

#define VNUM_QPVOUCHER_025		907
#define VNUM_QPVOUCHER_050		908
#define VNUM_QPVOUCHER_075		914
#define VNUM_QPVOUCHER_100		909
#define VNUM_QPVOUCHER_125		910
#define VNUM_QPVOUCHER_150		911
#define VNUM_QPVOUCHER_175		912
#define VNUM_QPVOUCHER_200		913

typedef struct quest_item QUEST_ITEM;
typedef struct quest_verb QUEST_VERB;
typedef void (*QITEM_FN)(CHAR_DATA*, CHAR_DATA *, QUEST_VERB*, QUEST_ITEM*, char*);
#define QITEM_FN_NAME(Name)	questitem_fn_ ## Name
#define QITEM_FN_DEF(Name)	void QITEM_FN_NAME(Name) (CHAR_DATA *ch, CHAR_DATA *qm, QUEST_VERB *verb, QUEST_ITEM *item, char *argument)

struct quest_item {
	char		*name;			// Name/keyword.
	int			obj_vnum;		// Item object vnum, if applicable.
	sh_int		cost_type;		// Type of price, i.e. plat/IQ/AQP
	int			cost;			// Cost in cost_type units.
	QITEM_FN	func;			// Function for the item.
	QUEST_ITEM	*items;			// Child items.
	char		*description;	// About the item.
};

struct quest_verb {
	char		*name;			// Name/keyword.
	bool		need_qm;		// Whether or not a quest master needs to be present.
	QUEST_ITEM	*items;			// Items.
	char		*description;	// What the verb does.
};

void end_quest(CHAR_DATA *ch, int timer);
void set_locker_name(CHAR_DATA *ch);
