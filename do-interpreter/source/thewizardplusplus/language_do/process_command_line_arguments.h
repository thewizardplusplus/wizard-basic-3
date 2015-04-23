#ifndef WIZARD_BASIC_3_PROCESS_COMMAND_LINE_ARGUMENTS_HEADER
#define WIZARD_BASIC_3_PROCESS_COMMAND_LINE_ARGUMENTS_HEADER

#include "utils.h"
#include <boost/filesystem.hpp>

namespace thewizardplusplus {
namespace language_do {

enum class FinalStage : uint8_t {
	NONE,
	CODE,
	AST,
	ANSI_C
};
struct CommandLineArguments {
	boost::filesystem::path interpreter_base_path;
	FinalStage final_stage = FinalStage::NONE;
	boost::filesystem::path output_file;
	bool use_output;
	boost::filesystem::path script_file;
	StringGroup script_arguments;
};

auto ProcessCommandLineArguments(
	const int number_of_arguments,
	char* arguments[]
) -> CommandLineArguments;

}
}
#endif
