#ifndef WIZARD_BASIC_3_TRANSLATOR_TRANSLATOR_HEADER
#define WIZARD_BASIC_3_TRANSLATOR_TRANSLATOR_HEADER

#include <wizard_parser/node/Node.h>

namespace thewizardplusplus {
namespace wizard_basic_3 {
namespace translator {

auto Translate(const wizard_parser::node::Node& ast) -> std::string;

}
}
}
#endif
