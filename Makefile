CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic

all: client server

client: client.c client.h common.h common.c
	$(CC) $(CFLAGS) $^ -o $@

server: server.c server.h common.h common.c
	$(CC) $(CFLAGS) $^ -o $@

