#ifndef WIZARD_BASIC_3_RUN_HEADER
#define WIZARD_BASIC_3_RUN_HEADER

#include "utils.h"
#include <boost/filesystem.hpp>

namespace thewizardplusplus {
namespace wizard_basic_3 {

int Run(
	const std::string& ansi_c,
	const boost::filesystem::path& interpreter_base_path,
	const StringGroup& script_file
);

}
}
#endif
