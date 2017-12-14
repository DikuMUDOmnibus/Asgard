/*
 * newrandom.c
 *
 *  Created on: 2 Feb 2011
 *      Author: Nico and Ishamael
 *  Based on random object code by Chris Langlois(tas@intrepid.inetsolve.com) and Gabe Volker.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "newclan.h"
#include "newrandom.h"
#include "recycle.h"

// TODO: move the uniques/randoms into separate areas,
// remembering to edit pfiles for the new vnums.
#define VNUM_UNIQUE_LOWER	850
#define VNUM_UNIQUE_UPPER	999
#define VNUM_RANDOM_LOWER	24500
#define VNUM_RANDOM_UPPER	25499

// 400 each for prefixes/suffixes
#define VNUM_PREFIX_LOWER	25500
#define VNUM_PREFIX_UPPER	25899
#define VNUM_SUFFIX_LOWER	25900
#define VNUM_SUFFIX_UPPER	26299
//#define VNUM_PREFIX_LOWER	700
//#define VNUM_PREFIX_UPPER	849
//#define VNUM_SUFFIX_LOWER	700
//#define VNUM_SUFFIX_UPPER	849

// Default maximum equipment (not pop) level for items.
#define DEFAULT_MAX_LEVEL	101

// Quality types.
#define QUALITY_WORST		-3
#define QUALITY_WORSE		-2
#define QUALITY_BAD			-1
#define QUALITY_NORMAL		0
#define QUALITY_GOOD		1
#define QUALITY_BETTER		2
#define QUALITY_BEST		3

// Range around pop_level to use for min/max prefix/suffix levels.
#define POP_RANGE			20

OBJ_INDEX_LIST *obj_index_list_free;

static const struct quality_type quality_names[] = {
	{ "broken", QUALITY_WORST, "{r" },
	{ "dented", QUALITY_WORSE, "{y" },
	{ "scratched", QUALITY_BAD, "{D" },
	{ "normal", QUALITY_NORMAL, "" },
	{ "improved", QUALITY_GOOD, "{C" },
	{ "exceptional", QUALITY_BETTER, "{M" },
	{ "flawless", QUALITY_BEST, "{G" },
	{NULL},
};

// Start of helper functions.
OBJ_INDEX_LIST *new_obj_index_list(OBJ_INDEX_DATA *object_index) {
	OBJ_INDEX_LIST *oil;
	if (obj_index_list_free != NULL) {
		oil = obj_index_list_free;
		obj_index_list_free = obj_index_list_free->next;
	} else
		oil = malloc(sizeof(OBJ_INDEX_LIST));

	oil->object_index = object_index;
	oil->next = NULL;
	return oil;
}
void free_obj_index_list(OBJ_INDEX_LIST *oil, bool all) {
	if (oil == NULL)
		return;
	// Recursive free.
	if (all)
		free_obj_index_list(oil->next, TRUE);
	oil->next = obj_index_list_free;
	obj_index_list_free = oil;
}
bool is_in_obj_index_list(OBJ_INDEX_DATA *objIndex, OBJ_INDEX_LIST *oil) {
	for (; oil != NULL; oil = oil->next)
		if (oil->object_index->vnum == objIndex->vnum)
			return TRUE;
	return FALSE;
}

// Util functions mainly for specific loads in act_wiz.c
static OBJ_INDEX_DATA *ritem_lookup(char *name, int min_vnum, int max_vnum) {
	OBJ_INDEX_DATA *objIndex;
	int vnum = min_vnum;

	for (vnum = min_vnum; vnum <= max_vnum; vnum++) {
		if ((objIndex = get_obj_index(vnum)) != NULL && is_name(name, objIndex->name))
			return objIndex;
	}

	return NULL;
}

OBJ_INDEX_LIST *prefix_lookup(char *name) {
	OBJ_INDEX_DATA *objIndex = ritem_lookup(name, VNUM_PREFIX_LOWER, VNUM_PREFIX_UPPER);

	if (objIndex == NULL)
		return NULL;
	return new_obj_index_list(objIndex);
}

OBJ_INDEX_LIST *suffix_lookup(char *name) {
	OBJ_INDEX_DATA *objIndex = ritem_lookup(name, VNUM_SUFFIX_LOWER, VNUM_SUFFIX_UPPER);

	if (objIndex == NULL)
		return NULL;
	return new_obj_index_list(objIndex);
}

OBJ_INDEX_DATA *rbase_object_lookup(char *name) {
	return ritem_lookup(name, VNUM_RANDOM_LOWER, VNUM_RANDOM_UPPER);
}

const struct quality_type *quality_lookup_name(char *name) {
	const struct quality_type *quality_struct;
	for (quality_struct = quality_names; quality_struct->name != NULL; quality_struct++)
		if (!str_cmp(quality_struct->name, name))
			return quality_struct;
	return NULL;
}

const struct quality_type *quality_lookup_index(int index) {
	const struct quality_type *quality_struct;
	for (quality_struct = quality_names; quality_struct->name != NULL; quality_struct++)
		if (quality_struct->index == index)
			return quality_struct;
	return NULL;
}

// Amount to drop the level by on unsuccessful searches by level_based_load.
#define LEVEL_STEP	10
static OBJ_INDEX_DATA *level_based_load(int vnum_lower, int vnum_upper, int level_lower, int level_upper, bool try_lower) {
	OBJ_INDEX_DATA *object_index;
	OBJ_INDEX_LIST *list = NULL, *elem;
	int count = 0, vnum;

	// Why do we use a list? Stops having to redo the hash list lookup/level checking all over again.

	// Build a list of object indexes that pass level conditions.
	for (vnum = vnum_lower; vnum <= vnum_upper; vnum++) {
		object_index = get_obj_index(vnum);
		if (object_index == NULL
		|| object_index->level < level_lower
		|| object_index->level > level_upper)
			continue;
		// Add to list, increment count.
		elem = new_obj_index_list(object_index);
		elem->next = list; list = elem;
		count++;
	}

	// No objects that match conditions.
	if (count == 0) {
		if (try_lower && level_lower > 1) {
			// Lets drop the lower bound and try again.
			return level_based_load(vnum_lower, vnum_upper, UMAX(1, level_lower-LEVEL_STEP), level_upper, try_lower);
		}
		return NULL;
	}

	// Get an index into the built list and find the corresponding object index.
	int random_pick = number_range(0, count-1);
	for (elem = list; random_pick-- > 0; elem = elem->next);
	object_index = elem->object_index;

	free_obj_index_list(list, TRUE);
	return object_index;
}

#define assign_field(Field, Value)	\
	free_string(Field);				\
	Field = str_dup(Value);

#define sprintf_field(TempBuf, Field, Args...)	\
	sprintf(TempBuf, Args);						\
	assign_field(Field, TempBuf);

// Start of the proper meat.

static void add_modifier(OBJ_INDEX_DATA *modifier, OBJ_DATA *object, bool prefix_not_suffix, char *connector, int mob_tier, int base_tier_bonus) {
	char name[MAX_STRING_LENGTH];
	// Set keywords.
	sprintf_field(name, object->name, "%s %s", strip_bespaces(process_name(modifier->name)), object->name);

	// Set short description.
	if (prefix_not_suffix)	// Prefix, modifier's short description comes first.
		sprintf(name, "%s%s%s", modifier->short_descr, connector, object->short_descr);
	else					// Suffix, modifier's short description comes last.
		sprintf(name, "%s%s%s", object->short_descr, connector, modifier->short_descr);
	assign_field(object->short_descr, name);
	assign_field(object->description, name);

	// Add effects and whatnot.
	if (modifier->item_type == ITEM_WEAPON && object->item_type == ITEM_WEAPON)
		SET_BIT(object->value[4], modifier->value[4]);
	SET_BIT(object->extra_flags, modifier->extra_flags);
	AFFECT_DATA *affect, temp_affect;
	for (affect = modifier->affected; affect != NULL; affect = affect->next) {
		if (affect->location != APPLY_MORPH_FORM) {
			temp_affect = *affect;
			temp_affect.modifier = affect->modifier + number_range((affect->modifier * mob_tier / 99 / base_tier_bonus), (affect->modifier * mob_tier / 99));
			affect_to_obj(object, &temp_affect);
		}
	}
}

static void combine_fuzz_effects(OBJ_DATA *object) {
	AFFECT_DATA *current, *search, *search_next, *search_prev;

	for (current = object->affected; current != NULL; current = current->next) {
		if (current->location == APPLY_MORPH_FORM)
			continue;
		// Combine.
		for (search = current->next, search_prev = current; search != NULL; search = search_next) {
			search_next = search->next;
			if (current->where == search->where && current->type == search->type
			&&  current->location == search->location && current->bitvector == search->bitvector) {
				search_prev->next = search_next;
				current->modifier += search->modifier;
				free_affect(search);
			} else
				search_prev = search;
		}
		// Fuzzy
		current->modifier = number_range((current->modifier*9)/10, (current->modifier*11)/10);
	}

	// Remove zero effects.
	for (search = object->affected, search_prev = NULL; search != NULL; search = search_next) {
		search_next = search->next;
		if (search->modifier == 0 && search->bitvector == 0) {
			if (search_prev == NULL)
				object->affected = search_next;
			else
				search_prev->next = search_next;
			free_affect(search);
		} else
			search_prev = search;
	}
}

// Apply effects from prefixes, suffixes and modifiers to object chosen.
// Slightly random in terms of effect numbers.
OBJ_DATA *specific_object(int quality, OBJ_DATA *object, OBJ_INDEX_LIST *prefixes, OBJ_INDEX_LIST *suffixes, bool unique, int mob_tier) {
	// Calculate the alignment of the object - currently held in the weight field of the object.
	int alignment = 0, count = 0, chance = 0, modifier = 0, levelr = 0;
	OBJ_INDEX_LIST *elem;
	char name[MAX_STRING_LENGTH];

	chance = number_range(2,5); // Set a lower cap for tier bonus on object. Higher number is worse.

	if ((object->item_type == ITEM_ARMOR) && (unique == FALSE) && (mob_tier > 2)) {
		object->value[0] = object->value[0] + number_range(object->value[0] * mob_tier / 99 * ( 10 - chance ) / 10, object->value[0] * mob_tier / 99);
		object->value[1] = object->value[1] + number_range(object->value[1] * mob_tier / 99 * ( 10 - chance ) / 10, object->value[1] * mob_tier / 99);
		object->value[2] = object->value[2] + number_range(object->value[2] * mob_tier / 99 * ( 10 - chance ) / 10, object->value[2] * mob_tier / 99);
		object->value[3] = object->value[3] + number_range(object->value[3] * mob_tier / 99 * ( 10 - chance ) / 10, object->value[3] * mob_tier / 99);
	}
	if ((object->item_type == ITEM_ARMOR) && (unique == FALSE) && (quality < QUALITY_NORMAL)) {
		// Let's reduce base AC on broken/cracked things.
		// Higher chance = more corrosive effect!
		// BAD = -1, WORSE = -2, WORST = -3
		//modifier = -((quality * 5) - chance + 7); // -13 - 5 = -18.  -5 - -2 = -7 + 7 = 0
						       // Range is -11 to 0
		modifier = number_range((-quality * 4), (-quality * 5));

		if (modifier > 0) {
			object->value[0] = object->value[0] * (15 - modifier) / 15; // ( 1 - 11 )
			object->value[1] = object->value[1] * (15 - modifier) / 15; // ( 1 - 11 )
			object->value[2] = object->value[2] * (15 - modifier) / 15; // ( 1 - 11 )
			object->value[3] = object->value[3] * (15 - modifier) / 15; // ( 1 - 11 )
		}
	}
	if ((object->item_type == ITEM_ARMOR) && ((object->value[0] < 0) || (object->value[1] < 0) || (object->value[2] < 0) || (object->value[3] < 0))) {
		object->value[0] = 0;
		object->value[1] = 0;
		object->value[2] = 0;
		object->value[3] = 0;
	}
	if ((object->item_type == ITEM_WEAPON) && (unique == FALSE) && (quality < QUALITY_NORMAL)) {
		if ((quality < QUALITY_BAD) || ((quality == QUALITY_BAD) && (number_range(0,1) == 1)))
			object->value[1] = object->value[1] - number_range((-quality - 1), ((-quality - 1) * 2));
		object->value[2] = object->value[2] - number_range((-quality - 1), ((-quality - 1) * 2));

		if (object->value[1] < 1)
			object->value[1] = 1;
		if (object->value[2] < 1)
			object->value[2] = 1;
	}

	if (mob_tier > 1) {
		if (quality >= QUALITY_NORMAL)
			levelr = mob_tier;
		else if (quality == QUALITY_BAD)
			levelr = number_range(mob_tier * 7 / 10, mob_tier);
		else if (quality == QUALITY_WORSE)
			levelr = number_range(mob_tier * 5 / 10, mob_tier * 7 / 10);
		else if (quality == QUALITY_WORST)
			levelr = number_range(mob_tier * 2 / 10, mob_tier * 4 / 10);
	}

	if ((unique == TRUE) || (levelr < 0) || (mob_tier <= 0))
		object->tier_level = 0;
	else
		object->tier_level = levelr;


	if (prefixes != NULL) {
		// Add all the prefixes.
		for (elem = prefixes; elem != NULL; elem = elem->next) {
			alignment += elem->object_index->weight;
			add_modifier(elem->object_index, object, TRUE, " ", mob_tier, chance);
		}
		free_obj_index_list(prefixes, TRUE);
	}
	if (suffixes != NULL) {
		char *connector;
		// Add all the suffixes.
		count = 0;
		for (elem = suffixes; elem != NULL; elem = elem->next, count++) {
			// We want it to be <item name> of <suffix>, <suffix> and <suffix>.
			if (count == 0)
				connector = " of ";
			else if (elem->next == NULL)
				connector = " and ";
			else
				connector = ", ";
			alignment += elem->object_index->weight;
			add_modifier(elem->object_index, object, FALSE, connector, mob_tier, chance);
		}
		free_obj_index_list(suffixes, TRUE);
	}

	// Too evil or too good, make it anti-neutral.
	if (alignment <= -600 || alignment >= 600)
		SET_BIT(object->extra_flags, ITEM_ANTI_NEUTRAL);

	// Opposite alignment flags.
	if (alignment <= -300)
		SET_BIT(object->extra_flags, ITEM_ANTI_GOOD);
	else if (alignment >= 300)
		SET_BIT(object->extra_flags, ITEM_ANTI_EVIL);

	// Set level.
	object->level = URANGE(1, object->level, DEFAULT_MAX_LEVEL);

	// Decode quality index.
	const struct quality_type *quality_struct = quality_lookup_index(quality);
	if (quality_struct == NULL) {
		printf_debug("specific_object: invalid quality (%d).", quality);
		// Skip application of quality modifier.
		goto afterModifier;
	}

	// Apply some extra flags.
	SET_BIT(object->extra_flags, ITEM_NORESTRING);

	//if (quality >= QUALITY_GOOD)
	//	SET_BIT(object->extra_flags, ITEM_NOPURGE);
	//else 
		REMOVE_BIT(object->extra_flags, ITEM_NOPURGE);

	if (quality >= QUALITY_BETTER)
		SET_BIT(object->extra_flags, ITEM_NOSAC);
	else
		REMOVE_BIT(object->extra_flags, ITEM_NOSAC);

	chance = number_percent();

	if ((chance < 35) && (quality == QUALITY_NORMAL))
		SET_BIT(object->extra_flags, ITEM_BURN_PROOF);
	else if ((chance < 45) && (quality == QUALITY_GOOD))
		SET_BIT(object->extra_flags, ITEM_BURN_PROOF);
	else if ((chance < 65) && (quality == QUALITY_BETTER))
		SET_BIT(object->extra_flags, ITEM_BURN_PROOF);
	else if ((chance < 80) && (quality == QUALITY_BEST))
		SET_BIT(object->extra_flags, ITEM_BURN_PROOF);
	else
		REMOVE_BIT(object->extra_flags, ITEM_BURN_PROOF);

	// Add quality word to the keywords, as well as a tag for easy identification.
	sprintf_field(name, object->name, "%s %s %s", quality_struct->name, object->name, (unique ? "unique" : "random"));

	// Apply quality modifier to the item effects.
	// Also, we don't like the word "normal" popping up on items that are normal.
	if (quality != QUALITY_NORMAL) {
		sprintf_field(name, object->short_descr, "%s %s", quality_struct->name, object->short_descr);
		sprintf_field(name, object->description, "%s %s", quality_struct->name, object->description);

		// Adjust price of equipment.
		object->cost = (object->cost) + ((object->cost * quality) / 4); // almost double or 0

		AFFECT_DATA *affect;
		int modifier;
		for (affect = object->affected; affect != NULL; affect = affect->next) {
			if (affect->location == APPLY_MORPH_FORM
			||  affect->where != TO_OBJECT)
				continue;

			modifier = (abs(affect->modifier)*quality)/10;

			// AC/Saves are backwards scales so we need to subtract the modifier.
			if (affect->location == APPLY_AC || affect->location == APPLY_SAVES)
				affect->modifier = affect->modifier-modifier;
			else
				affect->modifier = affect->modifier+modifier;
		}
	}

afterModifier:
	if (!unique) {
		// Format the description, putting a/an or nothing depending on whether the object is plural
		// and whether the first letter is a vowel.
		char *check_plural = strip_spec_char_col(strip_bespaces(object->pIndexData->short_descr));

		if (check_plural[strlen(check_plural)-1] != 's') {
			char *check_vowel = strip_spec_char_col(strip_bespaces(object->short_descr));
			char first = UPPER(check_vowel[0]);
			bool vowel_first = first == 'A' || first == 'E' || first == 'I' || first == 'O' || first == 'U';

			sprintf_field(name, object->description, "A%s %s", vowel_first ? "n" : "", object->description);
			sprintf_field(name, object->short_descr, "a%s %s", vowel_first ? "n" : "", object->short_descr);
		}
		// Colourise and format names.
		char *quality_colour = quality_struct ? quality_struct->colour : "{x";

		sprintf_field(name, object->short_descr, "%s%s{x", quality_colour, object->short_descr);
		sprintf_field(name, object->description, "%s%s lie%s here.{x",
				quality_colour, object->description, check_plural[strlen(check_plural)-1] == 's' ? "" : "s");
	}

	combine_fuzz_effects(object);
	return object;
}

// Random(ish) load capped at pop_level.
OBJ_DATA *random_object(int pop_level, int mob_tier) {
	OBJ_INDEX_LIST *prefixes = NULL, *suffixes = NULL;
	OBJ_INDEX_DATA *obj_index;
	OBJ_DATA *object;
	int chance, pop_min, pop_max;
	int quality;

	// Drop level a bit.
	pop_level -= number_range(0, 40);

	// Always pop at level one at the very least.
	pop_level	= UMAX(1, pop_level);

	// Calculate pop range for unique/object/prefix/suffix loads.
	pop_min		= UMAX(1, pop_level - POP_RANGE);
	pop_max		= pop_level + POP_RANGE/2;

	// Level to unique pop chance:
	// <140 : 2%
	// 141-152 : 3%
	// 153-164 : 4%
	// 165+ : 5%
	chance = UMAX(0, pop_level-140);	// x = 0 or pop_level-140 (whichever higher).
	chance = 2 + UMIN(3, chance/12);	// Min of 2%, max of 5%(2+3) or 2% + 1% per 12 levels over 140.
	if (number_percent() <= chance) {
		// Lets pop a unique!
		char name[MAX_STRING_LENGTH];

		obj_index = level_based_load(VNUM_UNIQUE_LOWER, VNUM_UNIQUE_UPPER, pop_min, pop_max, TRUE);
		object = create_object(obj_index, DEFAULT_MAX_LEVEL);
		sprintf_field(name, object->name, "unique %s", object->name);

		return specific_object(QUALITY_NORMAL, object, NULL, NULL, TRUE, mob_tier);
	}

	// Calculate the quality of the item (ugly, yes, but practical).
	chance = number_range(1,200); //number_percent();
/*	if (chance <= 5)
		quality = QUALITY_WORST;	// 5%
	else if (chance <= 12)
		quality = QUALITY_WORSE;	// 7%
	else if (chance <= 27)
		quality = QUALITY_BAD;		// 15%
	else if (chance <= 85)
		quality = QUALITY_NORMAL;	// 58%
	else if (chance <= 93)
		quality = QUALITY_GOOD;		// 8%
	else if (chance <= 98)
		quality = QUALITY_BETTER;	// 5%
	else
		quality = QUALITY_BEST;		// 2% */

	if (chance <= 10)
		quality = QUALITY_WORST;	// 5%
	else if (chance <= 40)
		quality = QUALITY_WORSE;	// 15%
	else if (chance <= 110)
		quality = QUALITY_BAD;		// 35%
	else if (chance <= 170)
		quality = QUALITY_NORMAL;	// 30%
	else if (chance <= 190)
		quality = QUALITY_GOOD;		// 10%
	else if (chance <= 198)
		quality = QUALITY_BETTER;	// 4%
	else
		quality = QUALITY_BEST;		// 1%

	// Find index data first.
	obj_index = level_based_load(VNUM_RANDOM_LOWER, VNUM_RANDOM_UPPER, pop_min, pop_max, TRUE);

	if (obj_index == NULL) {
		printf_debug("random_object: could not find any objects between %d and %d (level %d).", pop_min, pop_max, pop_level);
		return NULL;
	}

	// Load up the object.
	object = create_object(obj_index, DEFAULT_MAX_LEVEL);
