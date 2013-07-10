#include <gtest/gtest.h>

#include <cg/primitives/contour.h>
#include <cg/primitives/point.h>
#include <cg/operations/polygon_orientation.h>
#include <vector>
#include "random_utils.h"

TEST(polygon_orientation, test1)
{
    std::vector<cg::point_2> v;
    v.push_back(cg::point_2(0, 0));
    v.push_back(cg::point_2(0, 1));
    v.push_back(cg::point_2(1, 2));
    v.push_back(cg::point_2(2, 1));
    v.push_back(cg::point_2(1, 0));

    cg::contour_2 poly(v);
    EXPECT_TRUE(cg::polygon_orientation(poly) == cg::CLOCKWISE);
}
