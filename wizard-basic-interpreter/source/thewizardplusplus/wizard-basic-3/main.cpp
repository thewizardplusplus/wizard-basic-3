#include "process_command_line_arguments.h"
#include "get_code.h"
#include "parse.h"
#include "translate.h"
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/version.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

using namespace thewizardplusplus::wizard_basic_3;
using namespace thewizardplusplus::wizard_parser::node;
using namespace boost;
using namespace boost::property_tree;

const auto XML_INDENT_SYMBOL = ' ';
const auto XML_INDENT_SIZE = 4;

auto FormatAst(const Node& ast) -> std::string {
	std::stringstream source;
	source << ast;

	ptree property_tree;
	read_xml(source, property_tree);

	std::ostringstream out;
	write_xml(
		out,
		property_tree,
		#if BOOST_VERSION >= 105600
			xml_writer_make_settings<ptree::key_type>(
				XML_INDENT_SYMBOL,
				XML_INDENT_SIZE
			)
		#else
			xml_writer_make_settings(XML_INDENT_SYMBOL, XML_INDENT_SIZE)
		#endif
	);

	return out.str();
}

auto FormatAnsiC(const TranslationResult& ansi_c) -> std::string {
	auto structures_descriptions = std::string();
	for (const auto& structure: ansi_c.structures_descriptions) {
		structures_descriptions +=
			(format("// %s:\n") % std::get<0>(structure)).str();
		for (const auto& field: std::get<1>(structure)) {
			structures_descriptions +=
				(format("//     %s;\n") % std::get<0>(field)).str();
		}
		structures_descriptions += "\n";
	}

	auto code = ansi_c.code;
	replace_all(code, "{", " {\n");
	replace_all(code, "}", "}\n");
	replace_all(code, ";", ";\n");
	replace_all(code, ",", ", ");
	replace_all(code, "=", " = ");

	return structures_descriptions + code;
}

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
	ProcessResult<FinalStage::AST>(command_line_arguments, FormatAst(ast));

	const auto ansi_c = Translate(ast);
	ProcessResult<FinalStage::ANSI_C>(
		command_line_arguments,
		FormatAnsiC(ansi_c)
	);
} catch (const std::exception& exception) {
	std::cerr << (format("Error: %s.\n") % exception.what()).str();
}
