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

lib_static: $(LIB).c
	$(CC) $(CFLAGS) -c $(LIB).c
	ar cr $(LIB).a $(LIB).o

lib_shared: $(LIB).c
	$(CC) $(CFLAGS) -c -fPIC $(LIB).c
	$(CC) $(CFLAGS) -shared -o $(LIB).so $(LIB).o

main_static: $(LIB).c $(REPLDLL)
	$(CC) $(CFLAGS) $(REPL) -o main_static -L . $(LIB).a

main_shared: $(LIB).c $(REPLDLL)
	$(CC) $(CFLAGS) $(REPL) -o main_shared -L . $(LIB).so

main_dynamic: $(LIB).c $(REPLDLL)
	$(CC) $(CFLAGS) $(REPL) -o main_dynamic -ldl -D USE_DLL

clean:
	rm -f *.o *.out *.a *.so *.txt main_static main_shared main_dynamic