#pragma once

#include <cg/primitives/segment.h>

#include <cg/operations/orientation.h>

namespace cg
{
   template<class Scalar>
   bool has_intersection(segment_2t<Scalar> const & a, segment_2t<Scalar> const & b)
   {
      orientation_t ab[2];
      for (size_t l = 0; l != 2; ++l)
         ab[l] = orientation(a[0], a[1], b[l]);

      if (ab[0] == ab[1] && ab[0] == CG_COLLINEAR)
         return (min(a) <= b[0] && max(a) >= b[0])
            || (min(a) <= b[1] && max(a) >= b[1])
            || (min(b) <= a[0] && max(b) >= a[0])
            || (min(b) <= a[1] && max(b) >= a[1]);
   
      if (ab[0] == ab[1])
         return false;

      for (size_t l = 0; l != 2; ++l)
         ab[l] = orientation(b[0], b[1], a[l]);

      return ab[0] != ab[1];
   }
}
