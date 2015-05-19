#include "arguments.h"
#include "../utils/version.h"
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>

using namespace thewizardplusplus::language_do::arguments;
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
auto GetParameters(
	const std::vector<std::string>& original_arguments,
	const variables_map& parsed_arguments
) -> Parameters;
auto GetInterpreterPathOption(
	const std::vector<std::string>& arguments
) -> std::string;
auto GetFinalStageOption(const variables_map& arguments) -> FinalStage;
auto GetOutputFileOption(const variables_map& arguments) -> std::string;
auto GetUseOutputOption(const variables_map& arguments) -> bool;
auto GetScriptFileOption(const variables_map& arguments) -> std::string;
auto GetScriptArgumentOptions(
	const variables_map& arguments
) -> std::vector<std::string>;

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

	return GetParameters(arguments, parsed_arguments);
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
			"script-argument",
			value<std::vector<std::string>>()->composing(),
			"- script argument."
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
		.add("script-argument", POSITIONAL_ARGUMENT_UNLIMITED_REPETITIONS);
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
	if (!arguments.count("version")) {
		return;
	}

	std::cout
		<< "Do interpreter, " << VERSION_STRING << "\n"
		<< "(c) thewizardplusplus, 2015\n";
	std::exit(EXIT_SUCCESS);
}

auto ProcessHelpOption(
	const variables_map& arguments,
	const options_description& arguments_description
) -> void {
	if (!arguments.count("help")) {
		return;
	}

	std::cout
		<< "Usage:\n"
		<< "  doi [options] <script-file> [<script-argument>...]\n"
		<< "\n"
		<< arguments_description;
	std::exit(EXIT_SUCCESS);
}

auto GetParameters(
	const std::vector<std::string>& original_arguments,
	const variables_map& parsed_arguments
) -> Parameters {
	auto parameters = Parameters();
	parameters.interpreter_path = GetInterpreterPathOption(original_arguments);
	parameters.final_stage = GetFinalStageOption(parsed_arguments);
	parameters.output_file = GetOutputFileOption(parsed_arguments);
	parameters.use_output = GetUseOutputOption(parsed_arguments);
	parameters.script_file = GetScriptFileOption(parsed_arguments);
	parameters.script_arguments = GetScriptArgumentOptions(parsed_arguments);

	return parameters;
}

auto GetInterpreterPathOption(
	const std::vector<std::string>& arguments
) -> std::string {
	return path(arguments[0]).parent_path().string();
}

auto GetFinalStageOption(const variables_map& arguments) -> FinalStage {
	if (!arguments.count("final-stage")) {
		return FinalStage::NONE;
	}

	auto final_stage = FinalStage::NONE;
	const auto string_final_stage = arguments["final-stage"].as<std::string>();
	if (string_final_stage == "code") {
		final_stage = FinalStage::CODE;
	} else if (string_final_stage == "ast") {
		final_stage = FinalStage::AST;
	} else if (string_final_stage == "c") {
		final_stage = FinalStage::C;
	} else {
		throw std::runtime_error(
			(format(R"(unknown final stage "%s")") % string_final_stage).str()
		);
	}

	return final_stage;
}

auto GetOutputFileOption(const variables_map& arguments) -> std::string {
	if (!arguments.count("output-file")) {
		return "";
	}

	return arguments["output-file"].as<std::string>();
}

auto GetUseOutputOption(const variables_map& arguments) -> bool {
	return arguments.count("use-output") && arguments.count("output-file");
}

auto GetScriptFileOption(const variables_map& arguments) -> std::string {
	if (!arguments.count("script-file")) {
		throw std::runtime_error("script file not specified");
	}

	return arguments["script-file"].as<std::string>();
}

auto GetScriptArgumentOptions(
	const variables_map& arguments
) -> std::vector<std::string> {
	if (!arguments.count("script-argument")) {
		return {};
	}

	return arguments["script-argument"].as<std::vector<std::string>>();
}

}
