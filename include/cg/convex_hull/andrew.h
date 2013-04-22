#pragma once

#include <algorithm>
#include <cg/operations/orientation.h>

#include "graham.h"

namespace cg
{
   template <class RandIter>
   RandIter andrew_hull(RandIter p, RandIter q)
   {
      if (p == q)
         return p;

      std::iter_swap(p, std::min_element(p, q));

      RandIter t = p++;

      if (p == q)
         return p;

      std::iter_swap(p, std::max_element(p, q));

      RandIter pt = p++;

      if (p == q)
         return p;

      RandIter m = std::partition(p, q, [t, pt] (point_2 const & a)
                                        { return orientation(*t, *pt, a) != CG_LEFT; }
                                 );

      std::iter_swap(pt, m - 1);

      std::sort(pt, m - 1);
      std::sort(m, q, std::greater<typename std::iterator_traits<RandIter>::value_type>());

      return contour_graham_hull(t, q);
   }
}
