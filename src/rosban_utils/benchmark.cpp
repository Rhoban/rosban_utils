#include "rosban_utils/benchmark.h"

#include <algorithm>
#include <exception>
#include <iomanip>
#include <iostream>
#include <tuple>

using namespace std::chrono;

namespace rosban_utils
{

/* Static variables */
std::shared_ptr<Benchmark> Benchmark::current;

Benchmark::Benchmark(std::shared_ptr<Benchmark> f, const std::string & n)
  : father(f), name(n), elapsed_ticks(0)
{
  startSession();
}

std::shared_ptr<Benchmark> Benchmark::getCurrent()
{
  if (!current)
    open("Default");
  return current;
}

Benchmark::~Benchmark()
{
}

void Benchmark::startSession()
{
  opening_time = steady_clock::now();
}

void Benchmark::endSession()
{
  closing_time = steady_clock::now();
  elapsed_ticks += double((closing_time - opening_time).count());
}


void Benchmark::open(const std::string &benchmark_name)
{
  // If child is not existing yet:
  std::shared_ptr<Benchmark> child_benchmark(new Benchmark(current, benchmark_name));
  current = child_benchmark;
}

double Benchmark::close(bool print, int detailLevel, std::ostream &out)
{
  if (!current)
    throw std::runtime_error("No active benchmark to close");
  std::shared_ptr<Benchmark> to_close = current;
  to_close->endSession();

  current = to_close->father;
  if (print) {
    to_close->print(out, detailLevel);
  }
      
  return to_close->getTime();
}

double Benchmark::getTime() const
{
  double time = elapsed_ticks * steady_clock::period::num / steady_clock::period::den;
  return time;
      
}

double Benchmark::getSubTime() const
{
  double t = 0;
  for (auto& c : children) {
    t += c->getTime();
  }
  return t;
}

void Benchmark::print(std::ostream &out, int max_depth)
{
  // Formatting specifically
  int precision = 3;
  int width = 8;
  int old_precision = out.precision();
  out.precision(precision);
  out.setf(std::ios::fixed, std::ios::floatfield);

  print(out, 0, width, max_depth);

  // Restoring default
  out.precision(old_precision);
  out.unsetf(std::ios::floatfield);
}

// Printable data: <name, elapsed_time, link>
typedef std::tuple<std::string, double, std::shared_ptr<Benchmark> > PrintableEntry;
int cmp(const PrintableEntry& a,
        const PrintableEntry& b)
{
  return std::get<1>(a) < std::get<1>(b);
}

void Benchmark::print(std::ostream & out, int depth, int width,
                      int max_depth)
{
  // Build and sort printable data
  std::vector<PrintableEntry> sub_fields;
  for(auto& c : children) {
    sub_fields.push_back(PrintableEntry(c->name,
                                       c->getTime(),
                                       c));
  }
  // Add Unknown field only if there are other information
  if (sub_fields.size() > 0) {
    sub_fields.push_back(PrintableEntry("Unknown",
                                        getTime() - getSubTime(),
                                        std::shared_ptr<Benchmark>()));
  }
  // Ordering
  std::sort(sub_fields.begin(), sub_fields.end(), cmp);
  // Printing if allowed
  if (max_depth < 0 || depth < max_depth) {
    for(auto& f : sub_fields) {
      // For Benchmark, print them
      if (std::get<2>(f)){
        std::get<2>(f)->print(out, depth + 1, width, max_depth);
      }
      // Print entries
      else {
        for (int i = 0; i < depth + 1; i++) out << '\t';
        out << std::setw(width) << (std::get<1>(f) * 1000) << " ms : "
            << std::get<0>(f) << std::endl;
      }
    }
  }
  for (int i = 0; i < depth; i++) out << '\t';
  out << std::setw(width) << getTime() * 1000 << " ms : "
      << name << std::endl;
}

}
