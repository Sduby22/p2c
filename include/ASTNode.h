#include <memory>
#include <vector>

namespace p2c {

struct ASTNode {
  enum class TYPE { UNDEF, PROGRAM, FUNCTION, FOR, IF, VAR, ASSIGN };
  TYPE type;
  std::vector<std::unique_ptr<ASTNode>> childs;
  ASTNode *parent;
};

} // namespace p2c