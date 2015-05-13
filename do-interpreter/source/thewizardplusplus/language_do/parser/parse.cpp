#include "parse.h"
#include <boost/spirit/home/classic/tree/common.hpp>
#include <boost/spirit/home/classic/tree/parse_tree.hpp>
#include <boost/spirit/home/classic/tree/ast.hpp>
#include <boost/spirit/home/classic/utility.hpp>
#include <boost/spirit/home/classic/core.hpp>
#include <boost/spirit/home/classic/tree/tree_to_xml.hpp>
#include <boost/algorithm/string.hpp>

using namespace thewizardplusplus::wizard_parser::node;
using namespace boost;
using namespace boost::spirit::classic;

namespace {

enum class GrammarRule : uint8_t {
	ROOT,

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

			root = number | identifier | string_definition;
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
					discard_node_d[ch_p('"')]
						>> token_node_d[*(c_escape_ch_p - '"')]
					>> discard_node_d[ch_p('"')]
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
	const auto trimmed_code = trim_copy(code);
	tree_parse_info<
		std::string::const_iterator,
		node_iter_data_factory<>
	> parse_tree_info = ast_parse<node_iter_data_factory<>>(
		trimmed_code.begin(),
		trimmed_code.end(),
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
