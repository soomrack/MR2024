CC = gcc
CFLAGS = -Wall -g

all: libmatrix.a libmatrix.so

libmatrix.a: matrix.o
	ar rcs libmatrix.a matrix.o

libmatrix.so: matrix.o
	$(CC) -shared $(LDFLAGS) matrix.o -o libmatrix.so

test: matrix.o test.o
	$(CC) $(CFLAGS) matrix.o test.o -o test
	./test

matrix.o: matrix.c matrix.h
	$(CC) $(CFLAGS) -c matrix.c

test.o: test.c matrix.h
	$(CC) $(CFLAGS) -c test.c

clean:
	rm -f *.o test libmatrix.so libmatrix.a

