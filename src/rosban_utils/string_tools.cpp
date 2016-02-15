#include "rosban_utils/string_tools.h"

#include <iomanip>
#include <sstream>

namespace rosban_utils
{

ConversionError::ConversionError(const std::string & msg)
  : std::runtime_error("Conversion error" + msg)
{
}

// Specializations
template <>
std::string str2<std::string>(const std::string &s)
{
  return s;
}

template <>
int str2<int>(const std::string &s)
{
  return std::stoi(s);
}

template <>
bool str2<bool>(const std::string &s)
{
  if (s == "true") return true;
  if (s == "false") return false;
  throw ("Cannot convert '" + s + "' to bool");
}

template <>
char str2<char>(const std::string &s)
{
  return (char)str2<int>(s);
}

template <>
double str2<double>(const std::string &s)
{
  return std::stod(s);
}

template <>
float str2<float>(const std::string &s)
{
  return (float)str2<double>(s);
}

std::string to_string(double val, int precision)
{
  std::ostringstream oss;
  oss << std::setprecision(precision) << val;
  return oss.str();
}

std::vector<std::string> split_string(const std::string &s, char separator)
{
  std::vector<std::string> elems;
  std::stringstream ss(s);
  std::string item;
  while(getline(ss, item, separator)) {
    elems.push_back(item);
  }
  return elems;
}

}
