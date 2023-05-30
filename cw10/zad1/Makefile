CC=gcc
CFLAGS=-Wall

all:
	$(CC) $(CFLAGS) client.c -o client -lpthread
	$(CC) $(CFLAGS) server.c -o server -lpthread

clean:
	rm -f client server