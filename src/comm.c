/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku vMud improvments copyright (C) 1992, 1993 by Michael         *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Thanks to abaddon for proof-reading our comm.c and pointing out bugs.  *
 *  Any remaining bugs are, of course, our work, not his.  :)              *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/
/***************************************************************************
 *	ROM 2.4 is copyright 1993-1995 Russ Taylor			   *
 *	ROM has been brought to you by the ROM consortium		   *
 *	    Russ Taylor (rtaylor@pacinfo.com)				   *
 *	    Gabrielle Taylor (gtaylor@pacinfo.com)			   *
 *	    Brian Moore (rom@rom.efn.org)				   *
 *	By using this code, you have agreed to follow the terms of the	   *
 *	ROM license, in the file Rom24/doc/rom.license			   *
 ***************************************************************************/
/*************************************************************************** 
 *       ROT 1.4 is copyright 1996-1997 by Russ Walsh                       *
 *       By using this code, you have agreed to follow the terms of the     *
 *       ROT license, in the file doc/rot.license                           *
 ***************************************************************************/
/*
 * This file contains all of the OS-dependent stuff:
 *   startup, signals, BSD sockets for tcp/ip, i/o, timing.
 *
 * The data flow for input is:
 *    Game_loop ---> Read_from_descriptor ---> Read
 *    Game_loop ---> Read_from_buffer
 *
 * The data flow for output is:
 *    Game_loop ---> Process_Output ---> Write_to_descriptor -> Write
 *
 * The OS-dependent functions are Read_from_descriptor and Write_to_descriptor.
 * -- Furey  26 Jan 1993
 */
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h> /* printf_to_char */
#include <unistd.h> /* OLC -- for close read write etc */
#include "merc.h"
#include "recycle.h"
#include "tables.h"
#include "olc.h"
#include "newclan.h"
#include "copyover.h"
#include "achievements.h"
#include <sys/wait.h>
/* command procedures needed */
DECLARE_DO_FUN(do_showclass );
DECLARE_DO_FUN( do_asave );
DECLARE_DO_FUN(do_help );
DECLARE_DO_FUN(do_look );
DECLARE_DO_FUN(do_skills );
DECLARE_DO_FUN(do_outfit );
DECLARE_DO_FUN(do_unread );
DECLARE_DO_FUN(do_save );
DECLARE_DO_FUN(do_conceal);
long focus_left(long total);

char * get_descr_form(CHAR_DATA *ch,CHAR_DATA *looker, bool get_long);
void check_focus(CHAR_DATA *ch);
//void update_power(CHAR_DATA *barbarian);

void gcast_char(CHAR_DATA *ch, CHAR_DATA *target, int level);

#define ED_NONE 0

extern bool clantoggle;

#undef MALLOC_DEBUG
/*
 * Malloc debugging stuff.
 */
#if defined(sun)
#undef MALLOC_DEBUG
#endif

#if defined(MALLOC_DEBUG)
#include <malloc.h>
extern int malloc_debug ( int );
extern int malloc_verify ( void );
#endif

/*
 * Signal handling.
 * Apollo has a problem with __attribute(atomic) in signal.h,
 *   I dance around it.
 */
#if defined(apollo)
#define __attribute(x)
#endif

#if defined(unix)
#include <signal.h>
#endif

#if defined(apollo)
#undef __attribute
#endif

/* Signal Handling */
void init_signals(void);
void sig_handler(int sig);

/*
 * Socket and TCP/IP stuff.
 */
#if	defined(macintosh) || defined(MSDOS)
const char echo_off_str [] =
{	'\0'};
const char echo_on_str [] =
{	'\0'};
const char go_ahead_str [] =
{	'\0'};
const char compress_will [] =
{	'\0'};
const char compress_do [] =
{	'\0'};
const char compress_dont [] =
{	'\0'};
const char compress_start [] =
{	'\0'};

#endif

#if	defined(unix)
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "telnet.h"
const char echo_off_str[] =
{ IAC, WILL, TELOPT_ECHO, '\0' };
const char echo_on_str[] =
{ IAC, WONT, TELOPT_ECHO, '\0' };
const char go_ahead_str[] =
{ IAC, GA, '\0' };

/* mccp : compression negotiation strings */
const char compress_will[] =
{ IAC, WILL, TELOPT_COMPRESS, '\0' };
const char compress_do[] =
{ IAC, DO, TELOPT_COMPRESS, '\0' };
const char compress_dont[] =
{ IAC, DONT, TELOPT_COMPRESS, '\0' };

#endif

/*
 * OS-dependent declarations.
 */
#if	defined(_AIX)
#include <sys/select.h>
int accept ( int s, struct sockaddr *addr, int *addrlen );
int bind ( int s, struct sockaddr *name, int namelen );
void bzero ( char *b, int length );
int getpeername ( int s, struct sockaddr *name, int *namelen );
int getsockname ( int s, struct sockaddr *name, int *namelen );
int gettimeofday ( struct timeval *tp, struct timezone *tzp );
int listen ( int s, int backlog );
int setsockopt args( ( int s, int level, int optname, void *optval,
				int optlen ) );
int socket ( int domain, int type, int protocol );
#endif

#if	defined(apollo)
#include <unistd.h>
void bzero ( char *b, int length );
#endif

#if	defined(__hpux)
int accept ( int s, void *addr, int *addrlen );
int bind ( int s, const void *addr, int addrlen );
void bzero ( char *b, int length );
int getpeername ( int s, void *addr, int *addrlen );
int getsockname ( int s, void *name, int *addrlen );
int gettimeofday ( struct timeval *tp, struct timezone *tzp );
int listen ( int s, int backlog );
int setsockopt args( ( int s, int level, int optname,
				const void *optval, int optlen ) );
int socket ( int domain, int type, int protocol );
#endif

#if	defined(interactive)
#include <net/errno.h>
#include <sys/fnctl.h>
#endif

#if	defined(macintosh)
#include <console.h>
#include <fcntl.h>
#include <unix.h>
struct timeval
{
	time_t tv_sec;
	time_t tv_usec;
};
#if	!defined(isascii)
#define	isascii(c)		( (c) < 0200 )
#endif
static long theKeys [4];

int gettimeofday ( struct timeval *tp, void *tzp );
#endif

#if	defined(MIPS_OS)
extern int errno;
#endif

#if	defined(MSDOS)
int gettimeofday ( struct timeval *tp, void *tzp );
int kbhit ( void );
#endif

#if	defined(NeXT)
int close ( int fd );
int fcntl ( int fd, int cmd, int arg );
#if	!defined(htons)
u_short htons ( u_short hostshort );
#endif
#if	!defined(ntohl)
u_long ntohl ( u_long hostlong );
#endif
int read ( int fd, char *buf, int nbyte );
int select args( ( int width, fd_set *readfds, fd_set *writefds,
				fd_set *exceptfds, struct timeval *timeout ) );
int write ( int fd, char *buf, int nbyte );
#endif

#if	defined(sequent)
int accept ( int s, struct sockaddr *addr, int *addrlen );
int bind ( int s, struct sockaddr *name, int namelen );
int close ( int fd );
int fcntl ( int fd, int cmd, int arg );
int getpeername ( int s, struct sockaddr *name, int *namelen );
int getsockname ( int s, struct sockaddr *name, int *namelen );
int gettimeofday ( struct timeval *tp, struct timezone *tzp );
#if	!defined(htons)
u_short htons ( u_short hostshort );
#endif
int listen ( int s, int backlog );
#if	!defined(ntohl)
u_long ntohl ( u_long hostlong );
#endif
int read ( int fd, char *buf, int nbyte );
int select args( ( int width, fd_set *readfds, fd_set *writefds,
				fd_set *exceptfds, struct timeval *timeout ) );
int setsockopt args( ( int s, int level, int optname, caddr_t optval,
				int optlen ) );
int socket ( int domain, int type, int protocol );
int write ( int fd, char *buf, int nbyte );
#endif

/* This includes Solaris Sys V as well */
#if defined(sun)
int accept ( int s, struct sockaddr *addr, int *addrlen );
int bind ( int s, struct sockaddr *name, int namelen );
void bzero ( char *b, int length );
int close ( int fd );
int getpeername ( int s, struct sockaddr *name, int *namelen );
int getsockname ( int s, struct sockaddr *name, int *namelen );
int gettimeofday ( struct timeval *tp, struct timezone *tzp );
int listen ( int s, int backlog );
int read ( int fd, char *buf, int nbyte );
int select args( ( int width, fd_set *readfds, fd_set *writefds,
				fd_set *exceptfds, struct timeval *timeout ) );
#if defined(SYSV)
int setsockopt args( ( int s, int level, int optname,
				const char *optval, int optlen ) );
#else
int setsockopt args( ( int s, int level, int optname, void *optval,
				int optlen ) );
#endif
int socket ( int domain, int type, int protocol );
int write ( int fd, char *buf, int nbyte );
#endif

#if defined(ultrix)
int accept ( int s, struct sockaddr *addr, int *addrlen );
int bind ( int s, struct sockaddr *name, int namelen );
void bzero ( char *b, int length );
int close ( int fd );
int getpeername ( int s, struct sockaddr *name, int *namelen );
int getsockname ( int s, struct sockaddr *name, int *namelen );
int gettimeofday ( struct timeval *tp, struct timezone *tzp );
int listen ( int s, int backlog );
int read ( int fd, char *buf, int nbyte );
int select args( ( int width, fd_set *readfds, fd_set *writefds,
				fd_set *exceptfds, struct timeval *timeout ) );
int setsockopt args( ( int s, int level, int optname, void *optval,
				int optlen ) );
int socket ( int domain, int type, int protocol );
int write ( int fd, char *buf, int nbyte );
#endif

/*
 * Global variables.
 */
DESCRIPTOR_DATA * descriptor_list; /* All open descriptors		*/
DESCRIPTOR_DATA * d_next; /* Next descriptor in loop	*/
bool god; /* All new chars are gods!	*/
bool merc_down; /* Shutdown			*/
bool wizlock; /* Game is wizlocked		*/
bool newlock; /* Game is newlocked		*/
char str_boot_time[MAX_INPUT_LENGTH];
time_t current_time; /* time of this pulse */
time_t startup_time;
char clcode[MAX_INPUT_LENGTH];
bool MOBtrigger = TRUE; /* act() switch */
int arena;
volatile sig_atomic_t crashed = 0;
AUCTION_DATA *auction_list;
sh_int auction_ticket;

/*
 * OS-dependent local functions.
 */
#if defined(macintosh) || defined(MSDOS)
void game_loop_mac_msdos ( void );
bool read_from_descriptor ( DESCRIPTOR_DATA *d );
int write_to_descriptor ( int desc, char *txt, int length );
#endif

#if defined(unix)
void game_loop_unix( int control );
int init_socket( int port );
void cnew_descriptor( int control );
//void    init_descriptor_www     ( int wwwcontrol );
void cnew_descriptor_www( int wwwcontrol );
bool read_from_descriptor( DESCRIPTOR_DATA *d );
int write_to_descriptor( DESCRIPTOR_DATA *d, char *txt, int length );
int write_to_descriptor_2( int desc, char *txt, int length );
#endif

/*
 * Other local functions (OS-independent).
 */
bool check_parse_name( char *name );
bool check_reconnect( DESCRIPTOR_DATA *d, char *name, bool fConn );
bool check_playing( DESCRIPTOR_DATA *d, char *name );
int main( int argc, char **argv );
void nanny( DESCRIPTOR_DATA *d, char *argument );
bool process_output( DESCRIPTOR_DATA *d, bool fPrompt );
void read_from_buffer( DESCRIPTOR_DATA *d );
void stop_idling( CHAR_DATA *ch );
void bust_a_prompt( CHAR_DATA *ch );
void clear_stance_affects(CHAR_DATA *ch);

DECLARE_DO_FUN(do_announce );

int main(int argc, char **argv)
{
	struct timeval now_time;

	/*
	 * Memory debugging if needed.
	 */
#if defined(MALLOC_DEBUG)
	malloc_debug( 2 );
#endif

	// Set up signal/crash handlers.
	setup_signal_handlers();

	/*
	 * Init time.
	 */
	gettimeofday(&now_time, NULL );
	current_time = (time_t) now_time.tv_sec;
	strcpy(str_boot_time, ctime(&current_time));
	startup_time = current_time;

	// Initialise game.
	setup_from_args(argc, argv);
	boot_db();
	recover_hotboot_state();

	arena = FIGHT_OPEN;
	printf_system(GAME_NAME" is ready to rock on port %d (fd %d).", port, control);
	game_loop_unix(control);
	close(control);

	/*
	 * That's all, folks.
	 */
	printf_system("Normal termination.");
	exit(0);
	return 0;
}

#if defined(unix)
int init_socket(int port)
{
	static struct sockaddr_in sa_zero;
	struct sockaddr_in sa;
	int x = 1;
	int fd;

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Init_socket: socket");
		exit(1);
	}

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &x, sizeof(x)) < 0)
	{
		perror("Init_socket: SO_REUSEADDR");
		close(fd);
		exit(1);
	}

#if defined(SO_DONTLINGER) && !defined(SYSV)
	{
		struct linger ld;

		ld.l_onoff = 1;
		ld.l_linger = 1000;

		if ( setsockopt( fd, SOL_SOCKET, SO_DONTLINGER,
						(char *) &ld, sizeof(ld) ) < 0 )
		{
			perror( "Init_socket: SO_DONTLINGER" );
			close(fd);
			exit( 1 );
		}
	}
#endif

	sa = sa_zero;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	/*sa.sin_addr.s_addr = inet_addr("70.118.16.122");*/
	/*sa.sin_addr.s_addr = inet_addr( "distorted-illusions.no-ip.org" );*/

	if (bind(fd, (struct sockaddr *) &sa, sizeof(sa)) < 0)
	{
		perror("Init socket: bind");
		close(fd);
		exit(1);
	}

	if (listen(fd, 3) < 0)
	{
		perror("Init socket: listen");
		close(fd);
		exit(1);
	}

	return fd;
}
#endif

#if defined(unix)
void game_loop_unix(int control)
{
	static struct timeval null_time;
	struct timeval last_time;

	gettimeofday(&last_time, NULL );
	current_time = (time_t) last_time.tv_sec;

	signal(SIGPIPE, SIG_IGN );
	gettimeofday(&last_time, NULL );
	current_time = (time_t) last_time.tv_sec;

	/* Main loop */
	while (!merc_down)
	{
		fd_set in_set;
		fd_set out_set;
		fd_set exc_set;
		DESCRIPTOR_DATA *d;
		int maxdesc;

#if defined(MALLOC_DEBUG)
		if ( malloc_verify( ) != 1 )
		abort( );
#endif

		/*
		 * Poll all active descriptors.
		 */
		FD_ZERO( &in_set );
		FD_ZERO( &out_set );
		FD_ZERO( &exc_set );
		FD_SET( control, &in_set );
		maxdesc = control;
		for (d = descriptor_list; d; d = d->next)
		{
			maxdesc = UMAX( maxdesc, d->descriptor );
			FD_SET( d->descriptor, &in_set );
			FD_SET( d->descriptor, &out_set );
			FD_SET( d->descriptor, &exc_set );
		}

		if (select(maxdesc + 1, &in_set, &out_set, &exc_set, &null_time) < 0)
		{
			perror("Game_loop: select: poll");
			exit(1);
		}

		/*
		 * New connection?
		 */
		if (FD_ISSET( control, &in_set ))
			cnew_descriptor(control);

		/*	FD_ZERO( &in_set  );
		 FD_ZERO( &out_set );
		 FD_ZERO( &exc_set );
		 FD_SET( wwwcontrol, &in_set );
		 maxdesc	= wwwcontrol;
		 for ( d = descriptor_list; d; d = d->next )
		 {
		 maxdesc = UMAX( maxdesc, d->descriptor );
		 FD_SET( d->descriptor, &in_set  );
		 FD_SET( d->descriptor, &out_set );
		 FD_SET( d->descriptor, &exc_set );
		 }

		 if ( select( maxdesc+1, &in_set, &out_set, &exc_set, &null_time ) < 0 )
		 {
		 perror( "Game_loop: select: poll" );
		 exit( 1 );
		 }

		 if ( FD_ISSET( wwwcontrol, &in_set ) )
		 cnew_descriptor_www( wwwcontrol );
		 */
		/*
		 * Kick out the freaky folks.
		 */
		for (d = descriptor_list; d != NULL; d = d_next)
		{
			d_next = d->next;
			if (FD_ISSET( d->descriptor, &exc_set ))
			{
				continue; // TODO: delete
				FD_CLR( d->descriptor, &in_set );
				FD_CLR( d->descriptor, &out_set );
				if (d->character && d->character->level > 1)
					save_char_obj(d->character);
				d->outtop = 0;
				close_socket(d);
			}
		}

		/*
		 * Process input.
		 */
		for (d = descriptor_list; d != NULL; d = d_next)
		{
			d_next = d->next;
			d->fcommand = FALSE;

			if (FD_ISSET( d->descriptor, &in_set ))
			{
				if (!read_from_descriptor(d))
				{
					FD_CLR( d->descriptor, &out_set );
					if (d->character != NULL && d->character->level > 1)
						save_char_obj(d->character);
					d->outtop = 0;
					close_socket(d);
					continue;
				}
				if (d->character != NULL)
					d->character->timer = 0;
			}

			if ((d->character != NULL) && (d->character->daze > 0))
				--d->character->daze;

			if (d->character != NULL && d->character->wait > 0)
			{
				--d->character->wait;
				continue;
			}

			read_from_buffer(d);
			if (d->incomm[0] != '\0')
			{
				d->fcommand = TRUE;
				stop_idling(d->character);

				/* OLC */
				if (d->showstr_point)
					show_string(d, d->incomm);
				else if (d->pString)
					string_add(d->character, d->incomm);
				else
					switch (d->connected)
					{
					case CON_PLAYING:
						if (!run_olc_editor(d))
							substitute_alias(d, d->incomm);
						break;
					default:
						nanny(d, d->incomm);
						break;
					}

				d->incomm[0] = '\0';
			}
		}

		/*
		 * Autonomous game motion.
		 */
		update_handler(FALSE);

		/*
		 * Output.
		 */
		for (d = descriptor_list; d != NULL; d = d_next)
		{
			d_next = d->next;

			if ((d->fcommand || d->outtop > 0 || d->out_compress)
					&& FD_ISSET(d->descriptor, &out_set))
			{
				bool ok = TRUE;

				if (d->fcommand || d->outtop > 0)
					ok = process_output(d, TRUE );

				if (ok && d->out_compress)
					ok = processCompressed(d);

				if (!ok)
				{
					if (d->character != NULL && d->character->level > 1)
						save_char_obj(d->character);
					d->outtop = 0;
					close_socket(d);
				}
			}
		}

		/*
		 * Synchronize to a clock.
		 * Sleep( last_time + 1/PULSE_PER_SECOND - now ).
		 * Careful here of signed versus unsigned arithmetic.
		 */
		{
			struct timeval now_time;
			long secDelta;
			long usecDelta;

			gettimeofday(&now_time, NULL );
			usecDelta = ((int) last_time.tv_usec) - ((int) now_time.tv_usec)
					+ 1000000 / PULSE_PER_SECOND;
			secDelta = ((int) last_time.tv_sec) - ((int) now_time.tv_sec);
			while (usecDelta < 0)
			{
				usecDelta += 1000000;
				secDelta -= 1;
			}

			while (usecDelta >= 1000000)
			{
				usecDelta -= 1000000;
				secDelta += 1;
			}

			if (secDelta > 0 || (secDelta == 0 && usecDelta > 0))
			{
				struct timeval stall_time;

				stall_time.tv_usec = usecDelta;
				stall_time.tv_sec = secDelta;
				if (select(0, NULL, NULL, NULL, &stall_time) < 0)
				{
					perror("Game_loop: select: stall");
					exit(1);
				}
			}
		}

		gettimeofday(&last_time, NULL );
		current_time = (time_t) last_time.tv_sec;
	}

	return;
}
#endif

