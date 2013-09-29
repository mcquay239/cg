#pragma once
#include <cg/primitives/point.h>
#include <cg/primitives/segment.h>
#include <cmath>

namespace cg {
   template<class S>
   inline double distance(point_2t<S> const& a, point_2t<S> const& b)
   {
      auto v = b - a;
      return sqrt(v * v);
   }

   inline double projection_on_segment(segment_2 const& s, point_2 const& p)
   {
      auto v = s[1] - s[0];
      return (v * (p - s[0])) / (v * v);
   }

   template<class S>
   inline double length(segment_2t<S> const& s)
   {
      return distance(s[0], s[1]);
   }

   inline double distance(segment_2 const& s, point_2 const& p)
   {
      auto projection = projection_on_segment(s, p);
      if (projection < 0)
         return distance(s[0], p);
      else if (projection > 1)
         return distance(s[1], p);
      else
         return fabs((s[1] - s[0]) ^ (p - s[0])) / length(s);
   }
}
