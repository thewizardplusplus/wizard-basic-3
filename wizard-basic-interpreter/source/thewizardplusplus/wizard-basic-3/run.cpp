#include "run.h"
#include <boost/format.hpp>

using namespace boost;

namespace thewizardplusplus {
namespace wizard_basic_3 {

void Run(const std::string& ansi_c) {
	(void)ansi_c;
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
