#include <gtest/gtest.h>
#include <cg/primitives/contour.h>
#include <cg/primitives/point.h>
#include <cg/io/point.h>
#include <vector>
#include <iostream>
#include <boost/assign/list_of.hpp>
#include <cg/visibility/minkowski_sum.h>

using namespace std;
using cg::point_2;
using cg::contour_2;
//TODO: poor testing, need to be improved
TEST(minkovsky, sample)
{
   std::vector<point_2> add = boost::assign::list_of(point_2(0, 0))
                              (point_2(1, 0))
                              (point_2(1, 1))
                              (point_2(0, 1));

   std::vector<point_2> base = boost::assign::list_of(point_2(3, 0))
                               (point_2(4, 0))
                               (point_2(4, 1))
                               (point_2(3, 1));

   auto ans = get_minkovsky_sum_convex(contour_2(base), contour_2(add));
	EXPECT_TRUE(ans.size() == 8);
}
