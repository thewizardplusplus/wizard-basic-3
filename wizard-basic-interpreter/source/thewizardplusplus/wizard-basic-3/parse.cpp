#include "parse.h"
#include <wizard_parser/parser/Parser.h>
#include <wizard_parser/node/Node.h>
#include <algorithm>
#include <numeric>

using namespace thewizardplusplus::wizard_parser::parser;
using namespace thewizardplusplus::wizard_parser::node;

namespace thewizardplusplus {
namespace wizard_basic_3 {

static Parser CreateGrammar(void) {
	const auto expression = dummy();
	const auto statement_list_copy = dummy();

	WP_RULE(keywords)
		word(
			"and"_t
			| "break"_t
			| "continue"_t
			| "do"_t
			| "else"_t
			| "end"_t
			| "function"_t
			| "if"_t
			| "include"_t
			| "let"_t
			| "new"_t
			| "not"_t
			| "NULL"_t
			| "or"_t
			| "return"_t
			| "structure"_t
			| "then"_t
			| "while"_t
		)
	WP_END

	WP_RULE(null_definition) hide("NULL"_t) WP_END
	WP_RULE(number)
		disable_separation(lexeme(+digit() >> !('.'_s >> +digit())))
	WP_END
	WP_RULE(identifier)
		disable_separation(lexeme((letter() | '_'_s) >> *word()) - keywords)
	WP_END
	WP_RULE(string_definition)
		lexeme(
			disable_separation(
				hide('"'_s)
				>> *(('\\'_s >> any()) | (any() - '"'_s))
				>> hide('"'_s)
			)
		)
	WP_END

	WP_RULE(array_definition)
		hide('['_s) >> !list(expression, hide(','_s)) >> hide(']'_s)
	WP_END
	WP_RULE(function_call)
		identifier
		>> hide('('_s) >> !list(expression, hide(','_s)) >> hide(')'_s)
	WP_END
	WP_RULE(atom)
		null_definition
		| number
		| identifier
		| array_definition
		| string_definition
		| function_call
		| hide('('_s) >> expression >> hide(')'_s)
	WP_END

	WP_RULE(item_access) hide('['_s) >> expression >> hide(']'_s) WP_END
	WP_RULE(field_access) hide('.'_s) >> identifier WP_END
	WP_RULE(accessor) atom >> *(item_access | field_access) WP_END

	WP_RULE(unary)
		(word("new"_t) >> identifier)
		| (*('-'_s | word("not"_t)) >> accessor)
	WP_END
	WP_RULE(product) list(unary, '*'_s | '/'_s | '%'_s) WP_END
	WP_RULE(sum) list(product, '+'_s | '-'_s) WP_END
	WP_RULE(comparison) list(sum, '<'_s | "<="_t | '>'_s | ">="_t) WP_END
	WP_RULE(equality) list(comparison, "=="_t | "/="_t) WP_END
	WP_RULE(conjunction) list(equality, hide(word("and"_t))) WP_END
	WP_RULE(disjunction) list(conjunction, hide(word("or"_t))) WP_END
	assign(expression, disjunction);

	WP_RULE(variable_definition)
		hide(word("let"_t)) >> identifier >> hide('='_s) >> expression
	WP_END
	WP_RULE(assignment) expression >> hide('='_s) >> expression WP_END
	WP_RULE(condition)
		hide(*space())
		>> hide(word("if"_t)) >> expression >> hide(word("then"_t))
			>> statement_list_copy
		>> *(hide(word("else"_t))
			>> hide(word("if"_t))
			>> expression
			>> hide(word("then"_t))
			>> statement_list_copy)
		>> !(hide(word("else"_t))
			>> statement_list_copy)
		>> hide(word("end"_t))
		>> hide(*space())
	WP_END
	WP_RULE(loop)
		hide(*space())
		>> hide(word("while"_t)) >> expression >> hide(word("do"_t))
			>> statement_list_copy
		>> hide(word("end"_t))
		>> hide(*space())
	WP_END
	WP_RULE(loop_continue)
		hide(*space())
		>> hide(word("continue"_t))
		>> hide(*space())
	WP_END
	WP_RULE(loop_break)
		hide(*space())
		>> hide(word("break"_t))
		>> hide(*space())
	WP_END
	WP_RULE(function_return)
		hide(*space())
		>> hide(word("return"_t)) >> (!expression | hide(*space()))
	WP_END

	WP_RULE(statement)
		variable_definition
		| assignment
		| condition
		| loop
		| loop_continue
		| loop_break
		| function_return
		| expression
	WP_END
	WP_RULE(statement_list) +statement WP_END
	assign(statement_list_copy, statement_list);

	WP_RULE(structure_declaration)
		hide(*space())
		>> hide(word("structure"_t)) >> identifier
			>> +identifier
		>> hide(word("end"_t))
		>> hide(*space())
	WP_END
	WP_RULE(function_declaration)
		hide(*space())
		>> hide(word("function"_t))
			>> identifier
			>> hide('('_s)
			>> !list(identifier, hide(','_s))
			>> hide(')'_s)
			>> statement_list_copy
		>> hide(word("end"_t))
		>> hide(*space())
	WP_END

	WP_RULE(program)
		+(variable_definition | structure_declaration | function_declaration)
	WP_END

	return separation(hide(*space()), program >> end());
}

static Node SimplifyAst(const Node& node) {
	auto children = NodeGroup();
	std::transform(
		node.children.begin(),
		node.children.end(),
		std::back_inserter(children),
		[=] (const Node& node) {
			return SimplifyAst(node);
		}
	);

	auto simplified_node = Node{node.name, node.value, children};
	if (node.name == "disjunction" || node.name == "conjunction") {
		const auto second_child = children.back();
		if (
			second_child.name.empty()
			&& second_child.value.empty()
			&& second_child.children.size() > 1
		) {
			simplified_node = std::accumulate(
				second_child.children.begin(),
				second_child.children.end(),
				children.front(),
				[=] (const Node& simplified_node, const Node& child) {
					return Node{node.name, "", {simplified_node, child}};
				}
			);
		}
	} else if (
		node.name == "equality"
		|| node.name == "comparison"
		|| node.name == "sum"
		|| node.name == "product"
	) {
		const auto second_child = children.back();
		const auto first_subchild = second_child.children.front();
		if (!first_subchild.value.empty()) {
			simplified_node = Node{
				node.name,
				first_subchild.value,
				{children.front(), second_child.children.back()}
			};
		} else {
			simplified_node = std::accumulate(
				second_child.children.begin(),
				second_child.children.end(),
				children.front(),
				[=] (const Node& simplified_node, const Node& child) {
					return Node{
						node.name,
						child.children.front().value,
						{simplified_node, child.children.back()}
					};
				}
			);
		}
	} else if (node.name == "accessor") {
		const auto second_child = children.back();
		if (second_child.name.empty()) {
			simplified_node = std::accumulate(
				second_child.children.begin(),
				second_child.children.end(),
				children.front(),
				[=] (const Node& simplified_node, const Node& child) {
					return Node{node.name, "", {simplified_node, child}};
				}
			);
		}
	} else if (node.name == "unary") {
		const auto first_child = children.front();
		if (first_child.value != "new" && first_child.children.size() == 0) {
			simplified_node = Node{
				node.name,
				"",
				{{"", "", {first_child}}, children.back()}
			};
		}
	} else if (node.name == "array_definition") {
		if (children.size() == 2) {
			const auto second_child = children.back();
			if (
				second_child.name.empty()
				&& second_child.value.empty()
				&& second_child.children.size() > 1
			) {
				auto new_children = NodeGroup();
				new_children.push_back(children.front());
				std::copy(
					second_child.children.begin(),
					second_child.children.end(),
					std::back_inserter(new_children)
				);

				simplified_node = Node{node.name, "", new_children};
			}
		}
	} else if (node.name == "function_call") {
		const auto first_child = children.front();
		if (children.size() == 1) {
			simplified_node = Node{node.name, first_child.value, {}};
		} else if (children.size() == 2) {
			const auto second_child = children.back();
			if (
				!second_child.name.empty()
				|| !second_child.value.empty()
				|| second_child.children.size() < 2
			) {
				simplified_node = Node{
					node.name,
					first_child.value,
					{second_child}
				};
			} else {
				const auto first_subchild = second_child.children.front();
				const auto second_subchild = second_child.children.back();
				if (
					!second_subchild.name.empty()
					|| !second_subchild.value.empty()
					|| second_subchild.children.size() < 2
				) {
					simplified_node = Node{
						node.name,
						first_child.value,
						{first_subchild, second_subchild}
					};
				} else {
					auto new_children = NodeGroup();
					new_children.push_back(first_subchild);
					std::copy(
						second_subchild.children.begin(),
						second_subchild.children.end(),
						std::back_inserter(new_children)
					);

					simplified_node = Node{
						node.name,
						first_child.value,
						new_children
					};
				}
			}
		}
	} else if (node.name == "variable_definition") {
		simplified_node = Node{
			node.name,
			children.front().value,
			{children.back()}
		};
	} else if (node.name == "condition") {
		/*const auto first_child = children.front();
		if (
			first_child.name.empty()
			&& first_child.value.empty()
			&& first_child.children.size() > 1
		) {
			simplified_node = Node{
				node.name,
				"",
				{
					first_child.children.front(),
					first_child.children.back(),
					children.back()
				}
			};
		}*/
	} else if (node.name == "statement_list") {
		const auto second_child = children.back();
		if (
			second_child.name.empty()
			&& second_child.value.empty()
			&& second_child.children.size() > 1
		) {
			auto new_children = NodeGroup();
			new_children.push_back(children.front());
			std::copy(
				second_child.children.begin(),
				second_child.children.end(),
				std::back_inserter(new_children)
			);

			simplified_node = Node{node.name, "", new_children};
		}
	} else if (node.name == "program") {
		const auto second_child = children.back();
		if (
			second_child.name.empty()
			&& second_child.value.empty()
			&& second_child.children.size() > 1
		) {
			auto new_children = NodeGroup();
			new_children.push_back(children.front());
			std::copy(
				second_child.children.begin(),
				second_child.children.end(),
				std::back_inserter(new_children)
			);

			simplified_node = Node{node.name, "", new_children};
		}
	} else if (node.name == "structure_declaration") {
		const auto second_child = children.back();
		if (
			!second_child.name.empty()
			|| !second_child.value.empty()
			|| second_child.children.size() < 2
		) {
			simplified_node = Node{
				node.name,
				children.front().value,
				{second_child}
			};
		} else {
			const auto second_subchild = second_child.children.back();
			if (
				!second_subchild.name.empty()
				|| !second_subchild.value.empty()
				|| second_subchild.children.size() < 2
			) {
				simplified_node = Node{
					node.name,
					children.front().value,
					second_child.children
				};
			} else {
				auto new_children = NodeGroup();
				new_children.push_back(second_child.children.front());
				std::copy(
					second_subchild.children.begin(),
					second_subchild.children.end(),
					std::back_inserter(new_children)
				);

				simplified_node = Node{
					node.name,
					children.front().value,
					new_children
				};
			}
		}
	} else if (node.name == "function_declaration") {
		const auto first_child = children.front();
		const auto second_child = children.back();
		if (
			!first_child.name.empty()
			|| !first_child.value.empty()
			|| first_child.children.size() < 2
		) {
			simplified_node = Node{
				node.name,
				first_child.value,
				{Node(), second_child}
			};
		} else {
			const auto first_subchild = first_child.children.front();
			const auto second_subchild = first_child.children.back();
			if (
				!second_subchild.name.empty()
				|| !second_subchild.value.empty()
				|| second_subchild.children.size() < 2
			) {
				simplified_node = Node{
					node.name,
					first_subchild.value,
					{{"", "", {second_subchild}}, second_child}
				};
			} else {
				const auto second_subsubchild = second_subchild.children.back();
				if (
					!second_subsubchild.name.empty()
					|| !second_subsubchild.value.empty()
					|| second_subsubchild.children.size() < 2
				) {
					simplified_node = Node{
						node.name,
						first_subchild.value,
						{{"", "", second_subchild.children}, second_child}
					};
				} else {
					auto new_children = NodeGroup();
					new_children.push_back(second_subchild.children.front());
					std::copy(
						second_subsubchild.children.begin(),
						second_subsubchild.children.end(),
						std::back_inserter(new_children)
					);

					simplified_node = Node{
						node.name,
						first_subchild.value,
						{{"", "", new_children}, second_child}
					};
				}
			}
		}
	}

	return simplified_node;
}

auto Parse(const std::string& code) -> wizard_parser::node::Node {
	const auto grammar = CreateGrammar();
	const auto ast = parse(
		grammar,
		code,
		SimplifyLevel::AST,
		{
			"array_definition",
			"function_call",
			"item_access",
			"field_access",
			"function_return",
			"statement_list",
			"program"
		}
	);

	return SimplifyAst(ast);
}

}
}
