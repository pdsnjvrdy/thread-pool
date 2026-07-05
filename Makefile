CC = gcc
CFLAGS = -pthread -Wall

all: example test_drain bench

example: example.o pool.o
	$(CC) $(CFLAGS) -o example example.o pool.o

test_drain: test_drain.o pool.o
	$(CC) $(CFLAGS) -o test_drain test_drain.o pool.o

bench: bench_sum.o pool.o
	$(CC) $(CFLAGS) -o bench_sum bench_sum.o pool.o

example.o: example.c pool.h
	$(CC) $(CFLAGS) -c example.c

test_drain.o: test_drain.c pool.h
	$(CC) $(CFLAGS) -c test_drain.c

bench_sum.o: bench_sum.c pool.h
	$(CC) $(CFLAGS) -c bench_sum.c

pool.o: pool.c pool.h
	$(CC) $(CFLAGS) -c pool.c

clean:
	rm -f *.o example test_drain bench_sum