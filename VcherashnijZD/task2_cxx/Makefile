CXX = g++
CXXFLAGS = -I. -std=c++11 -Wall -Wextra

TARGET = task2_cpp
SRC = main.cpp matrix.cpp
OBJ = $(SRC:.cpp=.o)
DEPS = matrix.h

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJ)

