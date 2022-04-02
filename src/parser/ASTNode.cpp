#include "ASTNode.h"
#include "spdlog/fmt/fmt.h"
#include <vector>

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
        {
          string res = value + "(";
          for (auto& expression: _childs) {
            res += expression->genCCode() += ", ";
          }
          res.erase(res.end()-2, res.end());
          res += ")";
          return res;
        }
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
    string res = "[";
    for (auto& expression: _childs) {
      res += expression->genCCode() += "][";
    }
    res.erase(res.end()-1);
    return res; 
  }
  

  /* statement_list node */ 
  const string& StatementList::_getName() {
    static string name = "StatementList";
    return name;
  }

  string StatementList::_infoStr() {
    return "";
  }

  string StatementList::genCCode() {
    string res;
    for (auto& statement: _childs) {
      res += statement->genCCode();
    }
    return res + "\n";  
  }

  
  /* statement node */ 
  const string& Statement::_getName() {
    static string name = "Statement";
    return name;
  }

  string Statement::_infoStr() {
    return fmt::format("type: {}", type_name);
  }

  string Statement::genCCode() {
    switch (type) {
      case 1 :  //variable ASSIGN expression
        return _childs.front()->genCCode() + " = " + _childs.back()->genCCode() + ";\n";
      case 2 :  // procedure_call
        return _childs.front()->genCCode() + ";\n";
      case 3 :  // compound_statement
        return _childs.front()->genCCode();
      case 4 :  // IF expression THEN statement ELSE statement
        return "if (" + _childs[0]->genCCode() + ") {\n"
             + _childs[1]->genCCode() + "}\nelse {\n" + _childs[2]->genCCode() + "}\n";
      case 5 :  // IF expression THEN statement 
        return "if (" + _childs[0]->genCCode() + ") {\n" + _childs[1]->genCCode() + "}\n";
      case 6 :  // FOR IDENTIFIER ASSIGN expression TO expression DO statement
        return "for (" + type_info + " = " + _childs[0]->genCCode() + "; " 
             + type_info + " <= " + _childs[1]->genCCode() + "; " + type_info + "++) {\n"  
             + _childs[2]->genCCode() + "\n}\n"; 
      case 7 :  // READ LBRACKET variable_list RBRACKET
        {
          string var_list = _childs.front()->genCCode();
          int pos = 0, count = 1;
          var_list.insert(pos, "&");
          while (var_list.find(", ", pos) != var_list.npos) {
            pos = var_list.find(", ", pos) + 2;
            var_list.insert(pos, "&");  
            count++;
        }
        string res = "scanf(\"";
        for (int i = 0; i < count; i++) {
          res += "%d ";  //******
        }
        res.erase(res.end()-1);
        res += "\", " + var_list +");\n";
        return res;
        }  
      case 8 :  // WRITE LBRACKET expression_list RBRACKET
        {     
          string res = "printf(\""; 
          for (int i = 0; i < _childs.size(); i++) {
            res += "%d "; //******
          }
          res.erase(res.end()-1);
          res += "\", ";
          for (auto& expression: _childs) {
            res += expression->genCCode() += ", ";
          }
          res.erase(res.end()-2, res.end());
          res += ");\n";
          return res;
        }
      default: //case 9: empty
        return "";
    }
  }


  /* procedure_call  node */ 
  const string& ProcedureCall::_getName() {
    static string name = "ProcedureCall";
    return name;
  }

  string ProcedureCall::_infoStr() {
    return "";
  }

  string ProcedureCall::genCCode() {
    if (_childs.size() == 0) {
      return identifier;
    }
    string res = identifier + "(";
    for (auto& expression: _childs) {
      res += expression->genCCode() += ", ";
    }
    res.erase(res.end()-2, res.end());
    res += ")";
    return res; 
  }


  /* compound_statement node */ 
  const string& CompoundStatement::_getName() {
    static string name = "CompoundStatement";
    return name;
  }

  string CompoundStatement::_infoStr() {
    return "";
  }

  string CompoundStatement::genCCode() {
    return _childs.front()->genCCode() + "\n";
  }


  /* const_declaration node */ 
  const string& ConstDeclaration::_getName() {
    static string name = "ConstDeclaration";
    return name;
  }

  string ConstDeclaration::_infoStr() {
      string res;
      if (holds_alternative<int64_t>(const_value)) {
        res = fmt::format("const_id: {}, const_value: {}", identifier, get<0>(const_value));
      } else if (holds_alternative<float>(const_value)) {
        res = fmt::format("const_id: {}, const_value: {}", identifier, get<1>(const_value));
      } else { //const char type
        res = fmt::format("const_id: {}, const_value: '{}'", identifier, get<2>(const_value));
      }
      return res;
  }

  string ConstDeclaration::genCCode() {
      string res;
      if (holds_alternative<int64_t>(const_value)) {
        res = fmt::format("const int {} = {};\n", identifier, get<0>(const_value));
      } else if (holds_alternative<float>(const_value)) {
        res = fmt::format("const float {} = {};\n", identifier, get<1>(const_value));
      } else { //const char type
        res = fmt::format("const char {} = '{}';\n", identifier, get<2>(const_value));
      }
      return res;
  }


  /* const_declarations node */ 
  const string& ConstDeclarations::_getName() {
    static string name = "ConstDeclarations";
    return name;
  }

  string ConstDeclarations::_infoStr() {
      return "";
  }

  string ConstDeclarations::genCCode() {
      string res;
      for (auto& constdeclaration: _childs)
      {
        res += constdeclaration->genCCode();
      }
      return res;
  }


  /* var_declaration node */ 
  const string& VarDeclaration::_getName() {
    static string name = "VarDeclaration";
    return name;
  }

  string VarDeclaration::_infoStr() {
      return "";
  }

  string VarDeclaration::genCCode() {
      string res;      
      if (holds_alternative<BasicType>(type)) {
        switch (get<0>(type)) {
          case BasicType::INTEGER: 
          case BasicType::BOOLEAN:
            res += "int ";
            break;
          case BasicType::REAL:
            res += "float ";
            break;
          case BasicType::CHAR:
            res += "char ";
            break;
          default:
            break;
        }
        for (auto id : idlist)
        {
          res += (id+", ");
        }
      } else { //array type
        switch (get<1>(type).basictype)
        {
          case BasicType::INTEGER: 
          case BasicType::BOOLEAN:
            res += "int ";
            break;
          case BasicType::REAL:
            res += "float ";
            break;
          case BasicType::CHAR:
            res += "char ";
            break;
          default:
            break;
        }
        vector<std::tuple<int, int>> dimensions = get<1>(type).dimensions;
        string dimension_str;
        for (auto dimension : dimensions)
        {
          dimension_str += ("[" + to_string(get<1>(dimension)-get<0>(dimension)+1) + "]");
        }
        for (auto id : idlist)
        {
          res += (id + dimension_str + ", ");
        }
      }
      res.erase(res.end()-2, res.end());
      res += ";\n";
      return res;
  }


} // namespace p2c
