#pragma once

#include <cg/primitives/triangle.h>
#include <cg/primitives/segment.h>

#include <cg/operations/contains/triangle_point.h>
#include <cg/operations/has_intersection/segment_segment.h>

namespace cg
{
   template<class Scalar>
   bool has_intersection(triangle_2t<Scalar> const & t, segment_2t<Scalar> const & s)
   {
      if (contains(t, s[0]))
         return true;

      for (size_t l = 0; l != 3; ++l)
         if (has_intersection(t.side(l), s))
            return true;

      return false;
   }
}
