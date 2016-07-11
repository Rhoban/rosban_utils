#include "rosban_utils/io_tools.h"

#include <fstream>
#include <stdexcept>

#include <iostream>

namespace rosban_utils
{

std::string file2string(const std::string &path)
{
  std::ifstream in(path, std::ios::in | std::ios::binary);
  if (in)
  {
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    return contents;
  }
  throw std::runtime_error("Failed to open file '" + path + "'");
}

int writeInt(std::ostream & out, int val)
{
  out.write(reinterpret_cast<char*>(&val), sizeof(int));
  return sizeof(int);
}

int writeDouble(std::ostream & out, double val)
{
  out.write(reinterpret_cast<char*>(&val), sizeof(double));
  return sizeof(double);
}

int writeIntArray(std::ostream & out, int * values, int nb_values)
{
  int nb_bytes = sizeof(int) * nb_values;
  out.write(reinterpret_cast<char*>(values), nb_bytes);
  return nb_bytes;
}

int writeDoubleArray(std::ostream & out, double * values, int nb_values)
{
  int nb_bytes = sizeof(double) * nb_values;
  out.write(reinterpret_cast<char*>(values), nb_bytes);
  return nb_bytes;
}

}
