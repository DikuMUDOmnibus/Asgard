/*
 * achievements.h
 *
 *  Created on: 22 Jun 2011
 *      Author: deryck
 */

#ifndef ACHIEVEMENTS_H_
#define ACHIEVEMENTS_H_

// New/delete functions for ACH_DONE.
ACH_DONE *new_ach_done();
void free_ach_done(ACH_DONE *done);

// Main function that updates achievement counters.
void ach_increment(CHAR_DATA *ch, ACH_METRIC metric, unsigned long amount);

// Updating function.
void ach_update(CHAR_DATA *ch);

// See if an achievement has been unlocked.
bool ach_unlocked(CHAR_DATA *ch, const ACH_DEFINITION *achievement);

// Achievements table.
extern const ACH_DEFINITION achievements_table[];

#endif /* ACHIEVEMENTS_H_ */