#if defined(unix)
void cnew_descriptor(int control)
{
	char buf[MAX_STRING_LENGTH];
	DESCRIPTOR_DATA *dnew;
	struct sockaddr_in sock;
	struct hostent *from;
	int desc;
	size_t size;

	size = sizeof(sock);

	if (getsockname(control, (struct sockaddr *) &sock, &size) < 0)
	{
		printf_system("cnew_descriptor (%d): getsockname.", control);
		perror("NEW_descriptor: getsockname");
		return;
	}
	if ((desc = accept(control, (struct sockaddr *) &sock, &size)) < 0)
	{
		printf_system("cnew_descriptor (%d): accept (socket operation on non-socket).", control);
		return;
	}

#if !defined(FNDELAY)
#define FNDELAY O_NDELAY
#endif

	if (fcntl(desc, F_SETFL, FNDELAY ) == -1)
	{
		printf_system("cnew_descriptor (%d): FNDELAY.", control);
		perror("New_descriptor: fcntl: FNDELAY");
		return;
	}

	/*
	 * Cons a new descriptor.
	 */

	dnew = new_descriptor();

	dnew->descriptor = desc;
	dnew->connected = CON_GET_NAME;
	dnew->showstr_head = NULL;
	dnew->showstr_point = NULL;
	dnew->outsize = 2000;
	dnew->pEdit = NULL; /* OLC */
	dnew->pString = NULL; /* OLC */
	dnew->editor = 0; /* OLC */
	dnew->outbuf = alloc_mem(dnew->outsize);

	size = sizeof(sock);
	if (getpeername(desc, (struct sockaddr *) &sock, &size) < 0)
	{
		printf_system("cnew_descriptor (%d): getpeername.", control);
		perror("New_descriptor: getpeername");
		dnew->host = str_dup("(unknown)");
	}
	else
	{
		/*
		 * Would be nice to use inet_ntoa here but it takes a struct arg,
		 * which ain't very compatible between gcc and system libraries.
		 */
		int addr;

		addr = ntohl(sock.sin_addr.s_addr);
		sprintf(buf, "%d.%d.%d.%d", (addr >> 24) & 0xFF, (addr >> 16) & 0xFF,
				(addr >> 8) & 0xFF, (addr) & 0xFF);

		printf_system("New connection from %s.", buf);
		from = gethostbyaddr((char *) &sock.sin_addr, sizeof(sock.sin_addr),
				AF_INET );
		dnew->host = str_dup(from ? from->h_name : buf);
		/* changed to inet_ntoa when gethostbyaddr started acting
		 * really flaky. - Puck  I have to see this myself. .. --- Jodie
		 */
		dnew->hostip = str_dup(inet_ntoa(sock.sin_addr));
		printf_system("Address resolved as %s.", dnew->host);
	}

	/*
	 * Swiftest: I added the following to ban sites.  I don't
	 * endorse banning of sites, but Copper has few descriptors now
	 * and some people from certain sites keep abusing access by
	 * using automated 'autodialers' and leaving connections hanging.
	 *
	 * Furey: added suffix check by request of Nickel of HiddenWorlds.
	 */
	if (check_ban(dnew->host, BAN_ALL) || check_ban(dnew->hostip, BAN_ALL))
	{
		printf_system("Unknown@%s: disconnected due to BAN.", dnew->host);
		write_to_descriptor_2(desc,
				"Your site has been banned from this mud.\n\r", 0);
		close(desc);
		free_descriptor(dnew);
		return;
	}
	/*
	 * Init descriptor data.
	 */
	dnew->next = descriptor_list;
	descriptor_list = dnew;

	/*  {             * Give a char colour before they create! *
	 CHAR_DATA dummy;

	 if (!d->character)
	 {
	 memset(&dummy, 0, sizeof(dummy));
	 d->character = &dummy;
	 dummy->desc = d;
	 SET_BIT(dummy->act, PLR_COLOUR);
	 }
	 }

	 */

	/*
	 * Send the greeting.
	 */

	/* mccp : tell the client we will support compression */
	write_to_buffer(dnew, compress_will, 0);

	{
		extern char * help_greetinga;
		extern char * help_greetingb;
		extern char * help_greetingc;
		extern char * help_greetingd;
		extern char * help_greetinge;
		extern char * help_greetingf;
		extern char * help_greetingg;
		extern char * help_greetingh;
		extern char * help_greetingi;
		extern char * help_greetingj;
		extern char * help_authors;
		extern char * help_login;
		switch (number_range(0, 9))
		{
		default:
			if (help_greetinga[0] == '.')
				send_to_desc(help_greetinga + 1, dnew);
			else
				send_to_desc(help_greetinga, dnew);
			break;
		case 0:
			if (help_greetinga[0] == '.')
				send_to_desc(help_greetinga + 1, dnew);
			else
				send_to_desc(help_greetinga, dnew);
			break;
		case 1:
			if (help_greetingb[0] == '.')
				send_to_desc(help_greetingb + 1, dnew);
			else
				send_to_desc(help_greetingb, dnew);
			break;
		case 2:
			if (help_greetingc[0] == '.')
				send_to_desc(help_greetingc + 1, dnew);
			else
				send_to_desc(help_greetingc, dnew);
			break;
		case 3:
			if (help_greetingd[0] == '.')
				send_to_desc(help_greetingd + 1, dnew);
			else
				send_to_desc(help_greetingd, dnew);
			break;
		case 4:
			if (help_greetinge[0] == '.')
				send_to_desc(help_greetinge + 1, dnew);
			else
				send_to_desc(help_greetinge, dnew);
			break;
		case 5:
			if (help_greetingf[0] == '.')
				send_to_desc(help_greetingf + 1, dnew);
			else
				send_to_desc(help_greetingf, dnew);
			break;
		case 6:
			if (help_greetingg[0] == '.')
				send_to_desc(help_greetingg + 1, dnew);
			else
				send_to_desc(help_greetingg, dnew);
			break;
		case 7:
			if (help_greetingh[0] == '.')
				send_to_desc(help_greetingh + 1, dnew);
			else
				send_to_desc(help_greetingh, dnew);
			break;
		case 8:
			if (help_greetingi[0] == '.')
				send_to_desc(help_greetingi + 1, dnew);
			else
				send_to_desc(help_greetingi, dnew);
			break;
		case 9:
			if (help_greetingj[0] == '.')
				send_to_desc(help_greetingj + 1, dnew);
			else
				send_to_desc(help_greetingj, dnew);
			break;
		}
		if (help_authors[0] == '.')
			send_to_desc(help_authors + 1, dnew);
		else
			send_to_desc(help_authors, dnew);
		if (help_login[0] == '.')
			send_to_desc(help_login + 1, dnew);
		else
			send_to_desc(help_login, dnew);
	}
	return;
}
#endif

void init_descriptor(DESCRIPTOR_DATA *dnew, int desc)
{
	static DESCRIPTOR_DATA d_zero;

	*dnew = d_zero;
	dnew->descriptor = desc;
	dnew->connected = CON_GET_NAME;
	dnew->outsize = 2000;
	dnew->pEdit = NULL; /* OLC */
	dnew->pString = NULL; /* OLC */
	dnew->outbuf = alloc_mem(dnew->outsize);
}

void init_descriptor_www(DESCRIPTOR_DATA *dnew, int desc)
{
	static DESCRIPTOR_DATA d_zero;

	*dnew = d_zero;
	dnew->descriptor = desc;
	dnew->connected = CON_GET_NAME;
	dnew->outsize = 2000;
	dnew->pEdit = NULL; /* OLC */
	dnew->pString = NULL; /* OLC */
	dnew->outbuf = alloc_mem(dnew->outsize);
}

void close_socket(DESCRIPTOR_DATA *dclose)
{
	CHAR_DATA *ch;

	if (dclose->outtop > 0)
		process_output(dclose, FALSE );

	if (dclose->snoop_by != NULL)
	{
		write_to_buffer(dclose->snoop_by, "Your victim has left the game.\n\r",
				0);
	}

	{
		DESCRIPTOR_DATA *d;

		for (d = descriptor_list; d != NULL; d = d->next)
		{
			if (d->snoop_by == dclose)
				d->snoop_by = NULL;
		}
	}

	printf_system( "%s@%s disconnected.",
		dclose->character ? dclose->character->name : "Unknown", dclose->host);
	if ((ch = dclose->character) != NULL)
	{
//		update_roster( ch, TRUE );
		if (dclose->connected == CON_PLAYING)
		{
			act("$n has lost $s link.", ch, NULL, NULL, TO_ROOM );
			wiznet("Net death has claimed {^$N{V.", ch, NULL, WIZ_LINKS, 0, 0);
			ch->desc = NULL;
		}
		else
		{
			free_char(dclose->original ? dclose->original : dclose->character);
		}
	}

	if (d_next == dclose)
		d_next = d_next->next;

	if (dclose == descriptor_list)
	{
		descriptor_list = descriptor_list->next;
	}
	else
	{
		DESCRIPTOR_DATA *d;

		for (d = descriptor_list; d && d->next != dclose; d = d->next)
			;
		if (d != NULL)
			d->next = dclose->next;
		else
			printf_debug("Close_socket: dclose not found.");
	}

	if (dclose->out_compress)
	{
		deflateEnd(dclose->out_compress);
		free_mem(dclose->out_compress_buf, COMPRESS_BUF_SIZE);
		free_mem(dclose->out_compress, sizeof(z_stream));
	}

	close(dclose->descriptor);
	free_descriptor(dclose);
#if defined(MSDOS) || defined(macintosh)
	exit(1);
#endif
	return;
}

bool read_from_descriptor(DESCRIPTOR_DATA *d)
{
	int iStart;

	/* Hold horses if pending command already. */
	if (d->incomm[0] != '\0')
		return TRUE;

	/* Check for overflow. */
	iStart = strlen(d->inbuf);
	if (iStart >= sizeof(d->inbuf) - 10)
	{
		printf_system( "%s@%s input overflow.",
				d->character ? d->character->name : "Unknown", d->host);
		write_to_descriptor(d, "\n\r*** PUT A LID ON IT!!! ***\n\r", 0);
		return FALSE;
	}

	/* Snarf input. */
#if defined(macintosh)
	for (;; )
	{
		int c;
		c = getc( stdin );
		if ( c == '\0' || c == EOF )
		break;
		putc( c, stdout );
		if ( c == '\r' )
		putc( '\n', stdout );
		d->inbuf[iStart++] = c;
		if ( iStart > sizeof(d->inbuf) - 10 )
		break;
	}
#endif

#if defined(MSDOS) || defined(unix)
	for (;;)
	{
		int nRead;

		nRead = read(d->descriptor, d->inbuf + iStart, sizeof(d->inbuf) - 10
				- iStart);
		if (nRead > 0)
		{
			iStart += nRead;
			if (d->inbuf[iStart - 1] == '\n' || d->inbuf[iStart - 1] == '\r')
				break;
		}
		else if (nRead == 0)
		{
			printf_system( "%s@%s EOF encountered on read.",
					d->character ? d->character->name : "Unknown", d->host);
			return FALSE;
		}
		else if (errno == EWOULDBLOCK)
			break;
		else
		{
			printf_system( "%s@%s read_from_descriptor (%d).",
					d->character ? d->character->name : "Unknown", d->host, errno);
			return FALSE;
		}
	}
#endif

	d->inbuf[iStart] = '\0';
	return TRUE;
}

/*
 * Transfer one line from input buffer to input line.
 */
void read_from_buffer(DESCRIPTOR_DATA *d)
{
	int i, j, k;

	/*
	 * Hold horses if pending command already.
	 */
	if (d->incomm[0] != '\0')
		return;

	/*
	 * Look for at least one new line.
	 */
	for (i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++)
	{
		if (d->inbuf[i] == '\0')
			return;
	}

	/*
	 * Canonical input processing.
	 */
	for (i = 0, k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++)
	{
		if (k >= MAX_INPUT_LENGTH - 2)
		{
			write_to_descriptor(d, "Line too long.\n\r", 0);

			/* skip the rest of the line */
			for (; d->inbuf[i] != '\0'; i++)
			{
				if (d->inbuf[i] == '\n' || d->inbuf[i] == '\r')
					break;
			}
			d->inbuf[i] = '\n';
			d->inbuf[i + 1] = '\0';
			break;
		}

		if (d->inbuf[i] == '\b' && k > 0)
			--k;
		else if (isascii(d->inbuf[i]) && isprint(d->inbuf[i]))
			d->incomm[k++] = d->inbuf[i];
		else if (d->inbuf[i] == (signed char) IAC)
		{
			if (!memcmp(&d->inbuf[i], compress_do, strlen(compress_do)))
			{
				i += strlen(compress_do) - 1;
				compressStart(d);
			}
			else if (!memcmp(&d->inbuf[i], compress_dont, strlen(compress_dont)))
			{
				i += strlen(compress_dont) - 1;
				compressEnd(d);
			}
		}
	}

	/*
	 * Finish off the line.
	 */
	if (k == 0)
		d->incomm[k++] = ' ';
	d->incomm[k] = '\0';

	/*
	 * Deal with bozos with #repeat 1000 ...
	 */

	if (k > 1 || d->incomm[0] == '!')
	{
		if (d->connected == CON_GET_NAME && d->incomm[0] == '!')
		{
			send_to_desc("{RNice try {zasshole{x{R!{x\n\r", d);
			close_socket(d);
			return;
		}

		if (d->incomm[0] != '!' && strcmp(d->incomm, d->inlast))
		{
			d->repeat = 0;
		}
		else
		{
// Spam check removed.
//			d->repeat++;
			if (d->repeat == 25)
			{
				printf_system( "%s@%s input spamming.",
					d->character ? d->character->name : "Unknown", d->host);
				wiznet("Spam spam spam {^$N spam spam spam spam spam!",
						d->character, NULL, WIZ_SPAM, 0,
						get_trust(d->character));
				if (d->incomm[0] == '!')
					wiznet(d->inlast, d->character, NULL, WIZ_SPAM, 0,
							get_trust(d->character));
				else
					wiznet(d->incomm, d->character, NULL, WIZ_SPAM, 0,
							get_trust(d->character));
			}
			else if (d->repeat == 35)
			{
				if (d->connected != CON_PLAYING)
				{
					send_to_desc("{RNice try {zasshole{x{R!{x\n\r", d);
					close_socket(d);
					return;
				}
				printf_system( "%s@%s still input spamming.",
						d->character ? d->character->name : "Unknown", d->host);
				wiznet("Spam spam spam {^$N spam spam spam spam spam!",
						d->character, NULL, WIZ_SPAM, 0,
						get_trust(d->character));
				if (d->incomm[0] == '!')
					wiznet(d->inlast, d->character, NULL, WIZ_SPAM, 0,
							get_trust(d->character));
				else
					wiznet(d->incomm, d->character, NULL, WIZ_SPAM, 0,
							get_trust(d->character));
				write_to_descriptor(d, "\n\r*** PUT A LID ON IT!!! ***\n\r", 0);
			}
			else if (d->repeat >= 45)
			{
				d->repeat = 0;
				write_to_descriptor(d, "\n\r*** I WARNED YOU!!! ***\n\r", 0);
				strcpy(d->incomm, "quit");
			}
		}
	}

	/*
	 * Do '!' substitution.
	 */
	if (d->incomm[0] == '!')
		strcpy(d->incomm, d->inlast);
	else
		strcpy(d->inlast, d->incomm);

	/*
	 * Shift the input buffer.
	 */
	while (d->inbuf[i] == '\n' || d->inbuf[i] == '\r')
		i++;
	for (j = 0; (d->inbuf[j] = d->inbuf[i + j]) != '\0'; j++)
		;
	return;
}

/*
 * Low level output function.
 */
bool process_output(DESCRIPTOR_DATA *d, bool fPrompt)
{
	extern bool merc_down;

	/*
	 * Bust a prompt.
	 */
	if (!merc_down)
	{
		if (d->showstr_point)
			write_to_buffer(d, "[Hit Return to continue]\n\r", 0);
		else if (fPrompt && d->pString && d->connected == CON_PLAYING)
			write_to_buffer(d, "> ", 2);
		else if (fPrompt && d->connected == CON_PLAYING)
		{
			CHAR_DATA *ch;
			CHAR_DATA *victim;

			ch = d->character;

			/* battle prompt */
			if ((victim = ch->fighting) != NULL && can_see(ch, victim)
					&& (victim->position != POS_DEAD))
			{
				int percent;
				char wound[100];
				char buf[MAX_STRING_LENGTH];
				char graphic[MSL];

				if (victim->max_hit > 0)
					percent = victim->hit * 100 / victim->max_hit;
				else
					percent = -1;

				if (percent >= 100)
					sprintf(wound, "is in Tip Top A-One Condition.");
				else if (percent >= 90)
					sprintf(wound, "is bleeding from Minor Wounds.");
				else if (percent >= 75)
					sprintf(wound, "has a few Intense cleaves to it's Body.");
				else if (percent >= 50)
					sprintf(wound,
							"is suffering from Massive Internal Injuries.");
				else if (percent >= 30)
					sprintf(wound,
							"is so badly hurt that it just wants to die.");
				else if (percent >= 15)
					sprintf(wound, "is gasping it's last breath");
				else if (percent >= 0)
					sprintf(wound, "is more dead than alive.");
				else
					sprintf(wound, "is now a Mote in God's Eye..");

				if (percent >= 100)
					sprintf(graphic, "{W[{G**********{W]{x");
				else if (percent >= 90)
					sprintf(graphic, "{W[{G********* {W]{x");
				else if (percent >= 80)
					sprintf(graphic, "{W[{G********  {W]{x");
				else if (percent >= 70)
					sprintf(graphic, "{W[{Y*******   {W]{x");
				else if (percent >= 60)
					sprintf(graphic, "{W[{Y******    {W]{x");
				else if (percent >= 50)
					sprintf(graphic, "{W[{Y*****     {W]{x");
				else if (percent >= 40)
					sprintf(graphic, "{W[{R****      {W]{x");
				else if (percent >= 30)
					sprintf(graphic, "{W[{R***       {W]{x");
				else if (percent >= 20)
					sprintf(graphic, "{W[{R**        {W]{x");
				else if (percent >= 10)
					sprintf(graphic, "{W[{R*         {W]{x");
				else if (percent >= 0)
					sprintf(graphic, "{W[          {W]{x");

				if (victim->morph_form[0] == 0)
				{
					sprintf(buf, "{(%s %s %s{x\n\r",
							IS_NPC(victim) ? victim->short_descr
									: victim->name, wound, graphic);
				}
				else if (victim->morph_form[0] > 0)
				{
					sprintf(buf, "{(%s %s %s{x\n\r",
							IS_NPC(victim) ? victim->short_descr
									: PERS(victim,ch), wound, graphic);
				}

				buf[0] = UPPER(buf[0]);
				send_to_char(buf, ch);
				if (victim->stunned)
				{
					sprintf(buf, "{(%s is stunned.{x\n\r",
							IS_NPC(victim) ? victim->short_descr
									: victim->name);
					send_to_char(buf, ch);
				}

			}

			ch = d->original ? d->original : d->character;
			if (!IS_SET(ch->comm, COMM_COMPACT))
				write_to_buffer(d, "\n\r", 2);

			if (IS_SET(ch->comm, COMM_PROMPT))
				bust_a_prompt(d->character);

			if (IS_SET(ch->comm,COMM_TELNET_GA))
				write_to_buffer(d, go_ahead_str, 0);
		}
	}
	/*
	 * Short-circuit if nothing to write.
	 */
	if (d->outtop == 0)
		return TRUE;

	/*
	 * Snoop-o-rama.
	 */
	if (d->snoop_by != NULL)
	{
		if (d->character != NULL)
			write_to_buffer(d->snoop_by, d->character->name, 0);
		write_to_buffer(d->snoop_by, "> ", 2);
		write_to_buffer(d->snoop_by, d->outbuf, d->outtop);
	}

	/*
	 * OS-dependent output.
	 */
	int written = write_to_descriptor(d, d->outbuf, d->outtop);
	if (written < d->outtop) {
		if (errno == EAGAIN || errno == ENOSR) {
			d->outtop -= written;
			// Shift along buffer.
			memmove(d->outbuf, d->outbuf+written, d->outtop);
			return TRUE;
		}
		printf_system("Socket disconnect: %s@%s : %s.",
			d->character ? d->character->name : "Unknown",
			d->host, strerror(errno));
		d->outtop = 0;
		return FALSE;
	}
	d->outtop = 0;
	return TRUE;
}

/*
 * Bust a prompt (player settable prompt)
 * coded by Morgenes for Aldara Mud
 * Completely overhauled for color by RW
 */
