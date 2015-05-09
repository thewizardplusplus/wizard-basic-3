#ifndef DO_GRAMMAR_RULE_HEADER
#define DO_GRAMMAR_RULE_HEADER

#include <boost/spirit/home/classic/core.hpp>
#include <map>

namespace thewizardplusplus {
namespace language_do {
namespace parser {

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

extern const GrammarRuleMap GRAMMAR_RULE_NAMES;

}
}
}
#endif
