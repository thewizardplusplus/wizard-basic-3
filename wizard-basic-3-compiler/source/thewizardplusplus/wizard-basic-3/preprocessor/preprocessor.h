#ifndef WIZARD_BASIC_3_PREPROCESSOR_PREPROCESSOR_HEADER
#define WIZARD_BASIC_3_PREPROCESSOR_PREPROCESSOR_HEADER

#include <string>

namespace thewizardplusplus {
namespace wizard_basic_3 {
namespace preprocessor {

auto GetCode(
	const std::string& interpreter_base_path,
	const std::string& filename
) -> std::string;

}
}
}
#endif
