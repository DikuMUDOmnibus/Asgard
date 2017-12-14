#include "include.h"

void do_help(CHAR_DATA *ch, char *argument)
{
	HELP_DATA *pHelp, *fHelp;
	char argall[MAX_INPUT_LENGTH], argone[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	int number, count;

	fHelp = NULL;

	if (argument[0] == '\0')
		argument = "summary";
	else if (str_prefix(argument, "race_") && is_racename(argument)) {
		sprintf(buf, "race_%s", argument);
		do_help(ch, buf);
		do_racehelp(ch, argument);
		return;
	}
	number = 0;
	argall[0] = '\0';
	argument = one_argument(argument, argall);

	if (is_number(argall))
	{
		number = atoi(argall);
		argall[0] = '\0';
	}

	/* this parts handles help a b so that it returns help 'a b' */
	while (argument[0] != '\0')
	{
		argument = one_argument(argument, argone);
		if (argall[0] != '\0')
			strcat(argall, " ");
		strcat(argall, argone);
	}

	buf[0] = '\0';
	count = 0;

	strcat(
			buf,
			"{y+------------------------------------------------------------------------------+\n\r");
	strcat(
			buf,
			"{y| {WMultiple help files found.  Choose from the help files listed below with the {y|\n\r");
	strcat(
			buf,
			"{y| {Wfollowing command: help # <original string> or using the full name of the    {y|\n\r");
	strcat(
			buf,
			"{y| {Whelp file (i.e. \"help detect evil\" instead of \"help det\").                   {y|\n\r{x");
	strcat(
			buf,
			"{y+------------------------------------------------------------------------------+\n\r");

	for (pHelp = help_first; pHelp != NULL; pHelp = pHelp->next)
	{
		if (pHelp->level > get_trust(ch))
			continue;

		if (is_name(argall, pHelp->keyword))
		{
			count++;
			fHelp = pHelp;
			if (count == number)
				break;

			sprintf(buf2, "{y| {W%3d) {C%-71s {y|{x\n\r", count, pHelp->keyword);
			strcat(buf, buf2);
		}
	}

	strcat(
			buf,
			"{y+------------------------------------------------------------------------------+{x\n\r");

	if (fHelp == NULL)
		send_to_char("No help on that word.\n\r", ch);
	else if (count > 1 && number == 0)
	{
		page_to_char(buf, ch);
	}
	else
	{
		if (fHelp->level >= 0 && str_cmp(argall, "imotd"))
		{
			sprintf(buf2, "{g[ {w%-76s {g]{x\n\r", fHelp->keyword);
			send_to_char(buf2, ch);
			send_to_char(
					"{g--------------------------------------------------------------------------------{x\n\r",
					ch);
		}

		/* Strip leading '.' to allow initial blanks. */
		if (fHelp->text[0] == '.')
			page_to_char(fHelp->text + 1, ch);
		else
			page_to_char(fHelp->text, ch);
		return;

	}

	return;

}

