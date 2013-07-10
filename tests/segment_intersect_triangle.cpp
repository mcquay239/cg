#include <gtest/gtest.h>

#include "cg/operations/segment_intersect_triangle.h"
#include <cg/primitives/segment.h>
#include <cg/primitives/point.h>
#include "random_utils.h"

TEST(segment_intersect_triangle, test1)
{
    cg::triangle_2 t(cg::point_2(0, 0), cg::point_2(1, 0), cg::point_2(0, 1));
    cg::segment_2 seg(cg::point_2(0, 0), cg::point_2(0, 100));
    EXPECT_TRUE(cg::segment_intersect_triangle(seg, t));
}

TEST(segment_intersect_triangle, test2)
{
    cg::triangle_2 t(cg::point_2(0, 0), cg::point_2(1, 0), cg::point_2(0, 1));
    cg::segment_2 seg(cg::point_2(0, 0), cg::point_2(20, 100));
    EXPECT_TRUE(cg::segment_intersect_triangle(seg, t));
}

TEST(segment_intersect_triangle, test3)
{
    cg::triangle_2 t(cg::point_2(1, 2), cg::point_2(1, 6), cg::point_2(-2, 3));
    cg::segment_2 seg(cg::point_2(-1, 4), cg::point_2(2, 7));
    EXPECT_TRUE(cg::segment_intersect_triangle(seg, t));
}

TEST(segment_intersect_triangle, test4)
{
    cg::triangle_2 t(cg::point_2(1, 2), cg::point_2(1, 6), cg::point_2(-2, 3));
    cg::segment_2 seg(cg::point_2(2, 7), cg::point_2(3, 8));
    EXPECT_FALSE(cg::segment_intersect_triangle(seg, t));
}

TEST(segment_intersect_triangle, test5)
{
    cg::triangle_2 t(cg::point_2(1, 2), cg::point_2(1, 6), cg::point_2(-2, 3));
    cg::segment_2 seg(cg::point_2(0, 3), cg::point_2(1, 3));
    EXPECT_TRUE(cg::segment_intersect_triangle(seg, t));
}

TEST(segment_intersect_triangle, test6)
{
    cg::triangle_2 t(cg::point_2(1, 2), cg::point_2(1, 6), cg::point_2(-2, 3));
    cg::segment_2 seg(cg::point_2(-200, 3), cg::point_2(200, 3));
    EXPECT_TRUE(cg::segment_intersect_triangle(seg, t));
}

TEST(segment_intersect_triangle, test7)
{
    cg::triangle_2 t(cg::point_2(1, 2), cg::point_2(1, 6), cg::point_2(-2, 3));
    cg::segment_2 seg(cg::point_2(-200, 5), cg::point_2(200, 3));
    EXPECT_TRUE(cg::segment_intersect_triangle(seg, t));
}

TEST(segment_intersect_triangle, test8)
{
    cg::triangle_2 t(cg::point_2(1, 2), cg::point_2(1, 6), cg::point_2(-2, 3));
    cg::segment_2 seg(cg::point_2(-1, 3), cg::point_2(-1, 3));
    EXPECT_TRUE(cg::segment_intersect_triangle(seg, t));
}

TEST(segment_intersect_triangle, test9)
{
    cg::triangle_2 t(cg::point_2(1, 2), cg::point_2(1, 6), cg::point_2(-2, 3));
    cg::segment_2 seg(cg::point_2(-1, 6), cg::point_2(1, 7));
    EXPECT_FALSE(cg::segment_intersect_triangle(seg, t));
}

TEST(segment_intersect_triangle, test10)
{
    cg::triangle_2 t(cg::point_2(1, 2), cg::point_2(1, 6), cg::point_2(-2, 3));
    cg::segment_2 seg(cg::point_2(-84, 12), cg::point_2(122, 21333));
    EXPECT_FALSE(cg::segment_intersect_triangle(seg, t));
}

