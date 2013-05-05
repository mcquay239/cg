#pragma once
#include <cg/convex_hull/dynamic_hull_interface.h>
#include "graham.h"
typedef std::vector<point_2>::iterator vect_it;
namespace cg
{

   struct naive_dynamic_hull
   {
      void add_point(point_2 p)
      {
         points.push_back(p);
      }

      void remove_point(const point_2& p)
      {
         vect_it it = std::find(points.begin(), points.end(), p);

         if (it != points.end())
         {
            points.erase(it);
         }
      }

      const std::pair<vect_it, vect_it> get_hull()
      {
         vect_it it = cg::graham_hull(points.begin(), points.end());
         return std::pair<vect_it, vect_it>(points.begin(), it);
      }

      const std::pair<vect_it, vect_it> get_all_points()
      {
         return std::pair<vect_it, vect_it>(points.begin(), points.end());
      }

   private:
      std::vector<point_2> points;
   };
}
