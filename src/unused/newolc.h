/*
 * newolc.h
 *
 *  Created on: 27 May 2011
 *      Author: Nico
 */

#ifndef NEWOLC_H_
#define NEWOLC_H_

////////////////////////////// Type definitions //////////////////////////////
typedef size_t	offset_t;
typedef const struct olc_schema * OLC_SCHEMA;

/////////////////////////////////// Macros ///////////////////////////////////
typedef void (*OLC_GET_FUNC)(CHAR_DATA *, OLC_SCHEMA, void *, char *);
#define OLC_GET_FUNC_DEF(Name)	void Name(CHAR_DATA *ch, OLC_SCHEMA schema, void *field, char *buf)
typedef bool (*OLC_SET_FUNC)(CHAR_DATA *, OLC_SCHEMA, void *, void *);
#define OLC_SET_FUNC_DEF(Name)	bool Name(CHAR_DATA *ch, OLC_SCHEMA schema, void *field, void *value)

#define OLC_SCHEMA_NAME(Type)		olc_## Type ##_schema
#define OLC_SCHEMA_START(Type)								\
	static const Type template;								\
	const struct olc_schema OLC_SCHEMA_NAME(Type) [] = {
#define OLC_SCHEMA_END	{ NULL, 0, NULL, NULL, 0, NULL } };

#define OLC_FIELD_OFFSET(Field)								\
	(offset_t)(((void *)&template.Field)-((void *)&template))

#define OLC_FIELD(Name, Offset, GetFunc, SetFunc, Type, Table)			\
	{	Name,															\
		Offset,															\
		GetFunc,														\
		SetFunc,														\
		Type,															\
		Table,															\
	},
#define OLC_EDIT_FIELD(Field, GetFunc, SetFunc, Type, Table)			\
	OLC_FIELD(#Field, OLC_FIELD_OFFSET(Field), GetFunc, SetFunc, Type, Table)

// Convenience macros.
#define OLC_INT_FIELD(Field)					\
	OLC_EDIT_FIELD(Field, olc_generic_int_get, olc_generic_int_set, TYPE_INT, NULL)
#define OLC_RO_INT_FIELD(Field)					\
	OLC_EDIT_FIELD(Field, olc_generic_int_get, NULL, TYPE_INT, NULL)

#define OLC_STRING_FIELD(Field)					\
	OLC_EDIT_FIELD(Field, olc_generic_string_get, olc_generic_string_set, TYPE_STRING, NULL)
#define OLC_RO_STRING_FIELD(Field)				\
	OLC_EDIT_FIELD(Field, olc_generic_string_get, NULL, TYPE_STRING, NULL)

#define OLC_ESTRING_FIELD(Field)				\
	OLC_EDIT_FIELD(Field, olc_generic_estring_get, olc_generic_string_set, TYPE_ESTRING, NULL)
#define OLC_RO_ESTRING_FIELD(Field)				\
	OLC_EDIT_FIELD(Field, olc_generic_estring_get, NULL, TYPE_ESTRING, NULL)

#define OLC_FLAG_FIELD(Field, Table)			\
	OLC_EDIT_FIELD(Field, olc_generic_flag_get, olc_generic_flag_set, TYPE_STRING, Table)
#define OLC_RO_FLAG_FIELD(Field, Table)			\
	OLC_EDIT_FIELD(Field, olc_generic_flag_get, NULL, TYPE_STRING, Table)

#define OLC_SYM_FIELD(Name, GetFunc, SetFunc)	\
	OLC_FIELD(#Name, 0, GetFunc, SetFunc, TYPE_STRING, NULL)

////////////////////////////// Structure Types //////////////////////////////
typedef enum olc_parse_type {
	TYPE_INT,
	TYPE_STRING,
	TYPE_ESTRING,		// Editor string - room descriptions and the like.
	TYPE_BOOL,
} OLC_PARSE_TYPE;

struct olc_schema {
	char *					name;			// Name of the field to display.
	offset_t				offset;			// Offset to the field within the struct.
	OLC_GET_FUNC			get_func;		// Getter func - gets the formatted value of the field.
	OLC_SET_FUNC			set_func;		// Setter func - sets the field within an instance.
	OLC_PARSE_TYPE			type;			// What type the input should be parsed into.
	const struct flag_type*	flag_table;		// Flag table - for flag-based fields.
};

typedef struct olc_edit_state {
	OLC_SCHEMA	schema;
	void *		editing;
} OLC_EDIT_STATE;

//////////////////////////// Function Prototypes ////////////////////////////
OLC_GET_FUNC_DEF(olc_generic_int_get);
OLC_SET_FUNC_DEF(olc_generic_int_set);
OLC_GET_FUNC_DEF(olc_generic_estring_get);
OLC_GET_FUNC_DEF(olc_generic_string_get);
OLC_SET_FUNC_DEF(olc_generic_string_set);
OLC_GET_FUNC_DEF(olc_generic_flag_get);
OLC_SET_FUNC_DEF(olc_generic_flag_set);
OLC_GET_FUNC_DEF(olc_area_get);

//////////////////////////// Schema Delcarations ////////////////////////////
extern const struct olc_schema OLC_SCHEMA_NAME(AREA_DATA)[];
extern const struct olc_schema OLC_SCHEMA_NAME(ROOM_INDEX_DATA)[];
extern const struct olc_schema OLC_SCHEMA_NAME(OBJ_INDEX_DATA)[];

#endif /* NEWOLC_H_ */
