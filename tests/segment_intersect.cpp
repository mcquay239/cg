#include <gtest/gtest.h>

#include "cg/operations/segment_intersect.h"
#include <cg/primitives/segment.h>
#include "random_utils.h"

TEST(segment_intersect, test1)
{
    cg::segment_2 seg1(cg::point_2(0, 1), cg::point_2(1, 0));
    cg::segment_2 seg2(cg::point_2(1, 1), cg::point_2(0, 0));

    EXPECT_TRUE(cg::segments_intersected(seg1, seg2));
}

TEST(segment_intersect, test2)
{
    cg::segment_2 seg1(cg::point_2(0, 0), cg::point_2(0, 2));
    cg::segment_2 seg2(cg::point_2(0, 1), cg::point_2(0, 3));

    EXPECT_TRUE(cg::segments_intersected(seg1, seg2));
}

TEST(segment_intersect, test3)
{
    cg::segment_2 seg1(cg::point_2(0, 0), cg::point_2(0, 1));
    cg::segment_2 seg2(cg::point_2(0, 2), cg::point_2(0, 3));

    EXPECT_FALSE(cg::segments_intersected(seg1, seg2));
}

TEST(segment_intersect, test4)
{
    cg::segment_2 seg1(cg::point_2(1, 2), cg::point_2(3, 6));
    cg::segment_2 seg2(cg::point_2(2, 4), cg::point_2(4, 8));

    EXPECT_TRUE(cg::segments_intersected(seg1, seg2));
}

TEST(segment_intersect, test5)
{
    cg::segment_2 seg1(cg::point_2(1, 2), cg::point_2(3, 6));
    cg::segment_2 seg2(cg::point_2(2, 4), cg::point_2(-100, 21));

    EXPECT_TRUE(cg::segments_intersected(seg1, seg2));
}

TEST(segment_intersect, test6)
{
    cg::segment_2 seg1(cg::point_2(1, 2), cg::point_2(3, 6));
    cg::segment_2 seg2(cg::point_2(1, -1), cg::point_2(2, -2));

    EXPECT_FALSE(cg::segments_intersected(seg1, seg2));
}

TEST(segment_intersect, test7)
{
    cg::segment_2 seg1(cg::point_2(1, 2), cg::point_2(12, 54));
    cg::segment_2 seg2(cg::point_2(1, 2), cg::point_2(-12, -54));

    EXPECT_TRUE(cg::segments_intersected(seg1, seg2));
}

TEST(segment_intersect, test8)
{
    cg::segment_2 seg1(cg::point_2(1, 2), cg::point_2(12, 54));
    cg::segment_2 seg2(cg::point_2(20, 102), cg::point_2(30, 154));

    EXPECT_FALSE(cg::segments_intersected(seg1, seg2));
}

TEST(segment_intersect, test9)
{
    cg::segment_2 seg1(cg::point_2(1, 1), cg::point_2(1, 1));
    cg::segment_2 seg2(cg::point_2(0, 0), cg::point_2(12, 12));

    EXPECT_TRUE(cg::segments_intersected(seg1, seg2));
}

TEST(segment_intersect, test10)
{
    cg::segment_2 seg1(cg::point_2(1, 1), cg::point_2(200, 4));
    cg::segment_2 seg2(cg::point_2(8, 8), cg::point_2(8, 8));

    EXPECT_FALSE(cg::segments_intersected(seg1, seg2));
}

TEST(segment_intersect, test11)
{
    cg::segment_2 seg1(cg::point_2(1, 1), cg::point_2(1, 1));
    cg::segment_2 seg2(cg::point_2(1, 1), cg::point_2(1, 1));

    EXPECT_TRUE(cg::segments_intersected(seg1, seg2));
}

TEST(segment_intersect, test12)
{
    cg::segment_2 seg1(cg::point_2(1, 1), cg::point_2(1, 1));
    cg::segment_2 seg2(cg::point_2(1, 2), cg::point_2(1, 2));

    EXPECT_FALSE(cg::segments_intersected(seg1, seg2));
}

int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}





