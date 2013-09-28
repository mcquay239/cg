#pragma once
#include <cg/primitives/point.h>
#include <cg/convex_hull/andrew.h>

namespace cg {

   template<class RandIter>
   std::pair<RandIter, RandIter> diameter(RandIter begin, RandIter end)
   {
      if (begin == end || std::next(begin) == end)
         return make_pair(begin, begin);

      end = andrew_hull(begin, end);
      RandIter left = begin;
      RandIter right = std::max_element(begin, end);
      auto get_lr_distance = [&left, &right] ()
      {
         auto v = *left - *right;
         return v * v;
      };

      auto max_dist = get_lr_distance();

      RandIter left_next = std::next(left),
            right_next = std::next(right);

      if (left_next == right && right_next == end)
         return make_pair(left, right);

      auto left_bound = right;
      auto left_candidate = left, right_candidate = right;

      while(left != left_bound || right != begin)
      {
         if (orientation4(*left_next, *left, *right, *right_next) == CG_RIGHT)
         {
            right = right_next++;
            if (right_next == end)
               right_next = begin;
            auto d = get_lr_distance();
            if (d > max_dist)
            {
               max_dist = d;
               left_candidate = left;
               right_candidate = right;
            }
         } else {
            left = left_next++;
            auto d = get_lr_distance();
            if (d > max_dist)
            {
               max_dist = d;
               left_candidate = left;
               right_candidate = right;
            }
         }
      }

      return make_pair(left_candidate, right_candidate);
   }

}
