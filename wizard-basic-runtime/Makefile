BASE_TESTS_SOURCES = tests/
RUNTIME_TESTS_OBJECTS = $(BASE_TESTS_SOURCES)runtime_tests.o
STRUCTURE_STORAGE_TESTS_OBJECTS = $(BASE_TESTS_SOURCES)structure_storage_tests.o
IO_TEST_OBJECTS = $(BASE_TESTS_SOURCES)io_test.o
RUNTIME = ../runtime/
BASE_FLAGS = -Wpedantic -Wall -Wextra -O2
CXXFLAGS += -std=c++11 $(BASE_FLAGS)
CFLAGS += -std=c99 $(BASE_FLAGS)
LDFLAGS += -lm -lgtest -lpthread

.PHONY: runtime clean

runtime:
	$(RM) $(RUNTIME)*
	cp -v source/* $(RUNTIME)

all_tests: runtime_tests structure_storage_tests io_test

runtime_tests: $(RUNTIME_TESTS_OBJECTS)
	$(CXX) $(CXXFLAGS) -o runtime_tests $(RUNTIME_TESTS_OBJECTS) $(LDFLAGS)

structure_storage_tests: $(STRUCTURE_STORAGE_TESTS_OBJECTS)
	$(CXX) $(CXXFLAGS) -o structure_storage_tests $(STRUCTURE_STORAGE_TESTS_OBJECTS) $(LDFLAGS)

io_test: $(IO_TEST_OBJECTS)
	$(CC) $(CFLAGS) -o io_test $(IO_TEST_OBJECTS) $(LDFLAGS)

clean:
	find . -name "*.o" -type f -delete
