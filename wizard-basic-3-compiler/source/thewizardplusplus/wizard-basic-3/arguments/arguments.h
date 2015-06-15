#ifndef WIZARD_BASIC_3_ARGUMENTS_ARGUMENTS_HEADER
#define WIZARD_BASIC_3_ARGUMENTS_ARGUMENTS_HEADER

#include <string>
#include <vector>

namespace thewizardplusplus {
namespace wizard_basic_3 {
namespace arguments {

enum class FinalStage: uint8_t {
	NONE,
	CODE,
	AST,
	C
};

struct Parameters {
	std::string interpreter_path;
	FinalStage final_stage = FinalStage::NONE;
	std::string output_file;
	bool use_output;
	std::string script_file;
	std::vector<std::string> script_arguments;
};

auto ProcessArguments(const std::vector<std::string>& arguments) -> Parameters;

}
}
}
#endif
