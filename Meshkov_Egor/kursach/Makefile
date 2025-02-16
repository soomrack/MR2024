# Компилятор
CC=g++
# Флаги
CFLAGS=-Wall -g
# Подключение библиотек
# LDFLAGS=
# Файлы для компиляции
SOURCES=point.cpp main.cpp vec.cpp jarvis.cpp
# Объектные файлы
OBJECTS=$(SOURCES:.cpp=.o)
# Итоговый файл
EXECUTABLE=result

# Заголовочные файлы
HEADERS=point.hpp MTL.hpp vec.hpp

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
	rm -f $(OBJECTS)

# Правило для компиляции .cpp файлов
%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@
	
# Очистка
clean:
	rm -rf *.o $(EXECUTABLE)
