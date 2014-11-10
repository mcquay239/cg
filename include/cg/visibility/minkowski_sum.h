#pragma once

#include <boost/utility.hpp>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cg/primitives/contour.h>
#include <cg/operations/orientation.h>

namespace cg
{
   template <class Scalar>
   contour_2t<Scalar> get_minkovsky_sum_convex(contour_2t<Scalar> const& base, contour_2t<Scalar> const& add)
   {
      std::vector<point_2t<Scalar> > ans;
      auto base_it = base.circulator(std::min_element(base.begin(), base.end()));
      auto add_it = add.circulator(std::min_element(add.begin(), add.end()));
      while (ans.size() < base.size() + add.size()) {
         ans.push_back(*base_it + (vector_2t<Scalar>) *add_it);
         point_2t<Scalar> help_point  = *base_it + (*boost::next(add_it) - *add_it);
         orientation(*base_it, *boost::next(base_it), help_point) == CG_RIGHT ? add_it++ : base_it++;
      }
      return ans;
   }
}
