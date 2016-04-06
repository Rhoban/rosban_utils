#pragma once

#include "rosban_utils/time_stamp.h"

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <stdexcept>

namespace rosban_utils {

/// This class allows to provide benchmark in mono-thread situation.
/// It ensures a low time consumption, except when printing results
/// It is based on the notion of zone and build a tree of consumption
class Benchmark
{
public:
  Benchmark(std::shared_ptr<Benchmark> father, const std::string & name);
  ~Benchmark();

  /// Open a new benchmark or subBenchmark (if current benchmark is not null)
  /// It is not recommended to open several subBenchmark with the same name
  static void open(const std::string &benchmark_name);

  /// Close current benchmark or subBenchmark and return to previous context
  /// if needed
  static double close(bool print = false,
                      int detail_level = -1,
                      std::ostream &out = std::cout);

private:  
  /// Print until max_depth has been reached, if max_depth is -1, then there is no limits
  void print(std::ostream & out, int max_depth = -1);
  /// Print from current depth to max_depth
  void print(std::ostream & out, int depth, int width , int max_depth = -1);
  
  void startSession();
  void endSession();

  double getTime() const;
  double getSubTime() const;

  /// Create a Benchmark if no benchmark is open
  static std::shared_ptr<Benchmark> getCurrent();

  /// Current level of benchmark for static access
  static std::shared_ptr<Benchmark> current;

  /// Access to the calling benchmark, pointer is null if there is no father
  std::shared_ptr<Benchmark> father;

  /// Name of the current benchmark zone
  std::string name;

  /// When did we enter the zone
  TimeStamp opening_time;
  /// When did we leave the zone
  TimeStamp closing_time;
  /// How much ticks were spent in the zone
  double elapsed_ticks;
  /// Storing all the children
  std::vector<std::shared_ptr<Benchmark>> children;
};

}
