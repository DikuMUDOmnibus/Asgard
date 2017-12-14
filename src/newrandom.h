/*
 * newrandom.h
 *
 *  Created on: 6 Feb 2011
 *      Author: Nico
 */

#ifndef NEWRANDOM_H_
#define NEWRANDOM_H_

// OBJ_INDEX_DATA list (utility).
typedef struct obj_index_data_list OBJ_INDEX_LIST;
struct obj_index_data_list {
	OBJ_INDEX_LIST *next;
	OBJ_INDEX_DATA *object_index;
};

struct quality_type {
	char *name;
	int index;
	char *colour;
};

const struct quality_type *quality_lookup_name(char *name);
const struct quality_type *quality_lookup_index(int index);

OBJ_INDEX_LIST *prefix_lookup(char *name);
OBJ_INDEX_LIST *suffix_lookup(char *name);
OBJ_INDEX_DATA *rbase_object_lookup(char *name);

OBJ_DATA *specific_object(int quality, OBJ_DATA *object, OBJ_INDEX_LIST *prefixes, OBJ_INDEX_LIST *suffixes, bool unique, int mob_tier);
OBJ_DATA *random_object(int pop_level, int mob_tier);

#endif /* NEWRANDOM_H_ */
