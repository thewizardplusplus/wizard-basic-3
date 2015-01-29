#include "translate.h"
#include <algorithm>
#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>

using namespace thewizardplusplus::wizard_parser::node;
using namespace boost;
using namespace boost::algorithm;

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
	} else if (ast.name == "array_definition") {
		std::vector<std::string> items;
		std::transform(
			ast.children.begin(),
			ast.children.end(),
			std::back_inserter(items),
			TranslateExpression
		);

		return (format("CreateArrayFromList(%d,%s)")
			% items.size()
			% join(items, ",")).str();
	} else if (ast.name == "function_call") {
		std::vector<std::string> arguments;
		std::transform(
			ast.children.begin(),
			ast.children.end(),
			std::back_inserter(arguments),
			TranslateExpression
		);

		return (format("%s(%s)") % ast.value % join(arguments, ",")).str();
	} else {
		throw std::runtime_error("unknown expression");
	}
}

auto Translate(const Node& ast) -> std::string {
	return TranslateExpression(ast);
}

}
}
