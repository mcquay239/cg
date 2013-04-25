#pragma once

#include <cg/primitives/contour.h>
#include "orientation.h"

namespace cg
{
   // c is ccw contour
   inline bool convex(contour_2 const & c)
   {
      size_t cnt_vertices = c.size();

      if (cnt_vertices < 3)
      {
         return true;
      }

      contour_2::circulator_t t3 = c.circulator();
      contour_2::circulator_t t1 = t3++;
      contour_2::circulator_t t2 = t3++;

      for (size_t i = 0; i < cnt_vertices; ++i)
      {
         if (orientation(*t1, *t2, *t3) == CG_RIGHT)
         {
            return false;
         }
         ++t1;
         ++t2;
         ++t3;
      }

      return true;
   }
}