#ifndef DO_PREPROCESSOR_PREPROCESSOR_HEADER
#define DO_PREPROCESSOR_PREPROCESSOR_HEADER

#include <string>

namespace thewizardplusplus {
namespace language_do {
namespace preprocessor {

auto GetCode(
	const std::string& interpreter_base_path,
	const std::string& filename
) -> std::string;

}
}
}
#endif
