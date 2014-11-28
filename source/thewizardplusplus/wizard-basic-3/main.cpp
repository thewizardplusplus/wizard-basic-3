#include "process_command_line_arguments.h"
#include "get_code.h"
#include <iostream>
#include <boost/format.hpp>

using namespace thewizardplusplus::wizard_basic_3;
using namespace boost;

int main(int number_of_arguments, char* arguments[]) try {
	const auto command_line_arguments = ProcessCommandLineArguments(
		number_of_arguments,
		arguments
	);
	const auto code = GetCode(command_line_arguments.script_file);
	std::cout << code << '\n';
} catch (const std::exception& exception) {
	std::cerr << (format("Error: %s.\n") % exception.what()).str();
}