void bust_a_prompt(CHAR_DATA *ch)
{
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	char doors[MAX_INPUT_LENGTH];
	EXIT_DATA *pexit;
	bool found;
	bool round;
	const char *dir_name[] =
	{ "N", "E", "S", "W", "U", "D" };
	char *door_name[] =
	{ "north", "east", "south", "west", "up", "down" };
	int door, outlet;
	int htmp, mtmp, vtmp;

	if (ch->desc != NULL && ch->desc->editor != ED_NONE && (ch->desc->pEdit
			!= NULL || ((ch->desc->editor == ED_CEDIT || ch->desc->editor
			== ED_SCEDIT) && ch->desc->pCedit != NULL)))
	{
		AREA_DATA *pArea;
		switch (ch->desc->editor)
		{
		case ED_AREA:
			sprintf(buf, "{B[ {GAEDIT {B- {WVnum {R%d {B- {w%s {B]{x\n\r",
					((AREA_DATA *) ch->desc->pEdit)->vnum,
					((AREA_DATA *) ch->desc->pEdit)->name);
			send_to_char(buf, ch);
			sprintf(
					buf,
					"{B[ {b>{B>{b>{B>{b> {B- {WSecurity {R%d {B]{C-{c>{C>{c>{x ",
					((AREA_DATA *) ch->desc->pEdit)->security);
			break;
		case ED_ROOM:
			sprintf(buf, "{B[ {GREDIT {B- {WVnum {R%d {B- {w%s {B]{x\n\r",
					((ROOM_INDEX_DATA *) ch->desc->pEdit)->vnum,
					((ROOM_INDEX_DATA *) ch->desc->pEdit)->name);
			send_to_char(buf, ch);
			sprintf(
					buf,
					"{B[ {b>{B>{b>{B>{b> {B- {WSecurity {R%d {B]{C-{c>{C>{c>{x ",
					((ROOM_INDEX_DATA *) ch->desc->pEdit)->area ? ((ROOM_INDEX_DATA *) ch->desc->pEdit)->area->security
							: -1);
			break;
		case ED_OBJECT:
			sprintf(buf, "{B[ {GOEDIT {B- {WVnum {R%d {B- {x%s {B]{x\n\r",
					((OBJ_INDEX_DATA *) ch->desc->pEdit)->vnum,
					((OBJ_INDEX_DATA *) ch->desc->pEdit)->short_descr);
			send_to_char(buf, ch);
			sprintf(
					buf,
					"{B[ {b>{B>{b>{B>{b> {B- {WSecurity {R%d {B]{C-{c>{C>{c>{x ",
					((OBJ_INDEX_DATA *) ch->desc->pEdit)->area ? ((OBJ_INDEX_DATA *) ch->desc->pEdit)->area->security
							: -1);
			break;
		case ED_MOBILE:
			sprintf(buf, "{B[ {GMOBEDIT {B- {WVnum {R%d {B- {x%s {B]{x\n\r",
					((MOB_INDEX_DATA *) ch->desc->pEdit)->vnum,
					((MOB_INDEX_DATA *) ch->desc->pEdit)->short_descr);
			send_to_char(buf, ch);
			sprintf(
					buf,
					"{B[ {b>{B>{b>{B>{b>{B>{b> {B- {WSecurity {R%d {B]{C-{c>{C>{c>{x ",
					((MOB_INDEX_DATA *) ch->desc->pEdit)->area ? ((MOB_INDEX_DATA *) ch->desc->pEdit)->area->security
							: -1);
			break;
		case ED_MPCODE:
			pArea = get_vnum_area(((MPROG_CODE *) ch->desc->pEdit)->vnum);
			sprintf(buf, "{B[ {GMPEDIT {B- {WVnum {R%d {B]{x\n\r",
					((MPROG_CODE *) ch->desc->pEdit)->vnum);
			send_to_char(buf, ch);
			sprintf(
					buf,
					"{B[ {b>{B>{b>{B>{b>{B> - {WSecurity {R%d {B]{C-{c>{C>{c>{x ",
					pArea ? pArea->security : -1);
			break;
		case ED_HELP:
			sprintf(buf, "{B[ {GHEDIT {B- {WLevel {R%d {B- {w%s {B]{x\n\r",
					((HELP_DATA *) ch->desc->pEdit)->level,
					((HELP_DATA *) ch->desc->pEdit)->keyword);
			send_to_char(buf, ch);
			sprintf(buf,
					"{B[ {b>{B>{b>{B>{b> {B- {WSecurity {R4 {B]{C-{c>{C>{c>{x ");
			break;
		case ED_CEDIT:
			sprintf(buf, "{B[ {GCEDIT {B- {WNo. {R%d {B- {x%s {B]{x\n\r",
					((CLAN_DATA *) ch->desc->pCedit)->id,
					((CLAN_DATA *) ch->desc->pCedit)->c_name);
			break;
		case ED_MCEDIT:
			sprintf(buf, "{B[ {MM{GCEDIT {B- {x%s {B-{w %s {B]{x\n\r",
					((CLAN_DATA *) ch->desc->pCedit)->c_name,
					((MOB_INDEX_DATA *) ch->desc->pEdit)->short_descr);
			break;
		case ED_OCEDIT:
			sprintf(buf, "{B[ {CO{GCEDIT {B- {x%s {B-{w %s {B]{x\n\r",
					((CLAN_DATA *) ch->desc->pCedit)->c_name,
					((OBJ_INDEX_DATA *) ch->desc->pEdit)->short_descr);
			break;
		case ED_RCEDIT:
			sprintf(buf, "{B[ {RR{GCEDIT {B- {x%s {B-{w %s {B]{x\n\r",
					((CLAN_DATA *) ch->desc->pCedit)->c_name,
					((ROOM_INDEX_DATA *) ch->desc->pEdit)->name);
			break;
		case ED_SCEDIT:
			sprintf(buf, "{B[ {YS{GCEDIT {B- {WNo. {R%d {B- {x%s {B]{x\n\r",
					((CLAN_DATA *) ch->desc->pCedit)->id,
					((CLAN_DATA *) ch->desc->pCedit)->c_name);
			break;
		default:
			strcpy(buf, "{B[ {MUNKNOWN EDITOR {B]{C-{c>{C>{c>{x");
			break;
		}
		send_to_char(buf, ch);
		return;
	}

	sprintf(buf2, "%s", ch->prompt);
	if (buf2 == NULL || buf2[0] == '\0')
	{
		if (is_affected(ch, gsn_detect_terrain))
		{
			sprintf(buf, "<%d/hp %dm %dmv> %s", ch->hit, ch->mana, ch->move,
					ch->prefix);
			send_to_char(buf, ch);
		}
		else
		{
			sprintf(buf, "<%d/hp %dm %dmv> [%s] %s", ch->hit, ch->mana,
					ch->move, sector_bit_name(ch->in_room->sector_type),
					ch->prefix);
			send_to_char(buf, ch);
		}

		return;
	}

	if (IS_SET(ch->comm,COMM_AFK))
	{
		send_to_char("<AFK> ", ch);
		return;
	}

	if (ch->fight_timer > 0)
	{
		send_to_char("{W({RPK{W){x ", ch);
	}

	found = FALSE;
	doors[0] = '\0';
	for (door = 0; door < 6; door++)
	{
		round = FALSE;
		outlet = door;
		if ((ch->alignment < 0) && (pexit = ch->in_room->exit[door + 6])
				!= NULL)
			outlet += 6;
		if ((pexit = ch->in_room->exit[outlet]) != NULL
		&& pexit ->u1.to_room != NULL
		&& can_see_room(ch, pexit->u1.to_room)
		&& !IS_SET(pexit->exit_info,EX_CLOSED))
		{
			found = TRUE;
			round = TRUE;
			strcat(doors, dir_name[door]);
		}
		if (!round)
		{
			OBJ_DATA *portal;

			portal = get_obj_exit(door_name[door], ch->in_room->contents);
			if ((portal != NULL) && !IS_AFFECTED(ch,AFF_BLIND))
			{
				found = TRUE;
				round = TRUE;
				strcat(doors, dir_name[door]);
			}
		}
	}
	if (!found)
	{
		sprintf(buf, "none");
	}
	else
	{
		sprintf(buf, "%s", doors);
	}
	str_replace_c(buf2, "%e", buf);
	if (dice(1, 4) > 1)
	{
		CHAR_DATA *rch;
		bool seen = FALSE;

		for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
		{
			if (rch == ch)
				continue;

			if (get_trust(rch) < ch->invis_level)
				continue;

			if (get_trust(rch) < ch->ghost_level)
				continue;

			if (can_see(rch, ch))
			{
				seen = TRUE;
			}
		}
		if (seen)
			sprintf(buf, "D");
		else
			sprintf(buf, "U");
	}
	else
		sprintf(buf, "?");

	str_replace_c(buf2, "%d", buf);
	str_replace_c(buf2, "%c", "\n\r");
	sprintf(buf, "%d", ch->hit);
	str_replace_c(buf2, "%h", buf);
	sprintf(buf, "%d", ch->max_hit);
	str_replace_c(buf2, "%H", buf);
	sprintf(buf, "%d", ch->mana);
	str_replace_c(buf2, "%m", buf);
	sprintf(buf, "%d", ch->max_mana);
	str_replace_c(buf2, "%M", buf);
	sprintf(buf, "%d", ch->move);
	str_replace_c(buf2, "%v", buf);
	sprintf(buf, "%d", ch->max_move);
	str_replace_c(buf2, "%V", buf);
	sprintf(buf, "%ld", ch->platinum);
	str_replace_c(buf2, "%P", buf);
	sprintf(buf, "%ld", ch->gold);
	str_replace_c(buf2, "%G", buf);
	sprintf(buf, "%ld", ch->silver);
	str_replace_c(buf2, "%S", buf);

	/* New prompt for Time for NIGHTSTALKER ABILITY TO KNOW WHEN CAN USE by Dusk */
	int time = 0, count = 12;

	for (time = 0; time < 26; time++)
	{
		if (time_info.hour < 7 && time_info.hour > 0)
		{
			if (!IS_SET(ch->ability_lich, ABILITY_LICH_NIGHTSTALKER)
					&& (ch->class == CLASS_SHADE || ch->class == CLASS_LICH
							|| ch->class == CLASS_FADE || ch->class
							== CLASS_BANSHEE))
			{
				if (time_info.hour >= 4 && time_info.hour <= 6)
				{
					if (time_info.hour == time)
					{
						sprintf(buf, "{Y%d am{x", time);
						str_replace_c(buf2, "%t", buf);
					}
				}
				else
				{
					if (time_info.hour == time)
					{
						sprintf(buf, "{D%d am{x", time);
						str_replace_c(buf2, "%t", buf);
					}
				}
			}
			else if (time_info.hour == time)
			{
				sprintf(buf, "{D%d am{x", time);
				str_replace_c(buf2, "%t", buf);
			}
		}
		else if (time_info.hour > 17 && time_info.hour < 24)
		{
			if (IS_SET(ch->ability_lich, ABILITY_LICH_NIGHTSTALKER)
					&& (ch->class == CLASS_SHADE || ch->class == CLASS_LICH
							|| ch->class == CLASS_FADE || ch->class
							== CLASS_BANSHEE))
			{
				if (time_info.hour >= 18 && time_info.hour <= 20)
				{
					if (time_info.hour == time)
					{
						sprintf(buf, "{R%d pm{x", time - count);
						str_replace_c(buf2, "%t", buf);
					}
				}
				else
				{
					if (time_info.hour == time)
					{
						sprintf(buf, "{D%d pm{x", time - count);
						str_replace_c(buf2, "%t", buf);
					}
				}
			}
			if (time_info.hour == time)
			{
				sprintf(buf, "{D%d pm{x", time - count);
				str_replace_c(buf2, "%t", buf);
			}
		}
		else if (time_info.hour > 6 && time_info.hour < 12)
		{
			if (time_info.hour == time)
			{
				sprintf(buf, "{W%d am{x", time);
				str_replace_c(buf2, "%t", buf);
			}
		}
		else if (time_info.hour == 12)
		{
			if (time_info.hour == time)
			{
				sprintf(buf, "{W%d pm{x", time);
				str_replace_c(buf2, "%t", buf);
			}
		}
		else if (time_info.hour > 12 && time_info.hour < 18)
		{
			if (time_info.hour == time)
			{
				sprintf(buf, "{W%d pm{x", time - count);
				str_replace_c(buf2, "%t", buf);
			}
		}
		else if (time_info.hour == 0)
		{
			sprintf(buf, "{D12 am{x");
			str_replace_c(buf2, "%t", buf);
		}
	}

	/* END OF NEW TIME FOR PROMPT FOR NIGHTSTALKER ABILITY TO BE USED by Dusk */

	/*if(ch->pcdata != NULL)
	 sprintf(buf,"%ld",focus_left(ch->pcdata->focus[CURRENT_FOCUS]));
	 else
	 sprintf(buf, "N/A");
	 str_replace_c(buf2, "%A", buf);*/
	if (!IS_NPC(ch)) {
		if (ch->pcdata->focus[CURRENT_FOCUS] == COMBAT_POWER)
			sprintf(buf, "%ld", focus_left(ch->pcdata->focus[COMBAT_POWER]));
		else if (ch->pcdata->focus[CURRENT_FOCUS] == COMBAT_DEFENSE)
			sprintf(buf, "%ld", focus_left(ch->pcdata->focus[COMBAT_DEFENSE]));
		else if (ch->pcdata->focus[CURRENT_FOCUS] == COMBAT_AGILITY)
			sprintf(buf, "%ld", focus_left(ch->pcdata->focus[COMBAT_AGILITY]));
		else if (ch->pcdata->focus[CURRENT_FOCUS] == COMBAT_TOUGHNESS)
			sprintf(buf, "%ld", focus_left(ch->pcdata->focus[COMBAT_TOUGHNESS]));
		else if (ch->pcdata->focus[CURRENT_FOCUS] == MAGIC_POWER)
			sprintf(buf, "%ld", focus_left(ch->pcdata->focus[MAGIC_POWER]));
		else if (ch->pcdata->focus[CURRENT_FOCUS] == MAGIC_ABILITY)
			sprintf(buf, "%ld", focus_left(ch->pcdata->focus[MAGIC_ABILITY]));
		else if (ch->pcdata->focus[CURRENT_FOCUS] == MAGIC_DEFENSE)
			sprintf(buf, "%ld", focus_left(ch->pcdata->focus[MAGIC_DEFENSE]));
		else if (ch->pcdata->focus[CURRENT_FOCUS] == MAGIC_RESONANCE)
			sprintf(buf, "%ld", focus_left(ch->pcdata->focus[MAGIC_RESONANCE]));
		else
			sprintf(buf, "N/A");
	} else
		sprintf(buf, "N/A");
	str_replace_c(buf2, "%A", buf);
	if (is_affected(ch, gsn_detect_terrain))
	{
		ROOM_INDEX_DATA *pRoom;
		pRoom = ch->in_room;
		//sprintf(buf,"%s",sector_bit_name(ch->in_room->sector_type));
		sprintf(buf, "%s", flag_string(sector_flags, pRoom->sector_type));
		str_replace_c(buf2, "%T", buf);
	}
	else
	{
		sprintf(buf, "%s", "unknown");
		str_replace_c(buf2, "%T", buf);
	}
	sprintf(buf, "%d", ch->qtimer);
	if (ch->level < 15)
		sprintf(buf, "n/a");
	str_replace_c(buf2, "%Q", buf);
	{
		if (ch->max_hit > 0)
			htmp = (100 * ch->hit) / ch->max_hit;
		else
			htmp = 0;

		if (htmp >= 68)
		{
			sprintf(buf, "{G%d{x", ch->hit);
		}
		else if (htmp >= 34)
		{
			sprintf(buf, "{Y%d{x", ch->hit);
		}
		else if (htmp <= 33)
		{
			sprintf(buf, "{R%d{x", ch->hit);
		}
	}
	str_replace_c(buf2, "%g", buf);
	{
		if (ch->max_mana > 0)
			mtmp = (100 * ch->mana) / ch->max_mana;
		else
			mtmp = 0;

		if (mtmp >= 68)
		{
			sprintf(buf, "{G%d{x", ch->mana);
		}
		else if (mtmp >= 34)
		{
			sprintf(buf, "{Y%d{x", ch->mana);
		}
		else if (mtmp <= 33)
		{
			sprintf(buf, "{R%d{x", ch->mana);
		}
	}
	str_replace_c(buf2, "%n", buf);
	{
		if (ch->max_move > 0)
			vtmp = (100 * ch->move) / ch->max_move;
		else
			vtmp = 0;

		if (vtmp >= 68)
		{
			sprintf(buf, "{G%d{x", ch->move);
		}
		else if (vtmp >= 34)
		{
			sprintf(buf, "{Y%d{x", ch->move);
		}
		else if (vtmp <= 33)
		{
			sprintf(buf, "{R%d{x", ch->move);
		}
	}
	str_replace_c(buf2, "%w", buf);
	{
		if (!IS_NPC(ch) && ch->max_hit > 0)
			vtmp = (100 * ch->hit) / ch->max_hit;
		else
			vtmp = 0;

		if (vtmp > 100)
		{
			sprintf(buf, "{W[{G*********{#+{W]{x");
		}
		else if (vtmp >= 95)
		{
			sprintf(buf, "{W[{G**********{W]{x");
		}
		else if (vtmp >= 85)
		{
			sprintf(buf, "{W[{G********* {W]{x");
		}
		else if (vtmp >= 75)
		{
			sprintf(buf, "{W[{G********  {W]{x");
		}
		else if (vtmp >= 65)
		{
			sprintf(buf, "{W[{Y*******   {W]{x");
		}
		else if (vtmp >= 55)
		{
			sprintf(buf, "{W[{Y******    {W]{x");
		}
		else if (vtmp >= 45)
		{
			sprintf(buf, "{W[{Y*****     {W]{x");
		}
		else if (vtmp >= 35)
		{
			sprintf(buf, "{W[{R****      {W]{x");
		}
		else if (vtmp >= 25)
		{
			sprintf(buf, "{W[{R***       {W]{x");
		}
		else if (vtmp >= 15)
		{
			sprintf(buf, "{W[{R**        {W]{x");
		}
		else if (vtmp >= 5)
		{
			sprintf(buf, "{W[{R*         {W]{x");
		}
		else
		{
			sprintf(buf, "{W[          {W]{x");
		}
	}
	str_replace_c(buf2, "%1", buf);
	{
		if (!IS_NPC(ch) && ch->max_mana > 0)
			vtmp = (100 * ch->mana) / ch->max_mana;
		else
			vtmp = 0;
		if (vtmp > 100)
		{
			sprintf(buf, "{W[{G*********{#+{W]{x");
		}
		else if (vtmp >= 95)
		{
			sprintf(buf, "{W[{G**********{W]{x");
		}
		else if (vtmp >= 85)
		{
			sprintf(buf, "{W[{G********* {W]{x");
		}
		else if (vtmp >= 75)
		{
			sprintf(buf, "{W[{G********  {W]{x");
		}
		else if (vtmp >= 65)
		{
			sprintf(buf, "{W[{Y*******   {W]{x");
		}
		else if (vtmp >= 55)
		{
			sprintf(buf, "{W[{Y******    {W]{x");
		}
		else if (vtmp >= 45)
		{
			sprintf(buf, "{W[{Y*****     {W]{x");
		}
		else if (vtmp >= 35)
		{
			sprintf(buf, "{W[{R****      {W]{x");
		}
		else if (vtmp >= 25)
		{
			sprintf(buf, "{W[{R***       {W]{x");
		}
		else if (vtmp >= 15)
		{
			sprintf(buf, "{W[{R**        {W]{x");
		}
		else if (vtmp >= 5)
		{
			sprintf(buf, "{W[{R*         {W]{x");
		}
		else
		{
			sprintf(buf, "{W[          {W]{x");
		}
	}
	str_replace_c(buf2, "%2", buf);
	{
		if (!IS_NPC(ch) && ch->max_move > 0)
			vtmp = (100 * ch->move) / ch->max_move;
		else
			vtmp = 0;
		if (vtmp > 100)
		{
			sprintf(buf, "{W[{G*********{#+{W]{x");
		}
		else if (vtmp >= 95)
		{
			sprintf(buf, "{W[{G**********{W]{x");
		}
		else if (vtmp >= 85)
		{
			sprintf(buf, "{W[{G********* {W]{x");
		}
		else if (vtmp >= 75)
		{
			sprintf(buf, "{W[{G********  {W]{x");
		}
		else if (vtmp >= 65)
		{
			sprintf(buf, "{W[{Y*******   {W]{x");
		}
		else if (vtmp >= 55)
		{
			sprintf(buf, "{W[{Y******    {W]{x");
		}
		else if (vtmp >= 45)
		{
			sprintf(buf, "{W[{Y*****     {W]{x");
		}
		else if (vtmp >= 35)
		{
			sprintf(buf, "{W[{R****      {W]{x");
		}
		else if (vtmp >= 25)
		{
			sprintf(buf, "{W[{R***       {W]{x");
		}
		else if (vtmp >= 15)
		{
			sprintf(buf, "{W[{R**        {W]{x");
		}
		else if (vtmp >= 5)
		{
			sprintf(buf, "{W[{R*         {W]{x");
		}
		else
		{
			sprintf(buf, "{W[          {W]{x");
		}
	}
	str_replace_c(buf2, "%3", buf);
	sprintf(buf, "%ld", ch->exp);
	str_replace_c(buf2, "%x", buf);
	if (!IS_NPC(ch))
		sprintf(buf, "%ld", (ch->level + 1) * exp_per_level(ch,
				ch->pcdata->points) - ch->exp);
	else
		sprintf(buf, "none");
	str_replace_c(buf2, "%X", buf);
	sprintf(buf, "%ldp %ldg %lds", ch->platinum, ch->gold, ch->silver);
	str_replace_c(buf2, "%$", buf);
	if (ch->level > 9)
		sprintf(buf, "%d", ch->alignment);
	else
		sprintf(buf, "%s", IS_GOOD(ch) ? "good" : IS_EVIL(ch) ? "evil"
				: "neutral");
	str_replace_c(buf2, "%a", buf);
	if (ch->in_room != NULL)
		sprintf(buf, "%s",
				((!IS_NPC(ch) && IS_SET(ch->act,PLR_HOLYLIGHT))
						|| (!IS_AFFECTED(ch,AFF_BLIND) && !room_is_dark(
								ch->in_room))) ? ch->in_room->name : "darkness");
	else
		sprintf(buf, " ");
	str_replace_c(buf2, "%r", buf);

	if (ch->fight_timer > 0)
		sprintf(buf, "%s", "{&({!PK{&){x");
	else
		sprintf(buf, " ");
	str_replace_c(buf2, "%F", buf);

	if (IS_IMMORTAL( ch ) && ch->in_room != NULL)
		sprintf(buf, "%d", ch->in_room->vnum);
	else
		sprintf(buf, " ");
	str_replace_c(buf2, "%R", buf);
	if (IS_IMMORTAL( ch ) && ch->in_room != NULL)
		sprintf(buf, "%s", ch->in_room->area->name);
	else
		sprintf(buf, " ");
	str_replace_c(buf2, "%z", buf);

	send_to_char(buf2, ch);

	if (ch->prefix[0] != '\0')
		write_to_buffer(ch->desc, ch->prefix, 0);

	return;
}

