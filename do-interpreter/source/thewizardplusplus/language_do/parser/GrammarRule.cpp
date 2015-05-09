#include "GrammarRule.h"
#include <boost/spirit/home/classic/core.hpp>

using namespace thewizardplusplus::language_do::parser;
using namespace boost::spirit::classic;

namespace {

auto ToId(const GrammarRule& rule) -> parser_id {
	return parser_id(static_cast<size_t>(rule));
}

}

namespace thewizardplusplus {
namespace language_do {
namespace parser {

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

}
}
}
