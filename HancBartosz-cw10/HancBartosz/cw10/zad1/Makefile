CC=gcc
CFLAGS=-Wall -pthread
OFLAGS=-O0

server:
	$(CC) $(CFLAGS) $(OFLAGS) -o server server.c

client:
	$(CC) $(CFLAGS) $(OFLAGS) -o client client.c

all:
	make server
	make client

clean:
	rm server client
