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

namespace thewizardplusplus {
namespace language_do {
namespace arguments {

auto ProcessArguments(const std::vector<std::string>& arguments) -> Parameters {
	auto arguments_description = options_description("Options");
	arguments_description.add_options()
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
	const auto positional_arguments_description =
		positional_options_description()
		.add("script-file", POSITIONAL_ARGUMENT_SIGNLE_REPETITION)
		.add("script-argument", POSITIONAL_ARGUMENT_UNLIMITED_REPETITIONS);

	auto parsed_arguments = variables_map();
	const auto parsing_result =
		command_line_parser(
			std::vector<std::string>(
				std::begin(arguments) + 1,
				std::end(arguments)
			)
		)
		.options(arguments_description)
		.positional(positional_arguments_description)
		.run();
	store(parsing_result, parsed_arguments);
	notify(parsed_arguments);

	if (parsed_arguments.count("version")) {
		std::cout
			<< "Do interpreter, " << VERSION_STRING << "\n"
			<< "(c) thewizardplusplus, 2015\n";
		std::exit(EXIT_SUCCESS);
	}
	if (parsed_arguments.count("help")) {
		std::cout
			<< "Usage:\n"
			<< "  doi [options] <script-file> [<script-argument>...]\n"
			<< "\n"
			<< arguments_description;
		std::exit(EXIT_SUCCESS);
	}

	auto parameters = Parameters();
	parameters.interpreter_path = path(arguments[0]).parent_path().string();
	if (parsed_arguments.count("final-stage")) {
		const auto final_stage =
			parsed_arguments["final-stage"]
			.as<std::string>();
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
		parameters.output_file =
			parsed_arguments["output-file"]
			.as<std::string>();
	}
	parameters.use_output =
		parsed_arguments.count("use-output")
		&& parsed_arguments.count("output-file");
	if (parsed_arguments.count("script-file")) {
		parameters.script_file =
			parsed_arguments["script-file"]
			.as<std::string>();
	} else {
		throw std::runtime_error("script file not specified");
	}
	if (parsed_arguments.count("script-argument")) {
		parameters.script_arguments =
			parsed_arguments["script-argument"]
			.as<std::vector<std::string>>();
	}

	return parameters;
}

}
}
}
