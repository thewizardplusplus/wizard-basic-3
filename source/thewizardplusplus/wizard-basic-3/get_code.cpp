#include "get_code.h"
#include <fstream>
#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>

using namespace thewizardplusplus::wizard_basic_3;
using namespace boost;
using namespace boost::filesystem;

static const auto INCLUDE_OPERATOR_PATTERN = regex(
	R"pattern(\binclude\s*"((?:\\.|[^"])*)")pattern"
);

struct CodeGetter {
	std::string base_path;

	std::string operator()(const smatch& match) const {
		return GetCode(base_path + match[1]);
	}
};

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

	auto base_path = path(filename).parent_path().string();
	if (!base_path.empty()) {
		base_path += '/';
	}

	return regex_replace(code, INCLUDE_OPERATOR_PATTERN, CodeGetter{base_path});
}

}
}
