#include <cg/algorithms/diameter.h>
#include <gtest/gtest.h>
#include <vector>
#include <cg/operations/distance.h>
#include "random_utils.h"

using namespace cg;

TEST(diameter, test1) {
   auto v = uniform_points(1000);

   auto max_d = distance(v[0], v[1]);
   for (auto s = v.begin(); s != v.end(); ++s)
      for (auto t = std::next(s); t != v.end(); ++t)
      {
         auto d = distance(*s, *t);
         max_d = std::max(d, max_d);
      }

   auto res = diameter(v.begin(), v.end());
   ASSERT_FALSE(res.first == v.end());
   auto d = distance(*res.first, *res.second);
   ASSERT_EQ(max_d, d);
}

