#include "ASTNode.h"
#include "magic_enum.hpp"
#include "spdlog/fmt/bundled/core.h"
#include "spdlog/fmt/fmt.h"
#include "symtable.h"
#include "logging.h"
#include "types.h"
#include <exception>
#include <memory>
#include <string>
#include <variant>
#include <vector>

static auto logger = logging::getLogger("AST");

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
  _childs.push_back(std::move(child));
}

/* expression node */
const string &Expression::_getName() {
  static string name = "Expression";
  return name;
}

string Expression::_infoStr() { return ""; }

string Expression::genCCode() {
  string res = _childs.front()->genCCode();
  if (_childs.size() == 2) {
    switch (relop) {
    case Operator::GREATER_THAN:
      res += " > ";
      break;
    case Operator::LESS_THAN:
      res += " < ";
      break;
    case Operator::GREATER_EQUAL:
      res += " >= ";
      break;
    case Operator::LESS_EQUAL:
      res += " <= ";
      break;
    case Operator::EQUAL:
      res += " == ";
      break;
    case Operator::NOT_EQUAL:
      res += " != ";
      break;
    default:
      break;
    }
    res += _childs.back()->genCCode();
  }
  return res;
}

void Expression::_printNode(int level, string &str) {
  auto infoStr = _infoStr();
  string indent(level * 2, ' ');
  if (infoStr.empty())
    str += fmt::format("{}<{}>\n", indent, _getName());
  else
    str += fmt::format("{}<{} {}>\n", indent, _getName(), infoStr);
  _childs.front()->_printNode(level + 1, str);
  if (_childs.size() == 2) {
    str += fmt::format("{}<Relop type: {}>\n", string((level + 1) * 2, ' '),
                       magic_enum::enum_name<Operator>(relop));
    _childs[1]->_printNode(level + 1, str);
  }
}

/* simple_expression node */
const string &SimpleExpression::_getName() {
  static string name = "SimpleExpression";
  return name;
}

string SimpleExpression::_infoStr() { return ""; }

string SimpleExpression::genCCode() {
  string res;
  int i = 0;
  for (auto &term : _childs) {
    res += term->genCCode();
    if (i < _childs.size() - 1) {
      switch (addops[i++]) {
      case Operator::ADD:
        res += " + ";
        break;
      case Operator::MINUS:
        res += " - ";
        break;
      case Operator::OR:
        res += " || ";
        break;
      default:
        break;
      }
    }
  }
  return res;
}

void SimpleExpression::_printNode(int level, string &str) {
  auto infoStr = _infoStr();
  string indent(level * 2, ' ');
  if (infoStr.empty())
    str += fmt::format("{}<{}>\n", indent, _getName());
  else
    str += fmt::format("{}<{} {}>\n", indent, _getName(), infoStr);
  int i = 0;
  for (auto &child : _childs) {
    child->_printNode(level + 1, str);
    if (i < _childs.size() - 1) {
      str += fmt::format("{}<Addop type: {}>\n", string((level + 1) * 2, ' '),
                         magic_enum::enum_name<Operator>(addops[i++]));
    }
  }
}

/* term node */
const string &Term::_getName() {
  static string name = "Term";
  return name;
}

string Term::_infoStr() { return ""; }

string Term::genCCode() {
  string res;
  int i = 0;
  for (auto &factor : _childs) {
    res += factor->genCCode();
    if (i < _childs.size() - 1) {
      switch (mulops[i++]) {
      case Operator::STAR:
        res += " * ";
        break;
      case Operator::SLASH:
        res += " / ";
        break;
      case Operator::DIV:
        res += " / ";
        break;
      case Operator::MOD:
        res += " % ";
        break;
      case Operator::AND:
        res += " && ";
        break;
      default:
        break;
      }
    }
  }
  return res;
}

