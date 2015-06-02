#ifndef DO_TRANSLATOR_TRANSLATOR_HEADER
#define DO_TRANSLATOR_TRANSLATOR_HEADER

#include <wizard_parser/node/Node.h>

namespace thewizardplusplus {
namespace language_do {
namespace translator {

auto Translate(const wizard_parser::node::Node& ast) -> std::string;

}
}
}
#endif
