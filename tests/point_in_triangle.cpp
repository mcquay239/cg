#include <gtest/gtest.h>

#include "cg/operations/point_in_triangle.h"
#include <cg/primitives/triangle.h>
#include "random_utils.h"

TEST(point_in_triangle, test1)
{
    cg::triangle_2 t(cg::point_2(0, 0), cg::point_2(1, 0), cg::point_2(0, 1));
    cg::point_2 p(0, 0);
    EXPECT_TRUE(cg::point_in_triangle(p, t));
}

TEST(point_in_triangle, test2)
{
    cg::triangle_2 t(cg::point_2(0, 0), cg::point_2(30, 0), cg::point_2(0, 30));
    cg::point_2 p(1, 1);
    EXPECT_TRUE(cg::point_in_triangle(p, t));
}

TEST(point_in_triangle, test3)
{
    cg::triangle_2 t(cg::point_2(0, 0), cg::point_2(30, 0), cg::point_2(15, 30));
    cg::point_2 p(16, 1);
    EXPECT_TRUE(cg::point_in_triangle(p, t));
}

TEST(point_in_triangle, test4)
{
    cg::triangle_2 t(cg::point_2(1, 2), cg::point_2(1, 5), cg::point_2(-2, 3));
    cg::point_2 p(1, 3);
    EXPECT_TRUE(cg::point_in_triangle(p, t));
}

TEST(point_in_triangle, test5)
{
    cg::triangle_2 t(cg::point_2(1, 2), cg::point_2(1, 5), cg::point_2(-2, 3));
    cg::point_2 p(-1, 2);
    EXPECT_FALSE(cg::point_in_triangle(p, t));
}

TEST(point_in_triangle, test6)
{
    cg::triangle_2 t(cg::point_2(0, 0), cg::point_2(100, 0), cg::point_2(50, 100));
    cg::point_2 p(50, 4.55);
    EXPECT_TRUE(cg::point_in_triangle(p, t));

}

TEST(point_in_triangle, test7)
{
    cg::triangle_2 t(cg::point_2(0, 0), cg::point_2(100, 0), cg::point_2(50, 100));
    cg::point_2 p(-50, 4.55);
    EXPECT_FALSE(cg::point_in_triangle(p, t));

}

TEST(point_in_triangle, test8)
{
    cg::triangle_2 t(cg::point_2(0, 0), cg::point_2(100, 0), cg::point_2(-50, 100));
    cg::point_2 p(100, 100);
    EXPECT_FALSE(cg::point_in_triangle(p, t));

}

TEST(point_in_triangle, test9)
{
    cg::triangle_2 t(cg::point_2(0, 0), cg::point_2(6, 0), cg::point_2(0, 8));
    cg::point_2 p(3, 4);
    EXPECT_TRUE(cg::point_in_triangle(p, t));
}

TEST(point_in_triangle, test10)
{
    cg::triangle_2 t(cg::point_2(100, 0), cg::point_2(101, 0), cg::point_2(-200, 300));
    cg::point_2 p(0, 100);
    EXPECT_TRUE(cg::point_in_triangle(p, t));
}

