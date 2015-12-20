#ifndef SERVER_H
#define SERVER_H

#include "common.h"

#define BACKLOG 10
#define RECEPTION_BUFFER_SIZE 10

void play_game(int client_sockfd);

void play_a_cell(char grid[GRID_HEIGHT][GRID_WIDTH]);

#endif // SERVER_H

