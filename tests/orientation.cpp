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


TEST(orientation_3d_vectors, simple)
{
    cg::point_3 i(1, 0, 0);
    cg::point_3 j(0, 1, 0);
    cg::point_3 k(0, 0, 1);
    EXPECT_EQ(cg::orientation_t::CG_RIGHT, cg::orientation_3d_vectors(i, j, k));
    EXPECT_EQ(cg::orientation_t::CG_LEFT, cg::orientation_3d_vectors(i, k, j));
    EXPECT_EQ(cg::orientation_t::CG_LEFT, cg::orientation_3d_vectors(k, j, i));
    EXPECT_EQ(cg::orientation_t::CG_LEFT, cg::orientation_3d_vectors(j, i, k));
    EXPECT_EQ(cg::orientation_t::CG_RIGHT, cg::orientation_3d_vectors(j, k, i));
    EXPECT_EQ(cg::orientation_t::CG_RIGHT, cg::orientation_3d_vectors(k, i, j));

    EXPECT_EQ(cg::orientation_t::CG_RIGHT, *cg::orientation_3d_vectors_r()(i, j, k));
    EXPECT_EQ(cg::orientation_t::CG_LEFT, *cg::orientation_3d_vectors_r()(i, k, j));
    EXPECT_EQ(cg::orientation_t::CG_LEFT, *cg::orientation_3d_vectors_r()(k, j, i));
    EXPECT_EQ(cg::orientation_t::CG_LEFT, *cg::orientation_3d_vectors_r()(j, i, k));
    EXPECT_EQ(cg::orientation_t::CG_RIGHT, *cg::orientation_3d_vectors_r()(j, k, i));
    EXPECT_EQ(cg::orientation_t::CG_RIGHT, *cg::orientation_3d_vectors_r()(k, i, j));
}

TEST(orientation_3d_vectors, uniform_line)
{
    uniform_random_real<double, std::mt19937> distr(-(1LL << 53), (1LL << 53));

    std::vector<cg::point_3> pts = uniform_points3d(1000);
    for (size_t l = 0; l < pts.size(); l++)
    {
        cg::point_3 a = pts[l];
        cg::point_3 b = a * distr();
        cg::point_3 c = a * distr();
        EXPECT_EQ(*cg::orientation_3d_vectors_r()(a, b, c), cg::orientation_3d_vectors(a, b, c));
    }
}