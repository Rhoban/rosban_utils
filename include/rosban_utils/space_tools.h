#pragma once

#include <Eigen/Core>

#include <vector>

namespace rosban_utils
{

/// Return a matrix containing pow(samples_by_dim, nb_dims) columns and limits.rows() rows
/// Each column is a different sample
Eigen::MatrixXd discretizeSpace(const Eigen::MatrixXd & limits,
                                int samples_by_dim);

/// Return a matrix containing product(samples_by_dim) columns and limits.rows() rows
/// Each column is a different sample
Eigen::MatrixXd discretizeSpace(const Eigen::MatrixXd & limits,
                                const std::vector<int> & samples_by_dim);
}
