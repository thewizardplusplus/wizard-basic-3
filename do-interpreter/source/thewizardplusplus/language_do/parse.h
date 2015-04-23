#ifndef WIZARD_BASIC_3_PARSE_HEADER
#define WIZARD_BASIC_3_PARSE_HEADER

#include <wizard_parser/node/Node.h>

namespace thewizardplusplus {
namespace language_do {

auto Parse(const std::string& code) -> wizard_parser::node::Node;

}
}
#endif
