#pragma once

#include "range.h"
#include "point.h"

namespace cg
{
   template <class Scalar> struct rectangle_2t;

   typedef rectangle_2t<double> rectangle_2;
   typedef rectangle_2t<float>  rectangle_2f;
   typedef rectangle_2t<int>    rectangle_2i;

   template <class Scalar>
   struct rectangle_2t
   {
      range_t<Scalar> x, y;

      rectangle_2t(range_t<Scalar> const & x, range_t<Scalar> const & y)
         : x(x)
         , y(y)
      {}

      rectangle_2t() {}

      bool is_empty() const
      {
         return x.is_empty() && y.is_empty();
      }

      bool contains(point_2t<Scalar> const & pt) const
      {
         return x.contains(pt.x) && y.contains(pt.y);
      }

      point_2t<Scalar> corner(size_t h, size_t v) const
      {
         return point_2t<Scalar> (  (h == 0) ? x.inf : x.sup,
                                    (v == 0) ? y.inf : y.sup  );
      }

      static rectangle_2t maximal()
      {
         return rectangle_2t(range_t<Scalar>::maximal(), range_t<Scalar>::maximal());
      }
   };

   template <class Scalar>
   rectangle_2t<Scalar> const operator & (rectangle_2t<Scalar> const & a, rectangle_2t<Scalar> const & b)
   {
      return rectangle_2t<Scalar>(a.x & b.x, a.y & b.y);
   }

   inline point_2f center(rectangle_2f const & rect)
   {
      return point_2f(center(rect.x), center(rect.y));
   }
}
