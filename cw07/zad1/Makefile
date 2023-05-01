CC=gcc
CFLAGS=-Wall -O3
OTHER=semaphore.c shared_memory.c queue.c

all: 
	make simulation

simulation: client barber
	$(CC) $(CFLAGS) $(SOURCES) simulation.c $(OTHER) -lrt -lpthread -o simulation

client:
	$(CC) $(CFLAGS) $(SOURCES) client.c $(OTHER) -lrt -lpthread -o client

barber:
	$(CC) $(CFLAGS) $(SOURCES) barber.c $(OTHER) -lrt -lpthread -o barber

clean:
	rm -f barber client simulation