void Term::_printNode(int level, string &str) {
  auto infoStr = _infoStr();
  string indent(level * 2, ' ');
  if (infoStr.empty())
    str += fmt::format("{}<{}>\n", indent, _getName());
  else
    str += fmt::format("{}<{} {}>\n", indent, _getName(), infoStr);
  int i = 0;
  for (auto &child : _childs) {
    child->_printNode(level + 1, str);
    if (i < _childs.size() - 1) {
      str += fmt::format("{}<Mulop type: {}>\n", string((level + 1) * 2, ' '),
                         magic_enum::enum_name<Operator>(mulops[i++]));
    }
  }
}

/* Factor node */
const string &Factor::_getName() {
  static string name = "Factor";
  return name;
}

string Factor::_infoStr() {
  if (type == 4) {
    return fmt::format("type: {}", type_info);
  } else if (type == 3) {
    return fmt::format("type: {} id: {}", type_info, value);
  }
  return fmt::format("type: {} value: {}", type_info, value);
}

string Factor::genCCode() {
  switch (type) {
  case 1: // num
    return value;
  case 2: // variable
    return _childs.front()->genCCode();
  case 3: // id (expression_list)
  {
    string res = value + "(";
    for (int i = 0; i < _childs.size(); i++) {
      string param = _childs[i]->genCCode();
      string _param = param;
      if (_param.substr(0, 2) == "(*" &&
          _param.find(')', 3) == _param.size() - 1) {
        _param = _param.substr(2, _param.size() - 3);
      }
      if (find_function(value).params[i].is_ref) {
        try {
          if (!find_symbol(_param).is_ref) {
            param = '&' + param;
          } else {
            param = _param;
          }
        } catch (...) {
          param = "&(" + param + ")";
        }
      }
      res += param + ", ";
    }
    res.erase(res.end() - 2, res.end());
    res += ")";
    return res;
  }
  case 4: // (expression)
    return "(" + _childs.front()->genCCode() + ")";
  case 5: // not factor
    return "!" + _childs.front()->genCCode();
  case 6: // uminus factor
    return "-" + _childs.front()->genCCode();
  default:
    return "";
  }
}

/* variable_list node */
const string &VariableList::_getName() {
  static string name = "VariableList";
  return name;
}

string VariableList::_infoStr() { return ""; }

string VariableList::genCCode() {
  string res;
  for (auto &variable : _childs) {
    res += variable->genCCode() + ", ";
  }
  res.erase(res.end() - 2, res.end());
  return res;
}

/* variable node */
const string &Variable::_getName() {
  static string name = "Variable";
  return name;
}

string Variable::_infoStr() {
  return fmt::format("IDENTIFIER: {}", identifier);
}

string Variable::genCCode() {
  try {
    if (find_symbol(identifier).is_ref) {
      identifier = "(*" + identifier + ")";
    }
  } catch (exception &e) {
    logger.critical("{}", e.what());
    throw e;
  }
  return identifier + _childs.front()->genCCode();
}

/* id_varpart node */
const string &IdVarpart::_getName() {
  static string name = "IdVarpart";
  return name;
}

string IdVarpart::_infoStr() { return ""; }

string IdVarpart::genCCode() {
  if (isEmpty) {
    return "";
  }
  try {
    vector<std::tuple<int, int>> offsets =
        get<1>(find_symbol(dynamic_cast<Variable &>(*_parent).identifier).type)
            .dimensions;
    string res = "[";
    int pos = 0;
    for (auto &expression : _childs) {
      string offset = " - " + to_string(get<0>(offsets[pos++]));
      res += expression->genCCode() += offset += "][";
    }
    res.erase(res.end() - 1);
    return res;
  } catch (exception &e) {
    logger.critical("{}", e.what());
    throw e;
  }
}

/* statement_list node */
const string &StatementList::_getName() {
  static string name = "StatementList";
  return name;
}

string StatementList::_infoStr() { return ""; }

