#pragma once

#include <cg/primitives/rectangle.h>
#include <cg/primitives/segment.h>

#include <cg/operations/has_intersection/segment_segment.h>

namespace cg
{
   template<class Scalar>
   bool has_intersection(rectangle_2t<Scalar> const& r, segment_2t<Scalar> const& s)
   {
      return r.contains(s[0]) ||
             has_intersection(segment_2t<Scalar>(r.corner(0, 0), r.corner(0, 1)), s) ||
             has_intersection(segment_2t<Scalar>(r.corner(0, 1), r.corner(1, 1)), s) ||
             has_intersection(segment_2t<Scalar>(r.corner(1, 1), r.corner(1, 0)), s) ||
             has_intersection(segment_2t<Scalar>(r.corner(1, 0), r.corner(0, 0)), s);
   }
}
