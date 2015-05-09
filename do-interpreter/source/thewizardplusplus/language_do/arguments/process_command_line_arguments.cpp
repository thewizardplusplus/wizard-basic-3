#include "process_command_line_arguments.h"
#include "../utils/version.h"
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/format.hpp>

using namespace thewizardplusplus::language_do::utils;
using namespace boost;
using namespace boost::program_options;
using namespace boost::filesystem;

namespace {

const auto POSITIONAL_ARGUMENT_SIGNLE_REPETITION = 1;
const auto POSITIONAL_ARGUMENT_UNLIMITED_REPETITIONS = -1;

}

namespace thewizardplusplus {
namespace language_do {
namespace arguments {

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
			R"(- set final stage (allowed: "code", "ast" and "c");)"
		)
		(
			"output-file,o",
			value<std::string>(),
			"- output file for compiling result;"
		)
		("use-output,u", "- output any final stage result to output file;")
		("script-file", value<std::string>(), "- script file;")
		(
			"script-arguments",
			value<StringGroup>()->composing(),
			"- script arguments."
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
			<< "Do interpreter, " << VERSION_STRING << "\n"
			<< "(c) thewizardplusplus, 2015\n";
		std::exit(EXIT_SUCCESS);
	}

	if (arguments_map.count("help")) {
		std::cout
			<< "Usage: do [options] <script-file> [<script-arguments>]\n"
			<< interpreter_arguments_description;
		std::exit(EXIT_SUCCESS);
	}

	auto command_line_arguments = CommandLineArguments();
	command_line_arguments.interpreter_base_path =
		path(arguments[0])
		.parent_path();

	if (arguments_map.count("final-stage")) {
		const auto final_stage = arguments_map["final-stage"].as<std::string>();
		if (final_stage == "code") {
			command_line_arguments.final_stage = FinalStage::CODE;
		} else if (final_stage == "ast") {
			command_line_arguments.final_stage = FinalStage::AST;
		} else if (final_stage == "c") {
			command_line_arguments.final_stage = FinalStage::C;
		} else {
			throw std::runtime_error(
				(format(R"(unknown final stage "%s")") % final_stage).str()
			);
		}
	}

	if (arguments_map.count("output-file")) {
		command_line_arguments.output_file =
			arguments_map["output-file"].as<std::string>();
	}
	command_line_arguments.use_output =
		arguments_map.count("use-output")
		&& !command_line_arguments.output_file.string().empty();

	if (arguments_map.count("script-file")) {
		command_line_arguments.script_file =
			arguments_map["script-file"].as<std::string>();
	} else {
		throw std::runtime_error("script file not specified");
	}

	if (arguments_map.count("script-arguments")) {
		command_line_arguments.script_arguments =
			arguments_map["script-arguments"].as<StringGroup>();
	}

	return command_line_arguments;
}

}
}
}
