#pragma once

#include <algorithm>

#include <cg/operations/orientation.h>

namespace cg
{
   template <class RandIter>
   RandIter remove_points_on_same_line(RandIter p, RandIter q)
   {
      if (p == q || p + 1 == q)
         return q;
      auto ok = p + 1;
      for (auto cur = p + 2; cur != q; cur++) {
         if (orientation(*(ok - 1), *ok, *cur) != CG_COLLINEAR)
            ok++;         
         std::iter_swap(ok, cur);
      }
      if (orientation(*(ok - 1), *ok, *p) == CG_COLLINEAR && ok != p + 1)
         ok--;
      return ok + 1;
   }

   template <class RandIter>
   RandIter jarvis_hull(RandIter p, RandIter q)
   {
      if (p == q || q == p + 1)
         return q;
      auto min_elem = std::min_element(p, q);
      std::iter_swap(p, min_elem);
      auto last = p;
      while (last != q - 1) {
         auto next_p = std::min_element(last + 1, q, [last] (typename std::iterator_traits< RandIter >::value_type const & a,
                                                             typename std::iterator_traits< RandIter >::value_type const & b)
                                           { 
                                             orientation_t orient = orientation(*last, a, b);
                                             if (orient == CG_RIGHT) return false;
                                             if (orient == CG_LEFT) return true;
                                             return collinear_are_ordered_along_line(*last, a, b);
                                          });
         if (orientation(*last, *next_p, *p) == CG_RIGHT)
            break;
         std::iter_swap(last + 1, next_p);
         last++;
      }
      return remove_points_on_same_line(p, last + 1);
   }
}
