#include <iostream>
#include <boost/program_options.hpp>

using namespace boost::program_options;

void ProcessCommandLineArguments(int number_of_arguments, char* arguments[]) {
	options_description interpreter_arguments_description("Options");
	interpreter_arguments_description.add_options()
		("version,v", "- show version;")
		("help,h", "- show help;")
		(
			"final-stage,s",
			value<std::string>(),
			"- set final stage ("
				R"(allowed: "preprocessed-code", "ast" and "llvm-ir")"
			")."
		);

	variables_map arguments_map;
	store(
		command_line_parser(number_of_arguments, arguments)
			.options(interpreter_arguments_description)
			.run(),
		arguments_map
	);
	notify(arguments_map);

	if (arguments_map.count("version")) {
		std::cout
			<< "Wizard Basic 3 interpreter, v1.0\n"
				"(c) thewizardplusplus, 2014\n";
		std::exit(EXIT_SUCCESS);
	}
	if (arguments_map.count("help")) {
		std::cout
			<< "Usage: wb3i [options] <script-file> [<script-arguments>]\n"
			<< interpreter_arguments_description;
		std::exit(EXIT_SUCCESS);
	}
}

int main(int number_of_arguments, char* arguments[]) {
	ProcessCommandLineArguments(number_of_arguments, arguments);
}
