/*
 * logger.h
 *
 *  Created on: 17 Jan 2011
 *      Author: Nico
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#define DECLARE_LOG(LogName)					\
	void printf_ ## LogName(char *format, ...);

DECLARE_LOG(system)
DECLARE_LOG(debug)
void print_player(CHAR_DATA *ch, char *buf);
void printf_player(CHAR_DATA *ch, char* format, ...);

#endif /* LOGGER_H_ */