string StatementList::genCCode() {
  string res;
  for (auto &statement : _childs) {
    res += statement->genCCode();
  }
  return res + "\n";
}

/* statement node */
const string &Statement::_getName() {
  static string name = "Statement";
  return name;
}

string Statement::_infoStr() { return fmt::format("type: {}", type_name); }

bool Statement::find_type(string str, string &name) {
  int pos = 0;
  while ((pos = str.find(' ', pos)) != str.npos) {
    str.erase(pos, 1);
  }
  for (int pos = 0; pos < str.size(); pos++) {
    if (str[pos] == '(') {
      if (pos == 0) return false;
      char before = str[pos - 1];
      if ((before >= 'a' && before <= 'z') ||
          (before >= 'A' && before <= 'Z') ||
          (before >= '0' && before <= '9')) {
        name = str.substr(0, pos);
        return true;
      }
    }
  }
  return false;
}

string Statement::genCCode() {
  switch (type) {
  case 1: // variable ASSIGN expression
    if (dynamic_cast<Variable &>(*_childs.front()).identifier ==
        current_symbol_table().name) {
      return "return " + _childs.back()->genCCode() + ";\n";
    }
    return _childs.front()->genCCode() + " = " + _childs.back()->genCCode() +
           ";\n";
  case 2: // procedure_call
    return _childs.front()->genCCode() + ";\n";
  case 3: // compound_statement
    return _childs.front()->genCCode();
  case 4: // IF expression THEN statement ELSE statement
    return "if (" + _childs[0]->genCCode() + ") {\n" + _childs[1]->genCCode() +
           "}\nelse {\n" + _childs[2]->genCCode() + "}\n";
  case 5: // IF expression THEN statement
    return "if (" + _childs[0]->genCCode() + ") {\n" + _childs[1]->genCCode() +
           "}\n";
  case 6: // FOR IDENTIFIER ASSIGN expression TO expression DO statement
    return "for (" + type_info + " = " + _childs[0]->genCCode() + "; " +
           type_info + " <= " + _childs[1]->genCCode() + "; " + type_info +
           "++) {\n" + _childs[2]->genCCode() + "\n}\n";
  case 7: // READ LBRACKET variable_list RBRACKET
  {
    string res = "";
    string var_list = _childs.front()->genCCode();
    int _pos = 1, pos = 0;
    var_list.insert(pos++, "&");
    while (pos < var_list.size()) {
      pos = var_list.find(", ", pos);
      if (pos < var_list.size()) {
        pos += 2;
        var_list.insert(pos, "&");
      }
      string id = var_list.substr(_pos, pos - 2 - _pos);
      if (id.substr(0, 2) == "(*") {
        id = id.substr(2, id.size() - 3);
      }
      int lbracket = id.find('[');
      if (lbracket != id.npos) { // array
        id = id.substr(0, lbracket);
      }
      try {
        variant<BasicType, ArrayType> _type = find_symbol(id).type;
        BasicType type;
        if (holds_alternative<BasicType>(_type)) {
          type = get<0>(_type);
        } else {
          type = get<1>(_type).basictype;
        }
        switch (type) {
        case BasicType::INTEGER:
          res += "%d ";
          break;
        case BasicType::REAL:
          res += "%f ";
          break;
        case BasicType::BOOLEAN:
          res += "%d ";
          break;
        case BasicType::CHAR:
          res += "%c ";
          break;
        default:
          break;
        }
      } catch (exception &e) {
        logger.critical("{}", e.what());
        throw e;
      }
      _pos = pos + 1;
    }
    res.erase(res.end() - 1);
    res = fmt::format("scanf(\"{}\", {});\n", res, var_list);
    return res;
  }
  case 8: // WRITE LBRACKET expression_list RBRACKET
  {
    string res = "";
    string var_list = "";
    for (auto &expression : _childs) {
      string exp = expression->genCCode();
      var_list += exp + ", ";
      if (exp.substr(0, 2) == "(*" && exp.find(')', 3) == exp.size() - 1) {
        exp = exp.substr(2, exp.size() - 3);
      }
      variant<BasicType, ArrayType> _type;
      BasicType type;
      string func_name = "";
      if (find_type(exp, func_name)) { // function
        type = find_function(func_name).return_type;
      } else { // symbol
        int lbracket = exp.find('[');
        if (lbracket != exp.npos) { // array
          exp = exp.substr(0, lbracket);
        }
        try {
          _type = find_symbol(exp).type;
          if (holds_alternative<BasicType>(_type)) {
            type = get<0>(_type);
          } else {
            type = get<1>(_type).basictype;
          }
        } catch (...) {
          type = BasicType::INTEGER; // ******
        }
      }
      switch (type) {
      case BasicType::INTEGER:
        res += "%d ";
        break;
      case BasicType::REAL:
        res += "%f ";
        break;
      case BasicType::BOOLEAN:
        res += "%d ";
        break;
      case BasicType::CHAR:
        res += "%c ";
        break;
      default:
        break;
      }
    }
    res.erase(res.end() - 1);
    var_list.erase(var_list.end() - 2, var_list.end());
    res = fmt::format("printf(\"{}\", {});\n", res, var_list);
    return res;
  }
  case 9:
    return "return;\n";
  default: // case 10: empty
    return "";
  }
}

