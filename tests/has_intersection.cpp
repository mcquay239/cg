#include <gtest/gtest.h>

#include <cg/operations/has_intersection/segment_segment.h>
#include <cg/operations/has_intersection/triangle_segment.h>
#include <cg/operations/has_intersection/rectangle_segment.h>

TEST(has_intersection, segment_segment)
{
   using cg::point_2;
   using cg::segment_2;

   segment_2 s[] =
   {
      segment_2(point_2(0, 0), point_2(2, 2)),
      segment_2(point_2(1, 0), point_2(0, 1)),
      segment_2(point_2(0, 0), point_2(2, 0)),
      segment_2(point_2(0, 0), point_2(2, -1))
   };

   EXPECT_TRUE(cg::has_intersection(s[0], s[1]));
   EXPECT_TRUE(cg::has_intersection(s[1], s[2]));
   EXPECT_TRUE(cg::has_intersection(s[0], s[2]));

   EXPECT_FALSE(cg::has_intersection(s[1], s[3]));

   segment_2 a(point_2(1.0, 0.0), point_2(4.0, 0.0));
   EXPECT_TRUE(cg::has_intersection(a, segment_2(point_2(2.0, -2.0), point_2(2.0, 2.0))));
   EXPECT_TRUE(cg::has_intersection(a, segment_2(point_2(2.0, 0.0), point_2(2.0, 2.0))));
   EXPECT_FALSE(cg::has_intersection(a, segment_2(point_2(1.0, 2.0), point_2(2.0, 2.0))));

   point_2 a1(0.5, 0), a2(0.5, 0), a3(0, 0), a4(1, 0);
   segment_2 s1(a1, a2), s2(a3, a4);
   EXPECT_TRUE(cg::has_intersection(s1, s2));
}

TEST(has_intersection, triangle_segment)
{
   using cg::point_2;
   using cg::segment_2;
   using cg::triangle_2;

   triangle_2 t(point_2(0, 0), point_2(1, 1), point_2(2, 0));

   EXPECT_TRUE(cg::has_intersection(t, segment_2(point_2(0, 0), point_2(-1, -1))));

   EXPECT_TRUE(cg::has_intersection(t, segment_2(point_2(0, 1), point_2(2, 1))));
   EXPECT_FALSE(cg::has_intersection(t, segment_2(point_2(0, 2), point_2(2, 2))));

   triangle_2 t1(point_2(0.0, 0.0), point_2(2.0, 0.0), point_2(2.0, 2.0));
   EXPECT_TRUE(cg::has_intersection(t1, segment_2(point_2(0.0, 1.0), point_2(3.0, 1.0))));
   EXPECT_TRUE(cg::has_intersection(t1, segment_2(point_2(2.0, 1.0), point_2(3.0, 1.0))));
   EXPECT_TRUE(cg::has_intersection(t1, segment_2(point_2(2.0, 0.0), point_2(3.0, -1.0))));
   EXPECT_TRUE(cg::has_intersection(t1, segment_2(point_2(0.2, 0.1), point_2(1.5, 1.0))));
   EXPECT_FALSE(cg::has_intersection(t1, segment_2(point_2(-1.0, 1.0), point_2(2.0, 3.0))));
}

TEST(has_intersection, rectangle_segment)
{
   using cg::point_2;
   using cg::segment_2;
   using cg::rectangle_2;
   using cg::range;

   rectangle_2 r(range(0.0, 4.0), range(0.0, 4.0));
   EXPECT_FALSE(cg::has_intersection(r, segment_2(point_2(-2.0, -2.0), point_2(-1.0, -1.0))));
   EXPECT_TRUE(cg::has_intersection(r, segment_2(point_2(-1.0, -1.0), point_2(1.0, 1.0))));
   EXPECT_TRUE(cg::has_intersection(r, segment_2(point_2(-2.0, -2.0), point_2(0.0, 0.0))));

   range a(0, 2), b(0, 2);
   EXPECT_TRUE(cg::has_intersection(rectangle_2(a, b), segment_2(point_2(-2, 2), point_2(2, 2))));
   EXPECT_TRUE(cg::has_intersection(rectangle_2(a, b), segment_2(point_2(1, 1), point_2(1, 1.5))));
   EXPECT_FALSE(cg::has_intersection(rectangle_2(a, b), segment_2(point_2(0, 3), point_2(3, 3))));
   EXPECT_TRUE(cg::has_intersection(rectangle_2(a, b), segment_2(point_2(-1, -1), point_2(3, 3))));
   EXPECT_TRUE(cg::has_intersection(rectangle_2(a, b), segment_2(point_2(1, -1), point_2(1, 3))));
}
