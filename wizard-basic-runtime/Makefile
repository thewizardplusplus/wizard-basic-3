OBJECTS = tests.o
CXXFLAGS += -std=c++11 -Wpedantic -Wall -Wextra -O2
LDFLAGS += -lm -lgtest -lpthread

tests: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o tests $(OBJECTS) $(LDFLAGS)

clean:
	find . -name "*.o" -type f -delete
