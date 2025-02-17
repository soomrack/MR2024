CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -I./include
TARGET = bin/matrix_calculator
SRCS = src/main.cpp src/Matrix.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
