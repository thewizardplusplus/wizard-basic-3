#include "translate.h"
#include <algorithm>
#include <numeric>
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
		if (!ast.children.empty()) {
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
		} else {
			return "CreateArray(0)";
		}
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

static auto TranslateStatementList(const Node& ast) -> std::string {
	return std::accumulate(
		ast.children.begin(),
		ast.children.end(),
		std::string(),
		[] (const std::string& code, const Node& node) -> std::string {
			if (node.name == "variable_definition") {
				const auto expression = TranslateExpression(
					node.children.front()
				);
				return
					code
					+ (format("Value %s=%s;") % node.value % expression).str();
			} else if (node.name == "assignment") {
				const auto right_expression = TranslateExpression(
					node.children.back()
				);

				const auto first_child = node.children.front();
				if (first_child.name == "identifier") {
					return
						code
						+ (format("%s=%s;")
							% first_child.value
							% right_expression).str();
				} else if (first_child.name == "accessor") {
					const auto left_expression = TranslateExpression(
						first_child.children.front()
					);

					const auto second_subchild = first_child.children.back();
					const auto first_subsubchild =
						second_subchild
						.children
						.front();
					if (second_subchild.name == "item_access") {
						const auto index = TranslateExpression(
							first_subsubchild
						);
						return
							code
							+ (format("SetArrayItem(%s,%s,%s);")
								% left_expression
								% index
								% right_expression).str();
					} else {
						return
							code
							+ (format(R"(SetStructureField(%s,"%s",%s);)")
								% left_expression
								% first_subsubchild.value
								% right_expression).str();
					}
				} else {
					throw std::runtime_error("invalid l-value");
				}
			} else if (node.name == "condition") {
				auto child = node.children.begin();
				const auto condition = TranslateExpression(*child++);
				const auto true_body = TranslateStatementList(*child++);
				if (node.children.size() == 2) {
					return
						code
						+ (format("if(ToBoolean(%s)){%s}")
							% condition
							% true_body).str();
				} else {
					const auto false_body = TranslateStatementList(*child++);
					return
						code
						+ (format("if(ToBoolean(%s)){%s}else{%s}")
							% condition
							% true_body
							% false_body).str();
				}
			} else if (node.name == "loop") {
				const auto condition = TranslateExpression(
					node.children.front()
				);
				const auto body = TranslateStatementList(node.children.back());
				return
					code
					+ (format("while(ToBoolean(%s)){%s}")
						% condition
						% body).str();
			} else if (node.name == "loop_continue") {
				return code + "continue;";
			} else if (node.name == "loop_break") {
				return code + "break;";
			} else if (node.name == "function_return") {
				const auto expression =
					node.children.size() == 1
						? TranslateExpression(node.children.front())
						: "";
				return code + (format("return %s;") % expression).str();
			}

			return "";
		}
	);
}

auto Translate(const Node& ast) -> std::string {
	auto global_variables_declarations = std::string();
	auto global_variables_initializations = std::string();
	auto functions_declarations = std::string(
		"void InitializeGlobalVariables();"
	);
	auto functions_implementations = std::string();

	std::for_each(
		ast.children.begin(),
		ast.children.end(),
		[&] (const Node& node) -> std::string {
			if (node.name == "variable_definition") {
				global_variables_declarations +=
					(format("Value %s;") % node.value).str();

				const auto expression = TranslateExpression(
					node.children.front()
				);
				global_variables_initializations +=
					(format("%s=%s;") % node.value % expression).str();
			} else if (node.name == "structure_declaration") {

			} else if (node.name == "function_declaration") {
				const auto first_child = node.children.front();
				const auto function_arguments = std::accumulate(
					first_child.children.begin(),
					first_child.children.end(),
					std::string(),
					[] (
						const std::string& function_argument_list,
						const Node& node
					) -> std::string {
						return
							function_argument_list
							+ (!function_argument_list.empty() ? "," : "")
							+ (format("const Value %s") % node.value).str();
					}
				);
				const auto function_declaration =
					(format("Value %s(%s)")
						% node.value
						% function_arguments).str();

				functions_declarations += function_declaration + ";";

				const auto statement_list = TranslateStatementList(
					node.children.back()
				);
				functions_implementations +=
					(format("%s{%s}")
						% function_declaration
						% statement_list).str();
			}

			return "";
		}
	);

	return
		global_variables_declarations
		+ functions_declarations
		+ functions_implementations
		+ (format("void InitializeGlobalVariables(){%s}")
			% global_variables_initializations).str();
}

}
}
