#include "get_code.h"
#include <fstream>
#include <boost/format.hpp>

using namespace boost;

namespace thewizardplusplus {
namespace wizard_basic_3 {

auto GetCode(const std::string& filename) -> std::string {
	std::ifstream file(filename.c_str());
	if ((file.rdstate() & std::ifstream::failbit) != 0) {
		throw std::runtime_error(
			(format(R"(unable to open file "%s")") % filename).str()
		);
	}

	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	auto code = std::string();
	try {
		code = std::string(
			std::istreambuf_iterator<char>(file),
			std::istreambuf_iterator<char>()
		);
	} catch(const std::ifstream::failure& exception) {
		throw std::runtime_error(
			(format(R"(unable to read file "%s" (%s))")
				% filename
				% exception.what()).str()
		);
	}

	return code;
}

}
}
