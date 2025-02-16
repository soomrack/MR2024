# Компилятор
CC=g++
# Флаги
CFLAGS=-Wall -g 
#Подключение библиотек
LDFLAGS=-lgtest -lgtest_main -pthread
#Файлы для компиляции
SOURCES=matrix.cpp main.cpp
# Объектные файлы
OBJECTS=$(SOURCES:.c=.o)
# Итоговый файл
EXECUTABLE=result

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)
