CXXFLAGS += -std=c++11 -Wpedantic -Wall -Wextra -O2
CC = tcc
CFLAGS += -Wall -Wunusupported -Wwrite-strings

tests: tests.o
	$(CXX) $(CXXFLAGS) -o tests tests.o -lm -lgtest -lpthread

io_test: io_test.o
	$(CC) $(CFLAGS) -o io_test io_test.o -lm

clean:
	find . -name "*.o" -type f -delete
