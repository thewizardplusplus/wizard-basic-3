#ifndef DO_PARSER_PARSER_HEADER
#define DO_PARSER_PARSER_HEADER

#include <wizard_parser/node/Node.h>

namespace thewizardplusplus {
namespace language_do {
namespace parser {

auto Parse(const std::string& code) -> wizard_parser::node::Node;

}
}
}
#endif
