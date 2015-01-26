OBJECTS = main.o
CFLAGS += -std=c99 -Wpedantic -Wall -Wextra -O2
LDFLAGS += -lm

main: $(OBJECTS)
	$(CC) $(CFLAGS) -o main $(OBJECTS) $(LDFLAGS)

clean:
	find . -name "*.o" -type f -delete
