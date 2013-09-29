#pragma once

#include "cg/primitives/point.h"
#include "cg/primitives/contour.h"
#include <boost/numeric/interval.hpp>
#include <gmpxx.h>

#include <boost/optional.hpp>

namespace cg
{
   enum orientation_t
   {
      CG_RIGHT = -1,
      CG_COLLINEAR = 0,
      CG_LEFT = 1
   };

   inline bool opposite(orientation_t a, orientation_t b)
   {
      if (a == CG_COLLINEAR || b == CG_COLLINEAR)
         return false;

      return a == -b;
   }

   struct orientation_d
   {
      boost::optional<orientation_t> operator() (point_2 const& a, point_2 const& b, point_2 const& c, point_2 const& d) const
      {
         double l = (b.x - a.x) * (d.y - c.y);
         double r = (b.y - a.y) * (d.x - c.x);
         double res = l - r;
         double eps = (fabs(l) + fabs(r)) * 8 * std::numeric_limits<double>::epsilon();

         if (res > eps)
            return CG_LEFT;

         if (res < -eps)
            return CG_RIGHT;

         return boost::none;
      }
   };

   struct orientation_i
   {
      boost::optional<orientation_t> operator() (point_2 const& a, point_2 const& b, point_2 const& c, point_2 const& d) const
      {
         typedef boost::numeric::interval_lib::unprotect<boost::numeric::interval<double> >::type interval;

         boost::numeric::interval<double>::traits_type::rounding _;
         interval res =   (interval(b.x) - a.x) * (interval(d.y) - c.y)
                          - (interval(b.y) - a.y) * (interval(d.x) - c.x);

         if (res.lower() > 0)
            return CG_LEFT;

         if (res.upper() < 0)
            return CG_RIGHT;

         if (res.upper() == res.lower())
            return CG_COLLINEAR;

         return boost::none;
      }
   };

   struct orientation_r
   {
      boost::optional<orientation_t> operator() (point_2 const& a, point_2 const& b, point_2 const& c, point_2 const& d) const
      {
         mpq_class res =   (mpq_class(b.x) - a.x) * (mpq_class(d.y) - c.y)
                           - (mpq_class(b.y) - a.y) * (mpq_class(d.x) - c.x);

         int cres = cmp(res, 0);

         if (cres > 0)
            return CG_LEFT;

         if (cres < 0)
            return CG_RIGHT;

         return CG_COLLINEAR;
      }

      boost::optional<orientation_t> operator()(point_2 const& a, point_2 const& b, point_2 const& c) const
      {
         return (*this)(a, b, a, c);
      }
   };

   inline orientation_t orientation4(point_2 const& a, point_2 const& b, point_2 const& c, point_2 const& d)
   {
      if (boost::optional<orientation_t> v = orientation_d()(a, b, c, d))
         return *v;

      if (boost::optional<orientation_t> v = orientation_i()(a, b, c, d))
         return *v;

      return *orientation_r()(a, b, c, d);

   }

   inline orientation_t orientation(point_2 const& a, point_2 const& b, point_2 const& c)
   {
      return orientation4(a, b, a, c);
   }

   inline bool counterclockwise(contour_2 const & c)
   {
      if (c.size() < 3) return true;

      contour_2::const_iterator it_min_point = std::min_element(c.begin(), c.end());

      point_2 min_point = *it_min_point;

      contour_2::circulator_t it_prev = --c.circulator(it_min_point);
      contour_2::circulator_t it_next = ++c.circulator(it_min_point);

      point_2 prev = *it_prev;
      point_2 next = *it_next;

      return orientation(prev, min_point, next) == CG_LEFT;
   }

   template <class Scalar>
   bool collinear_are_ordered_along_line(point_2t<Scalar> const & a, point_2t<Scalar> const & b, point_2t<Scalar> const & c)
   {
      return (a <= b && b <= c) || (c <= b && b <= a);
   }
}
