CC=gcc
CFLAGS=-Wall

all:
	$(CC) $(CFLAGS) main.c -o main

clean:
	rm -f main

test:
	make all
	find .. | xargs wc --bytes; ./main ..