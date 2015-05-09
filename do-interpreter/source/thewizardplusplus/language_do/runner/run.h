#ifndef DO_RUN_HEADER
#define DO_RUN_HEADER

#include "../utils/utils.h"
#include <boost/filesystem.hpp>

namespace thewizardplusplus {
namespace language_do {
namespace runner {

int Run(
	const std::string& ansi_c,
	const boost::filesystem::path& interpreter_base_path,
	const boost::filesystem::path& output_file,
	const utils::StringGroup& script_file
);

}
}
}
#endif
