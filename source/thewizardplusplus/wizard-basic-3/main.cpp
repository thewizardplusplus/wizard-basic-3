#include <string>
#include <vector>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>

using namespace boost;
using namespace boost::program_options;
using namespace boost::algorithm;

enum class FinalStage : uint8_t {
	NONE,
	CODE,
	AST,
	IR
};

struct CommandLineArguments {
	FinalStage final_stage = FinalStage::NONE;
	std::string script_file;
	std::vector<std::string> script_arguments;
};

auto ProcessCommandLineArguments(
	const int number_of_arguments,
	char* arguments[]
) -> CommandLineArguments {
	auto interpreter_arguments_description = options_description("Options");
	interpreter_arguments_description.add_options()
		("version,v", "- show version;")
		("help,h", "- show help;")
		(
			"final-stage,s",
			value<std::string>(),
			R"(- set final stage (allowed: "code", "ast" and "ir").)"
		)
		("script-file", value<std::string>(), "script file")
		(
			"script-arguments",
			value<std::vector<std::string>>()->composing(),
			"script arguments"
		);

	auto script_arguments_description = positional_options_description();
	script_arguments_description.add("script-file", 1);
	script_arguments_description.add("script-arguments", -1);

	auto arguments_map = variables_map();
	store(
		command_line_parser(number_of_arguments, arguments)
			.options(interpreter_arguments_description)
			.positional(script_arguments_description)
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

	auto command_line_arguments = CommandLineArguments();
	if (arguments_map.count("final-stage")) {
		const auto final_stage = arguments_map["final-stage"].as<std::string>();
		if (final_stage == "code") {
			command_line_arguments.final_stage = FinalStage::CODE;
		} else if (final_stage == "ast") {
			command_line_arguments.final_stage = FinalStage::AST;
		} else if (final_stage == "ir") {
			command_line_arguments.final_stage = FinalStage::IR;
		} else {
			throw std::runtime_error(
				(format(R"(unknown final stage "%s")") % final_stage).str()
			);
		}
	}
	if (arguments_map.count("script-file")) {
		command_line_arguments.script_file =
			arguments_map["script-file"].as<std::string>();
	} else {
		throw std::runtime_error("script file not specified");
	}
	if (arguments_map.count("script-arguments")) {
		command_line_arguments.script_arguments =
			arguments_map["script-arguments"].as<std::vector<std::string>>();
	}

	return command_line_arguments;
}

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
		<< (format("Script arguments: \"%s\".\n")
			% join(command_line_arguments.script_arguments, "\", \"")).str();
} catch (const std::exception& exception) {
	std::cerr << (format("Error: %s.\n") % exception.what()).str();
}
