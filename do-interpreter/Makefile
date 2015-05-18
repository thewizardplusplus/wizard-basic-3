SOURCE = source/thewizardplusplus/language_do/
OBJECTS = \
	$(SOURCE)main.o \
	$(SOURCE)utils/version.o \
	$(SOURCE)arguments/arguments.o \
	$(SOURCE)preprocessor/get_code.o \
	$(SOURCE)parser/parse.o \
	$(SOURCE)translator/translate.o \
	$(SOURCE)runner/run.o
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
