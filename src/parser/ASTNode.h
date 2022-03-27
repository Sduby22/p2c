#include <memory>
#include <variant>
#include <string>
#include <utility>
#include <vector>
#include <queue>

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


/* ASTNode */
class ASTNode {
public:
  virtual ~ASTNode() = default;

  virtual std::string printNode();
  virtual std::string genCCode() = 0;
  void appendChild(std::unique_ptr<ASTNode> child);

protected:
  std::vector<std::unique_ptr<ASTNode>> _childs;
  ASTNode* _parent;
  int _indent;

private:
  virtual const std::string &_getName() = 0;
  virtual std::string _infoStr() = 0;
  virtual void _printNode(int level, std::string &str);
};


/* expression_list node */
class ExpressionList: public ASTNode {
public:
  virtual std::string genCCode();

private:
  virtual const std::string &_getName();
  virtual std::string _infoStr();
};


/* expression node */
class Expression: public ASTNode {
public:
  Operator relop;
  virtual std::string genCCode();

private:
  virtual const std::string &_getName();
  virtual std::string _infoStr();
};


/* simple_expression node */
class SimpleExpression: public ASTNode {
public:
  queue<Operator> addops;
  virtual std::string genCCode();

private:
  virtual const std::string &_getName();
  virtual std::string _infoStr();
};


/* term node */
class Term: public ASTNode {
public:
  queue<Operator> mulops;
  virtual std::string genCCode();

private:
  virtual const std::string &_getName();
  virtual std::string _infoStr();
};


/* Factor node */
class Factor: public ASTNode {
public:
  int type;
  string value;
  virtual std::string genCCode();

private:
  virtual const std::string &_getName();
  virtual std::string _infoStr();
};

//
template <typename NodeT, typename... Args>
std::unique_ptr<ASTNode> make_Node(Args &&...args) {
  return std::make_unique<NodeT>(std::forward(args)...);
}

} // namespace p2c