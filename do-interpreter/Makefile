SOURCE = source/thewizardplusplus/language_do/
OBJECTS = \
	$(SOURCE)main.o \
	$(SOURCE)arguments/arguments.o \
	$(SOURCE)preprocessor/preprocessor.o \
	$(SOURCE)parser/parser.o \
	$(SOURCE)translator/translator.o \
	$(SOURCE)runner/runner.o
BUILD = ../build/
CXXFLAGS += -std=c++11 -Wpedantic -Wall -Wextra -O2
LDFLAGS += \
	-lboost_program_options \
	-lboost_filesystem \
	-lboost_system \
	-lboost_regex \
	-lboost_random \
	-lWizardParser

.PHONY: clean

main: $(OBJECTS)
	mkdir -p $(BUILD)
	$(CXX) $(CXXFLAGS) -o $(BUILD)doi $(OBJECTS) $(LDFLAGS)

clean:
	find . -name "*.o" -type f -delete
