#include "parse.h"
#include <boost/spirit/home/classic/tree/common.hpp>
#include <boost/spirit/home/classic/tree/parse_tree.hpp>
#include <boost/spirit/home/classic/tree/ast.hpp>
#include <boost/spirit/home/classic/utility.hpp>
#include <boost/spirit/home/classic/core.hpp>
#include <boost/spirit/home/classic/tree/tree_to_xml.hpp>

using namespace thewizardplusplus::wizard_parser::node;
using namespace boost::spirit::classic;

namespace {

enum class GrammarRule : uint8_t {
	ROOT,
	PROGRAM,

	EXPRESSION,
	DISJUNCTION,
	CONJUNCTION,
	EQUALITY,
	COMPARISON,
	SUM,
	PRODUCT,
	UNARY,

	STRUCTURE_DEFINITION,
	ARRAY_SIZED_DEFINITION,
	ACCESSOR,
	ARRAY_ITEM_ACCESS,
	STRUCTURE_FIELD_ACCESS,

	ATOM,
	NULL_DEFINITION,
	ARRAY_LISTED_DEFINITION,
	FUNCTION_CALL,

	NUMBER,
	IDENTIFIER,
	STRING_DEFINITION,
};
class UnsignedRealParserPolicies : public ureal_parser_policies<double> {
public:
	static const bool allow_leading_dot =  false;
	static const bool allow_trailing_dot = false;

	template<typename ScannerType>
	static typename parser_result<chlit<>, ScannerType>::type parse_exp(
		ScannerType& scan
	) {
		(void)scan;
		return scan.no_match();
	}

	template<typename ScannerType>
	static typename parser_result<int_parser_t, ScannerType>::type parse_exp_n(
		ScannerType& scan
	) {
		(void)scan;
		return scan.no_match();
	}
};
class DoGrammar : public grammar<DoGrammar> {
public:
	template <typename ScannerType>
	class definition {
	public:
		inline definition(const DoGrammar& grammar) {
			(void)grammar;

			root = program;
			program = expression >> end_p;

			expression = disjunction;
			disjunction = infix_node_d[conjunction % "or"];
			conjunction = infix_node_d[equality % "and"];
			equality = comparison % root_node_d[str_p("==") | "/="];
			comparison =
				sum
				% root_node_d[longest_d[ch_p('<') | "<=" | '>' | ">="]];
			sum = product % root_node_d[ch_p('+') | '-'];
			product = unary % root_node_d[ch_p('*') | '/' | '%'];
			unary = longest_d[
				gen_pt_node_d[structure_definition]
				| gen_pt_node_d[array_sized_definition]
				| (*(ch_p('-') | "not") >> accessor)
			];

			structure_definition = discard_node_d[str_p("new")] >> identifier;
			array_sized_definition =
				discard_node_d[str_p("new")]
				>> discard_node_d[str_p("array")]
				>> inner_node_d[ch_p('(') >> expression >> ')'];
			accessor =
				atom
				>> *(
					gen_pt_node_d[array_item_access]
					| gen_pt_node_d[structure_field_access]
				);
			array_item_access = inner_node_d[ch_p('[') >> expression >> ']'];
			structure_field_access = discard_node_d[ch_p('.')] >> identifier;

			atom = longest_d[
				null_definition
				| number
				| identifier
				| gen_pt_node_d[array_listed_definition]
				| string_definition
				| gen_pt_node_d[function_call]
				| inner_node_d[ch_p('(') >> expression >> ')']
			];
			null_definition = str_p("NULL");
			array_listed_definition = inner_node_d[
				ch_p('[') >> !infix_node_d[expression % ','] >> ']'
			];
			function_call =
				identifier
				>> inner_node_d[
					ch_p('(') >> !infix_node_d[expression % ','] >> ')'
				];

			number = real_parser<double, UnsignedRealParserPolicies>();
			identifier =
				lexeme_d[token_node_d[(alpha_p ^ '_') >> *(alnum_p ^ '_')]]
				- "and"
				- "break"
				- "continue"
				- "do"
				- "else"
				- "end"
				- "function"
				- "if"
				- "include"
				- "let"
				- "new"
				- "not"
				- "note"
				- "NULL"
				- "or"
				- "return"
				- "structure"
				- "then"
				- "while";
			string_definition = lexeme_d[
				token_node_d[ch_p('"') >>  *(c_escape_ch_p - '"') >> '"']
			];
		}

		inline const rule<
			ScannerType,
			parser_tag<static_cast<int>(GrammarRule::ROOT)>
		>& start() const {
			return root;
		}

	private:
		rule<ScannerType, parser_tag<static_cast<int>(GrammarRule::ROOT)>> root;
		rule<
			ScannerType,
			parser_tag<static_cast<int>(GrammarRule::PROGRAM)>
		> program;

