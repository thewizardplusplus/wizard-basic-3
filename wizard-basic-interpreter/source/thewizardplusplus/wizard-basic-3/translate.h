#ifndef WIZARD_BASIC_3_TRANSLATE_HEADER
#define WIZARD_BASIC_3_TRANSLATE_HEADER

#include <unordered_map>
#include <wizard_parser/node/Node.h>

namespace thewizardplusplus {
namespace wizard_basic_3 {

using StructureDescription = std::unordered_map<std::string, size_t>;
using StructuresDescriptions = std::unordered_map<
	std::string,
	StructureDescription
>;
struct TranslationResult {
	std::string code;
	StructuresDescriptions structures_descriptions;
};

auto Translate(const wizard_parser::node::Node& ast) -> TranslationResult;

}
}
#endif
