#include <memory>
#include <variant>
#include <string>
#include <utility>
#include <vector>

using namespace std;
namespace p2c {

using ValueType = variant<uint64_t, float, bool, char>;

enum class Operator {
  ADD,
  MINUS,
  OR,
  STAR,
  SLASH,
  DIV,
  MOD,
  AND,
  GREATER_THAN,
  LESS_THAN,
  GREATER_EQUAL,
  LESS_EQUAL,
  EQUAL,
  NOT_EQUAL
  // ...
};

enum class BasicType {
  INTEGER,
  REAL,
  BOOLEAN,
  CHAR
};

struct ArrayType {
  BasicType basictype;
  std::vector<std::tuple<int, int>> dimensions;
};

class ASTNode {
public:
  virtual ~ASTNode() = default;

  virtual std::string printNode();
  virtual std::string genCCode() = 0;
  void appendChild(std::unique_ptr<ASTNode> child);

private:
  std::vector<std::unique_ptr<ASTNode>> childs;
  ASTNode *parent;

  virtual const std::string &_getName() = 0;
  virtual std::string _infoStr() = 0;
  virtual void _printNode(int level, std::string &str);
  int _indent;
};

template <typename NodeT, typename... Args>
std::unique_ptr<ASTNode> make_Node(Args &&...args) {
  return std::make_unique<NodeT>(std::forward(args)...);
}

} // namespace p2c
