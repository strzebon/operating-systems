CC=gcc
LIB=library
CFLAGS=-Wall
REPL=main.c
REPLDLL=main.c dllmanager.h

all:
	make lib_static
	make lib_shared
	make main_static
	make main_shared
	make main_dynamic

o:
	make all FLAG="-O"

o0:
	make all FLAG="-O0"

os:
	make all FLAG="-Os"

lib_static: $(LIB).c
	$(CC) $(CFLAGS) $(FLAG) -c $(LIB).c
	ar cr $(LIB).a $(LIB).o

lib_shared: $(LIB).c
	$(CC) $(CFLAGS) $(FLAG) -c -fPIC $(LIB).c
	$(CC) $(CFLAGS) $(FLAG) -shared -o $(LIB).so $(LIB).o

main_static: $(LIB).c $(REPLDLL)
	$(CC) $(CFLAGS) $(FLAG) $(REPL) -o main_static -L . $(LIB).a

main_shared: $(LIB).c $(REPLDLL)
	$(CC) $(CFLAGS) $(FLAG) $(REPL) -o main_shared -L . $(LIB).so

main_dynamic: $(LIB).c $(REPLDLL)
	$(CC) $(CFLAGS) $(FLAG) $(REPL) -o main_dynamic -ldl -D USE_DLL

create_test:
	echo "init 100">test.in
	(ls -p | grep -v / | sed 's/^/count /')>>test.in
	echo "show 0">>test.in
	echo "show 1">>test.in
	echo "show 2">>test.in
	echo "show 3">>test.in
	echo "show 4">>test.in
	echo "show 5">>test.in
	echo "show 6">>test.in
	echo "show 7">>test.in
	echo "delete 7">>test.in
	echo "delete 6">>test.in
	echo "delete 5">>test.in
	echo "delete 4">>test.in
	echo "delete 3">>test.in
	echo "delete 2">>test.in
	echo "delete 1">>test.in
	echo "delete 0">>test.in
	echo "destroy">>test.in
	echo "exit">>test.in

test:
	sudo mv library.so /usr/lib
	make create_test
	./main_static <test.in> results_static_$(FLAG).txt
	./main_shared <test.in> results_shared_$(FLAG).txt
	./main_dynamic <test.in> results_dynamic_$(FLAG).txt
	rm -f test.in

report:
	sudo ls > /dev/null
	make o
	make test FLAG="-O"
	make o0
	make test FLAG="-O0"
	make os
	make test FLAG="-Os"
clean:
	rm -f *.o *.out *.a *.so *.txt main_static main_shared main_dynamic