#pragma once

#include <Eigen/Core>

#include <vector>

namespace rosban_utils
{

/// This class implements the notion of space in continuous domain. It contains
/// specific notions such as the notion of cyclicity (e.g. angular spaces)
class Space
{
public:

  /// Default constructor
  Space();

  /// No cyclic dimensions 
  void setLimits(const Eigen::MatrixXd & limits);
  /// The possibility of cyclic dimensions is provided
  void setLimits(const Eigen::MatrixXd & limits,
                 const std::vector<bool> & cyclic);

  /// Return the number of dimensions
  int getDim() const;

  /// return true if the dimension 'dim' is marked as cyclic
  bool isCyclic(int dim);

  /// Return the absolute distance between p1 and p2 along each dimension, for
  /// cyclic dimension, the distance is defined as the shortest path when
  /// allowed to jump from the minimum to the maximum: 
  /// min(|p2(d) - p1(d)|, |max(d) - max(p1,p2)| + |min(p1,p2) - min(d)|)
  Eigen::VectorXd getDist(const Eigen::VectorXd & p1,
                          const Eigen::VectorXd & p2) const;

protected:
  Eigen::MatrixXd limits;
  std::vector<bool> cyclicity;
};

}
