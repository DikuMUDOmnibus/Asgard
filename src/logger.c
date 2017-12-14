/*
 * logger.c
 *
 *  Created on: 17 Jan 2011
 *      Author: Nico
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include "merc.h"
#include "newclan.h"

void _log_string_(char *logname, char *line, FILE *extraStream) {
	char time_name[32], filename[MAX_STRING_LENGTH];
	struct tm* time = localtime(&current_time);
	FILE *fp;

	strftime(time_name, MAX_STRING_LENGTH,
		"%Y_%m_%d", time);
	sprintf(filename, LOG_DIR"%s_%s.log", time_name, logname);

	strftime(time_name, MAX_STRING_LENGTH, "%H:%M:%S %Z", time);

	if (extraStream != NULL)
		fprintf(extraStream, "%s: %s\n", time_name, line);

	if ((fp = fopen(filename, "a")) == NULL) {
		fprintf(stderr, "Unable to open logfile: %s\r\n", filename);
		return;
	}

	fprintf(fp, "%s: %s\n", time_name, line);
	fclose(fp);
}

#define DEFINE_LOG(LogName, ExtraStream, ExtraCode)										\
	void printf_ ## LogName(char *format, ...) {										\
		char line[MAX_STRING_LENGTH];													\
		va_list args;																	\
		va_start(args, format);															\
		vsnprintf(line, MAX_STRING_LENGTH, format, args);								\
		va_end(args);																	\
		_log_string_(#LogName, line, ExtraStream);										\
		ExtraCode																		\
	}

// Log types.
DEFINE_LOG(system, stdout, )
DEFINE_LOG(debug, stderr, wiznet(line, NULL, NULL, WIZ_DEBUG, 0, 0);)

// Special player logs.
void print_player(CHAR_DATA *ch, char *buf) {
	char room[16], name[64], line[MAX_STRING_LENGTH], logname[64];

	// No NPCs unless its a switched character.
	if (ch == NULL || (IS_NPC(ch) && (!ch->desc || !ch->desc->original)))
		return;

	// Get the room index.
	snprintf(room, 16, "[%6d]", ch->in_room ? ch->in_room->vnum : 0);

	// Get the character name/mob vnum, stripping spaces.
	snprintf(name, 64, "%s", strip_spaces(IS_NPC(ch) ? ch->desc->original->name : ch->name));
	sprintf(logname, "plyr_%s", name);

	// Print to log file.
	snprintf(line, MAX_STRING_LENGTH, "%s %s: %s",
		room, name, buf);
	_log_string_(logname, line, stdout);
}

void printf_player(CHAR_DATA *ch, char *format, ...) {
	char line[MAX_STRING_LENGTH-16];

	// Format the arguments given.
	va_list args;
	va_start(args, format);
	vsnprintf(line, MAX_STRING_LENGTH-16, format, args);
	va_end(args);
	print_player(ch, line);
}
