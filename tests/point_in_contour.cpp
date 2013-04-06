#include <gtest/gtest.h>
#include "cg/operations/point_in_contour.h"
#include <cg/primitives/segment.h>
#include <cg/primitives/point.h>
#include <cg/primitives/contour.h>
#include "random_utils.h"

TEST(point_in_contour, test1)
{
    std::vector<cg::point_2> v;
    v.push_back(cg::point_2(0, 0));
    v.push_back(cg::point_2(1, 0));
    v.push_back(cg::point_2(2, 1));
    v.push_back(cg::point_2(1, 2));
    v.push_back(cg::point_2(0, 1));

    cg::contour_2 poly(v);
    cg::point_2 p[] = {cg::point_2(1, 1), cg::point_2(2, 2), cg::point_2(1, 0), cg::point_2(2, 0), cg::point_2(1, -1), cg::point_2(0, 1), cg::point_2(0, 2)};

    EXPECT_TRUE(cg::point_in_contour(p[0], poly));
    EXPECT_FALSE(cg::point_in_contour(p[1], poly));
    EXPECT_TRUE(cg::point_in_contour(p[2], poly));
    EXPECT_FALSE(cg::point_in_contour(p[3], poly));
    EXPECT_FALSE(cg::point_in_contour(p[4], poly));
    EXPECT_TRUE(cg::point_in_contour(p[5], poly));
    EXPECT_FALSE(cg::point_in_contour(p[6], poly));

}

TEST(point_in_contour, test2)
{
    std::vector<cg::point_2> v;
    v.push_back(cg::point_2(1, 4));
    v.push_back(cg::point_2(1, 1));
    v.push_back(cg::point_2(13, 1));
    v.push_back(cg::point_2(13, 6));
    v.push_back(cg::point_2(11, 6));
    v.push_back(cg::point_2(12, 4));
    v.push_back(cg::point_2(11, 2));
    v.push_back(cg::point_2(10, 4));
    v.push_back(cg::point_2(9, 2));
    v.push_back(cg::point_2(8, 4));
    v.push_back(cg::point_2(6, 4));
    v.push_back(cg::point_2(5, 2));
    v.push_back(cg::point_2(4, 4));
    v.push_back(cg::point_2(3, 4));
    v.push_back(cg::point_2(2, 6));

    cg::contour_2 poly(v);
    cg::point_2 p[] = {cg::point_2(1, 1), cg::point_2(2, 4), cg::point_2(3, 4), cg::point_2(4, 4), cg::point_2(5, 4),
                       cg::point_2(6, 4), cg::point_2(7, 4) , cg::point_2(8, 4), cg::point_2(9, 4), cg::point_2(10, 4),
                       cg::point_2(11, 4), cg::point_2(12, 4), cg::point_2(13, 4)};

    EXPECT_TRUE(cg::point_in_contour(p[0], poly));
    EXPECT_TRUE(cg::point_in_contour(p[1], poly));
    EXPECT_TRUE(cg::point_in_contour(p[2], poly));
    EXPECT_TRUE(cg::point_in_contour(p[3], poly));
    EXPECT_FALSE(cg::point_in_contour(p[4], poly));
    EXPECT_TRUE(cg::point_in_contour(p[5], poly));
    EXPECT_TRUE(cg::point_in_contour(p[6], poly));
    EXPECT_TRUE(cg::point_in_contour(p[7], poly));
    EXPECT_FALSE(cg::point_in_contour(p[8], poly));
    EXPECT_TRUE(cg::point_in_contour(p[9], poly));
    EXPECT_FALSE(cg::point_in_contour(p[10], poly));
    EXPECT_TRUE(cg::point_in_contour(p[11], poly));
    EXPECT_TRUE(cg::point_in_contour(p[12], poly));


    cg::point_2 p2[] = {cg::point_2(3, 3), cg::point_2(4, 2), cg::point_2(4, 3), cg::point_2(6, 2), cg::point_2(8, 2),
                       cg::point_2(9, 3), cg::point_2(11, 3) , cg::point_2(12, 3), cg::point_2(1, 5), cg::point_2(9, 6)};

    EXPECT_TRUE(cg::point_in_contour(p2[0], poly));
    EXPECT_TRUE(cg::point_in_contour(p2[1], poly));
    EXPECT_TRUE(cg::point_in_contour(p2[2], poly));
    EXPECT_TRUE(cg::point_in_contour(p2[3], poly));
    EXPECT_TRUE(cg::point_in_contour(p2[4], poly));
    EXPECT_FALSE(cg::point_in_contour(p2[5], poly));
    EXPECT_FALSE(cg::point_in_contour(p2[6], poly));
    EXPECT_TRUE(cg::point_in_contour(p2[7], poly));
    EXPECT_FALSE(cg::point_in_contour(p2[8], poly));
    EXPECT_FALSE(cg::point_in_contour(p2[9], poly));

}


TEST(point_in_contour, test3)
{
    std::vector<cg::point_2> v;
    v.push_back(cg::point_2(2, 2));
    v.push_back(cg::point_2(5, 2));
    v.push_back(cg::point_2(2, 5));

    cg::contour_2 poly(v);
    cg::point_2 p[] = {cg::point_2(2, 2), cg::point_2(3, 3), cg::point_2(-4, 0), cg::point_2(2, 4)};

    EXPECT_TRUE(cg::point_in_contour(p[0], poly));
    EXPECT_TRUE(cg::point_in_contour(p[1], poly));
    EXPECT_FALSE(cg::point_in_contour(p[2], poly));
    EXPECT_TRUE(cg::point_in_contour(p[3], poly));
}

TEST(point_in_contour, test4)
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

    EXPECT_TRUE(cg::point_in_contour(p[0], poly));
    EXPECT_TRUE(cg::point_in_contour(p[1], poly));
    EXPECT_FALSE(cg::point_in_contour(p[2], poly));
    EXPECT_TRUE(cg::point_in_contour(p[3], poly));
    EXPECT_TRUE(cg::point_in_contour(p[4], poly));
    EXPECT_FALSE(cg::point_in_contour(p[5], poly));
    EXPECT_FALSE(cg::point_in_contour(p[6], poly));
}
