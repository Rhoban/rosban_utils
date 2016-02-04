#pragma once

#include <stdexcept>

namespace rosban_utils
{

class ConversionError : public std::runtime_error
{
  ConversionError(const std::string & msg)
    : std::runtime_error("Conversion error" + msg)
    {
    }
};

// Convert a string to the given type
template <typename T>
T str2(const std::string &s)
{
  throw ConversionError("unspecified type");
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

}
