#pragma once

#include "range.h"

namespace cg
{
   template <class Scalar> struct vector_2t;
   typedef vector_2t<double> vector_2;
   typedef vector_2t<float>  vector_2f;
   typedef vector_2t<int>    vector_2i;

   template <class Scalar>
   struct vector_2t
   {
      Scalar x, y;

      vector_2t<Scalar> & operator *= (Scalar s)
      {
         x *= s;
         y *= s;

         return *this;
      }

      vector_2t(Scalar x, Scalar y)
         : x(x)
         , y(y)
      {}
   };

   template <class Scalar>
   Scalar operator ^ (vector_2t<Scalar> const & v1, vector_2t<Scalar> const & v2)
   {
      Scalar x1 = v1.x, y1 = v1.y, x2 = v2.x, y2 = v2.y;
      return x1 * y2 - x2 * y1;
   }

   template <class Scalar>
   Scalar operator * (vector_2t<Scalar> const & v1, vector_2t<Scalar> const & v2)
   {
      Scalar x1 = v1.x, y1 = v1.y, x2 = v2.x, y2 = v2.y;
      return x1 * x2 + y1 * y2;
   }

   template <class Scalar>
   vector_2t<Scalar> operator * (vector_2t<Scalar> v, Scalar s)
   {
      return v *= s;
   }

   template <class Scalar>
   vector_2t<Scalar> operator * (Scalar s, vector_2t<Scalar> v)
   {
      return v *= s;
   }

   template <class Scalar>
   vector_2t<Scalar> const operator - (vector_2t<Scalar> const & v) { return vector_2t<Scalar>(-v.x, -v.y); }
}
