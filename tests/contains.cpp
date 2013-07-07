#include <gtest/gtest.h>

#include "random_utils.h"

#include <boost/assign/list_of.hpp>

#include <cg/operations/contains/segment_point.h>
#include <cg/operations/contains/triangle_point.h>
#include <cg/operations/contains/contour_point.h>
#include <cg/convex_hull/graham.h>

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
   std::vector<cg::point_2> v = boost::assign::list_of
                                 (cg::point_2(0, 0))
                                 (cg::point_2(10, 0))
                                 (cg::point_2(10, 10));

   cg::point_2 p2(3, 3);
   cg::contour_2 tr(v);
   EXPECT_EQ(contains(tr, p2), true);
}

TEST(contains, non_convex_contour_point_test2)
{
   std::vector<cg::point_2> v = boost::assign::list_of
                                 (cg::point_2(2, 3))
                                 (cg::point_2(2, 2))
                                 (cg::point_2(3, 1))
                                 (cg::point_2(5, 1))
                                 (cg::point_2(8, 4))
                                 (cg::point_2(6, 6))
                                 (cg::point_2(4, 6))
                                 (cg::point_2(3, 5));

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
   std::vector<cg::point_2> v = boost::assign::list_of
                                 (cg::point_2(1, 0))
                                 (cg::point_2(2, -2))
                                 (cg::point_2(3, 0))
                                 (cg::point_2(4, -2))
                                 (cg::point_2(5, 1))
                                 (cg::point_2(6, -1))
                                 (cg::point_2(7, 0))
                                 (cg::point_2(7, 1))
                                 (cg::point_2(5, 2))
                                 (cg::point_2(4, 0))
                                 (cg::point_2(2, 2));

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
   std::vector<cg::point_2> v = boost::assign::list_of
                                 (cg::point_2(6, -1))
                                 (cg::point_2(7, 0))
                                 (cg::point_2(7, 1))
                                 (cg::point_2(5, 2))
                                 (cg::point_2(4, 0))
                                 (cg::point_2(2, 2))
                                 (cg::point_2(1, 0))
                                 (cg::point_2(2, -2))
                                 (cg::point_2(3, 0))
                                 (cg::point_2(4, -2))
                                 (cg::point_2(5, 1));

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
   std::vector<cg::point_2> v = boost::assign::list_of
                                 (cg::point_2(1, 0))
                                 (cg::point_2(2, -2))
                                 (cg::point_2(3, 0))
                                 (cg::point_2(4, -2))
                                 (cg::point_2(5, 1))
                                 (cg::point_2(6, -1))
                                 (cg::point_2(7, 0))

                                 (cg::point_2(9, 0))
                                 (cg::point_2(10, -2))
                                 (cg::point_2(11, 0))
                                 (cg::point_2(13, 0))
                                 (cg::point_2(14, 1))
                                 (cg::point_2(15, 0))
                                 (cg::point_2(15, 3))

                                 (cg::point_2(7, 1))
                                 (cg::point_2(5, 2))
                                 (cg::point_2(4, 0))
                                 (cg::point_2(2, 2));

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

namespace details
{
   bool point_in_ccw_contour(cg::contour_2 const & c, cg::point_2 const & q)
   {
      size_t cnt_vertices = c.size();
      if (cnt_vertices == 0)
         return false;

      if (std::find(c.begin(), c.end(), q) != c.end())
         return true;


      if (cnt_vertices == 1)
      {
         return c[0] == q;
      }

      if (cnt_vertices == 2)
      {
         return cg::contains(cg::segment_2(c[0], c[1]), q);
      }

      cg::contour_2::circulator_t t2 = c.circulator();
      cg::contour_2::circulator_t t1 = t2++;

      for (size_t i = 0; i < cnt_vertices; ++i)
      {
         if (cg::orientation(*t1, *t2, q) == cg::CG_RIGHT)
         {
            return false;
         }
         ++t1;
         ++t2;
      }

      return true;
   }
}

TEST(contains, convex_ccw_point1)
{
   using cg::point_2;
   using cg::contour_2;

   std::vector<point_2> pts = boost::assign::list_of(point_2(1, 1));

   contour_2 cont(pts);
   for (size_t i = 0; i < pts.size(); i++)
   {
      EXPECT_TRUE(cg::convex_contains(cont, pts[i]));
   }

   EXPECT_TRUE(cg::convex_contains(cont, point_2(1, 1)));

   EXPECT_FALSE(cg::convex_contains(cont, point_2(0, 0)));
   EXPECT_FALSE(cg::convex_contains(cont, point_2(2, 2)));
   EXPECT_FALSE(cg::convex_contains(cont, point_2(0, 1)));
   EXPECT_FALSE(cg::convex_contains(cont, point_2(1, 0)));
}

TEST(contains, convex_ccw_point2)
{
   using cg::point_2;
   using cg::contour_2;

   std::vector<point_2> pts = boost::assign::list_of(point_2(1, 1))
                                                    (point_2(3, 3));

   contour_2 cont(pts);
   for (size_t i = 0; i < pts.size(); i++)
   {
      EXPECT_TRUE(cg::convex_contains(cont, pts[i]));
   }

   EXPECT_TRUE(cg::convex_contains(cont, point_2(2, 2)));

   EXPECT_FALSE(cg::convex_contains(cont, point_2(0, 0)));
   EXPECT_FALSE(cg::convex_contains(cont, point_2(4, 4)));
   EXPECT_FALSE(cg::convex_contains(cont, point_2(0, 1)));
   EXPECT_FALSE(cg::convex_contains(cont, point_2(1, 0)));
}

TEST(contains, convex_ccw_point3)
{
   using cg::point_2;
   using cg::contour_2;

   std::vector<point_2> pts = boost::assign::list_of(point_2(0, 1))
                                                    (point_2(0, 0))
                                                    (point_2(1, -1));

   contour_2 cont(pts);
   for (size_t i = 0; i < pts.size(); i++)
   {
      EXPECT_TRUE(cg::convex_contains(cont, pts[i]));
   }

   for (size_t i = 0; i < pts.size(); i++)
   {
      EXPECT_TRUE(cg::convex_contains(cont, pts[i]));
   }
}

TEST(contains, convex_ccw_point4)
{
   using cg::point_2;
   using cg::contour_2;

   std::vector<point_2> pts = boost::assign::list_of(point_2(0, 0))
                                                    (point_2(2, 4))
                                                    (point_2(0, 4))
                                                    (point_2(-2, 4));

   contour_2 cont(pts);
   for (size_t i = 0; i < pts.size(); i++)
   {
      EXPECT_TRUE(cg::convex_contains(cont, pts[i]));
   }

   EXPECT_TRUE(cg::convex_contains(cont, point_2(1, 2)));
   EXPECT_TRUE(cg::convex_contains(cont, point_2(1, 3)));
   EXPECT_TRUE(cg::convex_contains(cont, point_2(0, 2)));
   EXPECT_TRUE(cg::convex_contains(cont, point_2(-1, 2)));
   EXPECT_TRUE(cg::convex_contains(cont, point_2(-1, 3)));


   EXPECT_FALSE(cg::convex_contains(cont, point_2(1, 1)));
   EXPECT_FALSE(cg::convex_contains(cont, point_2(-1, 1)));
   EXPECT_FALSE(cg::convex_contains(cont, point_2(1, 5)));
   EXPECT_FALSE(cg::convex_contains(cont, point_2(1, 6)));
   EXPECT_FALSE(cg::convex_contains(cont, point_2(-1, 5)));
   EXPECT_FALSE(cg::convex_contains(cont, point_2(-1, 6)));
}

TEST(contains, convex_ccw_point_uniform0)
{
   using cg::point_2;
   using cg::contour_2;

   for (size_t cnt_points = 3; cnt_points < 10; cnt_points++)
   {
      std::vector<point_2> pts = uniform_points(cnt_points);
      std::vector<point_2> pts2 = uniform_points(100000);

      auto it = cg::graham_hull(pts.begin(), pts.end());
      pts.resize(std::distance(pts.begin(), it));

      contour_2 cont(pts);

      for (size_t i = 0; i < cont.size(); i++)
      {
         EXPECT_TRUE(cg::convex_contains(cont, cont[i]));
      }

      for (size_t i = 0; i < pts2.size(); i++)
      {
         EXPECT_EQ(cg::convex_contains(cont, pts2[i]), details::point_in_ccw_contour(cont, pts2[i]));
      }
   }
}

TEST(contains, convex_ccw_point_uniform1)
{
   using cg::point_2;
   using cg::contour_2;

   for (size_t cnt_tests = 3; cnt_tests < 10; cnt_tests++)
   {
      std::vector<point_2> pts = uniform_points(100000);
      std::vector<point_2> pts2 = uniform_points(100000);

      auto it = cg::graham_hull(pts.begin(), pts.end());
      pts.resize(std::distance(pts.begin(), it));

      contour_2 cont(pts);

      for (size_t i = 0; i < cont.size(); i++)
      {
         EXPECT_TRUE(cg::convex_contains(cont, cont[i]));
      }

      for (size_t i = 0; i < pts2.size(); i++)
      {
         EXPECT_EQ(cg::convex_contains(cont, pts2[i]), details::point_in_ccw_contour(cont, pts2[i]));
      }
   }
}
