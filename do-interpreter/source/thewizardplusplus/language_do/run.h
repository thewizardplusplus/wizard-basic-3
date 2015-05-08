#ifndef DO_RUN_HEADER
#define DO_RUN_HEADER

#include "utils.h"
#include <boost/filesystem.hpp>

namespace thewizardplusplus {
namespace language_do {

int Run(
	const std::string& ansi_c,
	const boost::filesystem::path& interpreter_base_path,
	const boost::filesystem::path& output_file,
	const StringGroup& script_file
);

}
}
#endif
