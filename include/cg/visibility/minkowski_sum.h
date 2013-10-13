#pragma once

#include <vector>
#include <algorithm>
#include <iterator>
#include <cg/primitives/point.h>
#include <cg/primitives/contour.h>
#include <cg/operations/orientation.h>

namespace cg
{
   template <class Scalar>
   contour_2t<Scalar> get_minkovsky_sum(contour_2t<Scalar> const& base, contour_2t<Scalar> const& add)
   {
      typedef point_2t<Scalar> Point;
      typedef contour_2t<Scalar> Contour;
      typedef vector_2t<Scalar> Vector;

      std::vector<Point> ans;
      auto base_it = std::min_element(base.begin(), base.end()), add_it = std::min_element(add.begin(), add.end());
      for (int i = 0; i < base.size(); i++)
      {
         auto base_it_next = base_it == base.end() - 1 ? base.begin() : base_it + 1;
         while (true)
         {
            auto add_it_next = add_it == add.end() - 1 ? add.begin() : add_it + 1;
            // current vector in "add" polygon
            Vector vect(add_it_next->x - add_it->x, add_it_next->y - add_it->y);
            // point from the end of current "base" vector
            Point help_point(base_it_next->x + vect.x, base_it_next->y + vect.y);
            ans.push_back(Point(base_it->x + add_it->x, base_it->y + add_it->y));
            if (orientation(*base_it, *base_it_next, help_point) == CG_LEFT)
            {
               break;
            }
            add_it = add_it_next;
         }
         base_it = base_it_next;
      }
      return ans;
   }

}