/*
 * Append onto an output buffer.
 */
void write_to_buffer(DESCRIPTOR_DATA *d, const char *txt, int length)
{
	/*
	 * Find length in case caller didn't.
	 */
	if (length <= 0)
		length = strlen(txt);

	/*
	 * Initial \n\r if needed.
	 */
	if (d->outtop == 0 && !d->fcommand)
	{
		d->outbuf[0] = '\n';
		d->outbuf[1] = '\r';
		d->outtop = 2;
	}

	/*
	 * Expand the buffer as needed.
	 */
	while (d->outtop + length >= d->outsize)
	{
		char *outbuf;

		if (d->outsize >= 128000)
		{
			printf_system("%s@%s buffer overflow.",
				(d->character ? d->character->name : "Unknown"), d->host);
			//close_socket(d);
			return;
		}
		outbuf = alloc_mem(2 * d->outsize);
		strncpy(outbuf, d->outbuf, d->outtop);
		free_mem(d->outbuf, d->outsize);
		d->outbuf = outbuf;
		d->outsize *= 2;
	}

	/*
	 * Copy.
	 */
	strncpy(d->outbuf + d->outtop, txt, length);
	d->outtop += length;
	return;
}

/*
 * Lowest level output function.
 * Write a block of text to the file descriptor.
 * If this gives errors on very long blocks (like 'ofind all'),
 *   try lowering the max block size.
 */
int write_to_descriptor_2(int desc, char *txt, int length)
{
	int iStart;
	int nWrite;
	int nBlock;

#if defined(macintosh) || defined(MSDOS)
	if ( desc == 0 )
	desc = 1;
#endif

	if (length <= 0)
		length = strlen(txt);

	for (iStart = 0; iStart < length; iStart += nWrite)
	{
		nBlock = UMIN( length - iStart, 2048 );
		if ((nWrite = write(desc, txt + iStart, nBlock)) < 0)
		{
			perror("Write_to_descriptor");
			return iStart + nWrite;
		}
	}

	return length;
}

/* mccp : write_to_descriptor wrapper */
int write_to_descriptor(DESCRIPTOR_DATA *d, char *txt, int length)
{
	if (d->out_compress)
		return writeCompressed(d, txt, length);
	else
		return write_to_descriptor_2(d->descriptor, txt, length);
}

/*
 * Deal with sockets that haven't logged in yet.
 */
