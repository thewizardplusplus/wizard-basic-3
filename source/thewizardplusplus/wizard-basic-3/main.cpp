#include "process_command_line_arguments.h"
#include <iostream>
#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>

using namespace thewizardplusplus::wizard_basic_3;
using namespace boost;
using namespace boost::algorithm;

int main(int number_of_arguments, char* arguments[]) try {
	const auto command_line_arguments = ProcessCommandLineArguments(
		number_of_arguments,
		arguments
	);
	switch (command_line_arguments.final_stage) {
		case FinalStage::CODE:
			std::cout << "Final stage: code.\n";
			break;
		case FinalStage::AST:
			std::cout << "Final stage: AST.\n";
			break;
		case FinalStage::IR:
			std::cout << "Final stage: IR.\n";
			break;
		case FinalStage::NONE:
			std::cout << "Final stage: none.\n";
			break;
	}
	std::cout
		<< (format("Script file: \"%s\".\n")
			% command_line_arguments.script_file).str();
	std::cout
		<< (format("Script base path: \"%s\".\n")
			% command_line_arguments.script_base_path).str();
	std::cout
		<< (format("Script arguments: \"%s\".\n")
			% join(command_line_arguments.script_arguments, "\", \"")).str();
} catch (const std::exception& exception) {
	std::cerr << (format("Error: %s.\n") % exception.what()).str();
}