/* procedure_call  node */
const string &ProcedureCall::_getName() {
  static string name = "ProcedureCall";
  return name;
}

string ProcedureCall::_infoStr() { return ""; }

string ProcedureCall::genCCode() {
  if (_childs.size() == 0) {
    return identifier;
  }
  string res = identifier + "(";
  for (int i = 0; i < _childs.size(); i++) {
    string param = _childs[i]->genCCode();
    string _param = param;
    if (_param.substr(0, 2) == "(*" &&
        _param.find(')', 3) == _param.size() - 1) {
      _param = _param.substr(2, _param.size() - 3);
    }
    if (find_function(identifier).params[i].is_ref) {
      try {
        if (!find_symbol(_param).is_ref) {
          param = '&' + param;
        } else {
          param = _param;
        }
      } catch (...) {
        param = "&(" + param + ")";
      }
    }
    res += param + ", ";
  }
  res.erase(res.end() - 2, res.end());
  res += ")";
  return res;
}

/* compound_statement node */
const string &CompoundStatement::_getName() {
  static string name = "CompoundStatement";
  return name;
}

string CompoundStatement::_infoStr() { return ""; }

string CompoundStatement::genCCode() {
  return "{\n" + _childs.front()->genCCode() + "\n}\n";
}

/* const_declaration node */
const string &ConstDeclaration::_getName() {
  static string name = "ConstDeclaration";
  return name;
}

string ConstDeclaration::_infoStr() {
  string res;
  if (holds_alternative<int64_t>(const_value)) {
    res = fmt::format("id: {}, value: {}", identifier, get<0>(const_value));
  } else if (holds_alternative<float>(const_value)) {
    res = fmt::format("id: {}, value: {}", identifier, get<1>(const_value));
  } else { // const char type
    res = fmt::format("id: {}, value: '{}'", identifier, get<2>(const_value));
  }
  return res;
}

string ConstDeclaration::genCCode() {
  string res;
  if (holds_alternative<int64_t>(const_value)) {
    res = fmt::format("const int {} = {};\n", identifier, get<0>(const_value));
    current_symbol_table().add(identifier, BasicType::INTEGER);
  } else if (holds_alternative<float>(const_value)) {
    res =
        fmt::format("const float {} = {};\n", identifier, get<1>(const_value));
    current_symbol_table().add(identifier, BasicType::REAL);
  } else { // const char type
    res =
        fmt::format("const char {} = '{}';\n", identifier, get<2>(const_value));
    current_symbol_table().add(identifier, BasicType::CHAR);
  }
  return res;
}

