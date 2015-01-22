#include "process_command_line_arguments.h"
#include "get_code.h"
#include "parse.h"
#include "translate.h"
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/format.hpp>

using namespace thewizardplusplus::wizard_basic_3;
using namespace boost;
using namespace boost::property_tree;

const auto XML_INDENT_SIZE = 4;

template<FinalStage final_stage, typename ResultType>
void ProcessResult(
	CommandLineArguments command_line_arguments,
	ResultType result
) {
	if (command_line_arguments.final_stage == final_stage) {
		std::cout << result << '\n';
		std::exit(EXIT_SUCCESS);
	}
}

int main(int number_of_arguments, char* arguments[]) try {
	const auto command_line_arguments = ProcessCommandLineArguments(
		number_of_arguments,
		arguments
	);

	const auto code = GetCode(command_line_arguments.script_file);
	ProcessResult<FinalStage::CODE>(command_line_arguments, code);

	const auto ast = Parse(code);
	ProcessResult<FinalStage::AST>(
		command_line_arguments,
		([&] () -> std::string {
			std::stringstream source;
			source << ast;

			ptree property_tree;
			read_xml(source, property_tree);

			std::ostringstream out;
			write_xml(
				out,
				property_tree,
				xml_writer_make_settings<std::string>(' ', XML_INDENT_SIZE)
			);

			return out.str();
		})()
	);

	const auto ir = Translate(ast);
	ProcessResult<FinalStage::IR>(command_line_arguments, ir);
} catch (const std::exception& exception) {
	std::cerr << (format("Error: %s.\n") % exception.what()).str();
}
