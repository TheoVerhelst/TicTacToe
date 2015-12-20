#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "client.h"
#include "common.h"

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		fprintf(stderr, "usage: %s IP_ADDRESS\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int server_sockfd = create_sender_socket(argv[1], PORT);
	play_game(server_sockfd);
	close(server_sockfd);
	exit(EXIT_SUCCESS);
}

void play_game(int server_sockfd)
{
	char grid[GRID_HEIGHT][GRID_WIDTH];
	static unsigned int grid_size = sizeof(grid);

	puts("Envoyer le chiffre 1 pour jouer, le chiffre 2 pour se déconnecter");
	
	//Ask for confirmation to the user and send it to the server
	int confirmation;
	scanf("%d", &confirmation);
	confirmation = (confirmation == 1);
	send_message(server_sockfd, &confirmation, sizeof(confirmation));
	
	if(!confirmation)
		return;

	struct acting user_acting;
	int game_state;
	do
	{
		//Receive and print the grid
		receive_message(server_sockfd, grid, grid_size);
		system("clear");
		puts("État actuel du plateau :");
		print_grid(grid);

		game_state = get_game_state(grid);
		if(game_state != CONTINUE_GAME)
			break;

		//Get the acting from the user and send it to the server
		user_acting = get_user_input(grid);
		send_message(server_sockfd, &user_acting, sizeof(user_acting));

		if(!user_acting.continue_game)
			return;

		//Receive the new grid and compute the game state
		receive_message(server_sockfd, grid, grid_size);
		game_state = get_game_state(grid);
	} while(game_state == CONTINUE_GAME);

	//Display message in order to inform the user of the final state of the game
	switch(game_state)
	{
		case NULL_MATCH:
			puts("Égalité.");
			break;
		case X_WON:
			puts(PLAYER_TOKEN == X_TOKEN ? "Vous avez gagné." : "Vous avez perdu.");
			break;
		case O_WON:
			puts(PLAYER_TOKEN == O_TOKEN ? "Vous avez gagné." : "Vous avez perdu.");
			break;
		default:
			puts("Issue du match indéfinie.");
	}
}

struct acting get_user_input(char grid[GRID_HEIGHT][GRID_WIDTH])
{
	struct acting result;
	result.continue_game = 1;
	do
	{
		printf("Quelle case souhaitez-vous jouer (0 pour quitter) ? ");
		scanf("%d", &result.cell);
		--result.cell;
		if(result.cell == -1 || is_valid_play(result.cell, grid))
			break;
		printf("Choix invalide, veuillez réessayer :\n");
	} while(1);

	if(result.cell == -1)
		result.continue_game = 0;

	return result;
}

void print_grid(char grid[GRID_HEIGHT][GRID_WIDTH])
{
	for(unsigned int i = 0; i < GRID_HEIGHT; ++i)
	{
		printf("\n╋");
		for(unsigned int j = 0; j < GRID_WIDTH; ++j)
			printf("━╋");
		printf("\n┃");
		for(unsigned int j = 0; j < GRID_WIDTH; ++j)
		{
			const char cell = grid[i][j];
			if(cell == BLANK_TOKEN)
				printf("%d┃", i * GRID_WIDTH + j + 1);
			else
				printf("%c┃", cell);
		}
	}
	printf("\n╋");
	for(unsigned int j = 0; j < GRID_WIDTH; ++j)
		printf("━╋");
	printf("\n\n");
}

