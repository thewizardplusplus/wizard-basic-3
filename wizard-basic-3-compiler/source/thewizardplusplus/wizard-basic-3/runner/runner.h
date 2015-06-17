#ifndef WIZARD_BASIC_3_RUNNER_RUNNER_HEADER
#define WIZARD_BASIC_3_RUNNER_RUNNER_HEADER

#include <string>
#include <vector>

namespace thewizardplusplus {
namespace wizard_basic_3 {
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
