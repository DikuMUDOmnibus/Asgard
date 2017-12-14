/*
 * copyover.c
 *
 *  Created on: 9 Feb 2011
 *      Author: Nico
 *  Cleaner copyover/crash code.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#include "merc.h"
#include "recycle.h"
#include "newclan.h"

// Externals.
extern const char compress_will[];
int init_socket( int port );
bool process_output(DESCRIPTOR_DATA *d, bool fPrompt);
bool write_to_descriptor( DESCRIPTOR_DATA *d, char *txt, int length );
bool write_to_descriptor_2(int desc, char *txt, int length);
DECLARE_DO_FUN(do_look);
void shutdown_system(void);

// File to store hotboot data.
#define HOTBOOT_FILE		"hotboot.dat"

int port		= 4444;
bool hotboot	= FALSE;
int control		= 0;

// Counters for last crash time, as well as number of crashes.
static time_t last_crash = 0;
static int crash_count = 0;
static bool in_copyover = FALSE;
// If there are more than 10 crashes in quick succession, exit.
#define CRASH_INTERVAL		2
#define MAX_CRASHES			5

// Resets signal handlers to default system handlers.
static void unlink_signal_handlers() {
	struct sigaction default_action;

	// Default system action
    default_action.sa_handler = SIG_DFL;
    sigemptyset(&default_action.sa_mask);
    default_action.sa_flags = 0;

    sigaction(SIGQUIT, &default_action, NULL );
    sigaction(SIGILL, &default_action, NULL );
    sigaction(SIGFPE, &default_action, NULL );
    sigaction(SIGSEGV, &default_action, NULL );
    sigaction(SIGTERM, &default_action, NULL );
    sigaction(SIGBUS, &default_action, NULL );
    sigaction(SIGABRT, &default_action, NULL );
    sigaction(SIGPIPE, &default_action, NULL );
    sigaction(SIGHUP, &default_action, NULL );
    sigaction(SIGINT, &default_action, NULL );
}

// Perform a hotboot, indicating whether its a result of a crash or not.
void perform_hotboot(int state) {
	FILE *fp;
	bool save_stuff = FALSE;

	// state = 0, NORMAL
	// state = 1, CRASHED
	// state = 2, VALGRIND 

	// Only attempt to save stuff if this isn't the second time.
	if (!in_copyover) {
		save_stuff = TRUE;
		in_copyover = TRUE;
	} else {
		// Unlink signal handlers if this is take 2.
		unlink_signal_handlers();
	}

	if ((state == CRASHED) && (fp = fopen(LAST_COMMAND, "a")) != NULL) {
		in_copyover = TRUE;
		fprintf(fp, "Last Command: %s\n", last_command);
		fclose(fp);
	}

	if ((fp = fopen(HOTBOOT_FILE, "w")) == NULL) {
		printf_system("hotboot: Could not open " HOTBOOT_FILE " to dump game state.");
		perror("hotboot file " HOTBOOT_FILE);
		return;
	}

	// Reset auctions.
	AUCTION_DATA *auc;
	for (auc = auction_list; auc != NULL; auc = auc->next) {
		if (auc->item != NULL)
			obj_to_char(auc->item, auc->owner);

		if (auc->high_bidder != NULL)
			add_cost(auc->high_bidder, auc->bid_amount, auc->bid_type);
	}

	// Close arena.
	arena = FIGHT_LOCK;

	CHAR_DATA *ch;
	char buf[MAX_STRING_LENGTH];

	if (state == CRASHED)
		strcpy(buf, "\r\n{Y***{x " GAME_COL_NAME " {Ris crashing! Attemping recovery! {Y***{x\r\n");
	else
		strcpy(buf, "\r\n{Y***{x " GAME_COL_NAME " {Wis hotbooting {Y***{x\r\n");

	// Save and tell players what is happening, drop crappy sockets.
	for (ch = char_list; ch != NULL; ch = ch->next) {
		// Separate descriptor check from NPC check in case of switched characters.
		if (ch->desc != NULL) {
			ch->desc->wasCompressed = ch->desc->out_compress != NULL;
			if (compressEnd(ch->desc)) {
				send_to_char(buf, ch);
				if (!process_output(ch->desc, FALSE))
					close_socket(ch->desc);
			} else
				close_socket(ch->desc);
		}
		if (save_stuff && !IS_NPC(ch))
			save_char_obj(ch);
	}

	if (save_stuff) {
		// Save clan/roster data.
		save_all_clans();
		save_all_rosters();
	}

	// Save descriptor data.
	DESCRIPTOR_DATA *d, *d_next;
	for (d = descriptor_list; d != NULL; d = d_next) {
		d_next = d->next;
		if ((ch = d->character) != NULL && !IS_NPC(ch)) {
			fprintf(fp, "%d %d %.511s %.511s %.511s\n",
				d->descriptor, d->wasCompressed,
				ch->pload && ch->pload[0] != '\0' ? ch->pload : ch->name,
				d->host, d->hostip);
			d->wasCompressed = 0; // Ensure the flag is reset to 0.
		} else {
			write_to_descriptor(d, "\r\nSorry, we are rebooting, please reconnect in a few seconds.\r\n", 0);
			close_socket(d);
		}
	}

	// End marker.
	fprintf(fp, "-1\n");
	fclose(fp);

	// Do an execute.
	char port_arg[512], fd_arg[512], cc_arg[512] = "", ct_arg[512] = "";
	sprintf(port_arg, "%d", port);
	sprintf(fd_arg, "%d", control);
	if (state == CRASHED) {
		sprintf(cc_arg, "%d", (current_time-last_crash > CRASH_INTERVAL*2 ? 0 : crash_count)+1);
		sprintf(ct_arg, "%ld", current_time);
	}
	if ((state == NORMAL) || (state == CRASHED))
		execl(GAME_EXE, GAME_EXE, port_arg, fd_arg, cc_arg, ct_arg, (char *) NULL);
	else
		execl("./", VALGRIND1, GAME_EXE, port_arg, fd_arg, cc_arg, ct_arg, (char *) NULL);

	// Failed to execute.
	printf_system("hotboot: execl failed.");
	perror("hotboot: execl failed");
	if (state == CRASHED)	// Exit out if it was a crash so things don't break further.
		exit(EXIT_FAILURE);
}

// Prints the usage of program.
void usage() {
	printf_system("Usage: " GAME_EXE " <port>\r\n");
	exit(1);
}

// Check the program arguments to whether we've booted up from a hotboot.
void setup_from_args(int argc, char **argv) {
	switch (argc) {
	default:
		// Try to continue as normal.
	case 5:
		if (is_number(argv[3]))
			crash_count = atoi(argv[3]);
		if (is_number(argv[4]))
			last_crash = atol(argv[4]);
		// Prevent repeating crashes.
		if (current_time - last_crash < CRASH_INTERVAL) {
			if (crash_count >= MAX_CRASHES) {
				printf_system("Game crashed too many times, exiting.\r\n");
				exit(1);
			} else if (crash_count >= (MAX_CRASHES-1)) {
				printf_system("Game crashed many times. Attempting clean startup.\r\n");
				int i;
				for (i = 3; i < 1024; i++)
					close(i);
				goto clean_start;
			}
		}
	case 3:
		if (!is_number(argv[2]) || (control = atoi(argv[2])) < 0)
			printf_system("Invalid recovery FD: (%s). Attempting clean startup.\r\n", argv[2]);
		else {
			printf_system("Recovering from hotboot.\r\n");
			hotboot = TRUE;
		}
	case 2:
clean_start:
		if (!is_number(argv[1]) || (port = atoi(argv[1])) < 1024 || port >= 65536)
			usage();
		break;
	case 0:
		printf_system("argc == 0? Isn't that impossible? Trying to continue anyway.");
	case 1:
		break;	// Do nothing, use default settings.
	}
	if (!hotboot)
		control = init_socket(port);
}

// Load old connected descriptors.
void recover_hotboot_state() {
	if (!hotboot)
		return;

	FILE *fp;

	if ((fp = fopen(HOTBOOT_FILE, "r")) == NULL) {
		printf_system("hotboot: Could not open " HOTBOOT_FILE " to restore game state.");
		perror("hotboot file " HOTBOOT_FILE);
		return;
	}
	// Unlink the file so others can't find it just in case.
	unlink(HOTBOOT_FILE);

	char name[512], host[512], hostip[512];
	DESCRIPTOR_DATA *d;
	int desc, wasCompressed;

	do {
		fscanf(fp, "%d %d %s %s %s\n", &desc, &wasCompressed, name, host, hostip);

		// End marker.
		if (desc < 0)
			break;

		// Check a write is error-free, close if not.
		if (!write_to_descriptor_2(desc, "\n\rRestoring from Hot-Boot...\n\r", 0)) {
			close(desc);
			continue;
		}

		// Create descriptor and populate.
		d = new_descriptor();
		init_descriptor(d, desc);
		d->host = str_dup(host);
		d->hostip = str_dup(hostip);
		d->connected = CON_COPYOVER_RECOVER;
		// Add to descriptor list.
		d->next = descriptor_list;
		descriptor_list = d;

		// Load up the character.
		if (!load_char_obj(d, name)) {
			// No character found.
			write_to_descriptor(
				d, "\n\rSomehow, your character was lost in the copyover.Sorry.\n\r", 0);
			close_socket(d);
			continue;
		}

		// Insert character into character list.
		d->character->next = char_list;
		char_list = d->character;
		d->character->pcdata->socket = d->host;

		// Make sure the room the character is in exists before putting them in it.
		if (!d->character->in_room)
			d->character->in_room = get_room_index(ROOM_VNUM_TEMPLE);
		char_to_room(d->character, d->character->in_room);

		// We're now playing.
		d->connected = CON_PLAYING;

		// Don't forget about pets.
		if (d->character->pet != NULL)
			char_to_room(d->character->pet, d->character->in_room);

		write_to_descriptor(d, "\n\rHot-Boot recovery complete.\n\r", 0);

		do_look(d->character, "auto");
		act("$n steps out of limbo!", d->character, NULL, NULL, TO_ROOM);

		// Try to enable compression again.
//		write_to_buffer(d, compress_will, 0);
		if (wasCompressed)
			compressStart(d);
	} while (1);

	fclose(fp);
}

// Do a copyover.
void do_copyover(CHAR_DATA *ch, char *argument) {
	send_to_char("Performing copyover.\r\n", ch);

	// Check the game exe exists.
	FILE *fp;
	if ((fp = fopen(GAME_EXE, "r")) == NULL) {
		send_to_char("Game executable does not exist! Aborting.\r\n", ch);
		return;
	}
	fclose(fp);

	perform_hotboot(NORMAL);
	send_to_char("Copyover {RFAILED{x.\r\n", ch);
}

void crash_handler(int signal) {
	printf_system("*** CRASH *** Signal: %d, Last command: %s.", signal, last_command);

	wait(NULL);

	pid_t forkpid;

	// Fork into child process and wait for it to die before hotbooting.
	if ((forkpid = fork()) > 0) {
		waitpid(forkpid, NULL, WNOHANG | WUNTRACED );
		perform_hotboot(CRASHED);
		exit(0);
	} else if (forkpid < 0)
		printf_system("crash_handler: could not fork.");

	// Closes all of its file descriptors.
	int i;
	for (i = 1024; i >= 0; i--)
		close(i);

	// And calls the default crash handler.
	struct sigaction default_action;
	default_action.sa_handler = SIG_DFL;
	sigaction(signal, &default_action, NULL );
	raise(signal);
	exit(EXIT_FAILURE);	// Exit just in case.
}

void shutdown_handler(int signal) {
	printf_system("*** Received signal %d - terminating gracefully.", signal);

	// Save players.
	CHAR_DATA *ch;
	for (ch = char_list; ch != NULL; ch = ch->next)
		if (!IS_NPC(ch))
			save_char_obj(ch);

	// Save clan/roster data.
	save_all_clans();
	save_all_rosters();

	// Shutdown. Note: this will still cause the mud to be resurrected by the
	// startup script.
	shutdown_system();
}

// Set up signal handling.
void setup_signal_handlers() {
	struct sigaction crash_action, ignore_action, shutdown_action;

	// Set up the crash handler.
	crash_action.sa_handler = crash_handler;
	sigemptyset(&crash_action.sa_mask);
	crash_action.sa_flags = SA_NOMASK;
	sigaction(SIGQUIT, &crash_action, NULL );
	sigaction(SIGILL, &crash_action, NULL );
	sigaction(SIGFPE, &crash_action, NULL );
	sigaction(SIGSEGV, &crash_action, NULL );
	sigaction(SIGBUS, &crash_action, NULL );
	sigaction(SIGABRT, &crash_action, NULL );

	// Set up the ignore handler.
	ignore_action.sa_handler = SIG_IGN;
	sigemptyset(&ignore_action.sa_mask);
	ignore_action.sa_flags = 0;
	sigaction(SIGPIPE, &ignore_action, NULL );
	sigaction(SIGHUP, &ignore_action, NULL );
	sigaction(SIGINT, &ignore_action, NULL );

	// Graceful shutdown on SIGTERM.
	shutdown_action.sa_handler = shutdown_handler;
	sigemptyset(&crash_action.sa_mask);
	crash_action.sa_flags = SA_NOMASK;
	sigaction(SIGTERM, &shutdown_action, NULL );
}

void do_copyove(CHAR_DATA *ch, char *argument) {
	if (IS_IMMORTAL(ch))
		send_to_char("To \"copyover\", type the whole thing.\r\n", ch);
}
