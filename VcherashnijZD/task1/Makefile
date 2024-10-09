CC = gcc
CFLAGS = -Wall -g

all: task0 task1

run: task0 task1
	./task0
	./task1

task0: task0.o
	$(CC) $(CFLAGS) task0.o -o task0

task1: task1.o
	$(CC) $(CFLAGS) task1.o -o task1

task0.o: task0.c
	$(CC) $(CFLAGS) -c task0.c

task1.o: task1.c
	$(CC) $(CFLAGS) -c task1.c

clean:
	rm -f *.o task0 task1

