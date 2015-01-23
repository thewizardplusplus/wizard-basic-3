#ifndef WIZARD_BASIC_3_PROCESS_COMMAND_LINE_ARGUMENTS_HEADER
#define WIZARD_BASIC_3_PROCESS_COMMAND_LINE_ARGUMENTS_HEADER

#include <string>
#include <vector>
#include <boost/filesystem.hpp>

namespace thewizardplusplus {
namespace wizard_basic_3 {

enum class FinalStage : uint8_t {
	NONE,
	CODE,
	AST,
	ANSI_C
};
using StringGroup = std::vector<std::string>;
struct CommandLineArguments {
	FinalStage final_stage = FinalStage::NONE;
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
