#ifndef WIZARD_BASIC_3_RUN_HEADER
#define WIZARD_BASIC_3_RUN_HEADER

#include "process_command_line_arguments.h"
#include <string>

namespace thewizardplusplus {
namespace wizard_basic_3 {

void Run(
	const std::string& ansi_c,
	const CommandLineArguments& command_line_arguments
);

}
}
#endif
