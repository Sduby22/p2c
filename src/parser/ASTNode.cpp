#include "ASTNode.h"
#include "spdlog/fmt/fmt.h"

namespace p2c {
  using namespace std;

  string ASTNode::printNode() {
    string res;
    _printNode(0, res);
    return res;
  }

  void ASTNode::_printNode(int level, string &str) {
    auto infoStr = _infoStr();
    static string indent(level * 2, ' ');
    if (infoStr.empty())
      str += fmt::format("{}<{}>\n", indent, _getName());
    else
      str += fmt::format("{}<{} {}>\n", indent, _getName(), infoStr);
    for (auto &child : childs) {
      _printNode(level + 1, str);
    }
  }

  const string &ASTNode::_getName() {
    static string name = "ASTNode";
    return name;
  }

  void ASTNode::appendChild(std::unique_ptr<ASTNode> child) {
    childs.push_back(move(child));
    child->parent = this;
  }
} // namespace p2c
