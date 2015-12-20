#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include "common.h"

int create_sender_socket(char* ip_addr, unsigned int port)
{
	int ret;
    struct hostent *he;
    struct sockaddr_in addr;

	//Get the hostent of the ip address
    if((he = gethostbyname(ip_addr)) == NULL)
	{
        perror("gethostbyname");
        exit(EXIT_FAILURE);
	}

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr = *((struct in_addr *)he->h_addr);
    memset(&(addr.sin_zero), '\0', 8);

	//Get an IPv4 socket
    if((ret = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
        perror("socket");
        exit(EXIT_FAILURE);
    }

	//Assign the socket to the address
    if(connect(ret, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1)
	{
        perror("connect");
        exit(EXIT_FAILURE);
    }

	return ret;
}

void sigchld_handler(int s)
{
	(void)s;
	while(wait(NULL) > 0);
}

int create_listener_socket(unsigned int port, unsigned int backlog)
{
	int ret;
    struct sockaddr_in addr;
    struct sigaction sa;
    const int yes = 1;

	//Assign the address struct
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(addr.sin_zero), '\0', 8);

	//Get an IPv4 socket
    if((ret = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
        perror("socket");
        exit(EXIT_FAILURE);
    }
	
	//Change an option?
    if(setsockopt(ret, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
	//Assign the socket to the address (in this case, any address)
    if(bind(ret, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1)
	{
        perror("bind");
        exit(EXIT_FAILURE);
    }

	//Mark the socket as passive socket
    if(listen(ret, backlog) == -1)
	{
        perror("listen");
        exit(EXIT_FAILURE);
    }


	//Reap all dead processes
    sa.sa_handler = sigchld_handler; 
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if(sigaction(SIGCHLD, &sa, NULL) == -1)
	{
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

	return ret;
}

void send_message(int sockfd, void* message, unsigned int message_size)
{
	if(send(sockfd, message, message_size, 0) == -1)
	{
		perror("send");
		exit(EXIT_FAILURE);
	}
}

unsigned int receive_message(int sockfd, void* message, unsigned int message_max_size)
{
	int ret;
	if((ret = recv(sockfd, message, message_max_size, 0)) == -1)
	{
		perror("recv");
		exit(EXIT_FAILURE);
	}
	return (unsigned int)ret;
}

int is_valid_play(int cell, char grid[GRID_HEIGHT][GRID_WIDTH])
{
	return (cell >= 0)
		&& (cell < GRID_WIDTH * GRID_HEIGHT)
		&& (grid[cell / GRID_WIDTH][cell % GRID_WIDTH] == BLANK_TOKEN);
}

void set_cell(int cell, char char_to_write, char grid[GRID_HEIGHT][GRID_WIDTH])
{
	grid[cell / GRID_WIDTH][cell % GRID_WIDTH] = char_to_write;
}

int get_game_state(char grid[GRID_HEIGHT][GRID_WIDTH])
{
	//Check if there is a complete row
	for(unsigned int i = 0; i < GRID_HEIGHT; ++i)
	{
		int row_complete = 1;
		for(unsigned int j = 1; j < GRID_WIDTH; ++j)
			row_complete = row_complete && (grid[i][0] == grid[i][j]);
		if(row_complete && grid[i][0] != BLANK_TOKEN)
			return grid[i][0] == X_TOKEN ? X_WON : O_WON;
	}

	//Check if there is a complete column
	for(unsigned int i = 0; i < GRID_WIDTH; ++i)
	{
		int col_complete = 1;
		for(unsigned int j = 1; j < GRID_HEIGHT; ++j)
			col_complete = col_complete && (grid[0][i] == grid[j][i]);
		if(col_complete && grid[0][i] != BLANK_TOKEN)
			return grid[0][i] == X_TOKEN ? X_WON : O_WON;
	}

	//Check if there is a complete diagonal
	int diagonal1_complete = 1, diagonal2_complete = 1;
	for(unsigned int i = 1; i < GRID_WIDTH; ++i)
	{
		diagonal1_complete = diagonal1_complete && (grid[0][0] == grid[i][i]);
		diagonal2_complete = diagonal2_complete && (grid[0][GRID_WIDTH - 1] == grid[i][GRID_WIDTH - i - 1]);
	}
	if(diagonal1_complete && grid[0][0] != BLANK_TOKEN)
		return grid[0][0] == X_TOKEN ? X_WON : O_WON;
	if(diagonal2_complete && grid[0][GRID_WIDTH - 1] != BLANK_TOKEN)
		return grid[0][GRID_WIDTH - 1] == X_TOKEN ? X_WON : O_WON;

	//Check if the grid is full (null match)
	for(unsigned int i = 0; i < GRID_HEIGHT; ++i)
		for(unsigned int j = 0; j < GRID_WIDTH; ++j)
			if(grid[i][j] == BLANK_TOKEN)
				return CONTINUE_GAME;
	return NULL_MATCH;
}

