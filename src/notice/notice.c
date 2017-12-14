/*
 * Move notice server, multithreaded.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define BLOCK_SIZE	2048
#define min(A, B) 	((A) < (B) ? (A) : (B))

#define GREEN		"[1m[32m"
#define YELLOW		"[1m[33m"
#define WHITE		"[1m[37m"
#define RESET		"[0m"

const char *notice_message =
	"\r\n\r\n"
	YELLOW"**********************************************\r\n"
	YELLOW"* "WHITE"Asgard has moved to another address.       "YELLOW"*\r\n"
	YELLOW"* "WHITE"Please update your mud client settings to: "YELLOW"*\r\n"
	YELLOW"*                                            "YELLOW"*\r\n"
	YELLOW"*       "WHITE"Hostname: "GREEN"an.dyndns-server.com       "YELLOW"*\r\n"
	YELLOW"*       "WHITE"Port:     "GREEN"14242                      "YELLOW"*\r\n"
	YELLOW"*                                            "YELLOW"*\r\n"
	YELLOW"**********************************************\r\n"RESET
	"\r\n\r\n";

int main(int argc, char **argv)
{
	int message_len = strlen(notice_message);
	int port, srv_fd, cli_fd, option;
	struct sockaddr_in srv_addr, cli_addr;
	pid_t child_pid = 0;

	if (argc < 2 || (port = atoi(argv[1])) < 1024)
	{
		fprintf(stderr, "Please provide port greater than 1024.\n");
		return EXIT_FAILURE;
	}
	
	printf("Running notice on port %d.\n", port);

	// Set up a socket.
	if ((srv_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		perror("Call to socket() failed");
		return EXIT_FAILURE;
	}
	
	// Set reuse address so we can come up ASAP on reboot.
	option = 1;
	if (setsockopt(srv_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&option, sizeof(option)) < 0)
	{
		perror("Call to setsockopt() failed");
		close(srv_fd);
		return EXIT_FAILURE;
	}
	
	// Set up address structure for server.
	memset(&srv_addr, 0, sizeof(srv_addr));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	srv_addr.sin_port = htons(port);
	
	// Bind to port.
	if (bind(srv_fd, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) < 0)
	{
		perror("Call to bind() failed");
		close(srv_fd);
		return EXIT_FAILURE;
	}
	
	// Start listening for connections.
	if (listen(srv_fd,SOMAXCONN) < 0)
	{
		perror("Call to listen() failed");
		close(srv_fd);
		return EXIT_FAILURE;
	}
	
	// Start looping, accepting connections.
	while (1)
	{
		int cli_addr_len = sizeof(cli_addr), status;
		if ((cli_fd = accept(srv_fd, (struct sockaddr *)&cli_addr, &cli_addr_len)) < 0)
		{
			perror("Call to accept() failed");
			close(srv_fd);
			return EXIT_FAILURE;
		}
		printf("Accepted connection from %s.\n", inet_ntoa(cli_addr.sin_addr));

		// Kill previous child - should be finished by then.
		if (child_pid != 0 && waitpid(child_pid, &status, 0) < 0)
		{
			perror("Call to wait() failed");
			close(cli_fd);
			close(srv_fd);
			return EXIT_FAILURE;
		}

		if ((child_pid = fork()) == 0)
		{
			// Child process, write to socket and exit.
			int written = 0;
			while (written < message_len)
			{
				int this_round = 0;
				if ((this_round = write(cli_fd, notice_message+written, min(BLOCK_SIZE, message_len-written))) < 0)
				{
					perror("Call to write() failed");
					close(cli_fd);
					_exit(1);
				}
				written += this_round;
			}
			shutdown(cli_fd, SHUT_WR);
			_exit(0);
		}
		else if (child_pid < 0)
		{
			perror("Call to fork() failed");
			close(cli_fd);
			close(srv_fd);
			return EXIT_FAILURE;
		}
	}
	
	// Close socket.
	close(srv_fd);
	return EXIT_SUCCESS;
}

