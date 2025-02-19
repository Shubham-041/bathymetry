/* Copyright 2019 Ignacio Torroba (torroba@kth.se)
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "graph_optimization/types.h"

namespace ceres_optimizer {

std::istream& operator>>(std::istream& input, Pose3d& pose) {
    Eigen::Vector3d p;
    Eigen::Quaterniond q;

  input >> p.x() >> p.y() >> p.z() >> q.x() >>
      q.y() >> q.z() >> q.w();
  // Normalize the quaternion to account for precision loss due to
  // serialization.
  pose.p = p;
  pose.q = q.normalized().toRotationMatrix().eulerAngles(0,1,2);

  return input;
}

typedef std::map<int, Pose3d, std::less<int>,
                 Eigen::aligned_allocator<std::pair<const int, Pose3d> > >
    MapOfPoses;


std::istream& operator>>(std::istream& input, Constraint3d& constraint) {
  Pose3d& t_be = constraint.t_be;
  input >> constraint.id_begin >> constraint.id_end >> t_be;
//  t_be.q = constraint.t_be.q.toRotationMatrix().eulerAngles(0,1,2);

  for (int i = 0; i < 6 && input.good(); ++i) {
    for (int j = i; j < 6 && input.good(); ++j) {
      input >> constraint.information(i, j);
      if (i != j) {
        constraint.information(j, i) = constraint.information(i, j);
      }
    }
  }
  return input;
}

}  // namespace optimizer

