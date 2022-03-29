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
    string indent(level * 2, ' ');
    if (infoStr.empty())
      str += fmt::format("{}<{}>\n", indent, _getName());
    else
      str += fmt::format("{}<{} {}>\n", indent, _getName(), infoStr);
    for (auto &child : _childs) {
      child->_printNode(level + 1, str);
    }
  }

  const string &ASTNode::_getName() {
    static string name = "ASTNode";
    return name;
  }

  void ASTNode::appendChild(std::unique_ptr<ASTNode> child) {
    child->_parent = this;
    _childs.push_back(move(child));
  }
  

  /* expression_list node */ 
  const string& ExpressionList::_getName() {
    static string name = "ExpressionList";
    return name;
  }

  string ExpressionList::_infoStr() {
    return "";
  }

  string ExpressionList::genCCode() {
    string res;
    for (auto& exp: _childs) {
      res += exp->genCCode() += ", ";
    }
    res.erase(res.end()-2, res.end());
    return res;
  }

  
  /* expression node */ 
  const string& Expression::_getName() {
    static string name = "Expression";
    return name;
  }

  string Expression::_infoStr() {
    return "";
  }

  string Expression::genCCode() {
    string res = _childs.front()->genCCode();
    if (_childs.size() == 2) {
      switch (relop) {
        case Operator::GREATER_THAN :
          res += " > ";
          break;
        case Operator::LESS_THAN :
          res += " < ";
          break;
        case Operator::GREATER_EQUAL :
          res += " >= ";
          break;
        case Operator::LESS_EQUAL :
          res += " <= ";
          break;         
        case Operator::EQUAL :
          res += " == ";
          break;
        case Operator::NOT_EQUAL :
          res += " != ";
          break;
      }
      res += _childs.back()->genCCode();
    }
    return res;
  }

  
  /* simple_expression node */ 
  const string& SimpleExpression::_getName() {
    static string name = "SimpleExpression";
    return name;
  }

  string SimpleExpression::_infoStr() {
    return "";
  }

  string SimpleExpression::genCCode() {
    string res;
    for (auto& term: _childs) {
      res += term->genCCode();
      if ( !addops.empty() ) {
        switch (addops.front()) {
          case Operator::ADD :
            res += " + ";
            break;
          case Operator::MINUS :
            res += " - ";
            break;
          case Operator::OR :
            res += " || ";
            break;
        }
        addops.pop();
      }
    }
    return res;
  }

  
  /* term node */ 
  const string& Term::_getName() {
    static string name = "Term";
    return name;
  }

  string Term::_infoStr() {
    return "";
  }

  string Term::genCCode() {
    string res;
    for (auto& factor: _childs) {
      res += factor->genCCode();
      if ( !mulops.empty() ) {
        switch (mulops.front()) {
          case Operator::STAR :
            res += " * ";
            break;
          case Operator::SLASH :
            res += " / ";
            break;
          case Operator::DIV :
            res += " / ";
            break;
          case Operator::MOD :
            res += " % ";
            break;
          case Operator::AND :
            res += " && ";
            break;
        }
        mulops.pop();
      }
    }
    return res;
  }

  
  /* Factor node */ 
  const string& Factor::_getName() {
    static string name = "Factor";
    return name;
  }

  string Factor::_infoStr() {
    return fmt::format("type: {}", type);
  }

  string Factor::genCCode() {
    switch (type) {
      case 1 :  //num <-> value
        return value;
      case 2 :  // variable <-> _childs
        return _childs.front()->genCCode();
      case 3 :  // id (expression_list) <-> id:value, exp:_childs
        return value + "(" + _childs.front()->genCCode() + ")";
      case 4 :  // (expression) <-> _childs
        return "(" + _childs.front()->genCCode() + ")";
      case 5 :  // not factor <-> _childs
        return "!" + _childs.front()->genCCode();
      case 6 :  //uminus factor <-> _childs
        return "-" + _childs.front()->genCCode();  
      default:
        return "";
    }
  }

  
  /* variable_list node */ 
  const string& VariableList::_getName() {
    static string name = "VariableList";
    return name;
  }

  string VariableList::_infoStr() {
    return "";
  }

  string VariableList::genCCode() {
    string res;
    for (auto& variable: _childs) {
      res += variable->genCCode() += ", ";
    }
    res.erase(res.end()-2, res.end());
    return res;
  }

  
  /* variable node */ 
  const string& Variable::_getName() {
    static string name = "Variable";
    return name;
  }

  string Variable::_infoStr() {
    return fmt::format("IDENTIFIER: {}", identifier);
  }

  string Variable::genCCode() {
    return identifier +  _childs.front()->genCCode();
  }

  
  /* id_varpart node */ 
  const string& IdVarpart::_getName() {
    static string name = "IdVarpart";
    return name;
  }

  string IdVarpart::_infoStr() {
    return "";
  }

  string IdVarpart::genCCode() {
    if (isEmpty) {
      return "";
    }
    string res = "[" + _childs.front()->genCCode() + "]";
    int flag = 0;
    for (auto& ch: res) {
      if (flag == 1){
        ch = '[';
        flag = 0;
      } else if (ch == ',') {
        ch = ']';
        flag = 1;        
      }  
    }
    return res; 
  }


} // namespace p2c
