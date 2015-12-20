#ifndef COMMON_H
#define COMMON_H

#define GRID_WIDTH 3
#define GRID_HEIGHT 3
#define PORT 5555
#define X_TOKEN 'X'
#define O_TOKEN 'O'
#define PLAYER_TOKEN X_TOKEN
#define IA_TOKEN O_TOKEN
#define BLANK_TOKEN '\0'

enum
{
	NULL_MATCH,
	CONTINUE_GAME,
	X_WON,
	O_WON
};

struct acting
{
	int continue_game;
	int cell;
};

int create_sender_socket(char* ip_addr, unsigned int port);

void sigchld_handler(int s);

int create_listener_socket(unsigned int port, unsigned int backlog);

void send_message(int socket, void* message, unsigned int message_size);

unsigned int receive_message(int socket, void* message, unsigned int message_max_size);

int is_valid_play(int cell, char grid[GRID_HEIGHT][GRID_WIDTH]);

void set_cell(int cell, char char_to_write, char grid[GRID_HEIGHT][GRID_WIDTH]);

int get_game_state(char grid[GRID_HEIGHT][GRID_WIDTH]);

#endif // COMMON_H

