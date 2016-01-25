#pragma once

#include <chrono>

namespace rosban_utils {

class TimeStamp : public std::chrono::time_point<std::chrono::steady_clock> {
public:
  TimeStamp();
  TimeStamp(const std::chrono::time_point<std::chrono::steady_clock> & time_point);
  static TimeStamp now();
  
  static TimeStamp fromMS(unsigned long ms_since_epoch);
  
  double getTimeMS() const;
  
};

}

double diffSec(const rosban_utils::TimeStamp & src,
               const rosban_utils::TimeStamp & dst);
double diffMs(const rosban_utils::TimeStamp & src,
              const rosban_utils::TimeStamp & dst);

bool operator<(const rosban_utils::TimeStamp & ts1,
               const rosban_utils::TimeStamp & ts2);
bool operator>(const rosban_utils::TimeStamp & ts1,
               const rosban_utils::TimeStamp & ts2);
