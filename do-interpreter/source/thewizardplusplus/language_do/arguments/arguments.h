#ifndef DO_ARGUMENTS_ARGUMENTS_HEADER
#define DO_ARGUMENTS_ARGUMENTS_HEADER

#include <string>
#include <vector>

namespace thewizardplusplus {
namespace language_do {
namespace arguments {

enum class FinalStage: uint8_t {
	NONE,
	CODE,
	AST,
	C
};

struct Parameters {
	std::string interpreter_base_path;
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