		rule<
			ScannerType,
			parser_tag<static_cast<int>(GrammarRule::EXPRESSION)>
		> expression;
		rule<
			ScannerType,
			parser_tag<static_cast<int>(GrammarRule::DISJUNCTION)>
		> disjunction;
		rule<
			ScannerType,
			parser_tag<static_cast<int>(GrammarRule::CONJUNCTION)>
		> conjunction;
		rule<
			ScannerType,
			parser_tag<static_cast<int>(GrammarRule::EQUALITY)>
		> equality;
		rule<
			ScannerType,
			parser_tag<static_cast<int>(GrammarRule::COMPARISON)>
		> comparison;
		rule<ScannerType, parser_tag<static_cast<int>(GrammarRule::SUM)>> sum;
		rule<
			ScannerType,
			parser_tag<static_cast<int>(GrammarRule::PRODUCT)>
		> product;
		rule<
			ScannerType,
			parser_tag<static_cast<int>(GrammarRule::UNARY)>
		> unary;

		rule<
			ScannerType,
			parser_tag<static_cast<int>(GrammarRule::STRUCTURE_DEFINITION)>
		> structure_definition;
		rule<
			ScannerType,
			parser_tag<static_cast<int>(GrammarRule::ARRAY_SIZED_DEFINITION)>
		> array_sized_definition;
		rule<
			ScannerType,
			parser_tag<static_cast<int>(GrammarRule::ACCESSOR)>
		> accessor;
		rule<
			ScannerType,
			parser_tag<static_cast<int>(GrammarRule::ARRAY_ITEM_ACCESS)>
		> array_item_access;
		rule<
			ScannerType,
			parser_tag<static_cast<int>(GrammarRule::STRUCTURE_FIELD_ACCESS)>
		> structure_field_access;

		rule<ScannerType, parser_tag<static_cast<int>(GrammarRule::ATOM)>> atom;
		rule<
			ScannerType,
			parser_tag<static_cast<int>(GrammarRule::NULL_DEFINITION)>
		> null_definition;
		rule<
			ScannerType,
			parser_tag<static_cast<int>(GrammarRule::ARRAY_LISTED_DEFINITION)>
		> array_listed_definition;
		rule<
			ScannerType,
			parser_tag<static_cast<int>(GrammarRule::FUNCTION_CALL)>
		> function_call;

		rule<
			ScannerType,
			parser_tag<static_cast<int>(GrammarRule::NUMBER)>
		> number;
		rule<
			ScannerType,
			parser_tag<static_cast<int>(GrammarRule::IDENTIFIER)>
		> identifier;
		rule<
			ScannerType,
			parser_tag<static_cast<int>(GrammarRule::STRING_DEFINITION)>
		> string_definition;
	};
};

auto ToId(const GrammarRule& rule) -> parser_id;
const std::map<parser_id, std::string> GRAMMAR_RULE_NAMES = {
		{ToId(GrammarRule::ROOT), "root"},
		{ToId(GrammarRule::PROGRAM), "program"},

		{ToId(GrammarRule::EXPRESSION), "expression"},
		{ToId(GrammarRule::DISJUNCTION), "disjunction"},
		{ToId(GrammarRule::CONJUNCTION), "conjunction"},
		{ToId(GrammarRule::EQUALITY), "equality"},
		{ToId(GrammarRule::COMPARISON), "comparison"},
		{ToId(GrammarRule::SUM), "sum"},
		{ToId(GrammarRule::PRODUCT), "product"},
		{ToId(GrammarRule::UNARY), "unary"},

		{ToId(GrammarRule::STRUCTURE_DEFINITION), "structure-definition"},
		{ToId(GrammarRule::ARRAY_SIZED_DEFINITION), "array-sized-definition"},
		{ToId(GrammarRule::ACCESSOR), "accessor"},
		{ToId(GrammarRule::ARRAY_ITEM_ACCESS), "array-item-access"},
		{ToId(GrammarRule::STRUCTURE_FIELD_ACCESS), "structure-field-access"},

		{ToId(GrammarRule::ATOM), "atom"},
		{ToId(GrammarRule::NULL_DEFINITION), "null-definition"},
		{ToId(GrammarRule::ARRAY_LISTED_DEFINITION), "array-listed-definition"},
		{ToId(GrammarRule::FUNCTION_CALL), "function-call"},

		{ToId(GrammarRule::NUMBER), "number"},
		{ToId(GrammarRule::IDENTIFIER), "identifier"},
		{ToId(GrammarRule::STRING_DEFINITION), "string-definition"},
	};

auto ToId(const GrammarRule& rule) -> parser_id {
	return parser_id(static_cast<size_t>(rule));
}

}

namespace thewizardplusplus {
namespace language_do {
namespace parser {

auto Parse(const std::string& code) -> Node {
	tree_parse_info<
		std::string::const_iterator,
		node_iter_data_factory<>
	> parse_tree_info = ast_parse<node_iter_data_factory<>>(
		code.begin(),
		code.end(),
		DoGrammar(),
		space_p
	);
	if (!parse_tree_info.full) {
		throw std::runtime_error("parsing error");
	}

	tree_to_xml(std::cout, parse_tree_info.trees, "", GRAMMAR_RULE_NAMES);
	return Node();
}

}
}
}
