#ifndef DO_GET_CODE_HEADER
#define DO_GET_CODE_HEADER

#include <string>
#include <boost/filesystem.hpp>

namespace thewizardplusplus {
namespace language_do {

auto GetCode(
	const boost::filesystem::path& interpreter_base_path,
	const boost::filesystem::path& filename
) -> std::string;

}
}
#endif
