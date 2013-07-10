#include <gtest/gtest.h>

#include "cg/operations/point_in_convex_contour.h"
#include <cg/primitives/segment.h>
#include <cg/primitives/point.h>
#include <cg/primitives/contour.h>
#include "random_utils.h"

TEST(point_in_convex_contour, test1)
{
    std::vector<cg::point_2> v;
    v.push_back(cg::point_2(0, 0));
    v.push_back(cg::point_2(1, 0));
    v.push_back(cg::point_2(2, 1));
    v.push_back(cg::point_2(1, 2));
    v.push_back(cg::point_2(0, 1));

    cg::contour_2 poly(v);
    cg::point_2 p[] = {cg::point_2(1, 1), cg::point_2(2, 2), cg::point_2(1, 0), cg::point_2(2, 0), cg::point_2(1, -1), cg::point_2(0, 1), cg::point_2(0, 2)};

    EXPECT_TRUE(cg::point_in_convex_contour(p[0], poly));
    EXPECT_FALSE(cg::point_in_convex_contour(p[1], poly));
    EXPECT_TRUE(cg::point_in_convex_contour(p[2], poly));
    EXPECT_FALSE(cg::point_in_convex_contour(p[3], poly));
    EXPECT_FALSE(cg::point_in_convex_contour(p[4], poly));
    EXPECT_TRUE(cg::point_in_convex_contour(p[5], poly));
    EXPECT_FALSE(cg::point_in_convex_contour(p[6], poly));

}

TEST(point_in_convex_contour, test2)
{
    std::vector<cg::point_2> v;
    v.push_back(cg::point_2(2, 2));
    v.push_back(cg::point_2(5, 2));
    v.push_back(cg::point_2(2, 5));

    cg::contour_2 poly(v);
    cg::point_2 p[] = {cg::point_2(2, 2), cg::point_2(3, 3), cg::point_2(-4, 0), cg::point_2(2, 4)};

    EXPECT_TRUE(cg::point_in_convex_contour(p[0], poly));
    EXPECT_TRUE(cg::point_in_convex_contour(p[1], poly));
    EXPECT_FALSE(cg::point_in_convex_contour(p[2], poly));
    EXPECT_TRUE(cg::point_in_convex_contour(p[3], poly));
}

TEST(point_in_convex_contour, test3)
{
    std::vector<cg::point_2> v;
    v.push_back(cg::point_2(3, 6));
    v.push_back(cg::point_2(2, 2));
    v.push_back(cg::point_2(3, 1));
    v.push_back(cg::point_2(5, 2));
    v.push_back(cg::point_2(6, 3));
    v.push_back(cg::point_2(5, 5));

    cg::contour_2 poly(v);
    cg::point_2 p[] = {cg::point_2(3, 2), cg::point_2(5, 2), cg::point_2(3, 7), cg::point_2(4, 4), cg::point_2(5, 3), cg::point_2(2, 4), cg::point_2(6, 4)};

    EXPECT_TRUE(cg::point_in_convex_contour(p[0], poly));
    EXPECT_TRUE(cg::point_in_convex_contour(p[1], poly));
    EXPECT_FALSE(cg::point_in_convex_contour(p[2], poly));
    EXPECT_TRUE(cg::point_in_convex_contour(p[3], poly));
    EXPECT_TRUE(cg::point_in_convex_contour(p[4], poly));
    EXPECT_FALSE(cg::point_in_convex_contour(p[5], poly));
    EXPECT_FALSE(cg::point_in_convex_contour(p[6], poly));
}


