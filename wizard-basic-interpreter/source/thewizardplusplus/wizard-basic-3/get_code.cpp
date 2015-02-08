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
	R"pattern(\binclude\s*"((?:\\.|[^"])*)")pattern"
	// comment with closing quote for bugfix stupid syntax highlighter in Atom
	// "
);

struct CodeGetter {
	const path interpreter_base_path;
	const path script_base_path;

	std::string operator()(const smatch& match) const {
		const auto including_path = std::string(match[1]);

		auto path_relative_to_interpreter = path(interpreter_base_path);
		path_relative_to_interpreter /= LIBRARIES_PATH;
		path_relative_to_interpreter /= including_path;

		if (exists(path_relative_to_interpreter)) {
			return GetCode(interpreter_base_path, path_relative_to_interpreter);
		} else {
			auto path_relative_to_script = path(script_base_path);
			path_relative_to_script /= including_path;

			return GetCode(interpreter_base_path, path_relative_to_script);
		}
	}
};

namespace thewizardplusplus {
namespace wizard_basic_3 {

auto GetCode(
	const path& interpreter_base_path,
	const path& filename
) -> std::string {
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
		CodeGetter{interpreter_base_path, filename.parent_path()}
	);
}

}
}
