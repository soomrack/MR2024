CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

TARGET = matrix_calculator
SRCS = src/main.c src/matrix_lib.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
