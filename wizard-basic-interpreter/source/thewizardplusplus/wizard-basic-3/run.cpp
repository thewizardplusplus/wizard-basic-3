#include "run.h"
#include <algorithm>
#include <iostream>
#include <boost/regex.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/format.hpp>

using namespace boost;
using namespace boost::algorithm;

namespace thewizardplusplus {
namespace wizard_basic_3 {

auto EscapeString(const std::string& string) -> std::string {
	auto escaped_string = string;
	escaped_string = regex_replace(escaped_string, regex("\\\\"), "\\\\");
	escaped_string = regex_replace(escaped_string, regex("\a"), "\\a");
	escaped_string = regex_replace(escaped_string, regex("\b"), "\\b");
	escaped_string = regex_replace(escaped_string, regex("\f"), "\\f");
	escaped_string = regex_replace(escaped_string, regex("\n"), "\\n");
	escaped_string = regex_replace(escaped_string, regex("\r"), "\\r");
	escaped_string = regex_replace(escaped_string, regex("\t"), "\\t");
	escaped_string = regex_replace(escaped_string, regex("\v"), "\\v");
	escaped_string = regex_replace(escaped_string, regex("\'"), "\\\'");
	escaped_string = regex_replace(escaped_string, regex("\""), "\\\"");
	escaped_string = regex_replace(escaped_string, regex("\\\?"), "\\?");

	return escaped_string;
}

void Run(
	const std::string& ansi_c,
	const CommandLineArguments& command_line_arguments
) {
	auto script_arguments = StringGroup();
	std::transform(
		command_line_arguments.script_arguments.begin(),
		command_line_arguments.script_arguments.end(),
		std::back_inserter(script_arguments),
		EscapeString
	);
	script_arguments.insert(
		script_arguments.begin(),
		EscapeString(command_line_arguments.script_file.string())
	);

	const auto code =
		(format(
			"#include \"runtime/runtime.h\"\n"
			"#include \"runtime/structure_storage.h\"\n"
			"%s"
			"int main(){"
				"const char*command_line_arguments[]={\"%s\"};"
				"__Start("
					"command_line_arguments,"
					"sizeof(command_line_arguments)"
						"/sizeof(command_line_arguments[0])"
				");"
			"}"
		) % ansi_c % join(script_arguments, "\",\"")).str();
	std::cout << code << "\n";

	/*const auto source_filename = std::string(std::tmpnam(NULL)) + ".c";
	std::ofstream out(filename.c_str());
	if (!out) {
		throw std::runtime_error(
			(format(R"(unable to open ANSI C file "%s".)")
				% source_filename).str()
		);
	}
	out << ansi_c;
	out.close();

	const auto output_filename = std::string(std::tmpnam(NULL));
	std::string command =
		"gcc -std=c99 -O2 -o "
		+ output_filename
		+ " "
		+ input_filename;
	int status_code = std::system(command.c_str());
	if (status_code != 0) {
		throw std::runtime_error("assembling or linking error");
	}

	std::remove(input_filename.c_str());*/
}

}
}
