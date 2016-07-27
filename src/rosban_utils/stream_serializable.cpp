#include "rosban_utils/stream_serializable.h"

#include "rosban_utils/io_tools.h"

#include <fstream>
#include <ostream>
#include <sstream>
#include <stdexcept>

namespace rosban_utils
{

int StreamSerializable::write(std::ostream & out) const
{
  int bytes_written = 0;
  bytes_written += rosban_utils::write<int>(out, getClassID());
  bytes_written += writeInternal(out);
  return bytes_written;
}

int StreamSerializable::save(const std::string & filename, bool write_class_id) const
{
  std::ofstream out(filename, std::ios::binary);
  if (!out) {
    std::ostringstream oss;
    oss << "Failed to open '" << filename << "' for binary writing";
    throw std::runtime_error(oss.str());
  }
  int bytes_written;
  if (write_class_id)
    bytes_written = write(out);
  else
    bytes_written = writeInternal(out);
  out.close();
  return bytes_written;
}

}