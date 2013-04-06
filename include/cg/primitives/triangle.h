#pragma once

#include "range.h"
#include "point.h"

namespace cg
{
   template <class Scalar> struct triangle_2t;

   typedef triangle_2t<double> triangle_2;
   typedef triangle_2t<float> triangle_2f;
   typedef triangle_2t<int>   triangle_2i;

   template <class Scalar>
   struct triangle_2t
   {
       point_2t<Scalar> p[3];

      triangle_2t(point_2t<Scalar> const & a, point_2t<Scalar> const & b, point_2t<Scalar> const & c)
      {
          p[0] = point_2t<Scalar>(a);
          p[1] = point_2t<Scalar>(b);
          p[2] = point_2t<Scalar>(c);
      }

      point_2t<Scalar> const & operator[] (size_t i) const
      {
          if (i < 3) {
              return p[i];
          } else {
              throw std::logic_error("invalid index: " + boost::lexical_cast<std::string>(i));
         }
      }

      point_2t<Scalar> & operator[] (size_t i)
      {
          if (i < 3) {
              return p[i];
          } else {
              throw std::logic_error("invalid index: " + boost::lexical_cast<std::string>(i));
         }
      }
    };
}
