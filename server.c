#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "server.h"
#include "common.h"

int main(void)
{
	srand(time(NULL));
	//Address struct for connecting to clients
	struct sockaddr_in client_addr;
	memset(&client_addr, 0, sizeof(client_addr));
	unsigned int addr_size = sizeof(client_addr);

	//Sockets, the first is the connection to the client,
	//the second is the socket that listen for connections
	int client_sockfd, listener_sockfd = create_listener_socket(PORT, BACKLOG);
	
    while(1)
	{
		//If a client wants to connect
		if((client_sockfd = accept(listener_sockfd, (struct sockaddr*) &client_addr, &addr_size)) == -1)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}

		//Handle connection in child thread
		else if(!fork())
		{
			close(listener_sockfd);
			play_game(client_sockfd);
			close(client_sockfd);
			exit(EXIT_SUCCESS);
		}
    }
    exit(EXIT_SUCCESS);
}

void play_game(int client_sockfd)
{
	//Create the grid and fill it with blanks
	char grid[GRID_HEIGHT][GRID_WIDTH];
	static const unsigned int grid_size = sizeof(grid);
	memset(grid, BLANK_TOKEN, grid_size);

	int confirmation;
	receive_message(client_sockfd, &confirmation, sizeof(confirmation));
	if(!confirmation)
		return;

	int game_state;
	do
	{
		//Play and send the new grid
		play_a_cell(grid);
		send_message(client_sockfd, grid, grid_size);

		game_state = get_game_state(grid);
		if(game_state != CONTINUE_GAME)
			break;
		
		struct acting client_response;
		//Receive the acting of the client
		receive_message(client_sockfd, &client_response, sizeof(client_response));

		//If the client wants to stop the game
		if(!client_response.continue_game)
			break;
		
		set_cell(client_response.cell, PLAYER_TOKEN, grid);
		game_state = get_game_state(grid);
		send_message(client_sockfd, grid, grid_size);
	} while(game_state == CONTINUE_GAME);
}

void play_a_cell(char grid[GRID_HEIGHT][GRID_WIDTH])
{
	int cell = rand() % (GRID_WIDTH * GRID_HEIGHT);
	while(!is_valid_play(cell, grid))
		cell = (cell + 1) % (GRID_WIDTH * GRID_HEIGHT);
	set_cell(cell, IA_TOKEN, grid);
}

