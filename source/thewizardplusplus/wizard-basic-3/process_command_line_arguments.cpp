#include "process_command_line_arguments.h"
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>

using namespace boost;
using namespace boost::program_options;
using namespace boost::filesystem;

static const auto POSITIONAL_ARGUMENT_SIGNLE_REPETITION = 1;
static const auto POSITIONAL_ARGUMENT_UNLIMITED_REPETITIONS = -1;

namespace thewizardplusplus {
namespace wizard_basic_3 {

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
			value<StringGroup>()->composing(),
			"script arguments"
		);

	auto script_arguments_description = positional_options_description();
	script_arguments_description.add(
		"script-file",
		POSITIONAL_ARGUMENT_SIGNLE_REPETITION
	);
	script_arguments_description.add(
		"script-arguments",
		POSITIONAL_ARGUMENT_UNLIMITED_REPETITIONS
	);

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

	command_line_arguments.script_base_path = path(
		command_line_arguments.script_file
	).parent_path().string();
	if (!command_line_arguments.script_base_path.empty()) {
		command_line_arguments.script_base_path += '/';
	}

	if (arguments_map.count("script-arguments")) {
		command_line_arguments.script_arguments =
			arguments_map["script-arguments"].as<StringGroup>();
	}

	return command_line_arguments;
}

}
}
