#ifndef CPetBasicUtil_H
#define CPetBasicUtil_H

namespace CPetBasicUtil {

inline std::string toUpper(const std::string &str) {
  auto ustr = str;
  for (auto &c : ustr) c = char(toupper(c));
  return ustr;
}

}

#endif
