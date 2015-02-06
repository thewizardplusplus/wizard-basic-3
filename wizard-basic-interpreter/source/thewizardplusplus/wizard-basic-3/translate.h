#ifndef WIZARD_BASIC_3_TRANSLATE_HEADER
#define WIZARD_BASIC_3_TRANSLATE_HEADER

#include <unordered_map>
#include <wizard_parser/node/Node.h>

namespace thewizardplusplus {
namespace wizard_basic_3 {

auto Translate(const wizard_parser::node::Node& ast) -> std::string;

}
}
#endif
