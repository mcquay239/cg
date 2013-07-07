#include <gtest/gtest.h>

#include <boost/assign/list_of.hpp>

#include <cg/primitives/contour.h>
#include <cg/operations/orientation.h>
#include <cg/convex_hull/graham.h>
#include <misc/random_utils.h>

#include "random_utils.h"

using namespace util;

TEST(orientation, uniform_line)
{
   uniform_random_real<double, std::mt19937> distr(-(1LL << 53), (1LL << 53));

   std::vector<cg::point_2> pts = uniform_points(1000);
   for (size_t l = 0, ln = 1; ln < pts.size(); l = ln++)
   {
      cg::point_2 a = pts[l];
      cg::point_2 b = pts[ln];

      for (size_t k = 0; k != 300; ++k)
      {
         double t = distr();
         cg::point_2 c = a + t * (b - a);
         EXPECT_EQ(cg::orientation(a, b, c), *cg::orientation_r()(a, b, c));
      }
   }
}


TEST(orientation, counterclockwise0)
{
   using cg::point_2;

   std::vector<point_2> a = boost::assign::list_of(point_2(0, 0))
                                                  (point_2(1, 0))
                                                  (point_2(1, 1))
                                                  (point_2(0, 1));

   EXPECT_TRUE(cg::counterclockwise(cg::contour_2(a)));
}


TEST(orientation, counterclockwise1)
{
   using cg::point_2;

   std::vector<point_2> a = boost::assign::list_of(point_2(0, 0))
                                                  (point_2(2, 0))
                                                  (point_2(1, 2));

   EXPECT_TRUE(cg::counterclockwise(cg::contour_2(a)));
}


TEST(orientation, counterclockwise2)
{
   using cg::point_2;

   std::vector<point_2> a = boost::assign::list_of(point_2(1, 0))
                                                  (point_2(3, 0))
                                                  (point_2(0, 2));

   EXPECT_TRUE(cg::counterclockwise(cg::contour_2(a)));
}


TEST(orientation, counterclockwise3)
{
   using cg::point_2;

   std::vector<point_2> a = boost::assign::list_of(point_2(0, 0))
                                                  (point_2(0, 1))
                                                  (point_2(1, 1))
                                                  (point_2(1, 1));

   EXPECT_FALSE(cg::counterclockwise(cg::contour_2(a)));
}


#include <cg/io/point.h>
using std::cerr;
using std::endl;

TEST(orientation, uniform0)
{
   using cg::point_2;
   using cg::contour_2;


   for (size_t cnt_points = 3; cnt_points < 1000; cnt_points++)
   {
      std::vector<point_2> pts = uniform_points(cnt_points);

      auto it = cg::graham_hull(pts.begin(), pts.end());
      pts.resize(std::distance(pts.begin(), it));

      EXPECT_TRUE(cg::counterclockwise(contour_2(pts)));

      std::reverse(pts.begin(), pts.end());
      EXPECT_FALSE(cg::counterclockwise(contour_2(pts)));
   }
}

TEST(orientation, uniform1)
{
   using cg::point_2;
   using cg::contour_2;


   for (size_t cnt_tests = 1; cnt_tests < 20; cnt_tests++)
   {
      std::vector<point_2> pts = uniform_points(10000);

      auto it = cg::graham_hull(pts.begin(), pts.end());
      pts.resize(std::distance(pts.begin(), it));

      EXPECT_TRUE(cg::counterclockwise(contour_2(pts)));

      std::reverse(pts.begin(), pts.end());
      EXPECT_FALSE(cg::counterclockwise(contour_2(pts)));
   }
}

