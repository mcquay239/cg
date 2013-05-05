#include <gtest/gtest.h>

#include <cg/operations/contains/segment_point.h>
#include <cg/operations/contains/triangle_point.h>
#include <cg/operations/contains/contour_point.h>

TEST(contains, triangle_point)
{
   using cg::point_2;

   cg::triangle_2 t(point_2(0, 0), point_2(1, 1), point_2(2, 0));

   for (size_t l = 0; l != 3; ++l)
      EXPECT_TRUE(cg::contains(t, t[l]));

   EXPECT_TRUE(cg::contains(t, point_2(1, 0.5)));

   EXPECT_TRUE(cg::contains(t, point_2(1, 0)));
   EXPECT_TRUE(cg::contains(t, point_2(0.5, 0.5)));
   EXPECT_TRUE(cg::contains(t, point_2(1.5, 0.5)));

   EXPECT_FALSE(cg::contains(t, point_2(0, 1)));
   EXPECT_FALSE(cg::contains(t, point_2(2, 1)));
   EXPECT_FALSE(cg::contains(t, point_2(1, -1)));
}

TEST(contains, segment_point)
{
   using cg::point_2;

   cg::segment_2 s(point_2(0, 0), point_2(2, 2));
   for (size_t l = 0; l != 2; ++l)
      EXPECT_TRUE(cg::contains(s, s[l]));

   EXPECT_TRUE(cg::contains(s, point_2(1, 1)));

   EXPECT_FALSE(cg::contains(s, point_2(-1, -1)));
   EXPECT_FALSE(cg::contains(s, point_2(4, 4)));

   EXPECT_FALSE(cg::contains(s, point_2(1, 0)));
   EXPECT_FALSE(cg::contains(s, point_2(0, 1)));
}

TEST(contains, non_convex_contour_point_test1)
{
   std::vector<cg::point_2> v;
   v.push_back(cg::point_2(0, 0));
   v.push_back(cg::point_2(10, 0));
   v.push_back(cg::point_2(10, 10));
   cg::point_2 p2(3, 3);
   cg::contour_2 tr(v);
   EXPECT_EQ(contains(tr, p2), true);
}

TEST(contains, non_convex_contour_point_test2)
{
   std::vector<cg::point_2> v;
   v.push_back(cg::point_2(2, 3));
   v.push_back(cg::point_2(2, 2));
   v.push_back(cg::point_2(3, 1));
   v.push_back(cg::point_2(5, 1));
   v.push_back(cg::point_2(8, 4));
   v.push_back(cg::point_2(6, 6));
   v.push_back(cg::point_2(4, 6));
   v.push_back(cg::point_2(3, 5));
   cg::contour_2 tr(v);
   EXPECT_EQ(contains(tr, cg::point_2(2, 2)), true);
   EXPECT_EQ(contains(tr, cg::point_2(3, 1)), true);
   EXPECT_EQ(contains(tr, cg::point_2(3, 2)), true);
   EXPECT_EQ(contains(tr, cg::point_2(3, 5)), true);
   EXPECT_EQ(contains(tr, cg::point_2(3, 6)), false);
   EXPECT_EQ(contains(tr, cg::point_2(5, 1)), true);
   EXPECT_EQ(contains(tr, cg::point_2(5, 2)), true);
   EXPECT_EQ(contains(tr, cg::point_2(8, 4)), true);
   EXPECT_EQ(contains(tr, cg::point_2(8, 3)), false);
   EXPECT_EQ(contains(tr, cg::point_2(8, 5)), false);
   EXPECT_EQ(contains(tr, cg::point_2(6, 7)), false);
   EXPECT_EQ(contains(tr, cg::point_2(2.5, 3.5)), true);
}


TEST(contains, non_convex_contour_point_test3)
{
   std::vector<cg::point_2> v;
   v.push_back(cg::point_2(1, 0));
   v.push_back(cg::point_2(2, -2));
   v.push_back(cg::point_2(3, 0));
   v.push_back(cg::point_2(4, -2));
   v.push_back(cg::point_2(5, 1));
   v.push_back(cg::point_2(6, -1));
   v.push_back(cg::point_2(7, 0));
   v.push_back(cg::point_2(7, 1));
   v.push_back(cg::point_2(5, 2));
   v.push_back(cg::point_2(4, 0));
   v.push_back(cg::point_2(2, 2));
   cg::contour_2 tr(v);
   EXPECT_EQ(contains(tr, cg::point_2(1, 0)), true);
   EXPECT_EQ(contains(tr, cg::point_2(2, 0)), true);
   EXPECT_EQ(contains(tr, cg::point_2(3, 0)), true);
   EXPECT_EQ(contains(tr, cg::point_2(4, 0)), true);
   EXPECT_EQ(contains(tr, cg::point_2(5, 0)), false);
   EXPECT_EQ(contains(tr, cg::point_2(6, 0)), true);
   EXPECT_EQ(contains(tr, cg::point_2(7, 0)), true);
   EXPECT_EQ(contains(tr, cg::point_2(8, 0)), false);
   EXPECT_EQ(contains(tr, cg::point_2(0, 0)), false);
   EXPECT_EQ(contains(tr, cg::point_2(1, 1)), false);
   EXPECT_EQ(contains(tr, cg::point_2(2, 1)), true);
   EXPECT_EQ(contains(tr, cg::point_2(3.5, 0)), true);
   EXPECT_EQ(contains(tr, cg::point_2(4, 1)), false);
   EXPECT_EQ(contains(tr, cg::point_2(5, -1)), false);
   EXPECT_EQ(contains(tr, cg::point_2(4, -0.5)), true);
   EXPECT_EQ(contains(tr, cg::point_2(3.5, 0.001)), true);
}

