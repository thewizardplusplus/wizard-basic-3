#include "parser.h"
#include <boost/spirit/home/classic/tree/common.hpp>
#include <boost/spirit/home/classic/tree/parse_tree.hpp>
#include <boost/spirit/home/classic/tree/ast.hpp>
#include <boost/spirit/home/classic/utility.hpp>
#include <boost/spirit/home/classic/core.hpp>
#include <boost/spirit/home/classic/tree/tree_to_xml.hpp>
#include <boost/format.hpp>

using namespace thewizardplusplus::wizard_parser::node;
using namespace boost;
using namespace boost::spirit::classic;

namespace {

enum class GrammarRule : uint8_t {
	ROOT,
	PROGRAM,

	VARIABLE_DEFINITION,
	STRUCTURE_DECLARATION,
	FUNCTION_DECLARATION,

	STATEMENT_LIST,
	STATEMENT,
	ASSIGNMENT,
	CONDITION,
	LOOP,
	LOOP_CONTINUE,
	LOOP_BREAK,
	FUNCTION_RETURN,

	EXPRESSION,
	DISJUNCTION,
	CONJUNCTION,
	EQUALITY,
	COMPARISON,
	SUM,
	PRODUCT,
	UNARY,

	ARRAY_SIZED_DEFINITION,
	STRUCTURE_DEFINITION,
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
	static const bool allow_leading_dot = false;
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

template<typename ScannerType, GrammarRule rule_id>
using DoRule = rule<ScannerType, parser_tag<static_cast<int>(rule_id)>>;

class DoGrammar : public grammar<DoGrammar> {
public:
	template <typename ScannerType>
	class definition {
	public:
		inline definition(const DoGrammar& grammar) {
			(void)grammar;

			root = program;
			program =
				+(variable_definition
					| structure_declaration
					| function_declaration)
				>> end_p;

			variable_definition =
				discard_node_d[str_p("let")]
					>> identifier
				>> discard_node_d[ch_p('=')]
					>> expression;
			structure_declaration =
				discard_node_d[str_p("structure")] >> identifier
					>> +identifier
				>> discard_node_d[str_p("end")];
			function_declaration =
				discard_node_d[str_p("function")]
					>> identifier
					>> inner_node_d[
						ch_p('(') >> !infix_node_d[identifier % ','] >> ')'
					]
					>> gen_pt_node_d[statement_list]
				>> discard_node_d[str_p("end")];

			statement_list = +statement;
			statement = longest_d[
				assignment
				| condition
				| loop
				| loop_continue
				| loop_break
				| gen_pt_node_d[function_return]
				| expression
			];
			assignment = expression >> discard_node_d[ch_p('=')] >> expression;
			condition =
				inner_node_d[str_p("if") >> expression >> "then"]
					>> gen_pt_node_d[statement_list]
				>> *(discard_node_d[str_p("else")]
					>> inner_node_d[str_p("if") >> expression >> "then"]
					>> gen_pt_node_d[statement_list])
				>> !(discard_node_d[str_p("else")]
					>> gen_pt_node_d[statement_list])
				>> discard_node_d[str_p("end")];
			loop =
				inner_node_d[str_p("while") >> expression >> "do"]
					>> gen_pt_node_d[statement_list]
				>> discard_node_d[str_p("end")];
			loop_continue = str_p("continue");
			loop_break = str_p("break");
			function_return = discard_node_d[str_p("return")] >> !expression;

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
			array_listed_definition =
				ch_p('[')
				>> !infix_node_d[expression % ',']
				>> ']';
			function_call =
				identifier
				>> inner_node_d[
					ch_p('(') >> !infix_node_d[expression % ','] >> ')'
				];

			number =
				real_parser<double, UnsignedRealParserPolicies>()
				| lexeme_d[
					token_node_d[ch_p('\'') >>  (c_escape_ch_p - '\'') >> '\'']
				];
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
		DoRule<ScannerType, GrammarRule::ROOT> root;
		DoRule<ScannerType, GrammarRule::PROGRAM> program;

		DoRule<
			ScannerType,
			GrammarRule::VARIABLE_DEFINITION
		> variable_definition;
		DoRule<
			ScannerType,
			GrammarRule::STRUCTURE_DECLARATION
		> structure_declaration;
		DoRule<
			ScannerType,
			GrammarRule::FUNCTION_DECLARATION
		> function_declaration;

		DoRule<ScannerType, GrammarRule::STATEMENT_LIST> statement_list;
		DoRule<ScannerType, GrammarRule::STATEMENT> statement;
		DoRule<ScannerType, GrammarRule::ASSIGNMENT> assignment;
		DoRule<ScannerType, GrammarRule::CONDITION> condition;
		DoRule<ScannerType, GrammarRule::LOOP> loop;
		DoRule<ScannerType, GrammarRule::LOOP_CONTINUE> loop_continue;
		DoRule<ScannerType, GrammarRule::LOOP_BREAK> loop_break;
		DoRule<ScannerType, GrammarRule::FUNCTION_RETURN> function_return;

		DoRule<ScannerType, GrammarRule::EXPRESSION> expression;
		DoRule<ScannerType, GrammarRule::DISJUNCTION> disjunction;
		DoRule<ScannerType, GrammarRule::CONJUNCTION> conjunction;
		DoRule<ScannerType, GrammarRule::EQUALITY> equality;
		DoRule<ScannerType, GrammarRule::COMPARISON> comparison;
		DoRule<ScannerType, GrammarRule::SUM> sum;
		DoRule<ScannerType, GrammarRule::PRODUCT> product;
		DoRule<ScannerType, GrammarRule::UNARY> unary;

		DoRule<
			ScannerType,
			GrammarRule::ARRAY_SIZED_DEFINITION
		> array_sized_definition;
		DoRule<
			ScannerType,
			GrammarRule::STRUCTURE_DEFINITION
		> structure_definition;
		DoRule<ScannerType, GrammarRule::ACCESSOR> accessor;
		DoRule<ScannerType, GrammarRule::ARRAY_ITEM_ACCESS> array_item_access;
		DoRule<
			ScannerType,
			GrammarRule::STRUCTURE_FIELD_ACCESS
		> structure_field_access;

		DoRule<ScannerType, GrammarRule::ATOM> atom;
		DoRule<ScannerType, GrammarRule::NULL_DEFINITION> null_definition;
		DoRule<
			ScannerType,
			GrammarRule::ARRAY_LISTED_DEFINITION
		> array_listed_definition;
		DoRule<ScannerType, GrammarRule::FUNCTION_CALL> function_call;

		DoRule<ScannerType, GrammarRule::NUMBER> number;
		DoRule<ScannerType, GrammarRule::IDENTIFIER> identifier;
		DoRule<ScannerType, GrammarRule::STRING_DEFINITION> string_definition;
	};
};

auto ToId(const GrammarRule& rule) -> parser_id;
const std::map<parser_id, std::string> GRAMMAR_RULE_NAMES = {
		{ToId(GrammarRule::ROOT), "root"},
		{ToId(GrammarRule::PROGRAM), "program"},

		{ToId(GrammarRule::VARIABLE_DEFINITION), "variable-definition"},
		{ToId(GrammarRule::STRUCTURE_DECLARATION), "structure-declaration"},
		{ToId(GrammarRule::FUNCTION_DECLARATION), "function-declaration"},

		{ToId(GrammarRule::STATEMENT_LIST), "statement-list"},
		{ToId(GrammarRule::STATEMENT), "statement"},
		{ToId(GrammarRule::ASSIGNMENT), "assignment"},
		{ToId(GrammarRule::CONDITION), "condition"},
		{ToId(GrammarRule::LOOP), "loop"},
		{ToId(GrammarRule::LOOP_CONTINUE), "loop-continue"},
		{ToId(GrammarRule::LOOP_BREAK), "loop-break"},
		{ToId(GrammarRule::FUNCTION_RETURN), "function-return"},

		{ToId(GrammarRule::EXPRESSION), "expression"},
		{ToId(GrammarRule::DISJUNCTION), "disjunction"},
		{ToId(GrammarRule::CONJUNCTION), "conjunction"},
		{ToId(GrammarRule::EQUALITY), "equality"},
		{ToId(GrammarRule::COMPARISON), "comparison"},
		{ToId(GrammarRule::SUM), "sum"},
		{ToId(GrammarRule::PRODUCT), "product"},
		{ToId(GrammarRule::UNARY), "unary"},

		{ToId(GrammarRule::ARRAY_SIZED_DEFINITION), "array-sized-definition"},
		{ToId(GrammarRule::STRUCTURE_DEFINITION), "structure-definition"},
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
	const auto line_comment = comment_p("note");
	const auto block_comment = comment_p(
		str_p("long") >> +space_p >> "note",
		"..."
	);
	const auto space_grammar = space_p | line_comment | block_comment;

	tree_parse_info<
		std::string::const_iterator,
		node_iter_data_factory<>
	> parse_tree_info = ast_parse<node_iter_data_factory<>>(
		code.begin(),
		code.end(),
		DoGrammar(),
		space_grammar
	);
	if (!parse_tree_info.full) {
		const auto valid_code = code.substr(
			0,
			std::distance(code.begin(), parse_tree_info.stop)
		);

		const auto line =
			std::count(
				valid_code.begin(),
				valid_code.end(),
				'\n'
			)
			+ 1;

		const auto index = valid_code.find_last_of('\n');
		const auto column =
			(index != std::string::npos
				? valid_code.substr(index + 1).length()
				: valid_code.length())
			+ 1;

		throw std::runtime_error(
			(format("parsing error on line %1% in column %2%")
				% line
				% column).str()
		);
	}

	tree_to_xml(std::cout, parse_tree_info.trees, "", GRAMMAR_RULE_NAMES);
	return Node();
}

}
}
}
