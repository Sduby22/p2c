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

  friend class Term;
  friend class SimpleExpression;
  friend class Expression;
};


/* expression node */
class Expression: public ASTNode {
public:
  Operator relop;
  virtual std::string genCCode();

private:
  virtual const std::string &_getName();
  virtual std::string _infoStr();
  virtual void _printNode(int level, std::string &str);
};


/* simple_expression node */
class SimpleExpression: public ASTNode {
public:
  vector<Operator> addops;
  virtual std::string genCCode();

private:
  virtual const std::string &_getName();
  virtual std::string _infoStr();
  virtual void _printNode(int level, std::string &str);
};


/* term node */
class Term: public ASTNode {
public:
  vector<Operator> mulops;
  virtual std::string genCCode();

private:
  virtual const std::string &_getName();
  virtual std::string _infoStr();
  virtual void _printNode(int level, std::string &str);
};


/* Factor node */
class Factor: public ASTNode {
public:
  int type;
  string type_info;
  string value;
  virtual std::string genCCode();

private:
  virtual const std::string &_getName();
  virtual std::string _infoStr();
};


/* variable_list node */
class VariableList: public ASTNode {
public:
  virtual std::string genCCode();

private:
  virtual const std::string &_getName();
  virtual std::string _infoStr();
};


/* variable node */
class Variable: public ASTNode {
public:
  string identifier;
  virtual std::string genCCode();

private:
  virtual const std::string &_getName();
  virtual std::string _infoStr();
};


/* id_varpart node */
class IdVarpart: public ASTNode {
public:
  bool isEmpty;
  virtual std::string genCCode();

private:
  virtual const std::string &_getName();
  virtual std::string _infoStr();
};


/* statement_list node */
class StatementList: public ASTNode {
public:
  vector<string>statement_list;
  virtual std::string genCCode();

private:
  virtual const std::string &_getName();
  virtual std::string _infoStr();
};


/* statement node */
class Statement: public ASTNode {
public:
  int type;
  string type_name;
  string type_info;
  virtual std::string genCCode();

private:
  virtual const std::string &_getName();
  virtual std::string _infoStr();
};


/* procedure_call node */
class ProcedureCall : public ASTNode {
public:
  string identifier;
  virtual std::string genCCode();

private:
  virtual const std::string &_getName();
  virtual std::string _infoStr();
};


/* compound_statement node */
class CompoundStatement  : public ASTNode {
public:
  virtual std::string genCCode();

private:
  virtual const std::string &_getName();
  virtual std::string _infoStr();
};


/* const_declaration node */
class ConstDeclaration : public ASTNode {
public:
  string identifier;
  variant<int64_t, float, char>const_value;
  virtual std::string genCCode();

private:
  virtual const std::string &_getName();
  virtual std::string _infoStr();
};


/* const_declarations node */
class ConstDeclarations : public ASTNode {
public:
  virtual std::string genCCode();

private:
  virtual const std::string &_getName();
  virtual std::string _infoStr();
};


/* var_declaration node */
class VarDeclaration : public ASTNode {
public:
  vector<string>idlist;
  variant<BasicType, ArrayType>type;
  virtual std::string genCCode();

private:
  virtual const std::string &_getName();
  virtual std::string _infoStr();
};


/* var_declarations node */
class VarDeclarations : public ASTNode {
public:
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
