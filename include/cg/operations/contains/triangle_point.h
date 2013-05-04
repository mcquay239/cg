#pragma once

#include <cg/primitives/point.h>
#include <cg/primitives/triangle.h>
#include <cg/primitives/segment.h>

#include <cg/operations/orientation.h>
#include <cg/operations/contains/segment_point.h>
#include <algorithm>

namespace cg
{
   template<class Scalar>
   bool contains(triangle_2t<Scalar> const & t, point_2t<Scalar> const & q)
   {
      orientation_t to = orientation(t[0], t[1], t[2]);

      if (to == CG_COLLINEAR)
      {
         segment_2 s(*std::min_element(&t[0], &t[0] + 3),
                     *std::max_element(&t[0], &t[0] + 3));

         return contains(s, q);
      }

      for (size_t l = 0, lp = 2; l != 3; lp = l++)
         if (opposite(orientation(t[lp], t[l], q), to))
             return false;

      return true;
   }
}