/*
	if (mob_tier > 1) {
		if (quality >= QUALITY_NORMAL)
			levelr = mob_tier;
		else if (quality == QUALITY_BAD)
			levelr = number_range(mob_tier - 10, mob_tier);
		else if (quality == QUALITY_WORSE)
			levelr = number_range(mob_tier - 20, mob_tier - 10);
		else if (quality == QUALITY_WORST)
			levelr = number_range(mob_tier - 30, mob_tier - 20);
	}

	if ((levelr < 0) || (mob_tier <= 0))
		object->tier_level = 0;
	else
		object->tier_level = levelr;
*/

#define MAKE_PS(Index, Vmin, Vmax, List)															\
	case Index:																						\
		obj_index = level_based_load(Vmin, Vmax, 1, pop_max, TRUE);									\
		if (obj_index != NULL && !is_in_obj_index_list(obj_index, List)								\
		&&  (obj_index->item_type == ITEM_TRASH || obj_index->item_type == object->item_type)) {	\
			elem = new_obj_index_list(obj_index);													\
			elem->next = List;																		\
			List = elem;																			\
			/* Not a linear scale - lots of bad prefixes is more expensive than one good prefix. */	\
			pop_max = pop_max - obj_index->level - obj_index->level*ps_count++/6;					\
			break;																					\
		}

	// This should loop round till we've finally filled chosen as many prefixes/suffixes as we can.
	// Case 3 is for repetition - if we fail to find a prefix, we try finding a suffix, then if we
	// can't find a suffix, we fall through to case 4 where we set ps_count to ps_max and end the
	// loop. Vice versa for suffixes for cases 2 and 5.
	OBJ_INDEX_LIST *elem;
	int ps_count = 0, ps_max = number_range(-3, 3) + quality;

	// Drop the min pop level again, so theres more chance of cheaper tags being added.
//	pop_min = UMAX(1, pop_min/2);
	while (ps_count < ps_max && pop_max > 0) {
		// Make sure pop_min is equal or less than pop_max.
//		pop_min = UMIN(pop_min, pop_max);
		// Flip a coin to see what we should add.
		switch (number_bits(1)) {
		MAKE_PS(0, VNUM_PREFIX_LOWER, VNUM_PREFIX_UPPER, prefixes)
		MAKE_PS(2, VNUM_SUFFIX_LOWER, VNUM_SUFFIX_UPPER, suffixes)	// Fall through to here if unsuccessful.
		case 4:
			ps_count = ps_max;	// Exit loop.
			break;
		MAKE_PS(1, VNUM_SUFFIX_LOWER, VNUM_SUFFIX_UPPER, suffixes)
		MAKE_PS(3, VNUM_PREFIX_LOWER, VNUM_PREFIX_UPPER, prefixes)	// Fall through to here if unsuccessful.
		case 5:
			ps_count = ps_max;	// Exit loop.
			break;
		}
	}

	// Set as enchanted.
	object->enchanted = TRUE;

	// specific_object does all the fancy stuff with the object, prefixes and suffixes.
	return specific_object(quality, object, prefixes, suffixes, FALSE, mob_tier);
}
