#include "translate.h"
#include "utils.h"
#include <algorithm>
#include <numeric>
#include <set>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>

using namespace thewizardplusplus::wizard_parser::node;
using namespace boost;
using namespace boost::algorithm;
using namespace boost::random;

namespace thewizardplusplus {
namespace wizard_basic_3 {

const auto FUNCTION_NAME_LIST = StringGroup{
	"ToString",
	"GetLength",
	"GetType",
	"GetTime",
	"Exit",
	"Read",
	"Write",
	"Open",
	"Close",
	"Sin",
	"Cos",
	"Tg",
	"Arcsin",
	"Arccos",
	"Arctg",
	"SquareRoot",
	"Power",
	"Exp",
	"Ln",
	"Integral",
	"GetRandom",
	"Main"
};
const auto RANDOM_PREFIX_SYMBOLS = std::string(
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"1234567890"
);
const auto RANDOM_PREFIX_LENGTH = 5;

static auto GetRandomPrefix(void) -> std::string {
	random_device device;
	uniform_int_distribution<> index(0, RANDOM_PREFIX_SYMBOLS.size() - 1);

	auto result = std::string("_");
	for(size_t i = 0; i < RANDOM_PREFIX_LENGTH; i++) {
		result += RANDOM_PREFIX_SYMBOLS[index(device)];
	}
	result += "_";

	return result;
}

static auto WrapFunctionName(
	const std::string& name,
	const std::string& prefix
) -> std::string {
	return
		std::find(FUNCTION_NAME_LIST.begin(), FUNCTION_NAME_LIST.end(), name)
		== FUNCTION_NAME_LIST.end()
			? prefix + name
			: name;
}

static auto TranslateExpression(
	const Node& ast,
	const std::string& identify_prefix
) -> std::string {
	if (ast.name == "null_definition") {
		return "__NULL";
	} else if (ast.name == "number") {
		const auto number = ast.value;
		return (format("__CreateNumber(%s)") % number).str();
	} else if (ast.name == "identifier") {
		const auto identifier = ast.value;
		return identify_prefix + identifier;
	} else if (ast.name == "string_definition") {
		auto string = ast.value;
		replace_all(string, "\n", "\\n");
		replace_all(string, "\t", "\\t");

		return (format(R"(__CreateArrayFromString("%s"))") % string).str();
	} else if (ast.name == "array_listed_definition") {
		if (!ast.children.empty()) {
			std::vector<std::string> items;
			std::transform(
				ast.children.begin(),
				ast.children.end(),
				std::back_inserter(items),
				[&] (const Node& node) {
					return TranslateExpression(node, identify_prefix);
				}
			);

			return (format("__CreateArrayFromList(%d,%s)")
				% items.size()
				% join(items, ",")).str();
		} else {
			return "__CreateArray(__CreateNumber(0))";
		}
	} else if (ast.name == "function_call") {
		std::vector<std::string> arguments;
		std::transform(
			ast.children.begin(),
			ast.children.end(),
			std::back_inserter(arguments),
			[&] (const Node& node) {
				return TranslateExpression(node, identify_prefix);
			}
		);

		return
			(format("%s(%s)")
				% WrapFunctionName(ast.value, identify_prefix)
				% join(arguments, ",")).str();
	} else if (ast.name == "array_sized_definition") {
		const auto expression = TranslateExpression(
			ast.children.front(),
			identify_prefix
		);

		return (format("__CreateArray(%s)") % expression).str();
	} else if (ast.name == "accessor") {
		const auto base = TranslateExpression(
			ast.children.front(),
			identify_prefix
		);

		const auto second_child = ast.children.back();
		const auto first_subchild = second_child.children.front();
		if (second_child.name == "item_access") {
			const auto index = TranslateExpression(
				first_subchild,
				identify_prefix
			);
			return (format("__GetArrayItem(%s,%s)") % base % index).str();
		} else {
			return (format(R"(__GetStructureField(%s,"%s"))")
				% base
				% first_subchild.value).str();
		}
	} else if (ast.name == "unary") {
		const auto first_child = ast.children.front();
		const auto second_child = ast.children.back();
		if (first_child.value == "new") {
			return
				(format(R"(__CreateStructure("%s"))")
					% second_child.value).str();
		} else {
			auto expression = TranslateExpression(
				second_child,
				identify_prefix
			);

			auto children = NodeGroup();
			std::reverse_copy(
				first_child.children.begin(),
				first_child.children.end(),
				std::back_inserter(children)
			);

			for (const auto& subchild: children) {
				const auto expression_format =
					subchild.value == "-"
						? "__UnaryMinus(%s)"
						: "__Not(%s)";
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
		const auto expression_left = TranslateExpression(
			ast.children.front(),
			identify_prefix
		);
		const auto expression_right = TranslateExpression(
			ast.children.back(),
			identify_prefix
		);

		auto function = std::string();
		if (ast.name == "disjunction") {
			function = "__Or";
		} else if (ast.name == "conjunction") {
			function = "__And";
		} else {
			if (ast.value == "==") {
				function = "__Equal";
			} else if (ast.value == "/=") {
				function = "__NotEqual";
			} else if (ast.value == "<") {
				function = "__Less";
			} else if (ast.value == "<=") {
				function = "__LessOrEqual";
			} else if (ast.value == ">") {
				function = "__Greater";
			} else if (ast.value == ">=") {
				function = "__GreaterOrEqual";
			} else if (ast.value == "+") {
				function = "__Add";
			} else if (ast.value == "-") {
				function = "__Subtract";
			} else if (ast.value == "*") {
				function = "__Multiply";
			} else if (ast.value == "/") {
				function = "__Divide";
			} else {
				function = "__Modulo";
			}
		}

		return (format("%s(%s,%s)")
			% function
			% expression_left
			% expression_right).str();
	}

	return "";
}

static auto TranslateStatementList(
	const Node& ast,
	const std::string& identify_prefix
) -> std::string {
	return std::accumulate(
		ast.children.begin(),
		ast.children.end(),
		std::string(),
		[&] (const std::string& code, const Node& node) -> std::string {
			if (node.name == "variable_definition") {
				const auto expression = TranslateExpression(
					node.children.front(),
					identify_prefix
				);
				return
					code
					+ (format("__Value %s%s=%s;")
						% identify_prefix
						% node.value
						% expression).str();
			} else if (node.name == "assignment") {
				const auto right_expression = TranslateExpression(
					node.children.back(),
					identify_prefix
				);

				const auto first_child = node.children.front();
				if (first_child.name == "identifier") {
					return
						code
						+ (format("%s%s=%s;")
							% identify_prefix
							% first_child.value
							% right_expression).str();
				} else if (first_child.name == "accessor") {
					const auto left_expression = TranslateExpression(
						first_child.children.front(),
						identify_prefix
					);

					const auto second_subchild = first_child.children.back();
					const auto first_subsubchild =
						second_subchild
						.children
						.front();
					if (second_subchild.name == "item_access") {
						const auto index = TranslateExpression(
							first_subsubchild,
							identify_prefix
						);
						return
							code
							+ (format("__SetArrayItem(%s,%s,%s);")
								% left_expression
								% index
								% right_expression).str();
					} else {
						return
							code
							+ (format(R"(__SetStructureField(%s,"%s",%s);)")
								% left_expression
								% first_subsubchild.value
								% right_expression).str();
					}
				} else {
					throw std::runtime_error("invalid l-value");
				}
			} else if (node.name == "condition") {
				auto result = std::string();
				auto child = node.children.begin();
				do {
					if (child != node.children.begin()) {
						result += "else ";
					}

					const auto condition = TranslateExpression(
						child->children.front(),
						identify_prefix
					);
					const auto body = TranslateStatementList(
						child->children.back(),
						identify_prefix
					);
					result +=
						(format("if(__ToBoolean(%s)){%s}")
							% condition
							% body).str();

					child++;
				} while (child != node.children.end() && child->name.empty());

				if (child != node.children.end() && !child->name.empty()) {
					const auto body = TranslateStatementList(
						*child,
						identify_prefix
					);
					result += (format("else{%s}") % body).str();
				}

				return code + result;
			} else if (node.name == "loop") {
				const auto condition = TranslateExpression(
					node.children.front(),
					identify_prefix
				);
				const auto body = TranslateStatementList(
					node.children.back(),
					identify_prefix
				);
				return
					code
					+ (format("while(__ToBoolean(%s)){%s}")
						% condition
						% body).str();
			} else if (node.name == "loop_continue") {
				return code + "continue;";
			} else if (node.name == "loop_break") {
				return code + "break;";
			} else if (node.name == "function_return") {
				const auto expression =
					node.children.size() == 1
						? TranslateExpression(
							node.children.front(),
							identify_prefix
						)
						: "__NULL";
				return code + (format("return %s;") % expression).str();
			} else {
				const auto expression = TranslateExpression(
					node,
					identify_prefix
				);
				return code + (format("%s;") % expression).str();
			}

			return "";
		}
	);
}

auto Translate(const Node& ast) -> std::string {
	auto structures_registration = std::string();
	auto global_variables_declarations = std::string();
	auto global_variables_initializations = std::string();
	auto functions_declarations = std::string("void __Start();");
	auto functions_implementations = std::string();

	const auto identify_prefix = GetRandomPrefix();
	auto structures = std::set<std::string>();
	std::for_each(
		ast.children.begin(),
		ast.children.end(),
		[&] (const Node& node) -> std::string {
			if (node.name == "variable_definition") {
				global_variables_declarations +=
					(format("__Value %s%s;")
						% identify_prefix
						% node.value).str();

				const auto expression = TranslateExpression(
					node.children.front(),
					identify_prefix
				);
				global_variables_initializations +=
					(format("%s%s=%s;")
						% identify_prefix
						% node.value
						% expression).str();
			} else if (node.name == "structure_declaration") {
				const auto structure = node.value;
				if (structures.count(structure)) {
					throw std::runtime_error(
						(format("duplicate of structure %s") % structure).str()
					);
				}
				structures.insert(structure);

				auto fields = std::set<std::string>();
				for (size_t i = 0; i < node.children.size(); i++) {
					const auto field = node.children[i].value;
					if (fields.count(field)) {
						throw std::runtime_error(
							(format("duplicate of structure field %s.%s")
								% structure
								% field).str()
						);
					}
					fields.insert(field);

					structures_registration +=
						(format(R"(__RegisterStructureField("%s","%s",%d);)")
							% structure
							% field
							% i).str();
				}
			} else if (node.name == "function_declaration") {
				const auto first_child = node.children.front();
				const auto function_arguments = std::accumulate(
					first_child.children.begin(),
					first_child.children.end(),
					std::string(),
					[&] (
						const std::string& function_argument_list,
						const Node& node
					) -> std::string {
						return
							function_argument_list
							+ (!function_argument_list.empty() ? "," : "")
							+ (format("const __Value %s%s")
								% identify_prefix
								% node.value).str();
					}
				);
				const auto function_declaration =
					(format("__Value %s(%s)")
						% WrapFunctionName(node.value, identify_prefix)
						% function_arguments).str();

				functions_declarations += function_declaration + ";";

				const auto statement_list = TranslateStatementList(
					node.children.back(),
					identify_prefix
				);
				functions_implementations +=
					(format("%s{%sreturn __NULL;}")
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
		+ (format(
			std::string("void __Start(")
				+ "const char*arguments[],"
				+ "const size_t number_of_arguments"
			+ "){"
				+ "__InitializeGarbageCollector();"
				+ "__InitializeConstants();"
				+ "__InitializeStructureStorage();"
				+ "atexit(__CleanupStructureStorage);"
				+ "__InitializeOpenedFileStorage();"
				+ "atexit(__CleanupOpenedFileStorage);"
				+ "%s"
				+ "%s"
				+ "Main("
					+ "__WrapCommandLineArguments("
						+ "arguments,"
						+ "number_of_arguments"
					+ ")"
				+ ");"
			+ "}"
		)
			% structures_registration
			% global_variables_initializations).str();
}

}
}
