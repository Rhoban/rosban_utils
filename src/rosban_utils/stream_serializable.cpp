#include "rosban_utils/stream_serializable.h"

#include "rosban_utils/io_tools.h"

namespace rosban_utils
{

int StreamSerializable::write(std::ostream & out) const
{
  int bytes_written = 0;
  bytes_written += rosban_utils::write<int>(out, getClassID());
  bytes_written += writeInternal(out);
  return bytes_written;
}

}
