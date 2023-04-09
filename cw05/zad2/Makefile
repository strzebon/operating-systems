CC=gcc
CFLAGS=-Wall

all:
	$(CC) $(CFLAGS) main.c -o main

test:
	./main 0.1 1
	./main 0.01 1
	./main 0.000000001 1
	./main 0.000000001 2
	./main 0.000000001 10

report:
	./main 0.1 1 > report.txt
	./main 0.01 1 >> report.txt
	./main 0.000000001 1 >> report.txt
	./main 0.000000001 2 >> report.txt
	./main 0.000000001 10 >> report.txt

clean:
	rm -f main