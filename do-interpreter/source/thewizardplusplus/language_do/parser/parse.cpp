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
	LINE,

	STATEMENT,
	STATEMENT_ARRAY_DEFINITION,
	STATEMENT_ASSIGN,
	STATEMENT_JUMP,
	STATEMENT_CONDITION,

	EXPRESSION_ADDITION,
	EXPRESSION_MULTIPLICATION,
	EXPRESSION_ATOM,

	COMMENT,
	IDENTIFIER,
	ARRAY_ACCESS,
	FUNCTION_CALL,

	LINE_BREAK,
	CONSTANT_POSITIVE_INTEGER,
	CONSTANT_REAL,
	CONSTANT_STRING
};
using GrammarRuleMap = std::map<boost::spirit::classic::parser_id, std::string>;
class UnsignedRealParserPolicies : public boost::spirit::classic::
	ureal_parser_policies<float>
{
public:
	static const bool allow_leading_dot =  false;
	static const bool allow_trailing_dot = false;

	template<typename ScannerType>
	static typename boost::spirit::classic::parser_result<uint_parser_t,
		ScannerType>::type parse_n(ScannerType& scan) {
			using namespace boost::spirit::classic;
			return (ch_p('0') ^ (uint_parser<float, 10, 1, 1>() - '0') >> !uint_p).
				parse(scan);
		}
	template<typename ScannerType>
	static typename boost::spirit::classic::parser_result<boost::spirit::classic
		::chlit<>, ScannerType>::type parse_exp(ScannerType& scan) {
			(void)scan;
			return scan.no_match();
		}
	template<typename ScannerType>
	static typename boost::spirit::classic::parser_result<int_parser_t,
		ScannerType>::type parse_exp_n(ScannerType& scan) {
			(void)scan;
			return scan.no_match();
		}
};
class WizardBasicGrammar : public boost::spirit::classic::grammar<
	WizardBasicGrammar>
{
public:
	template <typename ScannerType>
	class definition {
	public:
		inline definition(const WizardBasicGrammar& wizard_basic_grammar) {
			(void)wizard_basic_grammar;
			using namespace boost::spirit::classic;

			root = gen_pt_node_d[program];
			program = infix_node_d[gen_pt_node_d[line] % line_break] >> !discard_node_d[
				line_break];
			line = constant_positive_integer >> !(statement ^ discard_node_d[comment]);

			statement = statement_array_definition ^ statement_assign ^ gen_pt_node_d[
				statement_jump] ^ statement_condition ^ gen_pt_node_d[function_call];
			statement_array_definition = discard_node_d[str_p("DIM")] >> identifier >>
				(inner_node_d[ch_p('[') >> constant_positive_integer >> ']'] ^
				discard_node_d[ch_p('=')] >> constant_string);
			statement_assign = discard_node_d[str_p("LET")] >> longest_d[identifier |
				array_access] >> discard_node_d[ch_p('=')] >> expression_addition;
			statement_jump = discard_node_d[str_p("GOTO")] >> lexeme_d[
				constant_positive_integer];
			statement_condition = discard_node_d[str_p("IF")] >> expression_addition >>
				root_node_d[ch_p('=') ^ '<' ^ '>'] >> expression_addition >>
				discard_node_d[str_p("THEN")] >> lexeme_d[constant_positive_integer];

			expression_addition = expression_multiplication % root_node_d[ch_p('+') ^
				'-'];
			expression_multiplication = expression_atom % root_node_d[ch_p('*') ^ '/'];
			expression_atom = longest_d[constant_real | identifier | array_access |
				gen_pt_node_d[function_call] | inner_node_d['(' >> expression_addition
				>> ')']];

			comment = discard_node_d[str_p("REM")] >> !lexeme_d[token_node_d[*(anychar_p
				- line_break)]];
			identifier = lexeme_d[token_node_d[(alpha_p ^ '_') >> *(alnum_p ^ '_')]] -
				"DIM" - "LET" - "GOTO" - "IF" - "THEN" - "REM";
			array_access = identifier >> inner_node_d[ch_p('[') >> expression_addition
				>> ']'];
			function_call = identifier >> inner_node_d[ch_p('(') >> !infix_node_d[
				expression_addition % ','] >> ')'];

			line_break = eol_p;
			constant_positive_integer = token_node_d[(uint_parser<unsigned int, 10, 1,
				1>() - '0') >> !uint_p];
			constant_real = real_parser<float, UnsignedRealParserPolicies>();
			constant_string = lexeme_d[discard_node_d[ch_p('"')] >> token_node_d[
				+(c_escape_ch_p - '"')] >> discard_node_d[ch_p('"')]];
		}
		inline const boost::spirit::classic::rule<ScannerType, boost::spirit::
			classic::parser_tag<static_cast<int>(GrammarRule::
			ROOT)> >& start() const {
				return root;
			}

	private:
		boost::spirit::classic::rule<ScannerType, boost::spirit::classic::
			parser_tag<static_cast<int>(GrammarRule::ROOT)> > root;
		boost::spirit::classic::rule<ScannerType, boost::spirit::classic::
			parser_tag<static_cast<int>(GrammarRule::PROGRAM)> >
			program;
		boost::spirit::classic::rule<ScannerType, boost::spirit::classic::
			parser_tag<static_cast<int>(GrammarRule::LINE)> > line;

		boost::spirit::classic::rule<ScannerType, boost::spirit::classic::
			parser_tag<static_cast<int>(GrammarRule::STATEMENT)> >
			statement;
		boost::spirit::classic::rule<ScannerType, boost::spirit::classic::
			parser_tag<static_cast<int>(GrammarRule::
			STATEMENT_ARRAY_DEFINITION)> > statement_array_definition;
		boost::spirit::classic::rule<ScannerType, boost::spirit::classic::
			parser_tag<static_cast<int>(GrammarRule::
			STATEMENT_ASSIGN)> > statement_assign;
		boost::spirit::classic::rule<ScannerType, boost::spirit::classic::
			parser_tag<static_cast<int>(GrammarRule::
			STATEMENT_JUMP)> > statement_jump;
		boost::spirit::classic::rule<ScannerType, boost::spirit::classic::
			parser_tag<static_cast<int>(GrammarRule::
			STATEMENT_CONDITION)> > statement_condition;

		boost::spirit::classic::rule<ScannerType, boost::spirit::classic::
			parser_tag<static_cast<int>(GrammarRule::
			EXPRESSION_ADDITION)> > expression_addition;
		boost::spirit::classic::rule<ScannerType, boost::spirit::classic::
			parser_tag<static_cast<int>(GrammarRule::
			EXPRESSION_MULTIPLICATION)> > expression_multiplication;
		boost::spirit::classic::rule<ScannerType, boost::spirit::classic::
			parser_tag<static_cast<int>(GrammarRule::
			EXPRESSION_ATOM)> > expression_atom;

		boost::spirit::classic::rule<ScannerType, boost::spirit::classic::
			parser_tag<static_cast<int>(GrammarRule::COMMENT)> >
			comment;
		boost::spirit::classic::rule<ScannerType, boost::spirit::classic::
			parser_tag<static_cast<int>(GrammarRule::IDENTIFIER)> >
			identifier;
		boost::spirit::classic::rule<ScannerType, boost::spirit::classic::
			parser_tag<static_cast<int>(GrammarRule::ARRAY_ACCESS)> >
			array_access;
		boost::spirit::classic::rule<ScannerType, boost::spirit::classic::
			parser_tag<static_cast<int>(GrammarRule::
			FUNCTION_CALL)> > function_call;

		boost::spirit::classic::rule<ScannerType, boost::spirit::classic::
			parser_tag<static_cast<int>(GrammarRule::LINE_BREAK)> >
			line_break;
		boost::spirit::classic::rule<ScannerType, boost::spirit::classic::
			parser_tag<static_cast<int>(GrammarRule::
			CONSTANT_POSITIVE_INTEGER)> > constant_positive_integer;
		boost::spirit::classic::rule<ScannerType, boost::spirit::classic::
			parser_tag<static_cast<int>(GrammarRule::
			CONSTANT_REAL)> > constant_real;
		boost::spirit::classic::rule<ScannerType, boost::spirit::classic::
			parser_tag<static_cast<int>(GrammarRule::
			CONSTANT_STRING)> > constant_string;
	};
};

