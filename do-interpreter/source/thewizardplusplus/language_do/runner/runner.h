#ifndef DO_RUNNER_RUNNER_HEADER
#define DO_RUNNER_RUNNER_HEADER

#include <string>
#include <vector>

namespace thewizardplusplus {
namespace language_do {
namespace runner {

int Run(
	const std::string& c_code,
	const std::string& interpreter_base_path,
	const std::string& output_filename,
	const std::vector<std::string>& script_arguments
);

}
}
}
#endif
