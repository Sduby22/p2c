#include <memory>
#include <string>
#include <vector>

namespace p2c {

class ASTNode {
public:
  std::vector<std::unique_ptr<ASTNode>> childs;
  ASTNode *parent;

  virtual void printNode();
  virtual std::string genCCode() = 0;
  void addChild(std::unique_ptr<ASTNode> child);

private:
  static constexpr auto name = "VirtualASTNode";
};

} // namespace p2c