/* const_declarations node */
const string &ConstDeclarations::_getName() {
  static string name = "ConstDeclarations";
  return name;
}

string ConstDeclarations::_infoStr() {
  if (isEmpty) {
    return "empty";
  } else {
    return "";
  }
}

string ConstDeclarations::genCCode() {
  string res;
  for (auto &constdeclaration : _childs) {
    res += constdeclaration->genCCode();
  }
  return res;
}

/* var_declaration node */
const string &VarDeclaration::_getName() {
  static string name = "VarDeclaration";
  return name;
}

string VarDeclaration::_infoStr() {
  string idlist_str = "", type_str;
  if (holds_alternative<BasicType>(type)) {
    switch (get<0>(type)) {
    case BasicType::INTEGER:
    case BasicType::BOOLEAN:
      type_str = "int ";
      break;
    case BasicType::REAL:
      type_str = "float ";
      break;
    case BasicType::CHAR:
      type_str = "char ";
      break;
    default:
      break;
    }
  } else { // array type
    type_str = "array";
  }
  for (auto id : idlist) {
    idlist_str += (" " + id);
  }
  return fmt::format("idlist:{}, type: {}\n", idlist_str, type_str);
  ;
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
    for (auto id : idlist) {
      res += (id + ", ");
      current_symbol_table().add(id, get<BasicType>(type));
    }
  } else { // array type
    switch (get<1>(type).basictype) {
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
    vector<std::tuple<int, int>> &dimensions = get<1>(type).dimensions;
    string dimension_str;
    for (auto dimension : dimensions) {
      dimension_str +=
          ("[" + to_string(get<1>(dimension) - get<0>(dimension) + 1) + "]");
    }
    for (auto id : idlist) {
      res += (id + dimension_str + ", ");
      current_symbol_table().add(id, get<ArrayType>(type));
    }
  }
  res.erase(res.end() - 2, res.end());
  res += ";\n";
  return res;
}

/* var_declarations node */
const string &VarDeclarations::_getName() {
  static string name = "VarDeclarations";
  return name;
}

string VarDeclarations::_infoStr() {
  if (isEmpty) {
    return "empty";
  } else {
    return "";
  }
}

string VarDeclarations::genCCode() {
  string res;
  for (auto &vardeclaration : _childs) {
    res += vardeclaration->genCCode();
  }
  return res;
}

/* subprogram_body node */
const string &SubprogramBody::_getName() {
  static string name = "SubprogramBody";
  return name;
}

string SubprogramBody::_infoStr() { return ""; }

string SubprogramBody::genCCode() {
  string res;
  for (auto &child : _childs) {
    res += child->genCCode();
  }
  current_symbol_table().print();
  pop_symbol_table();
  return res;
}

/* parameter node */
const string &Parameter::_getName() {
  static string name = "Parameter";
  return name;
}

string Parameter::_infoStr() {
  string para_type, idlist_str = "", type_str;
  if (isref) { // value type
    para_type = "ref";
  } else {
    para_type = "value";
  }
  switch (type) {
  case BasicType::INTEGER:
  case BasicType::BOOLEAN:
    type_str = "int ";
    break;
  case BasicType::REAL:
    type_str = "float ";
    break;
  case BasicType::CHAR:
    type_str = "char ";
    break;
  default:
    break;
  }
  for (auto id : idlist) {
    idlist_str += (" " + id);
  }
  return fmt::format("parameter_type: {}, idlist:{}, type: {}\n", para_type,
                     idlist_str, type_str);
}

string Parameter::genCCode() {
  string res, type_str;
  switch (type) {
  case BasicType::INTEGER:
  case BasicType::BOOLEAN:
    type_str = "int ";
    break;
  case BasicType::REAL:
    type_str = "float ";
    break;
  case BasicType::CHAR:
    type_str = "char ";
    break;
  default:
    break;
  }
  if (isref == 1) { // var
    type_str += "*";
  }
  for (auto id : idlist) {
    current_symbol_table().add(id, type, isref);
    res += (type_str + id + ", ");
  }
  return res;
}

