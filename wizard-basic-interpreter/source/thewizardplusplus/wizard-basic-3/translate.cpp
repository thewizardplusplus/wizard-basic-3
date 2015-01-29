#include "translate.h"
#include <algorithm>
#include <iostream>
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
	} else if (ast.name == "accessor") {
		const auto base = TranslateExpression(ast.children.front());

		const auto second_child = ast.children.back();
		const auto first_subchild = second_child.children.front();
		if (second_child.name == "item_access") {
			const auto index = TranslateExpression(first_subchild);
			return (format("GetArrayItem(%s,%s)") % base % index).str();
		} else {
			return (format(R"(GetStructureField(%s,"%s"))")
				% base
				% first_subchild.value).str();
		}
	} else if (ast.name == "unary") {
		const auto first_child = ast.children.front();
		const auto second_child = ast.children.back();
		if (first_child.value == "new") {
			return (format("CreateStructure(%s)") % second_child.value).str();
		} else {
			auto expression = TranslateExpression(second_child);

			auto children = NodeGroup();
			std::reverse_copy(
				first_child.children.begin(),
				first_child.children.end(),
				std::back_inserter(children)
			);

			for (const auto& subchild: children) {
				const auto expression_format =
					subchild.value == "-"
						? "UnaryMinus(%s)"
						: "Not(%s)";
				expression = (format(expression_format) % expression).str();
			}

			return expression;
		}
	} else if (
		ast.name == "disjunction"
		|| ast.name == "conjunction"
		|| ast.name == "equality"
		|| ast.name == "comparison"
		|| ast.name == "sum"
		|| ast.name == "product"
	) {
		const auto expression_left = TranslateExpression(ast.children.front());
		const auto expression_right = TranslateExpression(ast.children.back());

		auto function = std::string();
		if (ast.name == "disjunction") {
			function = "Or";
		} else if (ast.name == "conjunction") {
			function = "And";
		} else {
			if (ast.value == "==") {
				function = "Equal";
			} else if (ast.value == "/=") {
				function = "NotEqual";
			} else if (ast.value == "<") {
				function = "Less";
			} else if (ast.value == "<=") {
				function = "LessOrEqual";
			} else if (ast.value == ">") {
				function = "Greater";
			} else if (ast.value == ">=") {
				function = "GreaterOrEqual";
			} else if (ast.value == "+") {
				function = "Add";
			} else if (ast.value == "-") {
				function = "Subtract";
			} else if (ast.value == "*") {
				function = "Multiply";
			} else if (ast.value == "/") {
				function = "Divide";
			} else {
				function = "Modulo";
			}
		}

		return (format("%s(%s,%s)")
			% function
			% expression_left
			% expression_right).str();
	}

	return "";
}

auto Translate(const Node& ast) -> std::string {
	return TranslateExpression(ast);
}

}
}
