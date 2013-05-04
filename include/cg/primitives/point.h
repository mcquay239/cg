#pragma once

#include "vector.h"

namespace cg
{
   template <class Scalar> struct point_2t;

   typedef point_2t<double>   point_2;
   typedef point_2t<float>    point_2f;
   typedef point_2t<int>      point_2i;

   template <class Scalar>
   struct point_2t
   {
      Scalar x, y;

      point_2t(Scalar x, Scalar y)
         : x(x)
         , y(y)
      {}

      template <class UScalar>
      point_2t(point_2t<UScalar> const & o)
         : x(o.x)
         , y(o.y)
      {}

      point_2t()
         : x(0)
         , y(0)
      {}

      point_2t<Scalar> & operator += (vector_2t<Scalar> const & delta)
      {
         x += delta.x;
         y += delta.y;
         return *this;
      }
   };

   template <class Scalar>
   inline bool operator < (point_2t<Scalar> const & a, point_2t<Scalar> const & b)
   {
      if (a.x == b.x)
         return a.y < b.y;
      else
         return a.x < b.x;
   }

   template <class Scalar>
   bool operator > (point_2t<Scalar> const & a, point_2t<Scalar> const & b)
   {
      return b < a;
   }

   template <class Scalar>
   bool operator == (point_2t<Scalar> const & a, point_2t<Scalar> const & b)
   {
      return (a.x == b.x) && (a.y == b.y);
   }

   template <class Scalar>
   bool operator <= (point_2t<Scalar> const & a, point_2t<Scalar> const & b)
   {
      return !(a > b);
   }

   template <class Scalar>
   bool operator >= (point_2t<Scalar> const & a, point_2t<Scalar> const & b)
   {
      return !(a < b);
   }


   template <class Scalar>
   bool operator != (point_2t<Scalar> const & a, point_2t<Scalar> const & b)
   {
      return !(a == b);
   }

   // в этом месте возможно переполнение!
   template <class Scalar>
   vector_2t<Scalar> const operator - (point_2t<Scalar> const & a, point_2t<Scalar> const & b)
   {
      return vector_2t<Scalar>(a.x - b.x, a.y - b.y);
   }

   // в этом месте возможно переполнение!
   template <class Scalar>
   point_2t<Scalar> const operator + (point_2t<Scalar> const & pt, vector_2t<Scalar> const & delta)
   {
      point_2t<Scalar> res(pt);
      res += delta;
      return res;
   }
}
