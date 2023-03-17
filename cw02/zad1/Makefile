CC=gcc
CFLAGS=-Wall

all:
	make main_sys
	make main_lib

main_sys: main.c
	$(CC) $(CFLAGS) main.c -o main_sys -D SYS

main_lib: main.c
	$(CC) $(CFLAGS) main.c -o main_lib

test:
	make all
	head -c 1234567 </dev/urandom> test.in
	./main_sys a b test.in tmp_sys ; tr a b < test.in > tmp ; diff -s tmp tmp_sys
	./main_lib a b test.in tmp_lib ; tr a b < test.in > tmp ; diff -s tmp tmp_lib
	rm -f test.in tmp tmp_lib tmp_sys

report:
	make all
	head -c 1234567 </dev/urandom> test.in
	./main_sys a b test.in tmp_sys > pomiar_zad_1_sys.txt
	./main_lib a b test.in tmp_lib > pomiar_zad_1_lib.txt
	rm -f test.in tmp_lib tmp_sys

clean:
	rm -f main_sys main_lib