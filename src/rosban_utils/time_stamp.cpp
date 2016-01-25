#include "rosban_utils/time_stamp.h"

using std::chrono::duration_cast;
using std::chrono::steady_clock;
using std::chrono::milliseconds;
using std::chrono::nanoseconds;

namespace rosban_utils {
      
TimeStamp::TimeStamp(){}
TimeStamp::TimeStamp(const std::chrono::time_point<steady_clock> & time_point)
  : std::chrono::time_point<steady_clock>(time_point) {}

TimeStamp TimeStamp::now()
{
  return TimeStamp(steady_clock::now());
}

TimeStamp TimeStamp::fromMS(unsigned long msSinceEpoch)
{
  return TimeStamp(time_point<steady_clock>(milliseconds(msSinceEpoch)));
}

double TimeStamp::getTimeMS() const
{
  return duration_cast<nanoseconds>(time_since_epoch()).count() / 1000000.0;
}

}

double diffSec(const rosban_utils::TimeStamp & src,
               const rosban_utils::TimeStamp & dst)
{
  double elapsedTicks = (dst - src).count();
  return elapsedTicks * steady_clock::period::num / steady_clock::period::den;
}

double diffMs(const rosban_utils::TimeStamp & src,
              const rosban_utils::TimeStamp & dst)
{
  return diffSec(src, dst) * 1000;
}


bool operator<(const rosban_utils::TimeStamp & ts1,
               const rosban_utils::TimeStamp & ts2)
{
  return diffMs(ts1, ts2) > 0;
}
bool operator>(const rosban_utils::TimeStamp & ts1,
               const rosban_utils::TimeStamp & ts2)
{
  return diffMs(ts1, ts2) < 0;
}
