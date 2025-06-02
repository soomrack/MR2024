# Имя проекта
TARGET = taskhouse

# Платформа и порт
FQBN = arduino:avr:uno
PORT = /dev/ttyUSB0

# Путь к arduino-cli
ARDUINO_CLI = arduino-cli

# Путь к директории проекта
PROJECT_DIR = .

# Путь к основному файлу скетча
MAIN_SKETCH = $(PROJECT_DIR)/main.cpp

# Пути к дополнительным файлам проекта
SRC_FILES = $(PROJECT_DIR)/Microclimat.cpp

# Путь к библиотекам
LIBRARIES_DIR = $(HOME)/arduino/libraries

# Цели для сборки и загрузки
all: compile upload

# Установка необходимых библиотек и платформы
init:
	$(ARDUINO_CLI) core update-index
	$(ARDUINO_CLI) core install $(FQBN)
	$(ARDUINO_CLI) lib install "DHT sensor library"

# Компиляция проекта
compile: init
	$(ARDUINO_CLI) compile --fqbn $(FQBN) $(PROJECT_DIR) --libraries $(LIBRARIES_DIR)

# Загрузка проекта на плату
upload: compile
	$(ARDUINO_CLI) upload -p $(PORT) --fqbn $(FQBN) $(PROJECT_DIR)

# Очистка временных файлов
clean:
	rm -rf $(PROJECT_DIR)/.arduino15
	rm -rf $(PROJECT_DIR)/build

.PHONY: init compile upload clean

