#include <vector>
#include <string>
#include "dbi.hpp"

using namespace DbI;

std::string DbI::value_type_to_str(EavValueType type) {
  switch (type) {
    case EavValueType::STR: return "str";
    case EavValueType::INT: return "int";
    case EavValueType::BOOL: return "bool";
    case EavValueType::FLOAT: return "float";
    default: return "null";
  }
}

EavValueType DbI::str_to_value_type(std::string str) {
  if (str == "str") return EavValueType::STR;
  else if (str == "int") return EavValueType::INT;
  else if (str == "bool") return EavValueType::BOOL;
  else if (str == "float") return EavValueType::FLOAT;
  else return EavValueType::NONE;
}

bool DbI::isValidInteger(std::string str) {
  // Handle empty string case
  if (str.empty()) return false;
  int start = 0;
  // Check for optional leading '+' or '-'
  if (str[0] == '+' || str[0] == '-') start = 1;
  // Check if the rest of the string contains only digits
  for (int i = start; i < str.length(); i++) {
    if (!isdigit(str[i])) return false;
  }
  return true;
}

bool DbI::isValidDecimal(std::string str) {
  // Handle empty string case
  if (str.empty()) return false;
  bool hasDecimalPoint = false;
  int start = 0;
  // Check for optional leading '+' or '-'
  if (str[0] == '+' || str[0] == '-') start = 1;
  // Traverse the string to check for valid digits and at most one decimal point
  for (int i = start; i < str.length(); i++) {
    if (str[i] == '.') {
      // If we've already encountered a decimal point, it's invalid
      if (hasDecimalPoint) return false;
      hasDecimalPoint = true;
    } else if (!isdigit(str[i])) {
      // If any character is not a digit and not a decimal point, return false
      return false;
    }
  }
  return true;
}
