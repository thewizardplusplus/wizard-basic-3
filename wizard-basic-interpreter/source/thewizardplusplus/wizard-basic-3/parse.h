#ifndef WIZARD_BASIC_3_PARSE_HEADER
#define WIZARD_BASIC_3_PARSE_HEADER

#include <wizard_parser/node/Node.h>

namespace thewizardplusplus {
namespace wizard_basic_3 {

auto Parse(const std::string& code) -> wizard_parser::node::Node;

}
}
#endif
