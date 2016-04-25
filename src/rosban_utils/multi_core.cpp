#include "rosban_utils/multi_core.h"

namespace rosban_utils
{

MultiCore::Intervals MultiCore::buildIntervals(int nb_tasks, int nb_threads)
{
  /// Do not create more threads than tasks
  nb_threads = std::min(nb_tasks, nb_threads);
  /// There is two intervals possible, k and k+1
  int small_interval = nb_tasks / nb_threads;
  int big_interval = small_interval + 1;
  int nb_big_intervals = nb_tasks % nb_threads;
  /// Creating intervals
  int start = 0;
  Intervals result;
  for (int i = 0; i < nb_big_intervals; i++)
  {
    int end = start + big_interval;
    result.push_back(std::pair<int,int>(start, start + big_interval));
    start = end;
  }
  for (int i = 0; i < nb_threads - nb_big_intervals; i++)
  {
    int end = start + small_interval;
    result.push_back(std::pair<int,int>(start, start + small_interval));
    start = end;
  }
  return result;
}

}
