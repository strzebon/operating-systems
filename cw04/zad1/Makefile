CC=gcc
CFLAGS=-Wall

all:
	$(CC) $(CFLAGS) main.c -o main
	./main ignore 0
	./main handler 0
	./main mask 0
	./main pending 0
	./main ignore 1
	./main mask 1
	./main pending 1

clean:
	rm -f main