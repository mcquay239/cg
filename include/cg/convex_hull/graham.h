#pragma once

#include <algorithm>

#include <cg/operations/orientation.h>

namespace cg
{
   template <class BidIter>
   BidIter contour_graham_hull(BidIter p, BidIter q)
   {
      if (p == q)
         return p;

      BidIter b = p;

      BidIter pt = p++;

      if (p == q)
         return p;

      BidIter t = p++;

      if (p == q)
         return p;

      for (; p != q; )
      {
         switch (orientation(*pt, *t, *p))
         {
         case CG_LEFT:
            pt = t++;
            std::iter_swap(t, p++);
            break;
         case CG_RIGHT:
            if (pt == b)
            {
               std::iter_swap(t, p++);
               break;
            }
            t = pt--;
            break;
         case CG_COLLINEAR:
            std::iter_swap(t, p++);
         }
      }

      while (pt != b && orientation(*pt, *t, *b) != CG_LEFT)
         t = pt--;

      return ++t;
   }

   template <class RandIter>
   RandIter graham_hull(RandIter p, RandIter q)
   {
      if (p == q)
         return p;

      std::iter_swap(p, std::min_element(p, q));

      RandIter t = p++;

      if (p == q)
         return p;

      std::sort(p, q, [t] (point_2 const & a, point_2 const & b)
                        {
                           switch (orientation(*t, a, b))
                           {
                           case CG_LEFT: return true;
                           case CG_RIGHT: return false;
                           case CG_COLLINEAR: return a < b;
                           }
                        }
               );

      return contour_graham_hull(t, q);
   }
}
