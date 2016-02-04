#include "rosban_utils/io_tools.h"

#include <fstream>
#include <stdexcept>

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
}
