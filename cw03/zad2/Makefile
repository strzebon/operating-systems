CC=gcc
CFLAGS=-Wall

all:
	$(CC) $(CFLAGS) main.c -o main

test:
	./main ./empty_dir; echo

	./main .

	./main /

clean:
	rm -f main