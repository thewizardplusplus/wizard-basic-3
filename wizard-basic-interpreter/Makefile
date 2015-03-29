SOURCE = source/thewizardplusplus/wizard-basic-3/
OBJECTS = \
	$(SOURCE)main.o \
	$(SOURCE)version.o \
	$(SOURCE)process_command_line_arguments.o \
	$(SOURCE)get_code.o \
	$(SOURCE)parse.o \
	$(SOURCE)translate.o \
	$(SOURCE)run.o
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
	$(CXX) $(CXXFLAGS) -o $(BUILD)wb3i $(OBJECTS) $(LDFLAGS)

clean:
	find . -name "*.o" -type f -delete
