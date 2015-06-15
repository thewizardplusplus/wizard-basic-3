#include "runner.h"
#include "os.h"
#include <fstream>
#include <boost/algorithm/string/join.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#ifdef WIZARD_BASIC_3_OS_LINUX
	#include <sys/wait.h>
#endif

using namespace boost;
using namespace boost::algorithm;
using namespace boost::filesystem;

namespace thewizardplusplus {
namespace wizard_basic_3 {
namespace runner {

int Run(
	const std::string& c_code,
	const std::string& interpreter_base_path,
	const std::string& output_filename,
	const std::vector<std::string>& script_arguments
) {
	const auto full_code =
		(format(
			"#include <runtime/runtime.h>\n"
			"#include <runtime/allocator.h>\n"
			"#include <runtime/structure_storage.h>\n"
			"%s"
			"int main(){"
				"const char*command_line_arguments[]={\"%s\"};"
				"int exit_code=__Start("
					"command_line_arguments,"
					"sizeof(command_line_arguments)"
						"/sizeof(command_line_arguments[0])"
				");"
				"return exit_code;"
			"}"
		)
			% c_code
			% join(script_arguments, "\",\"")).str();

	const auto source_filename = std::string(std::tmpnam(NULL)) + ".c";
	std::ofstream out(source_filename.c_str());
	if (!out) {
		throw std::runtime_error(
			(format(R"(unable to open ANSI C file "%s".)")
				% source_filename).str()
		);
	}
	out << full_code;
	out.close();

	const auto final_output_filename =
		!output_filename.empty()
			? output_filename
			: std::tmpnam(NULL);
	const auto command =
		(format("gcc -std=c99 -O2 -o %s %s -I%s -lm -lgc")
			% final_output_filename
			% source_filename
			% interpreter_base_path).str();
	const auto compiling_status_code = std::system(command.c_str());
	std::remove(source_filename.c_str());
	if (compiling_status_code != 0) {
		throw std::runtime_error("compiling failure");
	}

	auto executing_status_code = std::system(final_output_filename.c_str());
	#ifdef WIZARD_BASIC_3_OS_LINUX
		executing_status_code =
			WIFEXITED(executing_status_code)
				? WEXITSTATUS(executing_status_code)
				: EXIT_FAILURE;
	#endif
	if (output_filename.empty()) {
		std::remove(final_output_filename.c_str());
	}

	return executing_status_code;
}

}
}
}
