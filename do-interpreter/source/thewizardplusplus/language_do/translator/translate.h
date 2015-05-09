#ifndef DO_TRANSLATE_HEADER
#define DO_TRANSLATE_HEADER

#include <unordered_map>
#include <wizard_parser/node/Node.h>

namespace thewizardplusplus {
namespace language_do {
namespace translator {

auto Translate(const wizard_parser::node::Node& ast) -> std::string;

}
}
}
#endif