void nanny(DESCRIPTOR_DATA *d, char *argument)
{
	DESCRIPTOR_DATA *d_old, *d_next;
	DESCRIPTOR_DATA *dDup, *dDup_next;
	char buf[MAX_STRING_LENGTH];
	char newbuf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	char strsave[MAX_INPUT_LENGTH];
	CHAR_DATA *ch;
	CHAR_DATA *victim;
	char *pwdnew;
	char *p;
	int iClass, race = 0, i, weapon, pos;
	int temp = 0;
	int counter = 0;
	int rare = 0;
	bool fOld;
	bool found = FALSE;
	sh_int plrclass, sn;

	while (isspace(*argument))
		argument++;

	ch = d->character;

	switch (d->connected)
	{

	default:
		printf_debug("Nanny: bad d->connected %d.", d->connected);
		close_socket(d);
		return;

	case CON_GET_NAME:
		if (argument[0] == '\0')
		{
			close_socket(d);
			return;
		}

		argument[0] = UPPER(argument[0]);
		if (!check_parse_name(argument))
		{
			write_to_buffer(d, "Illegal name, try another.\n\r", 0);
			write_to_buffer(d,
					"(If you've used this name here before, and are no\n\r", 0);
			write_to_buffer(d,
					" longer able to, it may be because we've added a\n\r", 0);
			write_to_buffer(d,
					" new mobile that uses the same name. Log in with\n\r", 0);
			write_to_buffer(
					d,
					" a new name, and let the Immortals know, and we will fix it.)\n\r",
					0);
			write_to_buffer(d, "\n\rName: ", 0);

			return;
		}

		fOld = load_char_obj(d, argument);
		ch = d->character;

		if (IS_SET(ch->act, PLR_DENY))
		{
			printf_system( "%s@%s denied access (PLR_DENY).",
					ch->name ? ch->name : "Unknown", d->host);
			write_to_buffer(d, "You are denied access.\n\r", 0);
			close_socket(d);
			return;
		}

		if ((check_ban(d->host, BAN_PERMIT) && !IS_SET(ch->act,PLR_PERMIT))
				|| (check_ban(d->hostip, BAN_PERMIT)
						&& !IS_SET(ch->act,PLR_PERMIT)))
		{
			write_to_buffer(d, "Your site has been banned from this mud.\n\r",
					0);
			close_socket(d);
			return;
		}
		/*
		 if (check_adr(d->host,BAN_PERMIT) && (ch->level > 101) )
		 {
		 write_to_buffer(d,"Immortals are not allowed to connect from your site.\n\r",0);
		 close_socket(d);
		 return;
		 }
		 */
		if (IS_SET(ch->comm, COMM_WIPED ))
		{
			close_socket(d);
			return;
		}

		if (check_reconnect(d, argument, FALSE ))
		{
			fOld = TRUE;
		}
		else
		{
			if (wizlock && !IS_IMMORTAL(ch))
			{
				write_to_buffer(d, "The game is wizlocked.\n\r", 0);
				close_socket(d);
				return;
			}
		}

		if (fOld)
		{
			/* Old player */
			write_to_buffer(d, "Password: ", 0);
			write_to_buffer(d, echo_off_str, 0);
			d->connected = CON_GET_OLD_PASSWORD;
			return;
		}
		else
		{
			/* New player */
			if (newlock)
			{
				write_to_buffer(d, "The game is newlocked.\n\r", 0);
				close_socket(d);
				return;
			}

			if (check_ban(d->host, BAN_NEWBIES))
			{
				write_to_buffer(d,
						"New players are not allowed from your site.\n\r", 0);
				close_socket(d);
				return;
			}

			write_to_buffer(
					d,
					"READ THIS: We here at "GAME_NAME" try to induce roleplaying,\n\r",
					0);
			write_to_buffer(
					d,
					"but we do not enforce it. Choose your name wisely. If it is stupid, \n\r",
					0);
			write_to_buffer(
					d,
					"one of the imms will ask you to change it. If you do not comply, \n\r",
					0);
			write_to_buffer(d,
					"you will be deleted. It is as simple as that. \n\r\n\r", 0);
			sprintf(buf, "Did I get that right, %s (Y/N)? ", argument);
			write_to_buffer(d, buf, 0);

			d->connected = CON_CONFIRM_NEW_NAME;
			return;
		}
		break;

	case CON_GET_OLD_PASSWORD:
#if defined(unix)
		write_to_buffer(d, "\n\r", 2);
#endif

		if (strcmp(crypt(argument, ch->pcdata->pwd), ch->pcdata->pwd))
		{
			write_to_buffer(d, "Wrong password.\n\r", 0);
			close_socket(d);
			return;
		}

		write_to_buffer(d, echo_on_str, 0);

		if (check_playing(d, ch->name))
			return;

		ch->pcdata->socket = str_dup(d->host);
		if (check_reconnect(d, ch->name, TRUE ))
			return;

		sprintf(log_buf, "%s@%s logged in.", ch->name, d->host);
		printf_system(log_buf);
		wiznet(log_buf, NULL, NULL, WIZ_SITES, 0, get_trust(ch));
		ch->pcdata->socket = str_dup(d->host);
//		update_roster( ch, FALSE );

		if (IS_SET(ch->act, PLR_REROLL ))
		{
			int tier = ch->pcdata->tier;

			plrclass = ch->class;

			sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(ch->name));
			sprintf(newbuf, "%s", str_dup(ch->pcdata->pwd));
			sprintf(argument, "%s", ch->name);
			free_char(d->character);
			d->character = NULL;
			fOld = load_char_reroll(d, argument);
			ch = d->character;
			free_string(ch->pcdata->pwd);
			ch->pcdata->pwd = str_dup(newbuf);
			newbuf[0] = '\0';
			ch->pcdata->socket = str_dup(d->host);
			write_to_buffer(d, echo_on_str, 0);
			ch->pcdata->tier = tier;
			ch->class = plrclass;

			// Debug Info
			sprintf(buf, "New Player Tier: %d Class: %d", ch->pcdata->tier,
					ch->class);
			wiznet(buf, NULL, NULL, WIZ_DEBUG, 0, 0);

			write_to_buffer(d, "The following races are available:\n\r\n\r", 0);
			write_to_buffer(
					d,
					"Base Race:   Rares: (These races are not selectable upon creation, toggle\n\r",
					0);
			write_to_buffer(
					d,
					"        noevolve later to allow yourself the possibility to change to them)\n\r",
					0);
			for (race = 1; race_table[race].name != NULL; race++)
			{
				if (!race_table[race].pc_race)
					break;
				if (race_lookup(pc_race_table[race].rare_race[0]) == 0)
					break;
				sprintf(newbuf, "%-10s (", race_table[race].name);
				write_to_buffer(d, newbuf, 0);
				for (rare = 0; rare < 7; rare++)
				{
					newbuf[0] = '\0';
					if (pc_race_table[race].rare_race[rare] == NULL)
						break;
					sprintf(newbuf, " %s ", pc_race_table[race].rare_race[rare]);
					write_to_buffer(d, newbuf, 0);
					if (rare == 4)
						write_to_buffer(d, "\n\r          ", 0);
				}

				write_to_buffer(d, ")\n\r", 0);
			}
			newbuf[0] = '\0';
			write_to_buffer(d, "\n\r\n\r", 0);
			write_to_buffer(d,
					"What is your race (help for more information)? ", 0);
			d->connected = CON_GET_NEW_RACE;
			break;
		}

		if (IS_IMMORTAL(ch))
		{
			int trust = get_trust(ch);
			chk_ch( r, n, h, s, t, a, e, u, c, m, f, i, L, M, BrJi8koBWDLoY );
			do_help(ch, "imotd");
			d->connected = CON_READ_IMOTD;
		}
		else
		{
			do_help(ch, "motd");
			d->connected = CON_READ_MOTD;
		}

		for (pos = 0; pos < MAX_DUPES; pos++)
		{
			if (ch->pcdata->dupes[pos] == NULL)
				break;

			if ((victim = get_char_mortal(ch, str_dup(ch->pcdata->dupes[pos])))
					!= NULL)
				force_quit(victim, "");
		}

		break;

		/* RT code for breaking link */

	case CON_BREAK_CONNECT:
		switch (*argument)
		{
		case 'y':
		case 'Y':
			for (d_old = descriptor_list; d_old != NULL; d_old = d_next)
			{
				d_next = d_old->next;
				if (d_old == d || d_old->character == NULL)
					continue;

				if (str_cmp(ch->name, d_old->original ? d_old->original->name
						: d_old->character->name))
					continue;

				close_socket(d_old);
			}
			ch->pcdata->socket = str_dup(d->host);
			if (check_reconnect(d, ch->name, TRUE))
				return;
			write_to_buffer(d, "Reconnect attempt failed.\n\rName: ", 0);
			if (d->character != NULL)
			{
				free_char(d->character);
				d->character = NULL;
			}
			d->connected = CON_GET_NAME;
			break;

		case 'n':
		case 'N':
			write_to_buffer(d, "Name: ", 0);
			if (d->character != NULL)
			{
				free_char(d->character);
				d->character = NULL;
			}
			d->connected = CON_GET_NAME;
			break;

		default:
			write_to_buffer(d, "Please type Y or N? ", 0);
			break;
		}
		break;

	case CON_CONFIRM_NEW_NAME:
		switch (*argument)
		{
		case 'y':
		case 'Y':
			sprintf(buf, "New character.\n\rGive me a password for %s: %s",
					ch->name, echo_off_str);
			ch->pcdata->socket = str_dup(d->host);
			write_to_buffer(d, buf, 0);
			d->connected = CON_GET_NEW_PASSWORD;
			break;

		case 'n':
		case 'N':
			write_to_buffer(d, "Ok, what IS it, then? ", 0);
			free_char(d->character);
			d->character = NULL;
			d->connected = CON_GET_NAME;
			break;

		default:
			write_to_buffer(d, "Please type Yes or No? ", 0);
			break;
		}
		break;

	case CON_GET_NEW_PASSWORD:
#if defined(unix)
		write_to_buffer(d, "\n\r", 2);
#endif

		if (strlen(argument) < 5)
		{
			write_to_buffer(
					d,
					"Password must be at least five characters long.\n\rPassword: ",
					0);
			return;
		}
		pwdnew = crypt(argument, ch->name);
		for (p = pwdnew; *p != '\0'; p++)
		{
			if (*p == '~')
			{
				write_to_buffer(
						d,
						"New password not acceptable, try again.\n\rPassword: ",
						0);
				return;
			}
		}

		free_string(ch->pcdata->pwd);
		ch->pcdata->pwd = str_dup(pwdnew);
		write_to_buffer(d, "Please retype password: ", 0);
		d->connected = CON_CONFIRM_NEW_PASSWORD;
		break;

	case CON_CONFIRM_NEW_PASSWORD:
#if defined(unix)
		write_to_buffer(d, "\n\r", 2);
#endif

		if (strcmp(crypt(argument, ch->pcdata->pwd), ch->pcdata->pwd))
		{
			write_to_buffer(d, "Passwords don't match.\n\rRetype password: ", 0);
			d->connected = CON_GET_NEW_PASSWORD;
			return;
		}

		ch->pcdata->socket = str_dup(d->host);
		write_to_buffer(d, echo_on_str, 0);
		write_to_buffer(d, "The following races are available:\n\r\n\r", 0);
		write_to_buffer(
				d,
				"Base Race:   Rares: (These races are not selectable upon creation, toggle\n\r",
				0);
		write_to_buffer(
				d,
				"        noevolve later to allow yourself the possibility to change to them)\n\r",
				0);
		pos = 0;
		for (race = 1; race_table[race].name != NULL; race++)
		{
			if (!race_table[race].pc_race)
				break;
			if (race_lookup(pc_race_table[race].rare_race[0]) == 0)
				break;

			sprintf(newbuf, "%-10s (", race_table[race].name);
			write_to_buffer(d, newbuf, 0);
			for (rare = 0; rare < 7; rare++)
			{
				newbuf[0] = '\0';
				if (pc_race_table[race].rare_race[rare] == NULL)
					break;
				sprintf(newbuf, " %s ", pc_race_table[race].rare_race[rare]);
				write_to_buffer(d, newbuf, 0);
				if (rare == 4)
					write_to_buffer(d, "\n\r          ", 0);
			}

			write_to_buffer(d, ")\n\r", 0);

		}
		newbuf[0] = '\0';
		write_to_buffer(d, "\n\r\n\r", 0);
		write_to_buffer(d, "What is your race (help for more information)? ", 0);
		d->connected = CON_GET_NEW_RACE;
		break;

	case CON_GET_NEW_RACE:
		one_argument(argument, arg);

		if (!strcmp(arg, "help"))
		{
			argument = one_argument(argument, arg);
			if (argument[0] == '\0')
				do_help(ch, "race help");
			else
				do_help(ch, argument);
			write_to_buffer(d,
					"What is your race (help for more information)? ", 0);
			break;
		}

		race = race_lookup(argument);

		if (race == 0 || !race_table[race].pc_race || race_lookup(
				pc_race_table[race].rare_race[0]) == 0)
		{
			write_to_buffer(d, "That is not a valid race.\n\r", 0);
			write_to_buffer(d, "The following races are available:\n\r  ", 0);
			write_to_buffer(
					d,
					"Base Race:   Rares: (These races are not selectable upon creation, toggle\n\r",
					0);
			write_to_buffer(
					d,
					"        noevolve later to allow yourself the possibility to change to them)\n\r",
					0);
			for (race = 1; race_table[race].name != NULL; race++)
			{
				if (!race_table[race].pc_race)
					break;
				if (race_lookup(pc_race_table[race].rare_race[0]) == 0)
					break;

				sprintf(newbuf, "%-10s (", race_table[race].name);
				write_to_buffer(d, newbuf, 0);
				for (rare = 0; rare < 7; rare++)
				{
					newbuf[0] = '\0';
					if (pc_race_table[race].rare_race[rare] == NULL)
						break;
					sprintf(newbuf, " %s ", pc_race_table[race].rare_race[rare]);
					write_to_buffer(d, newbuf, 0);
					if (rare == 4)
						write_to_buffer(d, "\n\r          ", 0);
				}

				write_to_buffer(d, ")\n\r", 0);
			}
			write_to_buffer(d, "\n\r", 0);
			write_to_buffer(d,
					"What is your race? (help for more information) ", 0);
			break;
		}

		ch->race = race;
		/* initialize stats */
		for (i = 0; i < MAX_STATS; i++)
			ch->perm_stat[i] = pc_race_table[race].stats[i];
		ch->affected_by = ch->affected_by | race_table[race].aff;
		ch->shielded_by = ch->shielded_by | race_table[race].shd;
		ch->imm_flags = ch->imm_flags | race_table[race].imm;
		ch->res_flags = ch->res_flags | race_table[race].res;
		ch->vuln_flags = ch->vuln_flags | race_table[race].vuln;
		ch->form = race_table[race].form;
		ch->parts = race_table[race].parts;

		/* add skills */
		for (i = 0; i < 5; i++)
		{
			if (pc_race_table[race].skills[i] == NULL)
				break;
			group_add(ch, pc_race_table[race].skills[i], FALSE);
		}
		/* add cost */
		ch->pcdata->points = pc_race_table[race].points;
		ch->size = pc_race_table[race].size;

		write_to_buffer(d, "What is your sex (M/F/N)? ", 0);
		d->connected = CON_GET_NEW_SEX;
		break;

	case CON_GET_NEW_SEX:
		switch (argument[0])
		{
		case 'm':
		case 'M':
			ch->sex = SEX_MALE;
			ch->pcdata->true_sex = SEX_MALE;
			break;
		case 'f':
		case 'F':
			ch->sex = SEX_FEMALE;
			ch->pcdata->true_sex = SEX_FEMALE;
			break;
		case 'n':
		case 'N':
			ch->sex = SEX_NEUTRAL;
			ch->pcdata->true_sex = SEX_NEUTRAL;
			break;
		default:
			write_to_buffer(d, "That's not a sex.\n\rWhat IS your sex? ", 0);
			return;
		}

		write_to_buffer(d, echo_on_str, 0);
		write_to_buffer(d, "The following classes are available:\n\r\n\r", 0);

		if (ch->pcdata->tier == 1)
		{
			for (iClass = 0; iClass < 7; iClass++)
			{
				write_to_buffer(d, "      ", 0);
				write_to_buffer(d, class_table[iClass].name, 0);
				write_to_buffer(d, "\n\r", 1);
			}
		}
		else if (ch->pcdata->tier == 2)
		{
			for (iClass = 7; iClass < 14; iClass++)
			{
				write_to_buffer(d, "      ", 0);
				write_to_buffer(d, class_table[iClass].name, 0);
				write_to_buffer(d, "\n\r", 1);
			}
		}
		else if (ch->pcdata->tier == 3)
		{
			if (ch->class == CLASS_WIZARD)
				temp = CLASS_FORSAKEN;
			else if (ch->class == CLASS_PRIEST)
				temp = CLASS_VOODAN;
			else if (ch->class == CLASS_MERCENARY)
				temp = CLASS_ASSASSIN;
			else if (ch->class == CLASS_GLADIATOR)
				temp = CLASS_BARBARIAN;
			else if (ch->class == CLASS_STRIDER)
				temp = CLASS_DARKPALADIN;
			else if (ch->class == CLASS_SAGE)
				temp = CLASS_WARLOCK;
			else if (ch->class == CLASS_LICH)
				temp = CLASS_FADE;
			else
				temp = ch->class; // This if already 3rd tier

			if (ch->class < CLASS_FORSAKEN) // Still second tier class
			{
				// Debug Info
				sprintf(buf, "2nd tier class listing -- Class: %d Temp: %d",
						ch->class, temp);
				wiznet(buf, NULL, NULL, WIZ_DEBUG, 0, 0);

				for (iClass = temp; iClass < (temp + 3); iClass++)
				{
					write_to_buffer(d, "      ", 0);
					write_to_buffer(d, class_table[iClass].name, 0);
					write_to_buffer(d, "\n\r", 0);
				}
			}
			else if (ch->class >= CLASS_FORSAKEN) // ALready 3rd tier
			{
				if (ch->class < CLASS_VOODAN)
					temp = CLASS_FORSAKEN;
				else if (ch->class < CLASS_ASSASSIN && ch->class
						> CLASS_ARCHMAGE)
					temp = CLASS_VOODAN;
				else if (ch->class < CLASS_BARBARIAN && ch->class
						> CLASS_SAINT)
					temp = CLASS_ASSASSIN;
				else if (ch->class < CLASS_DARKPALADIN && ch->class
						> CLASS_BARD)
					temp = CLASS_BARBARIAN;
				else if (ch->class < CLASS_WARLOCK && ch->class
						> CLASS_CRUSADER)
					temp = CLASS_DARKPALADIN;
				else if (ch->class < CLASS_FADE && ch->class > CLASS_PALADIN)
					temp = CLASS_WARLOCK;
				else if (ch->class > CLASS_SHAMAN)
					temp = CLASS_FADE;
				else
				{
					temp = ch->class; // This Shouldn't happen;
					sprintf(buf, "BUG in 3rd tier class transfer: Class: %d",
							temp);
					wiznet(buf, NULL, NULL, WIZ_DEBUG, 0, 0);
				}
				// Debug Info
				sprintf(buf, "3rd tier class listing -- Class: %d Temp: %d",
						ch->class, temp);
				wiznet(buf, NULL, NULL, WIZ_DEBUG, 0, 0);

				for (iClass = temp; iClass < (temp + 3); iClass++)
				{
					if (NOMONKS && iClass == CLASS_MONK)
					{
						continue;
					}
					else if (NONINJA && iClass == CLASS_NINJA)
					{
						continue;
					}
					else
					{
						write_to_buffer(d, "      ", 0);
						write_to_buffer(d, class_table[iClass].name, 0);
						write_to_buffer(d, "\n\r", 0);

					}
				}
			}
		}
		write_to_buffer(d, "\n\r\n\r", 0);
		write_to_buffer(d, "What is your class ? ", 0);
		d->connected = CON_GET_NEW_CLASS;
		break;

	case CON_GET_NEW_CLASS:
		one_argument(argument, arg);
		if (!strcmp(arg, "help"))
		{
			argument = one_argument(argument, arg);
			if (argument[0] == '\0')
				do_help(ch, "tier");
			else
				do_help(ch, argument);
			write_to_buffer(
					d,
					"What do you want your class to be? (type help for more info)",
					0);
			break;
		}

		if (!strcmp(arg, "showclass"))
		{
			argument = one_argument(argument, arg);
			if (argument[0] == '\0')
				do_help(ch, "showclass");
			else
				do_showclass(ch, argument);
			write_to_buffer(d, "What do you want your class to be?", 0);
			break;
		}

		iClass = class_lookup(argument);

		if (iClass == -1)
		{
			do_help(ch, "showclass");
			write_to_buffer(d, "That's not a class.\n\r", 0);
			write_to_buffer(d, echo_on_str, 0);
			write_to_buffer(d, "The following classes are available:\n\r\n\r",
					0);

			if (ch->pcdata->tier == 1)
			{
				for (iClass = 0; iClass < 7; iClass++)
				{
					write_to_buffer(d, "      ", 0);
					write_to_buffer(d, class_table[iClass].name, 0);
					write_to_buffer(d, "\n\r", 0);
				}
			}
			else if (ch->pcdata->tier == 2)
			{
				for (iClass = 7; iClass < 14; iClass++)
				{
					write_to_buffer(d, "      ", 0);
					write_to_buffer(d, class_table[iClass].name, 0);
					write_to_buffer(d, "\n\r", 0);
				}
			}
			else if (ch->pcdata->tier == 3)
			{
				if (ch->class == CLASS_WIZARD)
					temp = CLASS_FORSAKEN;
				else if (ch->class == CLASS_PRIEST)
					temp = CLASS_VOODAN;
				else if (ch->class == CLASS_MERCENARY)
					temp = CLASS_ASSASSIN;
				else if (ch->class == CLASS_GLADIATOR)
					temp = CLASS_BARBARIAN;
				else if (ch->class == CLASS_STRIDER)
					temp = CLASS_DARKPALADIN;
				else if (ch->class == CLASS_SAGE)
					temp = CLASS_WARLOCK;
				else if (ch->class == CLASS_LICH)
					temp = CLASS_FADE;
				else
					temp = ch->class; // This if already 3rd tier

				if (ch->class < CLASS_FORSAKEN) // Still second tier class
				{
					// Debug Info
					sprintf(buf,
							"2nd tier class listing -- Class: %d Temp: %d",
							ch->class, temp);
					wiznet(buf, NULL, NULL, WIZ_DEBUG, 0, 0);

					for (iClass = temp; iClass < (temp + 3); iClass++)
					{
						write_to_buffer(d, "      ", 0);
						write_to_buffer(d, class_table[iClass].name, 0);
						write_to_buffer(d, "\n\r", 0);
					}
				}
				else if (ch->class >= CLASS_FORSAKEN)
				{
					if (ch->class < CLASS_VOODAN)
						temp = CLASS_FORSAKEN;
					else if (ch->class < CLASS_ASSASSIN && ch->class
							> CLASS_ARCHMAGE)
						temp = CLASS_VOODAN;
					else if (ch->class < CLASS_BARBARIAN && ch->class
							> CLASS_SAINT)
						temp = CLASS_ASSASSIN;
					else if (ch->class < CLASS_DARKPALADIN && ch->class
							> CLASS_BARD)
						temp = CLASS_BARBARIAN;
					else if (ch->class < CLASS_WARLOCK && ch->class
							> CLASS_CRUSADER)
						temp = CLASS_DARKPALADIN;
					else if (ch->class < CLASS_FADE && ch->class
							> CLASS_PALADIN)
						temp = CLASS_WARLOCK;
					else if (ch->class > CLASS_SHAMAN)
						temp = CLASS_FADE;
					else
					{
						temp = ch->class; // This Shouldn't happen;
						sprintf(buf,
								"BUG in 3rd tier class transfer: Class: %d",
								temp);
						wiznet(buf, NULL, NULL, WIZ_DEBUG, 0, 0);
					}

					// Debug Info
					sprintf(buf,
							"3rd tier class listing -- Class: %d Temp: %d",
							ch->class, temp);
					wiznet(buf, NULL, NULL, WIZ_DEBUG, 0, 0);

					for (iClass = temp; iClass < (temp + 3); iClass++)
					{
						if (NOMONKS && iClass == CLASS_MONK)
						{
							continue;
						}
						else if (NONINJA && iClass == CLASS_NINJA)
						{
							continue;
						}
						else
						{
							write_to_buffer(d, "      ", 0);
							write_to_buffer(d, class_table[iClass].name, 0);
							write_to_buffer(d, "\n\r", 0);

						}
					}
				}
			}

			write_to_buffer(d, "\n\r\n\r", 0);
			write_to_buffer(d, "What is your class ? ", 0);
			d->connected = CON_GET_NEW_CLASS;
			break;

		}

		// Debug Info
		sprintf(buf, "Tier: %d iClass selected: %d", ch->pcdata->tier, iClass);
		wiznet(buf, NULL, NULL, WIZ_DEBUG, 0, 0);

		if (ch->pcdata->tier == 1 && iClass >= 7)
		{
			write_to_buffer(d,
					"That's not a first tier class.\n\rWhat IS your class? ", 0);
			return;
		}
		else if (ch->pcdata->tier == 2 && (iClass < 7 || iClass > 14))
		{
			write_to_buffer(d,
					"That's not a second tier class.\n\rWhat IS your class? ",
					0);
			return;
		}
		else if (ch->pcdata->tier >= 3 && (iClass < 14))
		{
			write_to_buffer(d,
					"That's not a third tier class.\n\rWhat IS your class? ", 0);
			return;
		}

		if (ch->pcdata->tier >= 3 && (iClass > 13))
		{
			if (ch->class == CLASS_WIZARD)
				temp = CLASS_FORSAKEN;
			else if (ch->class == CLASS_PRIEST)
				temp = CLASS_VOODAN;
			else if (ch->class == CLASS_MERCENARY)
				temp = CLASS_ASSASSIN;
			else if (ch->class == CLASS_GLADIATOR)
				temp = CLASS_BARBARIAN;
			else if (ch->class == CLASS_STRIDER)
				temp = CLASS_DARKPALADIN;
			else if (ch->class == CLASS_SAGE)
				temp = CLASS_WARLOCK;
			else if (ch->class == CLASS_LICH)
				temp = CLASS_FADE;
			else
				temp = ch->class; // This if already 3rd tier

			if (temp < CLASS_VOODAN)
				temp = CLASS_FORSAKEN;
			else if (temp < CLASS_ASSASSIN && temp > CLASS_ARCHMAGE)
				temp = CLASS_VOODAN;
			else if (temp < CLASS_BARBARIAN && temp > CLASS_SAINT)
				temp = CLASS_ASSASSIN;
			else if (temp < CLASS_DARKPALADIN && temp > CLASS_BARD)
				temp = CLASS_BARBARIAN;
			else if (temp < CLASS_WARLOCK && temp > CLASS_CRUSADER)
				temp = CLASS_DARKPALADIN;
			else if (temp < CLASS_FADE && temp > CLASS_PALADIN)
				temp = CLASS_WARLOCK;
			else if (temp > CLASS_SHAMAN)
				temp = CLASS_FADE;
			else
			{
				temp = ch->class; // This Shouldn't happen;
				sprintf(buf, "BUG in 3rd tier class transfer: Class: %d", temp);
				wiznet(buf, NULL, NULL, WIZ_DEBUG, 0, 0);
			}

			// Debug Info
			sprintf(buf, "3rd tier class choice -- iClass: %d Temp: %d",
					iClass, temp);
			wiznet(buf, NULL, NULL, WIZ_DEBUG, 0, 0);

			if (temp == CLASS_FORSAKEN)
			{
				if (iClass < CLASS_FORSAKEN || iClass > CLASS_ARCHMAGE)
				{
					write_to_buffer(d,
							"That is not a choice for your class.\n\r", 0);
					write_to_buffer(d, "What is your class?\n\r", 0);
					d->connected = CON_GET_NEW_CLASS;
					break;
				}
			}

			if (temp == CLASS_VOODAN)
			{
				if (iClass < CLASS_VOODAN || iClass > CLASS_SAINT)
				{
					write_to_buffer(d,
							"That is not a choice for your class.\n\r", 0);
					write_to_buffer(d, "What is your class?\n\r", 0);
					d->connected = CON_GET_NEW_CLASS;
					break;
				}
			}

			if (temp == CLASS_ASSASSIN)
			{
				if (iClass < CLASS_ASSASSIN || iClass > CLASS_BARD)
				{
					write_to_buffer(d,
							"That is not a choice for your class.\n\r", 0);
					write_to_buffer(d, "What is your class?\n\r", 0);
					d->connected = CON_GET_NEW_CLASS;
					break;
				}
			}

			if (temp == CLASS_BARBARIAN)
			{
				if (iClass < CLASS_BARBARIAN || iClass > CLASS_CRUSADER)
				{
					write_to_buffer(d,
							"That is not a choice for your class.\n\r", 0);
					write_to_buffer(d, "What is your class?\n\r", 0);
					d->connected = CON_GET_NEW_CLASS;
					break;
				}
			}

			if (temp == CLASS_DARKPALADIN)
			{
				if (iClass < CLASS_DARKPALADIN || iClass > CLASS_PALADIN)
				{
					write_to_buffer(d,
							"That is not a choice for your class.\n\r", 0);
					write_to_buffer(d, "What is your class?\n\r", 0);
					d->connected = CON_GET_NEW_CLASS;
					break;
				}
			}

			if (temp == CLASS_WARLOCK)
			{
				if (iClass < CLASS_WARLOCK || iClass > CLASS_SHAMAN)
				{
					write_to_buffer(d,
							"That is not a choice for your class.\n\r", 0);
					write_to_buffer(d, "What is your class?\n\r", 0);
					d->connected = CON_GET_NEW_CLASS;
					break;
				}
			}

			if (temp == CLASS_FADE)
			{
				if (iClass < CLASS_FADE || iClass > CLASS_BANSHEE)
				{
					write_to_buffer(d,
							"That is not a choice for your class.\n\r", 0);
					write_to_buffer(d, "What is your class?\n\r", 0);
					d->connected = CON_GET_NEW_CLASS;
					break;
				}
			}
		}
		ch->class = iClass;

		sprintf(log_buf, "%s@%s new player.", ch->name, d->host);
		printf_system(log_buf);
		wiznet("Newbie alert!  $N sighted.", ch, NULL, WIZ_NEWBIE, 0, 0);
		wiznet(log_buf, NULL, NULL, WIZ_SITES, 0, get_trust(ch));

		write_to_buffer(d, "\n\r", 2);
		write_to_buffer(d, "You may be good, neutral, or evil.\n\r", 0);
		write_to_buffer(d, "Which alignment (G/N/E)? ", 0);
		d->connected = CON_GET_ALIGNMENT;
		break;

	case CON_GET_ALIGNMENT:
		switch (argument[0])
		{
		case 'g':
		case 'G':
			ch->alignment = 750;
			break;
		case 'n':
		case 'N':
			ch->alignment = 0;
			break;
		case 'e':
		case 'E':
			ch->alignment = -750;
			break;
		default:
			write_to_buffer(d, "That's not a valid alignment.\n\r", 0);
			write_to_buffer(d, "Which alignment (G/N/E)? ", 0);
			return;
		}
		SET_BIT (ch->act, PLR_AUTOGOLD);
		SET_BIT (ch->act, PLR_AUTOLOOT);
		SET_BIT (ch->act, PLR_AUTOSAC);
		SET_BIT (ch->act, PLR_AUTOEXIT);
		write_to_buffer(d, "\n\r", 0);
		/* diety code */
		write_to_buffer(d, "The following gods are available:\n\r  ", 0);
		for (god = 0; god < MAX_GOD; god++)
		{
			if ((god_table[god].pc_evil && ch->alignment == -750)
					|| (god_table[god].pc_neutral && ch->alignment == 0)
					|| (god_table[god].pc_good && ch->alignment == 750))
			{
				if ((god_table[god].pc_mage && ch->class == 0)
						|| (god_table[god].pc_cleric && ch->class == 1)
						|| (god_table[god].pc_thief && ch->class == 2)
						|| (god_table[god].pc_warrior && ch->class == 3)
						|| (god_table[god].pc_ranger && ch->class == 4)
						|| (god_table[god].pc_druid && ch->class == 5)
						|| (god_table[god].pc_vampire && ch->class == 6)
						|| (god_table[god].pc_wizard && ch->class == 7)
						|| (god_table[god].pc_priest && ch->class == 8)
						|| (god_table[god].pc_mercenary && ch->class == 9)
						|| (god_table[god].pc_gladiator && ch->class == 10)
						|| (god_table[god].pc_strider && ch->class == 11)
						|| (god_table[god].pc_sage && ch->class == 12)
						|| (god_table[god].pc_lich && ch->class == 13)
						|| (god_table[god].pc_forsaken && ch->class == 14)
						|| (god_table[god].pc_conjurer && ch->class == 15)
						|| (god_table[god].pc_archmage && ch->class == 16)
						|| (god_table[god].pc_voodan && ch->class == 17)
						|| (god_table[god].pc_monk && ch->class == 18)
						|| (god_table[god].pc_saint && ch->class == 19)
						|| (god_table[god].pc_assassin && ch->class == 20)
						|| (god_table[god].pc_ninja && ch->class == 21)
						|| (god_table[god].pc_bard && ch->class == 22)
						|| (god_table[god].pc_barbarian && ch->class == 23)
						|| (god_table[god].pc_swashbuckler && ch->class == 24)
						|| (god_table[god].pc_crusader && ch->class == 25)
						|| (god_table[god].pc_darkpaladin && ch->class == 26)
						|| (god_table[god].pc_hunter && ch->class == 27)
						|| (god_table[god].pc_paladin && ch->class == 28)
						|| (god_table[god].pc_warlock && ch->class == 29)
						|| (god_table[god].pc_alchemist && ch->class == 30)
						|| (god_table[god].pc_shaman && ch->class == 31)
						|| (god_table[god].pc_fade && ch->class == 32)
						|| (god_table[god].pc_necromancer && ch->class == 33)
						|| (god_table[god].pc_banshee && ch->class == 34))
				{
					write_to_buffer(d, god_table[god].name, 0);
					write_to_buffer(d, " ", 1);
				}
			}
		}
		write_to_buffer(d, "\n\r", 0);
		write_to_buffer(d,
				"Who do you want to worship (help for more information)? ", 0);
		d->connected = CON_GET_GOD;
		break;

	case CON_GET_GOD:
		one_argument(argument, arg);

		if (!strcmp(arg, "help"))
		{
			argument = one_argument(argument, arg);
			if (argument[0] == '\0')
				do_help(ch, "gods");
			else
				do_help(ch, argument);
			write_to_buffer(d,
					"Who do you want to worship (help for more information)? ",
					0);
			break;
		}
		god = god_lookup(argument);

		if (god == 0 || (!god_table[god].pc_evil && ch->alignment == -750)
				|| (!god_table[god].pc_neutral && ch->alignment == 0)
				|| (!god_table[god].pc_good && ch->alignment == 750)
				|| (!god_table[god].pc_mage && ch->class == 0)
				|| (!god_table[god].pc_cleric && ch->class == 1)
				|| (!god_table[god].pc_thief && ch->class == 2)
				|| (!god_table[god].pc_warrior && ch->class == 3)
				|| (!god_table[god].pc_ranger && ch->class == 4)
				|| (!god_table[god].pc_druid && ch->class == 5)
				|| (!god_table[god].pc_vampire && ch->class == 6)
				|| (!god_table[god].pc_wizard && ch->class == 7)
				|| (!god_table[god].pc_priest && ch->class == 8)
				|| (!god_table[god].pc_mercenary && ch->class == 9)
				|| (!god_table[god].pc_gladiator && ch->class == 10)
				|| (!god_table[god].pc_strider && ch->class == 11)
				|| (!god_table[god].pc_sage && ch->class == 12)
				|| (!god_table[god].pc_lich && ch->class == 13)
				|| (!god_table[god].pc_forsaken && ch->class == 14)
				|| (!god_table[god].pc_conjurer && ch->class == 15)
				|| (!god_table[god].pc_archmage && ch->class == 16)
				|| (!god_table[god].pc_voodan && ch->class == 17)
				|| (!god_table[god].pc_monk && ch->class == 18)
				|| (!god_table[god].pc_saint && ch->class == 19)
				|| (!god_table[god].pc_assassin && ch->class == 20)
				|| (!god_table[god].pc_ninja && ch->class == 21)
				|| (!god_table[god].pc_bard && ch->class == 22)
				|| (!god_table[god].pc_barbarian && ch->class == 23)
				|| (!god_table[god].pc_swashbuckler && ch->class == 24)
				|| (!god_table[god].pc_crusader && ch->class == 25)
				|| (!god_table[god].pc_darkpaladin && ch->class == 26)
				|| (!god_table[god].pc_hunter && ch->class == 27)
				|| (!god_table[god].pc_paladin && ch->class == 28)
				|| (!god_table[god].pc_warlock && ch->class == 29)
				|| (!god_table[god].pc_alchemist && ch->class == 30)
				|| (!god_table[god].pc_shaman && ch->class == 31)
				|| (!god_table[god].pc_fade && ch->class == 32)
				|| (!god_table[god].pc_necromancer && ch->class == 33)
				|| (!god_table[god].pc_banshee && ch->class == 34))
		{
			write_to_buffer(d, "That is not a valid god.\n\r", 0);
			write_to_buffer(d, "The following gods are available:\n\r", 0);
			for (god = 0; god < MAX_GOD; god++)
			{
				if ((god_table[god].pc_evil && ch->alignment == -750)
						|| (god_table[god].pc_neutral && ch->alignment == 0)
						|| (god_table[god].pc_good && ch->alignment == 750))
				{
					if ((god_table[god].pc_mage && ch->class == 0)
							|| (god_table[god].pc_cleric && ch->class == 1)
							|| (god_table[god].pc_thief && ch->class == 2)
							|| (god_table[god].pc_warrior && ch->class == 3)
							|| (!god_table[god].pc_ranger && ch->class == 4)
							|| (!god_table[god].pc_druid && ch->class == 5)
							|| (!god_table[god].pc_vampire && ch->class == 6)
							|| (!god_table[god].pc_wizard && ch->class == 7)
							|| (!god_table[god].pc_priest && ch->class == 8)
							|| (!god_table[god].pc_mercenary && ch->class == 9)
							|| (!god_table[god].pc_gladiator && ch->class == 10)
							|| (!god_table[god].pc_strider && ch->class == 11)
							|| (!god_table[god].pc_sage && ch->class == 12)
							|| (!god_table[god].pc_lich && ch->class == 13)
							|| (!god_table[god].pc_forsaken && ch->class == 14)
							|| (!god_table[god].pc_conjurer && ch->class == 15)
							|| (!god_table[god].pc_archmage && ch->class == 16)
							|| (!god_table[god].pc_voodan && ch->class == 17)
							|| (!god_table[god].pc_monk && ch->class == 18)
							|| (!god_table[god].pc_saint && ch->class == 19)
							|| (!god_table[god].pc_assassin && ch->class == 20)
							|| (!god_table[god].pc_ninja && ch->class == 21)
							|| (!god_table[god].pc_bard && ch->class == 22)
							|| (!god_table[god].pc_barbarian && ch->class
									== 23) || (!god_table[god].pc_swashbuckler
							&& ch->class == 24) || (!god_table[god].pc_crusader
							&& ch->class == 25)
							|| (!god_table[god].pc_darkpaladin && ch->class
									== 26) || (!god_table[god].pc_hunter
							&& ch->class == 27) || (!god_table[god].pc_paladin
							&& ch->class == 28) || (!god_table[god].pc_warlock
							&& ch->class == 29)
							|| (!god_table[god].pc_alchemist && ch->class == 30)
							|| (!god_table[god].pc_shaman && ch->class == 31)
							|| (!god_table[god].pc_fade && ch->class == 32)
							|| (!god_table[god].pc_necromancer && ch->class
									== 33) || (!god_table[god].pc_banshee
							&& ch->class == 34))
					{
						write_to_buffer(d, god_table[god].name, 0);
						write_to_buffer(d, " ", 1);
					}
				}
			}
			write_to_buffer(d, "\n\r", 0);
			write_to_buffer(d,
					"Who do you want to worship (help for more information)?",
					0);
			break;
		}

		ch->god = god;

		write_to_buffer(d, "\n\r", 0);

		group_add(ch, "rom basics", FALSE);
		group_add(ch, class_table[ch->class].base_group, FALSE);
		ch->pcdata->learned[gsn_recall] = 100;
		write_to_buffer(d, "Do you wish to customize this character?\n\r", 0);
		write_to_buffer(
				d,
				"Customization takes time, but allows a wider range of skills and abilities.\n\r",
				0);
		write_to_buffer(d, "Customize (Y/N)? ", 0);
		d->connected = CON_DEFAULT_CHOICE;
		break;

	case CON_DEFAULT_CHOICE:
		write_to_buffer(d, "\n\r", 2);
		switch (argument[0])
		{
		case 'y':
		case 'Y':
			ch->gen_data = new_gen_data();
			ch->gen_data->points_chosen = ch->pcdata->points;
			do_help(ch, "group header");
			list_group_costs(ch);
			write_to_buffer(d, "You already have the following skills:\n\r", 0);
			do_skills(ch, "");
			do_help(ch, "menu choice");
			d->connected = CON_GEN_GROUPS;
			break;
		case 'n':
		case 'N':
			write_to_buffer(d, "Customization is manditory now (Pick Yes) ", 0);
			return;
		default:
			write_to_buffer(d, "Please answer (Y/N)? ", 0);
			return;
		}
		break;

	case CON_PICK_WEAPON:
		write_to_buffer(d, "\n\r", 2);
		weapon = weapon_lookup(argument);
		if ((weapon == -1 || ch->pcdata->learned[*weapon_table[weapon].gsn]
				<= 0) && str_cmp(argument, "none"))
		{
			write_to_buffer(d,
					"That's not a valid selection. Choices are:\n\r", 0);
			buf[0] = '\0';
			for (i = 0; weapon_table[i].name != NULL; i++)
				if (ch->pcdata->learned[*weapon_table[i].gsn] > 0)
				{
					strcat(buf, weapon_table[i].name);
					strcat(buf, " ");
				}
			strcat(buf, "\n\rYour choice? ");
			write_to_buffer(d, buf, 0);
			return;
		}
		if (str_cmp(argument, "none"))
			ch->pcdata->learned[*weapon_table[weapon].gsn] = 40;
		write_to_buffer(d, "\n\r", 2);
		do_help(ch, "motd");
		d->connected = CON_READ_MOTD;
		break;

	case CON_GEN_GROUPS:
		send_to_char("\n\r", ch);
		if (!str_cmp(argument, "done"))
		{
			sprintf(buf, "Creation points: %d\n\r", ch->pcdata->points);
			send_to_char(buf, ch);
			sprintf(buf, "Experience per level: %ld\n\r", (long) exp_per_level(
					ch, ch->gen_data->points_chosen));
			if (ch->pcdata->points < 40)
				ch->train = (40 - ch->pcdata->points + 1) / 2;
			free_gen_data(ch->gen_data);
			ch->gen_data = NULL;
			send_to_char(buf, ch);
			write_to_buffer(d, "\n\r", 2);
			write_to_buffer(d,
					"Please pick a weapon from the following choices:\n\r", 0);
			buf[0] = '\0';
			for (i = 0; weapon_table[i].name != NULL; i++)
				if (ch->pcdata->learned[*weapon_table[i].gsn] > 0)
				{
					found = TRUE;
					strcat(buf, weapon_table[i].name);
					strcat(buf, " ");
				}
			if (found != TRUE)
				strcat(buf, "none");
			strcat(buf, "\n\rYour choice? ");
			write_to_buffer(d, buf, 0);
			d->connected = CON_PICK_WEAPON;
			break;
		}

		if (!parse_gen_groups(ch, argument))
			send_to_char(
					"Choices are: list,learned,premise,add,drop,info,help, and done.\n\r",
					ch);

		do_help(ch, "menu choice");
		break;

	case CON_READ_IMOTD:
		write_to_buffer(d, "\n\r", 2);
		do_help(ch, "motd");
		d->connected = CON_READ_MOTD;
		break;

	case CON_READ_MOTD:
		// Nuke doppelgangers.
		for (dDup = descriptor_list; dDup != NULL; dDup = dDup_next) {
			dDup_next = dDup->next;
			if (dDup == d || dDup->character == NULL
			||  str_cmp(dDup->character->name, ch->name))
				continue;
			close_socket(dDup);
		}
		if (ch->pcdata == NULL || ch->pcdata->pwd[0] == '\0')
		{
			write_to_buffer(d, "Warning! Null password!\n\r", 0);
			write_to_buffer(d, "Please report old password with bug.\n\r", 0);
			write_to_buffer(d,
					"Type 'password null <new password>' to fix.\n\r", 0);
		}

		if (ch->level < MAX_LEVEL - 8)
		{
			sprintf(buf, "%s has left real life behind", ch->name);
			do_announce(ch, buf);
		}

		if (ch->level == 0)
		{
			sprintf(buf,
					"Welcome to "GAME_COL_NAME"{Y %s!",
					ch->name);
			do_announce(ch, buf);
		}

		write_to_buffer(d, "\n\rWelcome to "GAME_NAME". (ROT 1.4)\n\r",
				0);
		ch->next = char_list;
		char_list = ch;
		d->connected = CON_PLAYING;
		reset_char(ch);
//		update_roster( ch, FALSE );

		if (ch->level == 0)
		{

			//     ch->perm_stat[class_table[ch->class].attr_prime] += 3;

			/* add this while initializing all the racial stuff for new characters */
			ch->pcdata->plr_wager = 0; /* arena betting amount */
			ch->gladiator = NULL; /* set player to bet on to NULL */
			ch->level = 1;
			ch->exp = exp_per_level(ch, ch->pcdata->points);
			ch->hit = ch->max_hit;
			ch->mana = ch->max_mana;
			ch->move = ch->max_move;
			ch->train = 23;
			ch->practice = 25;
			sprintf(buf, "the %s", title_table[ch->class][ch->level][ch->sex
					== SEX_FEMALE ? 1 : 0]);
			set_title(ch, buf);
			ch->pcdata->usr_ttl = FALSE;

			for (sn = 0; sn < MAX_SKILL; sn++)
				ch->pcdata->learnlvl[sn]
						= skill_table[sn].skill_level[ch->class];

			if (ch->class == CLASS_WARRIOR || ch->class == CLASS_GLADIATOR
					|| ch->class == CLASS_CRUSADER || ch->class
					== CLASS_SWASHBUCKLER || ch->class == CLASS_BARBARIAN)
				SET_BIT(ch->plyr,PLAYER_WARRIOR);

			if (ch->class == CLASS_THIEF || ch->class == CLASS_MERCENARY
					|| ch->class == CLASS_ASSASSIN || ch->class == CLASS_NINJA
					|| ch->class == CLASS_BARD)
				SET_BIT(ch->plyr,PLAYER_THIEF);

			if (ch->race == race_lookup("grey-elf")
					&& IS_SET(ch->shielded_by,SHD_SANCTUARY))
			{
				REMOVE_BIT(ch->shielded_by,SHD_SANCTUARY);
			}

			SET_BIT(ch->act, PLR_AUTOEXIT|PLR_AUTOLOOT|PLR_AUTOSAC|PLR_AUTOGOLD|PLR_AUTOPEEK);
		//	SET_BIT(ch->plyr, PLAYER_NEWLOOK);

			do_outfit(ch, "");
			gcast_char(NULL, ch, 0);
			obj_to_char(create_object(get_obj_index(OBJ_VNUM_MAP), 0), ch);
			obj_to_char(create_object(get_obj_index(OBJ_VNUM_WMAP), 0), ch);

			char_to_room(ch, get_room_index(ROOM_VNUM_SCHOOL));
			send_to_char("\n\r", ch);

			send_to_char("New look with minimap enabled.\r\nIf you want to disable it, type \"newlook\"\r\n", ch);
			send_to_char("To enable ANSI colour, type \"colour\".\r\n", ch);
		}
		else if (ch->in_room != NULL)
		{
			char_to_room(ch, ch->in_room);
		}
		else if (IS_IMMORTAL(ch))
		{
			char_to_room(ch, get_room_index(ROOM_VNUM_CHAT));
		}
		else
		{
			if (ch->alignment < 0)
				char_to_room(ch, get_room_index(ROOM_VNUM_TEMPLEB));
			else
				char_to_room(ch, get_room_index(ROOM_VNUM_TEMPLE));
		}

		if (!IS_IMMORTAL(ch))
		{
			act("$n has entered the game.", ch, NULL, NULL, TO_ROOM );
		}

		do_look(ch, "auto");

		wiznet("{^$N{V has left real life behind.", ch, NULL, WIZ_LOGINS,
				WIZ_SITES, get_trust(ch));

		if (ch->pet != NULL)
		{
			char_to_room(ch->pet, ch->in_room);
			act("$n has entered the game.", ch->pet, NULL, NULL, TO_ROOM);
		}

		do_unread(ch, "");

		ROSTER_DATA *roster;
		GET_ROSTER(ch, roster);
		/*if (ch->class == CLASS_BARBARIAN && roster) {
			ch->pcdata->power[POWER_POINTS] = roster->pkilldata->kPoints;
			update_power(ch);
		}*/

		ch->imm_flags = race_table[race].imm;
		ch->res_flags = race_table[race].res;
		ch->vuln_flags = race_table[race].vuln;

		if (!IS_IMMORTAL(ch) && ch->wiznet != 0)
			ch->wiznet = 0;

		/*
		 * Add or Remove Skills based on Changes
		 */
		counter = 0;
		for (temp = 0; temp < MAX_SKILL; temp++)
		{
			/* Remove Skills that CLASS doesn't have */
			if (has_skill(ch, temp) && !class_has_skill(ch->class, temp))
			{
				lose_skill(ch, temp);
				counter -= 1;
			}
			/* Add skills to Gain List */
			/*if(temp > 150){
			 sprintf(buf,"Skill: %s",skill_table[temp].name);
			 wiznet(buf,NULL,NULL,WIZ_DEBUG,0,0);

			 if(class_has_skill(ch->class,temp))
			 wiznet("Class has the skill",NULL,NULL,WIZ_DEBUG,0,0);
			 if(!has_learned_skill(ch,temp))
			 wiznet("Player doesn't have skill",NULL,NULL,WIZ_DEBUG,0,0);
			 if(!has_gain(ch,temp))
			 wiznet("Player doesn't have gain",NULL,NULL,WIZ_DEBUG,0,0);
			 }*/

			if (class_has_skill(ch->class, temp)
					&& !has_learned_skill(ch, temp) && !has_gain(ch, temp))
			{
				gain_skill_gain(ch, temp);
				counter += 1;
			}
			/* If They Already have it in their list */
			//         else if(class_has_skill(ch->class,temp) &&
			//         !has_skill(ch,temp) && has_gain(ch,temp))
			//          gain_skill(ch,temp);
		}

		if (counter > 0) // More gains than loses
		{
			if (ch->train < counter * 3)
				ch->train += counter * 3;
			if (ch->practice < counter * 3)
				ch->practice += counter * 3;
		}

		if (ch->class == CLASS_WARRIOR || ch->class == CLASS_GLADIATOR
				|| ch->class == CLASS_CRUSADER || ch->class
				== CLASS_SWASHBUCKLER || ch->class == CLASS_BARBARIAN)
			SET_BIT(ch->plyr,PLAYER_WARRIOR);

		if (ch->class == CLASS_THIEF || ch->class == CLASS_MERCENARY
				|| ch->class == CLASS_ASSASSIN || ch->class == CLASS_NINJA
				|| ch->class == CLASS_BARD)
			SET_BIT(ch->plyr,PLAYER_THIEF);

		/* Stance Resets */
		clear_stance_affects(ch);
		ch->stance[0] = -1;
		send_to_char("You relax from your fighting stance.\n\r", ch);
		send_to_char(
				"Please set your fighting stance before going into combat.\n\r",
				ch);

		if (is_affected(ch, gsn_conceal) && ch->morph_form[0] != MORPH_CONCEAL)
		{
			send_to_char(
					"You have never lost your conceal to begin with, hopefuly...",
					ch);
		}

		/* Focus Bug Fix */ // Always run this now to regenerate hp on login. -Marduk
		if (ch->level == LEVEL_HERO
			//	&& !IS_SET(ch->plyr,PLAYER_STAT_FOCUS_FIXED))
			)
		{
			//send_to_char(
			//		"Focus Reset - Fixed.\n\r Please see the news about the matter.\n\r",
			//		ch);
			check_focus(ch);
		}

		if (ch->version < 7) {
			ROSTER_DATA *roster;
			GET_ROSTER(ch, roster);
			ch->pcdata->achievement_metrics[is_pkill(ch) ? ACH_PKKILLS : ACH_AKILLS] = roster->pkilldata->kills;
			ch->version = 7;
		}
		if (ch->version < 8) {
			// Legend update.
			if (ch->pcdata->tier == 3 && ch->level >= LEVEL_HERO)
				ch->pcdata->achievement_metrics[ACH_LGD] = 1;
			ch->version = 8;
		}

		if (ch->version < 9) {
			for (i = 0; i < 4; ++i)
				 ch->pcdata->achievement_metrics[ACH_BALANCE] = UMAX(ch->pcdata->achievement_metrics[ACH_BALANCE], ch->balance[i]);
			ch->version = 9;
		}

		// Update played counter.
		ch->pcdata->achievement_metrics[ACH_HOURS] = ch->played/(60*60);

		// Check for achievements.
		ach_update(ch);

		reset_char(ch);

		break;
	}

	return;
}

