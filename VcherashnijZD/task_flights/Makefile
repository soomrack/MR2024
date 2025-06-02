CXX = g++
CXXFLAGS = -I. -std=c++11 -Wall -Wextra

TARGETS = preprocessor main
PREPROCESSOR_OBJS = preprocessor.o csv_utils.o
MAIN_OBJS = main.o csv_utils.o

all: $(TARGETS)

preprocessor: $(PREPROCESSOR_OBJS)
	$(CXX) $(CXXFLAGS) -o preprocessor $(PREPROCESSOR_OBJS)

main: $(MAIN_OBJS)
	$(CXX) $(CXXFLAGS) -o main $(MAIN_OBJS)

preprocessor.o: preprocessor.cpp preprocessor.h csv_utils.h common.h
	$(CXX) $(CXXFLAGS) -c preprocessor.cpp

main.o: main.cpp csv_utils.h common.h
	$(CXX) $(CXXFLAGS) -c main.cpp

csv_utils.o: csv_utils.cpp csv_utils.h common.h
	$(CXX) $(CXXFLAGS) -c csv_utils.cpp

clean:
	rm -f $(TARGETS) *.o

