CC=gcc
CFLAGS=-Wall

all:
	$(CC) $(CFLAGS) main.c -o main

test:
	./main . CC
	./main .. \#include
	./main ../.. asdf
	./main ../.. zadanie

clean:
	rm -f main