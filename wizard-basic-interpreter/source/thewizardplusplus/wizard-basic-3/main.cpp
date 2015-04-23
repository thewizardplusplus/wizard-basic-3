#include "process_command_line_arguments.h"
#include "get_code.h"
#include "parse.h"
#include "translate.h"
#include "run.h"
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/version.hpp>
#include <boost/format.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

using namespace thewizardplusplus::wizard_basic_3;
using namespace thewizardplusplus::wizard_parser::node;
using namespace boost;
using namespace boost::property_tree;

const auto XML_INDENT_SYMBOL = ' ';
const auto XML_INDENT_SIZE = 4;
const auto ANSI_C_INDENT_STRING = "    ";

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

auto FormatAnsiC(const std::string& ansi_c) -> std::string {
	auto code = ansi_c;
	replace_all(code, ";", ";\n");
	replace_all(code, "{", " {\n");
	code = regex_replace(code, regex(R"(\}(?!else\b))"), "}\n");
	code = regex_replace(code, regex(R"(\}(?=else\b))"), "} ");
	replace_all(code, "=", " = ");
	replace_all(code, ",", ", ");
	code = regex_replace(code, regex(R"(\bif\b)"), "if ");
	code = regex_replace(code, regex(R"(\bwhile\b)"), "while ");

	auto lines = std::vector<std::string>();
	split(lines, code, is_any_of("\n"), token_compress_on);
	auto prefix_size = size_t(0);
	for (auto& line: lines) {
		if (line.empty()) {
			continue;
		}

		if (line.front() == '}') {
			prefix_size--;
		}
		for (size_t i = 0; i < prefix_size; i++) {
			line = ANSI_C_INDENT_STRING + line;
		}
		if (line.back() == '{') {
			prefix_size++;
		}
	}
	code = join(lines, "\n");

	return code;
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

	const auto code = GetCode(
		command_line_arguments.interpreter_base_path,
		command_line_arguments.script_file
	);
	ProcessResult<FinalStage::CODE>(command_line_arguments, code);

	const auto ast = Parse(code);
	ProcessResult<FinalStage::AST>(command_line_arguments, FormatAst(ast));

	const auto ansi_c = Translate(ast);
	ProcessResult<FinalStage::ANSI_C>(
		command_line_arguments,
		FormatAnsiC(ansi_c)
	);

	auto script_arguments = command_line_arguments.script_arguments;
	script_arguments.insert(
		script_arguments.begin(),
		command_line_arguments.script_file.string()
	);
	const auto exit_code = Run(
		ansi_c,
		command_line_arguments.interpreter_base_path,
		script_arguments
	);

	return exit_code;
} catch (const std::exception& exception) {
	std::cerr << (format("Error: %s.\n") % exception.what()).str();
}
