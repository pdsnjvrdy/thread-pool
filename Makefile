CC = gcc
CFLAGS = -pthread -Wall

all: example

example: example.o pool.o
	$(CC) $(CFLAGS) -o example example.o pool.o

example.o: example.c pool.h
	$(CC) $(CFLAGS) -c example.c

pool.o: pool.c pool.h
	$(CC) $(CFLAGS) -c pool.c

clean:
	rm -f *.o example