/*
 * Parse a name for acceptability.
 */
bool check_parse_name(char *name)
{
	/*
	 * Reserved words.
	 */
	if (is_name(
			name,
			"all auto immortal immortals self someone something the you demise balance circle loner honor"))
		return FALSE;

	/*
	 * Cursing
	 */

	if (get_clan_by_name( name) != NULL )
		return FALSE;
	if (!str_infix("immortal", name))
		return FALSE;
	if (!str_infix(" ", name))
		return FALSE;
	if (!str_infix("fuck", name))
		return FALSE;
	if (!str_infix("shit", name))
		return FALSE;
	if (!str_infix("asshole", name))
		return FALSE;
	if (!str_infix("pussy", name))
		return FALSE;
	if (!str_infix("cunt", name))
		return FALSE;
	if (!str_infix("dick", name))
		return FALSE;
	if (!str_infix("whore", name))
		return FALSE;
	if (!str_infix("bitch", name))
		return FALSE;
	if (!str_infix("suck", name))
		return FALSE;
	if (!str_infix("nigger", name))
		return FALSE;

	/*
	 * Length restrictions.
	 */

	if (strlen(name) < 2)
		return FALSE;

#if defined(MSDOS)
	if ( strlen(name) > 8 )
	return FALSE;
#endif

#if defined(macintosh) || defined(unix)
	if (strlen(name) > 12)
		return FALSE;
#endif

	/*
	 * Alphanumerics only.
	 * Lock out IllIll twits.
	 */
	{
		char *pc;
		bool fIll, adjcaps = FALSE, cleancaps = FALSE;
		int total_caps = 0;

		fIll = TRUE;
		for (pc = name; *pc != '\0'; pc++)
		{
			if (!isalpha(*pc))
				return FALSE;

			if (isupper(*pc)) /* ugly anti-caps hack */
			{
				if (adjcaps)
					cleancaps = TRUE;
				total_caps++;
				adjcaps = TRUE;
			}
			else
				adjcaps = FALSE;

			if (LOWER(*pc) != 'i' && LOWER(*pc) != 'l')
				fIll = FALSE;
		}

		if (fIll)
			return FALSE;

		if (cleancaps || (total_caps > (strlen(name)) / 2 && strlen(name) < 3))
			return FALSE;
	}

	/*
	 * Prevent players from naming themselves after mobs.
	 */
	{
		extern MOB_INDEX_DATA *mob_index_hash[MAX_KEY_HASH];
		MOB_INDEX_DATA *pMobIndex;
		int iHash;

		for (iHash = 0; iHash < MAX_KEY_HASH; iHash++)
		{
			for (pMobIndex = mob_index_hash[iHash]; pMobIndex != NULL; pMobIndex
					= pMobIndex->next)
			{
				if (is_name(name, pMobIndex->player_name))
					return FALSE;
			}
		}
	}

	return TRUE;
}

/*
 * Look for link-dead player to reconnect.
 */
