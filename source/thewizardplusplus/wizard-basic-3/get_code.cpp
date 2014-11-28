#include "get_code.h"
#include <set>
#include <fstream>
#include <boost/regex.hpp>
#include <boost/format.hpp>

using namespace thewizardplusplus::wizard_basic_3;
using namespace boost;
using namespace boost::filesystem;

const auto LIBRARIES_PATH = std::string("libraries");
const auto INCLUDE_OPERATOR_PATTERN = regex(
	R"pattern(\binclude\s*"((?:\\.|[^"])*)")pattern" //"
);

struct CodeGetter {
	const path script_base_path;

	std::string operator()(const smatch& match) const {
		const auto including_path = std::string(match[1]);
		const auto path_relative_to_interpreter =
			path(initial_path()).append(LIBRARIES_PATH).append(including_path);
		if (exists(path_relative_to_interpreter)) {
			return GetCode(path_relative_to_interpreter);
		} else {
			return GetCode(path(script_base_path).append(including_path));
		}
	}
};

namespace thewizardplusplus {
namespace wizard_basic_3 {

auto GetCode(const path& filename) -> std::string {
	static auto included_files = std::set<path>();

	const auto absolute_path = canonical(filename);
	if (included_files.count(absolute_path)) {
		return "";
	}
	included_files.insert(absolute_path);

	std::ifstream file;
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	auto code = std::string();
	try {
		file.open(filename.string());
		code = std::string(
			std::istreambuf_iterator<char>(file),
			std::istreambuf_iterator<char>()
		);
	} catch(const std::ifstream::failure& exception) {
		throw std::runtime_error(
			(format(R"(unable to read script %s (%s))")
				% filename
				% exception.what()).str()
		);
	}

	return regex_replace(
		code,
		INCLUDE_OPERATOR_PATTERN,
		CodeGetter{filename.parent_path()}
	);
}

}
}
