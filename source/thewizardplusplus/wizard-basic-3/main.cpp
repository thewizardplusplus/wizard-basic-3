#include "process_command_line_arguments.h"
#include "get_code.h"
#include "parse.h"
#include "translate.h"
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
	if (command_line_arguments.final_stage == FinalStage::CODE) {
		std::cout << code << '\n';
		std::exit(EXIT_SUCCESS);
	}

	const auto ast = Parse(code);
	if (command_line_arguments.final_stage == FinalStage::AST) {
		std::cout << ast << '\n';
		std::exit(EXIT_SUCCESS);
	}

	const auto ir = Translate(ast);
	if (command_line_arguments.final_stage == FinalStage::IR) {
		std::cout << ir << '\n';
		std::exit(EXIT_SUCCESS);
	}
} catch (const std::exception& exception) {
	std::cerr << (format("Error: %s.\n") % exception.what()).str();
}