bool check_reconnect(DESCRIPTOR_DATA *d, char *name, bool fConn)
{
	CHAR_DATA *ch;
	char buf[MAX_STRING_LENGTH];

	for (ch = char_list; ch != NULL; ch = ch->next)
	{
		if (!IS_NPC(ch) && (!fConn || ch->desc == NULL) && !str_cmp(
				d->character->name, ch->name))
		{
			if (fConn == FALSE)
			{
				free_string(d->character->pcdata->pwd);
				d->character->pcdata->pwd = str_dup(ch->pcdata->pwd);
			}
			else
			{
				OBJ_DATA *obj;

				free_char(d->character);
				d->character = ch;
				ch->desc = d;
				ch->timer = 0;
				if (ch->tells)
				{
					sprintf(
							buf,
							"Reconnecting.  You have {R%d{x tells waiting.\n\r",
							ch->tells);
					send_to_char(buf, ch);
					send_to_char("Type 'replay' to see tells.\n\r", ch);
				}
				else
				{
					send_to_char(
							"Reconnecting.  You have no tells waiting.\n\r", ch);
				}

				if (!IS_IMMORTAL(ch))
					act("$n has reconnected.", ch, NULL, NULL, TO_ROOM );

				if ((obj = get_eq_char(ch, WEAR_LIGHT)) != NULL
						&& obj->item_type == ITEM_LIGHT && obj->value[2] != 0)
					--ch->in_room->light;

				printf_system("%s@%s logged in (relogin).", ch->name, d->host);
				wiznet("{^$N{V groks the fullness of $S link.", ch, NULL,
						WIZ_LINKS, 0, 0);
				d->connected = CON_PLAYING;
			}
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * Check if already playing.
 */
bool check_playing(DESCRIPTOR_DATA *d, char *name)
{
	DESCRIPTOR_DATA *dold;

	for (dold = descriptor_list; dold; dold = dold->next)
	{
		if (dold != d && dold->character != NULL && dold->connected
				!= CON_GET_NAME && dold->connected != CON_GET_OLD_PASSWORD
				&& !str_cmp(name, dold->original ? dold->original->name
						: dold->character->name))
		{
			write_to_buffer(d, "That character is already playing.\n\r", 0);
			write_to_buffer(d, "Do you wish to connect anyway (Y/N)?", 0);
			d->connected = CON_BREAK_CONNECT;
			return TRUE;
		}
	}

	return FALSE;
}

void stop_idling(CHAR_DATA *ch)
{
	if (ch == NULL || ch->desc == NULL || ch->desc->connected != CON_PLAYING
			|| ch->was_in_room == NULL || ch->in_room != get_room_index(
			ROOM_VNUM_LIMBO))
		return;

	ch->timer = 0;
	char_from_room(ch);
	char_to_room(ch, ch->was_in_room);
	ch->was_in_room = NULL;
	act("$n has returned from the void.", ch, NULL, NULL, TO_ROOM );
	return;
}

/*
 * Write to one char.
 */
void send_to_char_bw(const char *txt, CHAR_DATA *ch)
{
	if (ch == NULL)
		return;

	if (txt != NULL && ch->desc != NULL)
		write_to_buffer(ch->desc, txt, strlen(txt));
	return;
}

char *filter_stuff(const char *orig, char *ch_name) {
	static char buf[MAX_STRING_LENGTH];

	strcpy(buf, orig);
	// TODO: delete
	if (!str_cmp(ch_name, "Natsu") || !str_cmp(ch_name, "Wayyne") || !str_cmp(ch_name, "Ike"))
		return str_replace(buf, "wayyne.dnds.com", "wayyne.dyndns.org");
	else
		return str_replace(buf, "wayyne.dyndns.org", "wayyne.dnds.com");
}

/*
 * Write to one char, new colour version, by Lope.
 */
void send_to_char(char *txt, CHAR_DATA *ch)
{
	const char *point;
	char *point2;
	char buf[MAX_STRING_LENGTH * 4];

	if (ch == NULL)
		return;

	txt = filter_stuff(txt, ch->name);

	buf[0] = '\0';
	point2 = buf;
	if (txt && ch->desc)
	{
		if (IS_SET( ch->act, PLR_COLOUR ))
		{
			for (point = txt; *point; point++)
			{
				if (*point == '{')
				{
					point++;
					strcat(buf, colour(*point, ch));
					for (point2 = buf; *point2; point2++)
						;
					continue;
				}
				*point2 = *point;
				*++point2 = '\0';
			}
			*point2 = '\0';
			write_to_buffer(ch->desc, buf, point2 - buf);
		}
		else
		{
			for (point = txt; *point; point++)
			{
				if (*point == '{')
				{
					point++;
					if (*point != '{')
					{
						continue;
					}
				}
				*point2 = *point;
				*++point2 = '\0';
			}
			*point2 = '\0';
			write_to_buffer(ch->desc, buf, point2 - buf);
		}
	}
	return;
}

void send_to_room(char *txt, CHAR_DATA *ch)
{
	CHAR_DATA *vch;//, *vch_next;

	if (ch == NULL)
		return;

	//for (vch = ch->in_room->people; vch != NULL; vch = vch_next)  {
	//	vch_next = vch->next;
	for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
		send_to_char(txt, vch);

	return;
}

/*
 * Send a page to one char.
 */
void page_to_char_bw(const char *txt, CHAR_DATA *ch)
{
	if (txt == NULL || ch->desc == NULL)

		if (ch->lines == 0)
		{
			send_to_char((char*)txt, ch);
			return;
		}

#if defined(macintosh)
	send_to_char(txt,ch);
#else
	ch->desc->showstr_head = alloc_mem(strlen(txt) + 1);
	strcpy(ch->desc->showstr_head, txt);
	ch->desc->showstr_point = ch->desc->showstr_head;
	show_string(ch->desc, "");
#endif
}

/*
 * Page to one char, new colour version, by Lope.
 */
void page_to_char(const char *txt, CHAR_DATA *ch)
{
	const char *point;
	char *point2;
	char buf[MAX_STRING_LENGTH * 25];

	buf[0] = '\0';
	point2 = buf;

	if (txt && ch->desc)
	{
		if (IS_SET( ch->act, PLR_COLOUR ))
		{
			for (point = txt; *point; point++)
			{
				if (*point == '{')
				{
					point++;
					strcat(buf, colour(*point, ch));
					for (point2 = buf; *point2; point2++)
						;
					continue;
				}
				*point2 = *point;
				*++point2 = '\0';
			}
			*point2 = '\0';
			free_string(ch->desc->showstr_head);
			ch->desc->showstr_head = str_dup(buf);
			ch->desc->showstr_point = ch->desc->showstr_head;
			show_string(ch->desc, "");
		}
		else
		{
			for (point = txt; *point; point++)
			{
				if (*point == '{')
				{
					point++;
					if (*point != '{')
					{
						continue;
					}
				}
				*point2 = *point;
				*++point2 = '\0';
			}
			*point2 = '\0';
			free_string(ch->desc->showstr_head);
			ch->desc->showstr_head = str_dup(buf);
			ch->desc->showstr_point = ch->desc->showstr_head;
			show_string(ch->desc, "");
		}
	}
	return;
}

int get_eol(char *str) {
	int i;
	char c;
	for (i = 0; (c = str[i]) != '\0'; i++) {
		if (c == '\n') {
			if ((c+1) == '\r')
				return i+1;
			break;
		} else if (c == '\r') {
			if ((c+1) == '\n')
				return i+1;
			break;
		}
	}
	return i;
}

/* string pager */
void show_string(struct descriptor_data *d, char *input)
{
	char buffer[8*MAX_STRING_LENGTH+4];
	int show_lines = 0, i, length = 0, lines = 0;
	bool overflow = FALSE;

	// Weird, but it was in the old code.. to exit the [press enter to continue] bit.
	char arg[MAX_INPUT_LENGTH];
	one_argument(input, arg);
	if (arg[0] != '\0') {
		if (d->showstr_head) {
			free_string(d->showstr_head);
			d->showstr_head = 0;
		}
		d->showstr_point = 0;
		return;
	}

	if (d->character)
		show_lines = d->character->lines;

	for (;;) {
		while (*d->showstr_point == '\r') d->showstr_point++;
		for (i = 0; d->showstr_point[i] != '\0' && d->showstr_point[i] != '\n'; i++);
		if ((i + length + 2) < (8*MAX_STRING_LENGTH)) {
			memcpy(buffer+length, d->showstr_point, i);
			length += i;
			d->showstr_point += i;
			if (*d->showstr_point != '\0') {
				buffer[length++] = '\r';
				buffer[length++] = '\n';
				d->showstr_point++;
			}
			lines++;
		} else
			overflow = TRUE;
		if (overflow || d->showstr_point[0] == '\0' || (show_lines > 0 && lines >= show_lines)) {
			buffer[length] = '\0';
			write_to_buffer(d, buffer, length);
			char *chk;
			for (chk = d->showstr_point; *chk != '\0' && isspace(*chk); chk++);
			if (*chk == '\0') {
				if (d->showstr_head) {
					free_string(d->showstr_head);
					d->showstr_head = 0;
				}
				d->showstr_point = 0;
			}
			return;
		}
	}
}

/* quick sex fixer */
void fix_sex(CHAR_DATA *ch)
{
	if (ch->sex < 0 || ch->sex > 2)
		ch->sex = IS_NPC(ch) ? 0 : ch->pcdata->true_sex;
}

void act(const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2,
		int type)
{
	/* to be compatible with older code */
	act_new(format, ch, arg1, arg2, type, POS_RESTING);
}

/*
 * The colour version of the act( ) function, -Lope
 */
void act_new(const char *format, CHAR_DATA *ch, const void *arg1,
		const void *arg2, int type, int min_pos)
{
	static char * const he_she[] =
	{ "it", "he", "she" };
	static char * const him_her[] =
	{ "it", "him", "her" };
	static char * const his_her[] =
	{ "its", "his", "her" };

	CHAR_DATA *to;
	CHAR_DATA *vch = (CHAR_DATA *) arg2;
	OBJ_DATA *obj1 = (OBJ_DATA *) arg1;
	OBJ_DATA *obj2 = (OBJ_DATA *) arg2;
	const char *str;
	char *i;
	char *point;
	char *i2;
	char fixed[MAX_STRING_LENGTH];
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	char fname[MAX_INPUT_LENGTH];
	bool fColour = FALSE;
	DESCRIPTOR_DATA *d;

	if (!format || !*format)
		return;

	if (!ch || !ch->in_room)
		return;

	to = ch->in_room->people;

	/* Must have a victim */
	if (type == TO_WORLD)
	{
		for (d = descriptor_list; d; d = d->next)
		{
			if (d->connected == CON_PLAYING)
			{

				point = buf;
				str = format;
				while (*str)
				{
					if (*str != '$' && *str != '{')
					{
						*point++ = *str++;
						continue;
					}

					i = NULL;
					switch (*str)
					{
					case '$':
						fColour = TRUE;
						++str;
						i = " <@@@> ";
						if (!arg2 && *str >= 'A' && *str <= 'Z' && *str != 'G')
						{
							printf_debug("Act: missing arg2 for code %d.", *str);
							i = " <@@@> ";
						}
						else
						{
							switch (*str)
							{
							default:
								printf_debug("Act: bad code %d.", *str);
								/* LOG WATCH - basically a debug */
								sprintf(buf, "Act: bad code %d.", *str);
								print_log_watch(buf);

								i = " <@@@> ";
								break;

							case 't':
								i = (char *) arg1;
								break;

							case 'T':
								i = (char *) arg2;
								break;

							case 'n':
								//                                i = get_descr_form(ch,to,FALSE);
								i = PERS(ch,to);
								break;

							case 'N':
								//                                i = get_descr_form(vch,to,FALSE);
								i = PERS(vch,to);
								break;

							case 'e':
								i = he_she[URANGE(0, ch ->sex, 2)];
								break;

							case 'E':
								i = he_she[URANGE(0, vch ->sex, 2)];
								break;

							case 'm':
								i = him_her[URANGE(0, ch ->sex, 2)];
								break;

							case 'M':
								i = him_her[URANGE(0, vch ->sex, 2)];
								break;

							case 'o':
								sprintf(buf2, "%s", olc_ed_name(ch));
								i = buf2;
								break;
							case 'O':
								sprintf(buf2, "%s", olc_ed_vnum(ch));
								i = buf2;
								break;

							case 's':
								i = his_her[URANGE(0, ch ->sex, 2)];
								break;

							case 'S':
								i = his_her[URANGE(0, vch ->sex, 2)];
								break;

							case 'p':
								i = can_see_obj(to, obj1) ? obj1->short_descr
										: "something";
								break;

							case 'P':
								i = can_see_obj(to, obj2) ? obj2->short_descr
										: "something";
								break;

							case 'v':
								i = obj1->short_descr;
								break;

							case 'V':
								i = obj2->short_descr;
								break;

							case 'd':
								if (!arg2 || ((char *) arg2)[0] == '\0')
								{
									i = "door";
								}
								else
								{
									one_argument((char *) arg2, fname);
									i = fname;
								}
								break;

							case 'g':
								i = god_table[ch->god].name;
								break;

							case 'c':
								i = (get_clan_by_ch( ch ))->c_name;
								break;

							}
						}
						break;

					case '{':
						fColour = FALSE;
						++str;
						i = NULL;
						if (IS_SET( to->act, PLR_COLOUR ))
						{
							i = colour(*str, to);
						}
						break;

					default:
						fColour = FALSE;
						*point++ = *str++;
						break;
					}

					++str;
					if (fColour && i)
					{
						fixed[0] = '\0';
						i2 = fixed;

						if (IS_SET( to->act, PLR_COLOUR ))
						{
							for (i2 = fixed; *i; i++)
							{
								if (*i == '{')
								{
									if (*(++i) == '\0')
										break;
									strcat(fixed, colour(*i, to));
									for (i2 = fixed; *i2; i2++)
										;
									continue;
								}
								*i2 = *i;
								*++i2 = '\0';
							}
							*i2 = '\0';
							i = &fixed[0];
						}
						else
						{
							for (i2 = fixed; *i; i++)
							{
								if (*i == '{')
								{
									if (*(++i) == '\0')
										break;
									if (*i != '{')
									{
										continue;
									}
								}
								*i2 = *i;
								*++i2 = '\0';
							}
							*i2 = '\0';
							i = &fixed[0];
						}
					}

					if (i)
					{
						while ((*point = *i) != '\0')
						{
							++point;
							++i;
						}
					}
				}

				*point++ = '\n';
				*point++ = '\r';
				*point = '\0';
				buf[0] = UPPER( buf[0] );

				if (d != NULL && d->character != ch && d->character != vch)
					write_to_buffer(d, filter_stuff(buf, d->character->name), 0);

			}
		}
	}

	if (type == TO_VICT)
	{
		if (!vch)
		{
			printf_debug("Act: null vch with TO_VICT.");
			return;
		}

		if (!vch->in_room)
			return;

		to = vch->in_room->people;
	}

	for (; to; to = to->next_in_room)
	{
		if ((!IS_NPC(to) && to->desc == NULL) || (IS_NPC(to)
				&& !HAS_TRIGGER(to, TRIG_ACT)) || to->position < min_pos)
			continue;

		if (type == TO_CHAR && to != ch)
			continue;
		if (type == TO_NONSHORT && ((IS_SET(to->act,PLR_SHORT_COMBAT)) || to
				== ch || to == vch))
			continue;
		if (type == TO_VICT && (to != vch || to == ch))
			continue;
		if (type == TO_ROOM && to == ch)
			continue;
		if (type == TO_NOTVICT && (to == ch || to == vch))
			continue;

		point = buf;
		str = format;
		while (*str)
		{
			if (*str != '$' && *str != '{')
			{
				*point++ = *str++;
				continue;
			}

			i = NULL;
			switch (*str)
			{
			case '$':
				fColour = TRUE;
				++str;
				i = " <@@@> ";
				if (!arg2 && *str >= 'A' && *str <= 'Z' && *str != 'G')
				{
					printf_debug("Act: missing arg2 for code %d.", *str);
					i = " <@@@> ";
				}
				else
				{
					switch (*str)
					{
					default:
						printf_debug("Act: bad code %d.", *str);
						/* LOG WATCH - basically a debug */
						sprintf(buf, "Act: bad code %d.", *str);
						print_log_watch(buf);

						i = " <@@@> ";
						break;

					case 't':
						i = (char *) arg1;
						break;

					case 'T':
						i = (char *) arg2;
						break;

					case 'n':
						//                                i = get_descr_form(ch,to,FALSE);
						i = PERS(ch,to);
						break;

					case 'N':
						//                                i = get_descr_form(vch,to,FALSE);
						i = PERS(vch,to);
						break;

					case 'e':
						i = he_she[URANGE(0, ch ->sex, 2)];
						break;

					case 'E':
						i = he_she[URANGE(0, vch ->sex, 2)];
						break;

					case 'm':
						i = him_her[URANGE(0, ch ->sex, 2)];
						break;

					case 'M':
						i = him_her[URANGE(0, vch ->sex, 2)];
						break;

					case 'o':
						sprintf(buf2, "%s", olc_ed_name(ch));
						i = buf2;
						break;
					case 'O':
						sprintf(buf2, "%s", olc_ed_vnum(ch));
						i = buf2;
						break;

					case 's':
						i = his_her[URANGE(0, ch ->sex, 2)];
						break;

					case 'S':
						i = his_her[URANGE(0, vch ->sex, 2)];
						break;

					case 'p':
						i = can_see_obj(to, obj1) ? obj1->short_descr
								: "something";
						break;

					case 'P':
						i = can_see_obj(to, obj2) ? obj2->short_descr
								: "something";
						break;

					case 'v':
						i = obj1->short_descr;
						break;

					case 'V':
						i = obj2->short_descr;
						break;

					case 'd':
						if (!arg2 || ((char *) arg2)[0] == '\0')
						{
							i = "door";
						}
						else
						{
							one_argument((char *) arg2, fname);
							i = fname;
						}
						break;

					case 'g':
						i = god_table[ch->god].name;
						break;

					case 'c':
						i = (get_clan_by_ch( ch ))->c_name;
						break;

					}
				}
				break;

			case '{':
				fColour = FALSE;
				++str;
				i = NULL;
				if (IS_SET( to->act, PLR_COLOUR ))
				{
					i = colour(*str, to);
				}
				break;

			default:
				fColour = FALSE;
				*point++ = *str++;
				break;
			}

			++str;
			if (fColour && i)
			{
				fixed[0] = '\0';
				i2 = fixed;

				if (IS_SET( to->act, PLR_COLOUR ))
				{
					for (i2 = fixed; *i; i++)
					{
						if (*i == '{')
						{
							if (*(++i) == '\0')
								break;
							strcat(fixed, colour(*i, to));
							for (i2 = fixed; *i2; i2++)
								;
							continue;
						}
						*i2 = *i;
						*++i2 = '\0';
					}
					*i2 = '\0';
					i = &fixed[0];
				}
				else
				{
					for (i2 = fixed; *i; i++)
					{
						if (*i == '{')
						{
							if (*(++i) == '\0')
								break;
							if (*i != '{')
							{
								continue;
							}
						}
						*i2 = *i;
						*++i2 = '\0';
					}
					*i2 = '\0';
					i = &fixed[0];
				}
			}

			if (i)
			{
				while ((*point = *i) != '\0')
				{
					++point;
					++i;
				}
			}
		}

		*point++ = '\n';
		*point++ = '\r';
		*point = '\0';
		buf[0] = UPPER( buf[0] );
		if (to->desc != NULL)
			write_to_buffer(to->desc, filter_stuff(buf, to->name), 0);
		else if (MOBtrigger)
			mp_act_trigger(buf, to, ch, arg1, arg2, TRIG_ACT );
	}
	return;
}

char *colour(char type, CHAR_DATA *ch)
{

	if (IS_NPC( ch ) && ch->desc == NULL)
		return ("");

	switch (type)
	{
	default:
		strcpy(clcode, colour_clear(ch));
		break;
	case 'x':
		strcpy(clcode, colour_clear(ch));
		break;
	case '0':
		strcpy(clcode, colour_clear(ch));
		break;
	case 'z':
		sprintf(clcode, BLINK );
		break;
	case 'i':
		sprintf(clcode, ITALIC );
		break;
	case 'f':
		sprintf(clcode, FAINT );
		break;
	case 'n':
		sprintf(clcode, NEGATIVE );
		break;
	case 'u':
		sprintf(clcode, UNDERLINE );
		break;
	case 'b':
		sprintf(clcode, C_BLUE );
		break;
	case '4':
		sprintf(clcode, C_BLUE );
		break;
	case 'c':
		sprintf(clcode, C_CYAN );
		break;
	case '6':
		sprintf(clcode, C_CYAN );
		break;
	case 'g':
		sprintf(clcode, C_GREEN );
		break;
	case '2':
		sprintf(clcode, C_GREEN );
		break;
	case 'm':
		sprintf(clcode, C_MAGENTA );
		break;
	case '5':
		sprintf(clcode, C_MAGENTA );
		break;
	case 'r':
		sprintf(clcode, C_RED );
		break;
	case '1':
		sprintf(clcode, C_RED );
		break;
	case 'w':
		sprintf(clcode, C_WHITE );
		break;
	case '7':
		sprintf(clcode, C_WHITE );
		break;
	case 'y':
		sprintf(clcode, C_YELLOW );
		break;
	case '3':
		sprintf(clcode, C_YELLOW );
		break;
	case 'B':
		sprintf(clcode, C_B_BLUE );
		break;
	case '$':
		sprintf(clcode, C_B_BLUE );
		break;
	case 'C':
		sprintf(clcode, C_B_CYAN );
		break;
	case '^':
		sprintf(clcode, C_B_CYAN );
		break;
	case 'G':
		sprintf(clcode, C_B_GREEN );
		break;
	case '@':
		sprintf(clcode, C_B_GREEN );
		break;
	case 'M':
		sprintf(clcode, C_B_MAGENTA );
		break;
	case '%':
		sprintf(clcode, C_B_MAGENTA );
		break;
	case 'R':
		sprintf(clcode, C_B_RED );
		break;
	case '!':
		sprintf(clcode, C_B_RED );
		break;
	case 'W':
		sprintf(clcode, C_B_WHITE );
		break;
	case '&':
		sprintf(clcode, C_B_WHITE );
		break;
	case 'Y':
		sprintf(clcode, C_B_YELLOW );
		break;
	case '#':
		sprintf(clcode, C_B_YELLOW );
		break;
	case 'D':
		sprintf(clcode, C_D_GREY );
		break;
	case '8':
		sprintf(clcode, C_D_GREY );
		break;
	case '*':
		sprintf(clcode, C_D_GREY );
		break;
	case 'k':
		sprintf(clcode, B_BLACK );
		break;
	case 't':
		sprintf(clcode, B_BLUE );
		break;
	case 'j':
		sprintf(clcode, B_CYAN );
		break;
	case 'o':
		sprintf(clcode, B_GREEN );
		break;
	case 'a':
		sprintf(clcode, B_MAGENTA );
		break;
	case 'e':
		sprintf(clcode, B_RED );
		break;
	case 'h':
		sprintf(clcode, B_WHITE );
		break;
	case 'l':
		sprintf(clcode, B_YELLOW );
		break;
	case 'K':
		sprintf(clcode, B_D_GREY );
		break;
	case 'T':
		sprintf(clcode, B_B_BLUE );
		break;
	case 'J':
		sprintf(clcode, B_B_CYAN );
		break;
	case 'O':
		sprintf(clcode, B_B_GREEN );
		break;
	case 'A':
		sprintf(clcode, B_B_MAGENTA );
		break;
	case 'E':
		sprintf(clcode, B_B_RED );
		break;
	case 'H':
		sprintf(clcode, B_B_WHITE );
		break;
	case 'L':
		sprintf(clcode, B_B_YELLOW );
		break;

	case '+': /* Auction Channel */
		if (ch->color_auc)
		{
			strcpy(clcode, colour_channel(ch->color_auc, ch));
		}
		else
		{
			sprintf(clcode, C_B_BLUE );
		}
		break;
	case ',': /* Clan Gossip Channel */
		if (ch->color_cgo)
		{
			strcpy(clcode, colour_channel(ch->color_cgo, ch));
		}
		else
		{
			sprintf(clcode, C_B_RED );
		}
		break;
	case '9': /* Clan Talk Channel */
		if (ch->color_cla)
		{
			strcpy(clcode, colour_channel(ch->color_cla, ch));
		}
		else
		{
			sprintf(clcode, C_B_MAGENTA );
		}
		break;
	case '>': /* IC Channel */
		if (ch->color_gos)
		{
			strcpy(clcode, colour_channel(ch->color_gos, ch));
		}
		else
		{
			sprintf(clcode, C_B_BLUE );
		}
		break;
	case '/': /* Grats Channel */
		if (ch->color_gra)
		{
			strcpy(clcode, colour_channel(ch->color_gra, ch));
		}
		else
		{
			sprintf(clcode, C_YELLOW );
		}
		break;
	case 'P': /* Group Tell Channel */
		if (ch->color_gte)
		{
			strcpy(clcode, colour_channel(ch->color_gte, ch));
		}
		else
		{
			sprintf(clcode, C_CYAN );
		}
		break;
	case 'Q': /* Immortal Talk Channel */
		if (ch->color_imm)
		{
			strcpy(clcode, colour_channel(ch->color_imm, ch));
		}
		else
		{
			sprintf(clcode, C_B_WHITE );
		}
		break;
	case '-': /* Music Channel */
		if (ch->color_mus)
		{
			strcpy(clcode, colour_channel(ch->color_mus, ch));
		}
		else
		{
			sprintf(clcode, C_B_CYAN );
		}
		break;
	case 'p': /* Question+Answer Channel */
		if (ch->color_que)
		{
			strcpy(clcode, colour_channel(ch->color_que, ch));
		}
		else
		{
			sprintf(clcode, C_B_YELLOW );
		}
		break;
	case 'q': /* Quote Channel */
		if (ch->color_quo)
		{
			strcpy(clcode, colour_channel(ch->color_quo, ch));
		}
		else
		{
			sprintf(clcode, C_GREEN );
		}
		break;
	case 'S': /* Say Channel */
		if (ch->color_say)
		{
			strcpy(clcode, colour_channel(ch->color_say, ch));
		}
		else
		{
			sprintf(clcode, C_MAGENTA );
		}
		break;
	case ':': /* Shout+Yell Channel */
		if (ch->color_sho)
		{
			strcpy(clcode, colour_channel(ch->color_sho, ch));
		}
		else
		{
			sprintf(clcode, C_RED );
		}
		break;
	case 's': /* Tell+Reply Channel */
		if (ch->color_tel)
		{
			strcpy(clcode, colour_channel(ch->color_tel, ch));
		}
		else
		{
			sprintf(clcode, C_CYAN );
		}
		break;
	case 'V': /* Wiznet Messages */
		if (ch->color_wiz)
		{
			strcpy(clcode, colour_channel(ch->color_wiz, ch));
		}
		else
		{
			sprintf(clcode, C_WHITE );
		}
		break;
	case '_': /* Mobile Talk */
		if (ch->color_mob)
		{
			strcpy(clcode, colour_channel(ch->color_mob, ch));
		}
		else
		{
			sprintf(clcode, C_MAGENTA );
		}
		break;
	case '<': /* Room Title */
		if (ch->color_roo)
		{
			strcpy(clcode, colour_channel(ch->color_roo, ch));
		}
		else
		{
			sprintf(clcode, C_B_BLUE );
		}
		break;
	case '(': /* Opponent Condition */
		if (ch->color_con)
		{
			strcpy(clcode, colour_channel(ch->color_con, ch));
		}
		else
		{
			sprintf(clcode, C_B_RED );
		}
		break;
	case '.': /* Fight Actions */
		if (ch->color_fig)
		{
			strcpy(clcode, colour_channel(ch->color_fig, ch));
		}
		else
		{
			sprintf(clcode, C_B_BLUE );
		}
		break;
	case ')': /* Opponents Fight Actions */
		if (ch->color_opp)
		{
			strcpy(clcode, colour_channel(ch->color_opp, ch));
		}
		else
		{
			sprintf(clcode, C_CYAN );
		}
		break;
	case '?': /* Disarm Messages */
		if (ch->color_dis)
		{
			strcpy(clcode, colour_channel(ch->color_dis, ch));
		}
		else
		{
			sprintf(clcode, C_B_YELLOW );
		}
		break;
	case ';': /* Witness Messages */
		if (ch->color_wit)
		{
			strcpy(clcode, colour_channel(ch->color_wit, ch));
		}
		else
		{
			strcpy(clcode, colour_clear(ch));
		}
		break;
	case 'v': /* Quest Gossip */
		if (ch->color_qgo)
		{
			strcpy(clcode, colour_channel(ch->color_qgo, ch));
		}
		else
		{
			sprintf(clcode, C_B_CYAN);
		}
		break;
	case '=': /* ooc to chat */
		if (ch->color_chat)
		{
			strcpy(clcode, colour_channel(ch->color_chat, ch));
		}
		else
		{
			sprintf(clcode, C_B_CYAN );
		}
		break;
	case '{':
		sprintf(clcode, "%c", '{');
		break;
	}
	return clcode;
}

char *colour_clear(CHAR_DATA *ch)
{

	if (ch->color)
	{
		if (ch->color == 1)
			sprintf(clcode, R_RED );
		else if (ch->color == 2)
			sprintf(clcode, R_GREEN );
		else if (ch->color == 3)
			sprintf(clcode, R_YELLOW );
		else if (ch->color == 4)
			sprintf(clcode, R_BLUE );
		else if (ch->color == 5)
			sprintf(clcode, R_MAGENTA );
		else if (ch->color == 6)
			sprintf(clcode, R_CYAN );
		else if (ch->color == 7)
			sprintf(clcode, R_WHITE );
		else if (ch->color == 8)
			sprintf(clcode, R_D_GREY );
		else if (ch->color == 9)
			sprintf(clcode, R_B_RED );
		else if (ch->color == 10)
			sprintf(clcode, R_B_GREEN );
		else if (ch->color == 11)
			sprintf(clcode, R_B_YELLOW );
		else if (ch->color == 12)
			sprintf(clcode, R_B_BLUE );
		else if (ch->color == 13)
			sprintf(clcode, R_B_MAGENTA );
		else if (ch->color == 14)
			sprintf(clcode, R_B_CYAN );
		else if (ch->color == 15)
			sprintf(clcode, R_B_WHITE );
		else if (ch->color == 16)
			sprintf(clcode, R_BLACK );
		else
			sprintf(clcode, CLEAR );
	}
	else
	{
		sprintf(clcode, CLEAR );
	}
	return clcode;
}

char *colour_channel(int colornum, CHAR_DATA *ch)
{

	if (colornum == 1)
		sprintf(clcode, C_RED );
	else if (colornum == 2)
		sprintf(clcode, C_GREEN );
	else if (colornum == 3)
		sprintf(clcode, C_YELLOW );
	else if (colornum == 4)
		sprintf(clcode, C_BLUE );
	else if (colornum == 5)
		sprintf(clcode, C_MAGENTA );
	else if (colornum == 6)
		sprintf(clcode, C_CYAN );
	else if (colornum == 7)
		sprintf(clcode, C_WHITE );
	else if (colornum == 8)
		sprintf(clcode, C_D_GREY );
	else if (colornum == 9)
		sprintf(clcode, C_B_RED );
	else if (colornum == 10)
		sprintf(clcode, C_B_GREEN );
	else if (colornum == 11)
		sprintf(clcode, C_B_YELLOW );
	else if (colornum == 12)
		sprintf(clcode, C_B_BLUE );
	else if (colornum == 13)
		sprintf(clcode, C_B_MAGENTA );
	else if (colornum == 14)
		sprintf(clcode, C_B_CYAN );
	else if (colornum == 15)
		sprintf(clcode, C_B_WHITE );
	else if (colornum == 16)
		sprintf(clcode, C_BLACK );
	else
		strcpy(clcode, colour_clear(ch));

	return clcode;
}

/*
 * Macintosh support functions.
 */
#if defined(macintosh)
int gettimeofday( struct timeval *tp, void *tzp )
{
	tp->tv_sec = time( NULL );
	tp->tv_usec = 0;
}
#endif

void printf_to_char(CHAR_DATA *ch, char *fmt, ...)
{
	char buf[MAX_STRING_LENGTH];
	va_list args;
	va_start (args, fmt);
	vsprintf(buf, fmt, args);
	va_end (args);

	send_to_char(buf, ch);
}

char *colour2(char type, DESCRIPTOR_DATA *d)
{

	switch (type)
	{
	default:
		sprintf(clcode, C_WHITE);
		break;
	case 'x':
		sprintf(clcode, C_WHITE);
		break;
	case '0':
		sprintf(clcode, C_WHITE);
		break;
	case 'z':
		sprintf(clcode, BLINK );
		break;
	case 'i':
		sprintf(clcode, ITALIC );
		break;
	case 'f':
		sprintf(clcode, FAINT );
		break;
	case 'n':
		sprintf(clcode, NEGATIVE );
		break;
	case 'u':
		sprintf(clcode, UNDERLINE );
		break;
	case 'b':
		sprintf(clcode, C_BLUE );
		break;
	case '4':
		sprintf(clcode, C_BLUE );
		break;
	case 'c':
		sprintf(clcode, C_CYAN );
		break;
	case '6':
		sprintf(clcode, C_CYAN );
		break;
	case 'g':
		sprintf(clcode, C_GREEN );
		break;
	case '2':
		sprintf(clcode, C_GREEN );
		break;
	case 'm':
		sprintf(clcode, C_MAGENTA );
		break;
	case '5':
		sprintf(clcode, C_MAGENTA );
		break;
	case 'r':
		sprintf(clcode, C_RED );
		break;
	case '1':
		sprintf(clcode, C_RED );
		break;
	case 'w':
		sprintf(clcode, C_WHITE );
		break;
	case '7':
		sprintf(clcode, C_WHITE );
		break;
	case 'y':
		sprintf(clcode, C_YELLOW );
		break;
	case '3':
		sprintf(clcode, C_YELLOW );
		break;
	case 'B':
		sprintf(clcode, C_B_BLUE );
		break;
	case '$':
		sprintf(clcode, C_B_BLUE );
		break;
	case 'C':
		sprintf(clcode, C_B_CYAN );
		break;
	case '^':
		sprintf(clcode, C_B_CYAN );
		break;
	case 'G':
		sprintf(clcode, C_B_GREEN );
		break;
	case '@':
		sprintf(clcode, C_B_GREEN );
		break;
	case 'M':
		sprintf(clcode, C_B_MAGENTA );
		break;
	case '%':
		sprintf(clcode, C_B_MAGENTA );
		break;
	case 'R':
		sprintf(clcode, C_B_RED );
		break;
	case '!':
		sprintf(clcode, C_B_RED );
		break;
	case 'W':
		sprintf(clcode, C_B_WHITE );
		break;
	case '&':
		sprintf(clcode, C_B_WHITE );
		break;
	case 'Y':
		sprintf(clcode, C_B_YELLOW );
		break;
	case '#':
		sprintf(clcode, C_B_YELLOW );
		break;
	case 'D':
		sprintf(clcode, C_D_GREY );
		break;
	case '8':
		sprintf(clcode, C_D_GREY );
		break;
	case '*':
		sprintf(clcode, C_D_GREY );
		break;
	case 't':
		sprintf(clcode, B_BLUE );
		break;
	case 'j':
		sprintf(clcode, B_CYAN );
		break;
	case 'o':
		sprintf(clcode, B_GREEN );
		break;
	case 'a':
		sprintf(clcode, B_MAGENTA );
		break;
	case 'e':
		sprintf(clcode, B_RED );
		break;
	case 'h':
		sprintf(clcode, B_WHITE );
		break;
	case 'l':
		sprintf(clcode, B_YELLOW );
		break;
	case 'k':
		sprintf(clcode, B_BLACK );
		break;
	case 'K':
		sprintf(clcode, B_D_GREY );
		break;
	case 'T':
		sprintf(clcode, B_B_BLUE );
		break;
	case 'J':
		sprintf(clcode, B_B_CYAN );
		break;
	case 'O':
		sprintf(clcode, B_B_GREEN );
		break;
	case 'A':
		sprintf(clcode, B_B_MAGENTA );
		break;
	case 'E':
		sprintf(clcode, B_B_RED );
		break;
	case 'H':
		sprintf(clcode, B_B_WHITE );
		break;
	case 'L':
		sprintf(clcode, B_B_YELLOW );
		break;

	case '{':
		sprintf(clcode, "%c", '{');
		break;
	}
	return clcode;
}

void send_to_desc(const char *txt, DESCRIPTOR_DATA *d)
{
	const char *point;
	char *point2;
	char buf[MAX_STRING_LENGTH * 4];

	buf[0] = '\0';
	point2 = buf;
	if (txt && d)
	{
		for (point = txt; *point; point++)
		{
			if (*point == '{')
			{
				point++;
				strcat(buf, colour2(*point, d));
				for (point2 = buf; *point2; point2++)
					;
				continue;
			}
			*point2 = *point;
			*++point2 = '\0';
		}
		*point2 = '\0';
		write_to_buffer(d, buf, point2 - buf);
	}
	else
	{
		for (point = txt; *point; point++)
		{
			if (*point == '{')
			{
				point++;
				if (*point != '{')
				{
					continue;
				}
			}
			*point2 = *point;
			*++point2 = '\0';
		}
		*point2 = '\0';
		write_to_buffer(d, buf, point2 - buf);
	}
	return;
}

char *colour3(char type, int desc)
{

	switch (type)
	{
	default:
		sprintf(clcode, C_WHITE);
		break;
	case 'x':
		sprintf(clcode, C_WHITE);
		break;
	case '0':
		sprintf(clcode, C_WHITE);
		break;
	case 'z':
		sprintf(clcode, BLINK );
		break;
	case 'i':
		sprintf(clcode, ITALIC );
		break;
	case 'f':
		sprintf(clcode, FAINT );
		break;
	case 'n':
		sprintf(clcode, NEGATIVE );
		break;
	case 'u':
		sprintf(clcode, UNDERLINE );
		break;
	case 'b':
		sprintf(clcode, C_BLUE );
		break;
	case '4':
		sprintf(clcode, C_BLUE );
		break;
	case 'c':
		sprintf(clcode, C_CYAN );
		break;
	case '6':
		sprintf(clcode, C_CYAN );
		break;
	case 'g':
		sprintf(clcode, C_GREEN );
		break;
	case '2':
		sprintf(clcode, C_GREEN );
		break;
	case 'm':
		sprintf(clcode, C_MAGENTA );
		break;
	case '5':
		sprintf(clcode, C_MAGENTA );
		break;
	case 'r':
		sprintf(clcode, C_RED );
		break;
	case '1':
		sprintf(clcode, C_RED );
		break;
	case 'w':
		sprintf(clcode, C_WHITE );
		break;
	case '7':
		sprintf(clcode, C_WHITE );
		break;
	case 'y':
		sprintf(clcode, C_YELLOW );
		break;
	case '3':
		sprintf(clcode, C_YELLOW );
		break;
	case 'B':
		sprintf(clcode, C_B_BLUE );
		break;
	case '$':
		sprintf(clcode, C_B_BLUE );
		break;
	case 'C':
		sprintf(clcode, C_B_CYAN );
		break;
	case '^':
		sprintf(clcode, C_B_CYAN );
		break;
	case 'G':
		sprintf(clcode, C_B_GREEN );
		break;
	case '@':
		sprintf(clcode, C_B_GREEN );
		break;
	case 'M':
		sprintf(clcode, C_B_MAGENTA );
		break;
	case '%':
		sprintf(clcode, C_B_MAGENTA );
		break;
	case 'R':
		sprintf(clcode, C_B_RED );
		break;
	case '!':
		sprintf(clcode, C_B_RED );
		break;
	case 'W':
		sprintf(clcode, C_B_WHITE );
		break;
	case '&':
		sprintf(clcode, C_B_WHITE );
		break;
	case 'Y':
		sprintf(clcode, C_B_YELLOW );
		break;
	case '#':
		sprintf(clcode, C_B_YELLOW );
		break;
	case 'D':
		sprintf(clcode, C_D_GREY );
		break;
	case '8':
		sprintf(clcode, C_D_GREY );
		break;
	case '*':
		sprintf(clcode, C_D_GREY );
		break;
	case 't':
		sprintf(clcode, B_BLUE );
		break;
	case 'j':
		sprintf(clcode, B_CYAN );
		break;
	case 'o':
		sprintf(clcode, B_GREEN );
		break;
	case 'a':
		sprintf(clcode, B_MAGENTA );
		break;
	case 'e':
		sprintf(clcode, B_RED );
		break;
	case 'h':
		sprintf(clcode, B_WHITE );
		break;
	case 'l':
		sprintf(clcode, B_YELLOW );
		break;
	case 'k':
		sprintf(clcode, B_BLACK );
		break;
	case 'K':
		sprintf(clcode, B_D_GREY );
		break;
	case 'T':
		sprintf(clcode, B_B_BLUE );
		break;
	case 'J':
		sprintf(clcode, B_B_CYAN );
		break;
	case 'O':
		sprintf(clcode, B_B_GREEN );
		break;
	case 'A':
		sprintf(clcode, B_B_MAGENTA );
		break;
	case 'E':
		sprintf(clcode, B_B_RED );
		break;
	case 'H':
		sprintf(clcode, B_B_WHITE );
		break;
	case 'L':
		sprintf(clcode, B_B_YELLOW );
		break;

	case '{':
		sprintf(clcode, "%c", '{');
		break;
	}
	return clcode;
}

char *total_length(char *input, int length)
{
	char buf[MSL];
	char buf2[MSL];
	int cnt = 0;
	int plc = 0;
	buf[0] = '\0';
	buf2[0] = '\0';

	if (input == NULL) {
		printf_debug( "total_length: NULL string passed as argument.");
		input = "null";
	}

	for (plc = 0; plc < strlen(input); plc++)
	{
		if (input[plc] != '{')
		{
			if (buf[0] == '\0')
			{
				sprintf(buf2, "%c", input[plc]);
			}
			else
			{
				sprintf(buf2, "%s%c", buf, input[plc]);
			}
			sprintf(buf, "%s", buf2);
			cnt++;
		}
		else if (input[plc + 1] == '{')
		{
			if (buf[0] == '\0')
			{
				sprintf(buf2, "{{");
			}
			else
			{
				sprintf(buf2, "%s{{", buf);
			}
			sprintf(buf, "%s", buf2);
			cnt++;
			plc++;
		}
		else
		{
			if (buf[0] == '\0')
			{
				sprintf(buf2, "{%c", input[plc + 1]);
			}
			else
			{
				sprintf(buf2, "%s{%c", buf, input[plc + 1]);
			}
			sprintf(buf, "%s", buf2);
			plc++;
		}

		if (cnt >= length)
		{
			buf[plc + 1] = '\0';
			plc = strlen(input);
			break;
		}

	}
	sprintf(buf2, "%s{0", buf);
	sprintf(buf, "%s", buf2);

	while (cnt < length)
	{
		sprintf(buf2, "%s ", buf);
		sprintf(buf, "%s", buf2);
		cnt++;
	}

	return str_dup(buf);
}


// All single threaded, so we can get away with this.
FILE * temp_fp = NULL;

// Opens a temporary file for writing.
FILE * get_temp_file()
{
	if (temp_fp != NULL)
		fclose(temp_fp);

	return (temp_fp = fopen(TEMP_FILE, "w"));
}

// Renames the temporary file to the other file.
bool close_write_file(char *filename)
{
	bool ok = FALSE;
	if (temp_fp != NULL)
	{
		// Only overwrite if we know things have written ok.
		if (fclose(temp_fp) == 0 && rename(TEMP_FILE, filename) == 0)
			ok = TRUE;
		temp_fp = NULL;
		unlink(TEMP_FILE);
	}
	return ok;
}
