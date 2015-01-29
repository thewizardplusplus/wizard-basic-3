#include "translate.h"
#include <boost/format.hpp>

using namespace thewizardplusplus::wizard_parser::node;
using namespace boost;

namespace thewizardplusplus {
namespace wizard_basic_3 {

static auto TranslateExpression(const Node& ast) -> std::string {
	if (ast.name == "null_definition") {
		return "CreateNull()";
	} else if (ast.name == "number") {
		const auto number = ast.value;
		return (format("CreateNumber(%s)") % number).str();
	} else if (ast.name == "identifier") {
		const auto identifier = ast.value;
		return identifier;
	} else if (ast.name == "string_definition") {
		const auto string = ast.value;
		return (format(R"(CreateArrayFromString("%s"))") % string).str();
	} else {
		throw std::runtime_error("unknown expression");
	}
}

auto Translate(const Node& ast) -> std::string {
	return TranslateExpression(ast);
}

}
}
