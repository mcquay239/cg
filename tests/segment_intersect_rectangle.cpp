#include <gtest/gtest.h>

#include "cg/operations/segment_intersect_rectangle.h"
#include <cg/primitives/segment.h>
#include <cg/primitives/point.h>
#include "random_utils.h"

TEST(segment_intersect_rectangle, test1)
{
    cg::rectangle_2 r(cg::range(1, 2), cg::range(2, 3));
    cg::segment_2 seg(cg::point_2(0, 0), cg::point_2(2, 3));
    EXPECT_TRUE(cg::segment_intersect_rectangle(seg, r));
}

TEST(segment_intersect_rectangle, test2)
{
    cg::rectangle_2 r(cg::range(1, 2), cg::range(2, 100));
    cg::segment_2 seg(cg::point_2(1, 2), cg::point_2(2, 2));
    EXPECT_TRUE(cg::segment_intersect_rectangle(seg, r));
}

TEST(segment_intersect_rectangle, test3)
{
    cg::rectangle_2 r(cg::range(1, 1), cg::range(2, 3));
    cg::segment_2 seg(cg::point_2(1, 2), cg::point_2(1, 3));
    EXPECT_TRUE(cg::segment_intersect_rectangle(seg, r));
}


TEST(segment_intersect_rectangle, test4)
{
    cg::rectangle_2 r(cg::range(-10, 2), cg::range(-20, 30));
    cg::segment_2 seg(cg::point_2(0, 0), cg::point_2(0, 100));
    EXPECT_TRUE(cg::segment_intersect_rectangle(seg, r));
}


TEST(segment_intersect_rectangle, test5)
{
    cg::rectangle_2 r(cg::range(1, 2), cg::range(2, 3));
    cg::segment_2 seg(cg::point_2(0, 0), cg::point_2(0, 100));
    EXPECT_FALSE(cg::segment_intersect_rectangle(seg, r));
}


TEST(segment_intersect_rectangle, test6)
{
    cg::rectangle_2 r(cg::range(1, 3), cg::range(2, 4));
    cg::segment_2 seg(cg::point_2(2, 3), cg::point_2(2, 3));
    EXPECT_TRUE(cg::segment_intersect_rectangle(seg, r));
}

TEST(segment_intersect_rectangle, test7)
{
    cg::rectangle_2 r(cg::range(-100, 100), cg::range(-1, 1));
    cg::segment_2 seg(cg::point_2(1, -3), cg::point_2(1, 3));
    EXPECT_TRUE(cg::segment_intersect_rectangle(seg, r));
}

TEST(segment_intersect_rectangle, test8)
{
    cg::rectangle_2 r(cg::range(-100, 100), cg::range(-1, 1));
    cg::segment_2 seg(cg::point_2(1, -3), cg::point_2(2, -8));
    EXPECT_FALSE(cg::segment_intersect_rectangle(seg, r));
}

TEST(segment_intersect_rectangle, test9)
{
    cg::rectangle_2 r(cg::range(-100, 100), cg::range(-1, 1));
    cg::segment_2 seg(cg::point_2(-10, 0), cg::point_2(100, 0));
    EXPECT_TRUE(cg::segment_intersect_rectangle(seg, r));
}

TEST(segment_intersect_rectangle, test10)
{
    cg::rectangle_2 r(cg::range(-100, 100), cg::range(-100, 100));
    cg::segment_2 seg(cg::point_2(-100, 10000), cg::point_2(-200, -100000));
    EXPECT_FALSE(cg::segment_intersect_rectangle(seg, r));
}

