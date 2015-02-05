SOURCE = source/thewizardplusplus/wizard-basic-3/
OBJECTS = \
	$(SOURCE)main.o \
	$(SOURCE)process_command_line_arguments.o \
	$(SOURCE)get_code.o \
	$(SOURCE)parse.o \
	$(SOURCE)translate.o
CXXFLAGS += -std=c++11 -Wpedantic -Wall -Wextra -O2 \
	-I../../wizard-parser/build/headers/
LDFLAGS += \
	-lboost_program_options \
	-lboost_filesystem \
	-lboost_system \
	-lboost_regex \
	-L../../wizard-parser/build/libraries/wizard_parser/ -lWizardParser

main: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o wbi $(OBJECTS) $(LDFLAGS)

clean:
	find . -name "*.o" -type f -delete
