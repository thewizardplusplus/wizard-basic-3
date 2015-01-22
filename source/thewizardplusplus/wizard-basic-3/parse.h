#ifndef WIZARD_BASIC_3_PARSE_HEADER
#define WIZARD_BASIC_3_PARSE_HEADER

#include <vector>
#include <string>

namespace thewizardplusplus {
namespace wizard_basic_3 {

enum class AstNodeType : uint8_t {
	UNKNOWN
};
struct AstNode;
using AstNodeGroup = std::vector<AstNode>;
struct AstNode {
	AstNodeType type = AstNodeType::UNKNOWN;
	std::string value;
	AstNodeGroup children;
};
auto Parse(const std::string& code) -> AstNode;

}
}
#endif
