#include "get_code.h"
#include <fstream>
#include <boost/format.hpp>
#include <boost/regex.hpp>

using namespace thewizardplusplus::wizard_basic_3;
using namespace boost;

static const auto INCLUDE_OPERATOR_PATTERN = regex(
	R"pattern(\binclude\s*"((?:\\.|[^"])*)")pattern"
);

std::string CleanMatchAndGetCode(const smatch& match) {
	return GetCode(match[1]);
}

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

	return regex_replace(code, INCLUDE_OPERATOR_PATTERN, CleanMatchAndGetCode);
}

}
}
