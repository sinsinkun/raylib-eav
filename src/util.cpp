#include <vector>
#include <string>
#include "app.hpp"

using namespace App;

std::vector<std::string> App::str_split(std::string str, std::string delimiter) {
  size_t pos_start = 0, pos_end, delim_len = delimiter.length();
  std::string token;
  std::vector<std::string> res;

  while ((pos_end = str.find(delimiter, pos_start)) != std::string::npos) {
    token = str.substr(pos_start, pos_end - pos_start);
    pos_start = pos_end + delim_len;
    res.push_back(token);
  }

  res.push_back(str.substr(pos_start));
  return res;
};

std::string App::trim_space(std::string str) {
  std::string o = "";
  int start = -1;
  int end = -1;
  int len = str.size();
  for (int i=0; i<len; i++) {
    if (start == -1 && str[i] != ' ') start = i;
    if (end == -1 && str[len-1-i] != ' ') end = len-1-i;
  }
  if (start == -1 || end == -1) return o;
  o = str.substr(start, end - start);
  return o;
}