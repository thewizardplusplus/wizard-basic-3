#ifndef WIZARD_BASIC_3_TRANSLATE_HEADER
#define WIZARD_BASIC_3_TRANSLATE_HEADER

#include <unordered_map>
#include <wizard_parser/node/Node.h>

namespace thewizardplusplus {
namespace language_do {

auto Translate(const wizard_parser::node::Node& ast) -> std::string;

}
}
#endif
