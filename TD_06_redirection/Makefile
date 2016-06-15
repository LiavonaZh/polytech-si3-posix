CC = gcc
CFLAGS = -Wall -std=c99

all: station.o ring.o

station.o: station.c station.h
	$(CC) $(CFLAGS) -o station station.c

ring.o: ring.c station.h
	$(CC) $(CFLAGS) -o ring ring.c

clean:
	rm -rf *.o ring station