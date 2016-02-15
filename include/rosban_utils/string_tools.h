#pragma once

#include <stdexcept>
#include <vector>

namespace rosban_utils
{

class ConversionError : public std::runtime_error
{
public:
  ConversionError(const std::string & msg);
};

// Convert a string to the given type
template <typename T>
T str2(const std::string &s)
{
  throw ConversionError("unspecified type");
}

// Specializations
template <>
std::string str2<std::string>(const std::string &s);

template <>
int str2<int>(const std::string &s);

template <>
bool str2<bool>(const std::string &s);

template <>
char str2<char>(const std::string &s);

template <>
double str2<double>(const std::string &s);

template <>
float str2<float>(const std::string &s);

/// Allow to choose the precision of the print (not available in std::to_string)
std::string to_string(double val, int precision);

std::vector<std::string> split_string(const std::string &s, char separator);

}
