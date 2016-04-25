#pragma once

#include <utility>
#include <vector>

namespace rosban_utils
{

class MultiCore
{
public:
  /// In each pair, the first element is start, the second is end and the interval is [start,end[
  typedef std::vector<std::pair<int,int>> Intervals;

  static Intervals buildIntervals(int nb_tasks, int nb_threads);
};

}
