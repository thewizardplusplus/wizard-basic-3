#ifndef WIZARD_BASIC_3_GET_CODE_HEADER
#define WIZARD_BASIC_3_GET_CODE_HEADER

#include <string>
#include <boost/filesystem.hpp>

namespace thewizardplusplus {
namespace wizard_basic_3 {

auto GetCode(
	const boost::filesystem::path& interpreter_base_path,
	const boost::filesystem::path& filename
) -> std::string;

}
}
#endif