/* parameter_list node */
const string &ParameterList::_getName() {
  static string name = "ParameterList";
  return name;
}

string ParameterList::_infoStr() {
  if (isEmpty) {
    return "empty";
  } else {
    return "";
  }
}

string ParameterList::genCCode() {
  string res = "";
  for (auto &child : _childs) {
    res += child->genCCode();
  }
  res.erase(res.end() - 2, res.end());
  return res;
}

/* subprogram_head node */
const string &SubprogramHead::_getName() {
  static string name = "SubprogramHead";
  return name;
}

string SubprogramHead::_infoStr() {
  string returnType_str;
  if (hasReturn) {
    switch (returnType) {
    case BasicType::INTEGER:
    case BasicType::BOOLEAN:
      returnType_str = "int";
      break;
    case BasicType::REAL:
      returnType_str = "float";
      break;
    case BasicType::CHAR:
      returnType_str = "char";
      break;
    default:
      break;
    }
  } else {
    returnType_str = "void";
  }
  return fmt::format("func_id: {}, return_type: {}\n", funcId, returnType_str);
}

string SubprogramHead::genCCode() {
  push_symbol_table(funcId);
  string res = "", returnType_str;
  if (hasReturn) {
    switch (returnType) {
    case BasicType::INTEGER:
    case BasicType::BOOLEAN:
      returnType_str = "int ";
      break;
    case BasicType::REAL:
      returnType_str = "float ";
      break;
    case BasicType::CHAR:
      returnType_str = "char ";
      break;
    default:
      break;
    }
  } else {
    returnType_str = "void ";
  }
  res += (returnType_str + funcId + "(");
  for (auto &child : _childs) {
    res += child->genCCode();
  }
  function_table().add(funcId, returnType, {});
  auto &func = function_table().get(funcId);
  // this assumes that the ASTTree Structure is SubprogramHead -> ParameterList
  // -> Parameter
  for (auto &child : _childs[0]->_childs) {
    auto &paramNode = dynamic_cast<Parameter &>(*child);
    for (auto id : paramNode.idlist)
      func.params.push_back({id, paramNode.type, paramNode.isref});
  }
  return (res + ")");
}

/* subprogram node */
const string &Subprogram::_getName() {
  static string name = "Subprogram";
  return name;
}

string Subprogram::_infoStr() { return ""; }

string Subprogram::genCCode() {
  string res = "";
  res += (_childs.at(0)->genCCode() + " {\n");
  res += (_childs.at(1)->genCCode() + " }\n");
  return (res + "\n");
}

/* subprogram_declarations node */
const string &SubprogramDeclarations::_getName() {
  static string name = "SubprogramDeclarations";
  return name;
}

string SubprogramDeclarations::_infoStr() {
  if (isEmpty) {
    return "empty";
  } else {
    return "";
  }
}

string SubprogramDeclarations::genCCode() {
  string res = "";
  for (auto &child : _childs) {
    res += child->genCCode();
  }
  function_table().print();
  return (res + "\n");
}

std::string ProgramDecl::genCCode() {
  std::string res = "#include <stdio.h>\n";
  for (int i = 0; i != _childs.size(); ++i) {
    auto &child = _childs[i];
    if (i == _childs.size() - 1)
      res += fmt::format("int main() {}", child->genCCode());
    else
      res += child->genCCode();
    res.push_back('\n');
  }
  current_symbol_table().print();
  return res;
}

const std::string &ProgramDecl::_getName() {
  static std::string name = "ProgramDecl";
  return name;
}

std::string ProgramDecl::_infoStr() { return fmt::format("name: {}", name); }

} // namespace p2c
