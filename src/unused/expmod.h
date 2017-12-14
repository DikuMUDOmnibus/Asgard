/*
 * expmod.h
 *
 *  Created on: 4 Feb 2011
 *      Author: Nico
 *
 *  Experience modifier prototypes.
 */

#ifndef EXPMOD_H_
#define EXPMOD_H_

// Checks if there is an experience modifier in effect.
bool expmod_is_enabled();

// The experience modifier as a string, i.e. "3/2" or "2".
char *expmod_multiplier();

// The experience modifier level range as a string.
char *expmod_level_range();

// The duration of the experience modifier as a string.
char *expmod_duration();

// Check if the experience modifier applies to the character.
bool expmod_applies(CHAR_DATA *ch);

// Calculate the new experience with modifier.
int expmod_calc(int exp);

// Experience modifier timer update.
void expmod_update();

#endif /* EXPMOD_H_ */