auto ToId(const GrammarRule& rule) -> parser_id;
const GrammarRuleMap GRAMMAR_RULE_NAMES = {
		{ToId(GrammarRule::ROOT), "ROOT"},
		{ToId(GrammarRule::PROGRAM), "PROGRAM"},
		{ToId(GrammarRule::LINE), "LINE"},

		{ToId(GrammarRule::STATEMENT), "STATEMENT"},
		{
			ToId(GrammarRule::STATEMENT_ARRAY_DEFINITION),
			"STATEMENT_ARRAY_DEFINITION"
		},
		{ToId(GrammarRule::STATEMENT_ASSIGN), "STATEMENT_ASSIGN"},
		{ToId(GrammarRule::STATEMENT_JUMP), "STATEMENT_JUMP"},
		{ToId(GrammarRule::STATEMENT_CONDITION), "STATEMENT_CONDITION"},

		{ToId(GrammarRule::EXPRESSION_ADDITION), "EXPRESSION_ADDITION"},
		{
			ToId(GrammarRule::EXPRESSION_MULTIPLICATION),
			"EXPRESSION_MULTIPLICATION"
		},
		{ToId(GrammarRule::EXPRESSION_ATOM), "EXPRESSION_ATOM"},

		{ToId(GrammarRule::COMMENT), "COMMENT"},
		{ToId(GrammarRule::IDENTIFIER), "IDENTIFIER"},
		{ToId(GrammarRule::ARRAY_ACCESS), "ARRAY_ACCESS"},
		{ToId(GrammarRule::FUNCTION_CALL), "FUNCTION_CALL"},

		{ToId(GrammarRule::LINE_BREAK), "LINE_BREAK"},
		{
			ToId(GrammarRule::CONSTANT_POSITIVE_INTEGER),
			"CONSTANT_POSITIVE_INTEGER"
		},
		{ToId(GrammarRule::CONSTANT_REAL), "CONSTANT_REAL"},
		{ToId(GrammarRule::CONSTANT_STRING), "CONSTANT_STRING"},
	};

auto ToId(const GrammarRule& rule) -> parser_id {
	return parser_id(static_cast<size_t>(rule));
}

}

namespace thewizardplusplus {
namespace language_do {
namespace parser {

auto Parse(const std::string& code) -> Node {
	tree_parse_info<std::string::const_iterator, node_iter_data_factory<>>
		parse_tree_info = ast_parse<node_iter_data_factory<>>(code.begin(),
		code.end(), WizardBasicGrammar(), space_p - eol_p);
	if (!parse_tree_info.full) {
		throw std::runtime_error("parsing error");
	}

	tree_to_xml(std::cout, parse_tree_info.full, code, GRAMMAR_RULE_NAMES);
	return Node();
}

}
}
}
