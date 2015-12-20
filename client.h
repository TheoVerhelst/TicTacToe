#ifndef CLIENT_H
#define CLIENT_H

#include "common.h"

void play_game(int server_sockfd);

struct acting get_user_input(char grid[GRID_HEIGHT][GRID_WIDTH]);

void print_grid(char grid[GRID_HEIGHT][GRID_WIDTH]);

#endif // CLIENT_H
