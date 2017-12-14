/*
 * pathfind.h
 *
 *  Created on: 28 Dec 2010
 *      Author: Nico
 */

// Exit codes for the pathfinding algorithm.
#define PF_TOO_LONG		-3
#define PF_NO_PATH		-2
#define PF_FAIL			-1
#define PF_FOUND		1
#define PF_HERE			2

// Maximum depth to search - adjust if necessary.
#define PF_MAX_DEPTH	8196

// Define termination conditions with these.
typedef bool PF_END_COND( ROOM_INDEX_DATA *room );
#define PF_END_COND_DEF( fun )		bool fun ( ROOM_INDEX_DATA *room )

// Define filter conditions with these.
typedef bool PF_FILT_COND( EXIT_DATA *exit );
#define PF_FILT_COND_DEF( fun )		bool fun ( EXIT_DATA *exit )

int bfs_pathfind(
	ROOM_INDEX_DATA *start,		// Room to start the search from.
	PF_END_COND *end_func,		// Function pointer to define the termination condition (TRUE to terminate).
	PF_FILT_COND *filter_func,	// Function pointer to define whether to exclude a child (TRUE to exclude).
	int max_depth,				// Maximum depth to search.
	bool compact				// Whether or not to compact the path (e.g. eeee -> 4e).
);

// Returns the path found, undefined if the last call to bfs_pathfind < 0.
char *get_path();

// Termination condition function getters.
PF_FILT_COND * pf_filt_normal(CHAR_DATA *ch);
PF_END_COND * pf_end_at_room(ROOM_INDEX_DATA *target);
PF_END_COND * pf_end_at_area(AREA_DATA *target);
PF_END_COND * pf_end_at_corpse(char *target);
PF_END_COND * pf_end_at_mobact(long act_flag);
