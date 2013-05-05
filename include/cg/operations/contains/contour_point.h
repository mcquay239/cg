#pragma once

#include <cg/primitives/contour.h>
#include <cg/operations/orientation.h>

namespace cg
{
   template<typename Scalar>
   inline bool contains(contour_2t<Scalar> const & a, point_2t<Scalar> const & b)
   {
      int num_intersections = 0;
      for (size_t pr = a.vertices_num() - 1, cur = 0; cur != a.vertices_num(); pr = cur++) {
         point_2t<Scalar> min_point = a[pr];
         point_2t<Scalar> max_point = a[cur];
         if (min_point.y > max_point.y)
            std::swap(min_point, max_point);
         orientation_t orient = orientation(min_point, max_point, b);
         if (orient == CG_COLLINEAR && std::min(min_point, max_point) <= b && b <= std::max(min_point, max_point))
            return true;
         if (max_point.y <= b.y || min_point.y > b.y)
            continue;
         if (orient == CG_LEFT)
            num_intersections++;
      }
      return num_intersections % 2;
   }
}
