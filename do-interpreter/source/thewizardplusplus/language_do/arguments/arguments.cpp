#include "arguments.h"
#include "../utils/version.h"
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>

using namespace thewizardplusplus::language_do::utils;
using namespace boost;
using namespace boost::program_options;
using namespace boost::filesystem;

const auto POSITIONAL_ARGUMENT_SIGNLE_REPETITION = 1;
const auto POSITIONAL_ARGUMENT_UNLIMITED_REPETITIONS = -1;

namespace {

auto MakeArgumentsDescription() -> options_description;
auto ParseArguments(
	const std::vector<std::string>& arguments,
	const options_description& arguments_description
) -> variables_map;
auto MakePositionalArgumentsDescription() -> positional_options_description;
auto MakeParser(
	const std::vector<std::string>& arguments,
	const options_description& arguments_description,
	const positional_options_description& positional_arguments_description
) -> command_line_parser;
auto RunParser(command_line_parser& parser) -> variables_map;

auto ProcessVersionOption(const variables_map& arguments) -> void;
auto ProcessHelpOption(
	const variables_map& arguments,
	const options_description& arguments_description
) -> void;

}

namespace thewizardplusplus {
namespace language_do {
namespace arguments {

auto ProcessArguments(const std::vector<std::string>& arguments) -> Parameters {
	const auto arguments_description = MakeArgumentsDescription();
	const auto parsed_arguments = ParseArguments(
		arguments,
		arguments_description
	);

	ProcessVersionOption(parsed_arguments);
	ProcessHelpOption(parsed_arguments, arguments_description);

	auto parameters = Parameters();
	parameters.interpreter_base_path =
		path(arguments[0]).parent_path().string();

	if (parsed_arguments.count("final-stage")) {
		const auto final_stage = parsed_arguments["final-stage"].as<std::string>();
		if (final_stage == "code") {
			parameters.final_stage = FinalStage::CODE;
		} else if (final_stage == "ast") {
			parameters.final_stage = FinalStage::AST;
		} else if (final_stage == "c") {
			parameters.final_stage = FinalStage::C;
		} else {
			throw std::runtime_error(
				(format(R"(unknown final stage "%s")") % final_stage).str()
			);
		}
	}

	if (parsed_arguments.count("output-file")) {
		parameters.output_file = parsed_arguments["output-file"].as<std::string>();
	}
	parameters.use_output =
		parsed_arguments.count("use-output") && parsed_arguments.count("output-file");

	if (parsed_arguments.count("script-file")) {
		parameters.script_file = parsed_arguments["script-file"].as<std::string>();
	} else {
		throw std::runtime_error("script file not specified");
	}

	if (parsed_arguments.count("script-arguments")) {
		parameters.script_arguments =
			parsed_arguments["script-arguments"].as<std::vector<std::string>>();
	}

	return parameters;
}

}
}
}

namespace {

auto MakeArgumentsDescription() -> options_description {
	auto description = options_description("Options");
	description.add_options()
		("version,v", "- show version;")
		("help,h", "- show help;")
		(
			"final-stage,f",
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
			value<std::vector<std::string>>()->composing(),
			"- script arguments."
		);

	return description;
}

auto ParseArguments(
	const std::vector<std::string>& arguments,
	const options_description& arguments_description
) -> variables_map {
	// должно быть создано здесь, т. к. время жизни должно включать время работы
	// RunParser()
	const auto positional_arguments_description =
		MakePositionalArgumentsDescription();
	auto parser = MakeParser(
		arguments,
		arguments_description,
		positional_arguments_description
	);
	return RunParser(parser);
}

auto MakePositionalArgumentsDescription() -> positional_options_description {
	return
		positional_options_description()
		.add("script-file", POSITIONAL_ARGUMENT_SIGNLE_REPETITION)
		.add("script-arguments", POSITIONAL_ARGUMENT_UNLIMITED_REPETITIONS);
}

auto MakeParser(
	const std::vector<std::string>& arguments,
	const options_description& arguments_description,
	const positional_options_description& positional_arguments_description
) -> command_line_parser {
	return
		command_line_parser(
			std::vector<std::string>(
				std::begin(arguments) + 1,
				std::end(arguments)
			)
		)
		.options(arguments_description)
		.positional(positional_arguments_description);
}

auto RunParser(command_line_parser& parser) -> variables_map {
	auto arguments = variables_map();
	const auto result = parser.run();
	store(result, arguments);

	notify(arguments);
	return arguments;
}

auto ProcessVersionOption(const variables_map& arguments) -> void {
	if (arguments.count("version")) {
		std::cout
			<< "Do interpreter, " << VERSION_STRING << "\n"
			<< "(c) thewizardplusplus, 2015\n";
		std::exit(EXIT_SUCCESS);
	}
}

auto ProcessHelpOption(
	const variables_map& arguments,
	const options_description& arguments_description
) -> void {
	if (arguments.count("help")) {
		std::cout
			<< "Usage: do [options] <script-file> [<script-arguments>]\n"
			<< arguments_description;
		std::exit(EXIT_SUCCESS);
	}
}

}