TEST(contains, non_convex_contour_point_test4)
{
   std::vector<cg::point_2> v;
   v.push_back(cg::point_2(6, -1));
   v.push_back(cg::point_2(7, 0));
   v.push_back(cg::point_2(7, 1));
   v.push_back(cg::point_2(5, 2));
   v.push_back(cg::point_2(4, 0));
   v.push_back(cg::point_2(2, 2));
   v.push_back(cg::point_2(1, 0));
   v.push_back(cg::point_2(2, -2));
   v.push_back(cg::point_2(3, 0));
   v.push_back(cg::point_2(4, -2));
   v.push_back(cg::point_2(5, 1));   
   cg::contour_2 tr(v);
   EXPECT_EQ(contains(tr, cg::point_2(1, 0)), true);
   EXPECT_EQ(contains(tr, cg::point_2(2, 0)), true);
   EXPECT_EQ(contains(tr, cg::point_2(3, 0)), true);
   EXPECT_EQ(contains(tr, cg::point_2(4, 0)), true);
   EXPECT_EQ(contains(tr, cg::point_2(5, 0)), false);
   EXPECT_EQ(contains(tr, cg::point_2(6, 0)), true);
   EXPECT_EQ(contains(tr, cg::point_2(7, 0)), true);
   EXPECT_EQ(contains(tr, cg::point_2(8, 0)), false);
   EXPECT_EQ(contains(tr, cg::point_2(0, 0)), false);
   EXPECT_EQ(contains(tr, cg::point_2(1, 1)), false);
   EXPECT_EQ(contains(tr, cg::point_2(2, 1)), true);
   EXPECT_EQ(contains(tr, cg::point_2(3.5, 0)), true);
   EXPECT_EQ(contains(tr, cg::point_2(4, 1)), false);
   EXPECT_EQ(contains(tr, cg::point_2(5, -1)), false);
   EXPECT_EQ(contains(tr, cg::point_2(4, -0.5)), true);
   EXPECT_EQ(contains(tr, cg::point_2(3.5, 0.001)), true);
}

TEST(contains, non_convex_contour_point_test5)
{
   std::vector<cg::point_2> v;
   v.push_back(cg::point_2(1, 0));
   v.push_back(cg::point_2(2, -2));
   v.push_back(cg::point_2(3, 0));
   v.push_back(cg::point_2(4, -2));
   v.push_back(cg::point_2(5, 1));
   v.push_back(cg::point_2(6, -1));
   v.push_back(cg::point_2(7, 0));

   v.push_back(cg::point_2(9, 0));
   v.push_back(cg::point_2(10, -2));
   v.push_back(cg::point_2(11, 0));
   v.push_back(cg::point_2(13, 0));
   v.push_back(cg::point_2(14, 1));
   v.push_back(cg::point_2(15, 0));
   v.push_back(cg::point_2(15, 3));

   v.push_back(cg::point_2(7, 1));
   v.push_back(cg::point_2(5, 2));
   v.push_back(cg::point_2(4, 0));
   v.push_back(cg::point_2(2, 2));
   cg::contour_2 tr(v);
   EXPECT_EQ(contains(tr, cg::point_2(1, 0)), true);
   EXPECT_EQ(contains(tr, cg::point_2(2, 0)), true);
   EXPECT_EQ(contains(tr, cg::point_2(3, 0)), true);
   EXPECT_EQ(contains(tr, cg::point_2(4, 0)), true);
   EXPECT_EQ(contains(tr, cg::point_2(5, 0)), false);
   EXPECT_EQ(contains(tr, cg::point_2(6, 0)), true);
   EXPECT_EQ(contains(tr, cg::point_2(7, 0)), true);
   EXPECT_EQ(contains(tr, cg::point_2(8, 0)), true);
   EXPECT_EQ(contains(tr, cg::point_2(9, 0)), true);
   EXPECT_EQ(contains(tr, cg::point_2(10, 0)), true);
   EXPECT_EQ(contains(tr, cg::point_2(11, 0)), true);
   EXPECT_EQ(contains(tr, cg::point_2(12, 0)), true);
   EXPECT_EQ(contains(tr, cg::point_2(13, 0)), true);
   EXPECT_EQ(contains(tr, cg::point_2(14, 0)), false);
   EXPECT_EQ(contains(tr, cg::point_2(15, 0)), true);
   EXPECT_EQ(contains(tr, cg::point_2(16, 0)), false);

   EXPECT_EQ(contains(tr, cg::point_2(0, 0)), false);
   EXPECT_EQ(contains(tr, cg::point_2(1, 1)), false);
   EXPECT_EQ(contains(tr, cg::point_2(2, 1)), true);
   EXPECT_EQ(contains(tr, cg::point_2(3.5, 0)), true);
   EXPECT_EQ(contains(tr, cg::point_2(4, 1)), false);
   EXPECT_EQ(contains(tr, cg::point_2(5, -1)), false);
   EXPECT_EQ(contains(tr, cg::point_2(4, -0.5)), true);
   EXPECT_EQ(contains(tr, cg::point_2(3.5, 0.001)), true);
}
