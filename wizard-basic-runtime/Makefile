FLAGS = -Wpedantic -Wall -Wextra -O2
CXXFLAGS += -std=c++11 $(FLAGS)
CFLAGS += -std=c99 $(FLAGS)
LDFLAGS += -lm -lgtest -lpthread

all: tests hash_table_tests io_test

tests: tests.o
	$(CXX) $(CXXFLAGS) -o tests tests.o $(LDFLAGS)

hash_table_tests: hash_table_tests.o
	$(CXX) $(CXXFLAGS) -o hash_table_tests hash_table_tests.o $(LDFLAGS)

io_test: io_test.o
	$(CC) $(CFLAGS) -o io_test io_test.o $(LDFLAGS)

clean:
	find . -name "*.o" -type f -delete
