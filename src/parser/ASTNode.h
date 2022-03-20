#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace p2c {

class ASTNode {
public:
  virtual ~ASTNode() = default;

  std::vector<std::unique_ptr<ASTNode>> childs;
  ASTNode *parent;

  virtual std::string printNode();
  virtual std::string genCCode() = 0;
  void appendChild(std::unique_ptr<ASTNode> child);

private:
  virtual const std::string &_getName() = 0;
  virtual std::string _infoStr() = 0;
  virtual void _printNode(int level, std::string &str);
  int _indent;
};

class VarDeclaration: public ASTNode {
private:
  virtual const std::string &_getName();
  virtual std::string _infoStr();
public:
  std::string var_name;
  std::string var_type;
  virtual std::string genCCode();
};

template <typename NodeT, typename... Args>
std::unique_ptr<ASTNode> make_Node(Args &&...args) {
  return std::make_unique<NodeT>(std::forward(args)...);
}

} // namespace p2c
