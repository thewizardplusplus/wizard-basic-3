FLAGS = -Wpedantic -Wall -Wextra -O2
CXXFLAGS += -std=c++11 $(FLAGS)
CFLAGS += -std=c99 $(FLAGS)

all: tests io_test

tests: tests.o
	$(CXX) $(CXXFLAGS) -o tests tests.o -lm -lgtest -lpthread

io_test: io_test.o
	$(CC) $(CFLAGS) -o io_test io_test.o -lm

clean:
	find . -name "*.o" -type f -delete
