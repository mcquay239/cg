#pragma once

#include <cg/primitives/contour.h>
#include <cg/primitives/point.h>
#include <cg/primitives/triangle.h>
#include <cg/operations/orientation.h>
#include <cg/operations/contains/triangle_point.h>
#include <cg/operations/contains/segment_point.h>

#include <iostream>
#include <cg/io/point.h>
using std::cerr;
using std::endl;

namespace cg
{
   // c is convex contour ccw orientation
   inline bool convex_contains(contour_2 const & c, point_2 const & q)
   {
      size_t cnt_vertices = c.size();

      if (cnt_vertices == 0)
         return false;
      if (cnt_vertices == 1)
         return c[0] == q;
      if (cnt_vertices == 2)
         return cg::contains(cg::segment_2(c[0], c[1]), q);

      if (cg::orientation(c[0], c[1], q) == CG_RIGHT)
         return false;

      contour_2::const_iterator it = std::lower_bound(c.begin() + 2, c.end(), q,
         [&c] (point_2 const& a, point_2 const& b)
         {
            return cg::orientation(c[0], a, b) == cg::CG_LEFT;
         }
      );

      if (it == c.end()) // out
         return false;

      return cg::orientation(*(it - 1), *(it), q) != CG_RIGHT;
   }

   // c is ordinary contour
   inline bool contains(contour_2 const & c, point_2 const & q);
}
