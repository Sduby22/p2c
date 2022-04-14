#include <memory>
#include <variant>
#include <string>
#include <utility>
#include <vector>
#include "types.h"

using namespace std;
namespace p2c {

using ValueType = variant<uint64_t, float, bool, char>;
/* ASTNode */
class ASTNode {
public:
  virtual ~ASTNode() = default;

  virtual std::string printNode();
  virtual std::string genCCode() = 0;
  virtual void _printNode(int level, std::string &str);
  void appendChild(std::unique_ptr<ASTNode> child);

  std::vector<std::unique_ptr<ASTNode>> _childs;
  ASTNode* _parent;
  int _indent;

private:
  virtual const std::string &_getName() = 0;
  virtual std::string _infoStr() = 0;
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
  // 判断id是function还是symbol, function: true, symbol: false:
  bool find_type(std::string, std::string&); 
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
  bool isEmpty;
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
  bool isEmpty;
  virtual std::string genCCode();

private:
  virtual const std::string &_getName();
  virtual std::string _infoStr();
};


/* subprogram_body node */
class SubprogramBody : public ASTNode {
public:
  virtual std::string genCCode();

private:
  virtual const std::string &_getName();
  virtual std::string _infoStr();
};


/* parameter node */
class Parameter : public ASTNode {
public:
  bool isref;
  vector<string>idlist;
  BasicType type;
  virtual std::string genCCode();

private:
  virtual const std::string &_getName();
  virtual std::string _infoStr();
};


/* parameter_list node */
class ParameterList : public ASTNode {
public:
  bool isEmpty;
  virtual std::string genCCode();

private:
  virtual const std::string &_getName();
  virtual std::string _infoStr();
};


/* subprogram_head node */
class SubprogramHead : public ASTNode {
public:
  bool hasReturn;
  string funcId;
  BasicType returnType;
  virtual std::string genCCode();

private:
  virtual const std::string &_getName();
  virtual std::string _infoStr();
};


/* subprogram node */
class Subprogram : public ASTNode {
public:
  virtual std::string genCCode();

private:
  virtual const std::string &_getName();
  virtual std::string _infoStr();
};


/* subprogram_declarations node */
class SubprogramDeclarations : public ASTNode {
public:
  bool isEmpty;
  virtual std::string genCCode();

private:
  virtual const std::string &_getName();
  virtual std::string _infoStr();
};

class ProgramDecl : public ASTNode {
public:
  virtual std::string genCCode();
  std::string name;

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
