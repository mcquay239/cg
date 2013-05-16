#pragma once
#include <cg/primitives/point.h>

using cg::point_2;
namespace cg
{

   template <class Iterator>
   struct dynamic_hull_interface
   {
      virtual void add_point(point_2 p) = 0;
      virtual void remove_point(const point_2& p) = 0;
      virtual const std::pair<Iterator, Iterator> get_hull() = 0;
      virtual const std::pair<Iterator, Iterator> get_all_points() = 0;
      virtual ~dynamic_hull_interface() {}
   };
}

