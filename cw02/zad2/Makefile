CC=gcc
CFLAGS=-Wall

all:
	make main_1
	make main_1024

main_1:
	$(CC) $(CFLAGS) main.c -o main_1 -D BLOCK_SIZE=1

main_1024:
	$(CC) $(CFLAGS) main.c -o main_1024 -D BLOCK_SIZE=1024

test:
	make all
	./main_1 test.txt tmp1 ; ./main_1 tmp1 tmp2 ; diff -s test.txt tmp2
	tac <test.txt | rev> tmp3; diff -s tmp3 tmp1
	rm -f tmp*
	./main_1024 test.txt tmp1 ; ./main_1024 tmp1 tmp2 ; diff -s test.txt tmp2
	tac <test.txt | rev> tmp3; diff -s tmp3 tmp1
	rm -f tmp*

report:
	make all
	head -c 1234567 </dev/urandom> test.in
	./main_1 test.in tmp_1 > pomiar_zad_2_1.txt
	./main_1024 test.in tmp_1024 > pomiar_zad_2_1024.txt
	rm -f test.in tmp_1 tmp_1024

clean:
	rm -f main_1 main_1024