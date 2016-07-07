#include "rosban_utils/space.h"

#include <sstream>
#include <stdexcept>

namespace rosban_utils
{

Space::Space() {}

void Space::setLimits(const Eigen::MatrixXd & limits_)
{
  std::vector<bool> cyclicity_(limits_.rows(), false);
  setLimits(limits_, cyclicity_);
}

void Space::setLimits(const Eigen::MatrixXd & limits_,
                      const std::vector<bool> & cyclicity_)
{
  if (limits_.cols() != 2) {
    std::ostringstream oss;
    oss << "Space:setLimits: invalid number of columns: " << limits_.cols();
    throw std::logic_error(oss.str());
  } 
  if (limits_.rows() != cyclicity_.size()) {
    std::ostringstream oss;
    oss << "Space:setLimits: inconsistent arguments: limits has "
        << limits_.rows() << " rows and cyclicity is of size: "
        << cyclicity_.size();
    throw std::logic_error(oss.str());
  }
  limits = limits_;
  cyclicity = cyclicity_;
}

int Space::getDim() const
{
  return limits.rows();
}

bool Space::isCyclic(int dim)
{
  if (dim < 0) {
    throw std::logic_error("Space::isCyclic: negative index provided");
  }
  if (dim >= getDim()) {
    std::ostringstream oss;
    oss << "Space::isCyclic: invalid index '" << dim << "' max is: '"
        << getDim() - 1 << "'";
    throw std::logic_error(oss.str());
  }
  return cyclicity[dim];
}

Eigen::VectorXd Space::getDist(const Eigen::VectorXd & p1,
                               const Eigen::VectorXd & p2) const
{
  // Test if dimensions matches:
  if (p1.rows() != getDim() || p2.rows() != getDim()) {
    std::ostringstream oss;
    oss << "Space::getDist: inconsistent sizes for points and space: "
        << "Size of space: '" << getDim() << "'"
        << "Size of p1: '" << p1.rows() << "'"
        << "Size of p2: '" << p2.rows() << "'";
    throw std::logic_error(oss.str());
  }
  Eigen::VectorXd dist(getDim());
  for (int d = 0; d < getDim(); d++) {
    dist(d) = std::fabs(p1(d) - p2(d));
    if (cyclicity(d)) {
      double pMin, pMax;
      if (p1(d) > p2(d)) {
        pMax = p1(d);
        pMin = p2(d);
      }
      else {
        pMax = p2(d);
        pMin = p1(d);
      }
      double altDist = (limits(d,1) - pMax) + (pMin - limits(d,0));
      if (altDist < dist(d)) {
        dist(d) = altDist;
      }
    }
  }
  return dist;
}

}
