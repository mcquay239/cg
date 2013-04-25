#include <gtest/gtest.h>

#include <boost/assign/list_of.hpp>

#include <cg/primitives/point.h>
#include <cg/primitives/contour.h>
#include <cg/operations/convex.h>
#include <cg/convex_hull/graham.h>

#include "random_utils.h"

TEST(convex, convex0)
{
   using cg::point_2;

   std::vector<point_2> a = boost::assign::list_of(point_2(0, 0))
                                                  (point_2(1, -1))
                                                  (point_2(2, 0))
                                                  (point_2(1, 3));

   EXPECT_TRUE(cg::convex(cg::contour_2(a)));
}

TEST(convex, convex1)
{
   using cg::point_2;

   std::vector<point_2> a = boost::assign::list_of(point_2(0, 0))
                                                  (point_2(1, 0))
                                                  (point_2(2, 0))
                                                  (point_2(1, 3));

   EXPECT_TRUE(cg::convex(cg::contour_2(a)));
}

TEST(convex, convex2)
{
   using cg::point_2;

   std::vector<point_2> a = boost::assign::list_of(point_2(0, 0))
                                                  (point_2(1, 1))
                                                  (point_2(2, 0))
                                                  (point_2(1, 3));

   EXPECT_FALSE(cg::convex(cg::contour_2(a)));
}

TEST(convex, convex3)
{
   using cg::point_2;

   std::vector<point_2> a = boost::assign::list_of(point_2(0, 0))
                                                  (point_2(1, 1));

   EXPECT_TRUE(cg::convex(cg::contour_2(a)));
}

TEST(convex, convex4)
{
   using cg::point_2;

   std::vector<point_2> a = boost::assign::list_of(point_2(1, 1));

   EXPECT_TRUE(cg::convex(cg::contour_2(a)));
}

TEST(convex, convex5)
{
   using cg::point_2;

   std::vector<point_2> a = boost::assign::list_of(point_2(0, 0))
                                                  (point_2(2, 0))
                                                  (point_2(1, 3));

   EXPECT_TRUE(cg::convex(cg::contour_2(a)));
}

TEST(convex, uniform0)
{
   using cg::point_2;
   using cg::contour_2;


   for (size_t cnt_points = 1; cnt_points < 1000; cnt_points++)
   {
      std::vector<point_2> pts = uniform_points(cnt_points);

      auto it = cg::graham_hull(pts.begin(), pts.end());
      pts.resize(std::distance(pts.begin(), it));

      contour_2 cont(pts);

      EXPECT_TRUE(cg::convex(cont));
   }
}

TEST(convex, uniform1)
{
   using cg::point_2;
   using cg::contour_2;


   for (size_t cnt_tests = 1; cnt_tests < 20; cnt_tests++)
   {
      std::vector<point_2> pts = uniform_points(10000);

      auto it = cg::graham_hull(pts.begin(), pts.end());
      pts.resize(std::distance(pts.begin(), it));

      contour_2 cont(pts);

      EXPECT_TRUE(cg::convex(cont));
   }
}
