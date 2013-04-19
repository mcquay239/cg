#pragma once

#include <cg/primitives/point.h>
#include <cg/primitives/segment.h>
#include <cg/primitives/range.h>

#include <cg/operations/orientation.h>

namespace cg
{
	template<class Scalar>
   bool contains(segment_2t<Scalar> const & s, point_2t<Scalar> const & q)
   {
      if (cg::orientation(s[0], s[1], q) != CG_COLLINEAR)
         return false;

      return collinear_are_ordered_along_line(s[0], q, s[1]);
   }
}
