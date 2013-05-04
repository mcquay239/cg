#pragma once

#include <limits>
#include <algorithm>

namespace cg
{
   // closed range
   template <class Scalar>
   struct range_t;

   typedef range_t<double> range;
   typedef range_t<float>  range_f;
   typedef range_t<int>    range_i;

   template <class Scalar>
   struct range_t
   {
      Scalar inf, sup;

      range_t(Scalar inf, Scalar sup)
         : inf(inf)
         , sup(sup)
      {}

      range_t()
         : inf(0)
         , sup(-1)
      {}

      bool is_empty() const { return inf > sup; }

      bool contains(Scalar x) const { return (inf <= x) && (x <= sup); }

      static range_t maximal()
      {
         static const Scalar max_val = std::numeric_limits<Scalar>::max();
         return range_t(-max_val, max_val);
      }
   };

   template <class Scalar>
   range_t<Scalar> const operator & (range_t<Scalar> const & a, range_t<Scalar> const & b)
   {
      return range_t<Scalar>(std::max(a.inf, b.inf), std::min(a.sup, b.sup));
   }

   inline float center(range_f const & r)
   {
      return .5f + r.inf / 2.f + r.sup / 2.f;
   }

   template <class Scalar>
   Scalar size(range_t<Scalar> const & r)
   {
      return r.sup - r.inf;
   }
}
