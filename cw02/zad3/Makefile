CC=gcc
CFLAGS=-Wall

all:
	$(CC) $(CFLAGS) main.c -o main

clean:
	rm -f main

test:
	make all
	wc --bytes Makefile main main.c
	./main .