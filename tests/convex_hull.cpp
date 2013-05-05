#include <gtest/gtest.h>

#include <boost/assign/list_of.hpp>

#include <cg/convex_hull/graham.h>
#include <cg/convex_hull/andrew.h>
#include <cg/operations/contains/segment_point.h>
#include <cg/convex_hull/quick_hull.h>

#include "random_utils.h"

template <class FwdIter>
bool is_convex_hull(FwdIter p, FwdIter c, FwdIter q)
{
   for (FwdIter t = boost::prior(c), s = p; s != c; t = s++)
   {
      for (FwdIter b = p; b != q; ++b)
      {
         switch (orientation(*t, *s, *b))
         {
         case cg::CG_RIGHT: return false;
         case cg::CG_COLLINEAR: if(!collinear_are_ordered_along_line(*t, *b, *s)) return false;
         case cg::CG_LEFT: continue;
         }
      }
   }

   return true;
}

TEST(graham_hull, simple)
{
   using cg::point_2;

   std::vector<point_2> pts = boost::assign::list_of(point_2(0, 0))
                                                    (point_2(1, 0))
                                                    (point_2(0, 1))
                                                    (point_2(2, 0))
                                                    (point_2(0, 2))
                                                    (point_2(3, 0));

   EXPECT_TRUE(is_convex_hull(pts.begin(), cg::graham_hull(pts.begin(), pts.end()), pts.end()));
}

TEST(graham_hull, simple2)
{
   using cg::point_2;

   std::vector<point_2> pts = boost::assign::list_of(point_2(0, 0))
                                                    (point_2(1, 1))
                                                    (point_2(2, 2))
                                                    (point_2(3, 3))
                                                    (point_2(4, 4))
                                                    (point_2(5, 5));

   EXPECT_TRUE(is_convex_hull(pts.begin(), cg::graham_hull(pts.begin(), pts.end()), pts.end()));
}

TEST(graham_hull, uniform)
{
   using cg::point_2;

   std::vector<point_2> pts = uniform_points(10000000);
   EXPECT_TRUE(is_convex_hull(pts.begin(), cg::graham_hull(pts.begin(), pts.end()), pts.end()));
}

TEST(andrew_hull, simple)
{
   using cg::point_2;

   std::vector<point_2> pts = boost::assign::list_of(point_2(0, 0))
                                                    (point_2(1, 0))
                                                    (point_2(0, 1))
                                                    (point_2(2, 0))
                                                    (point_2(0, 2))
                                                    (point_2(3, 0));

   EXPECT_TRUE(is_convex_hull(pts.begin(), cg::andrew_hull(pts.begin(), pts.end()), pts.end()));
}

TEST(andrew_hull, simple2)
{
   using cg::point_2;

   std::vector<point_2> pts = boost::assign::list_of(point_2(0, 0))
                                                    (point_2(3, 0))
                                                    (point_2(2, 3))
                                                    (point_2(1, 1));

   EXPECT_TRUE(is_convex_hull(pts.begin(), cg::andrew_hull(pts.begin(), pts.end()), pts.end()));  
}

TEST(andrew_hull, simple3)
{
   using cg::point_2;

   std::vector<point_2> pts = boost::assign::list_of(point_2(0, 0))
                                                    (point_2(1, 1))
                                                    (point_2(2, 2))
                                                    (point_2(3, 3))
                                                    (point_2(4, 4))
                                                    (point_2(5, 5));

   EXPECT_TRUE(is_convex_hull(pts.begin(), cg::andrew_hull(pts.begin(), pts.end()), pts.end()));
}

TEST(andrew_hull, simple4)
{
   using cg::point_2;

   std::vector<point_2> pts = boost::assign::list_of(point_2(0, 0));

   EXPECT_TRUE(is_convex_hull(pts.begin(), cg::andrew_hull(pts.begin(), pts.end()), pts.end()));
}

TEST(andrew_hull, simple5)
{
   using cg::point_2;

   std::vector<point_2> pts = boost::assign::list_of(point_2(0, 0))
                                                    (point_2(1, 1));

   EXPECT_TRUE(is_convex_hull(pts.begin(), cg::andrew_hull(pts.begin(), pts.end()), pts.end()));
}

TEST(andrew_hull, simple6)
{
   using cg::point_2;

   std::vector<point_2> pts = boost::assign::list_of(point_2(0, 0))
                                                    (point_2(1, 1))
                                                    (point_2(2, 2))
                                                    (point_2(2, 2))
                                                    (point_2(3, 3))
                                                    (point_2(4, 4))
                                                    (point_2(4, 4))
                                                    (point_2(5, 5));

   EXPECT_TRUE(is_convex_hull(pts.begin(), cg::andrew_hull(pts.begin(), pts.end()), pts.end()));
}

TEST(andrew_hull, uniform0)
{
   using cg::point_2;

   std::vector<point_2> pts = uniform_points(500000);
   EXPECT_TRUE(is_convex_hull(pts.begin(), cg::andrew_hull(pts.begin(), pts.end()), pts.end()));
}

TEST(andrew_hull, uniform1)
{
   using cg::point_2;

   for (int cnt = 2; cnt <= 6; ++cnt)
   {
      for (int i = 0; i < 1000; ++i)
      {
         std::vector<point_2> pts = uniform_points(cnt);
         EXPECT_TRUE(is_convex_hull(pts.begin(), cg::andrew_hull(pts.begin(), pts.end()), pts.end()));
      }
   }
}

TEST(quick_hull, simple)
{
   using cg::point_2;

   std::vector<point_2> pts = boost::assign::list_of(point_2(0, 0))
                                                    (point_2(1, 0))
                                                    (point_2(0, 1))
                                                    (point_2(2, 0))
                                                    (point_2(0, 2))
                                                    (point_2(3, 0));

   EXPECT_TRUE(is_convex_hull(pts.begin(), cg::quick_hull(pts.begin(), pts.end()), pts.end()));
}

TEST(quick_hull, simple2)
{
   using cg::point_2;

   std::vector<point_2> pts = boost::assign::list_of(point_2(0, 0))
                                                    (point_2(5, 0))
                                                    (point_2(5, 5))
                                                    (point_2(4, 3));

   EXPECT_TRUE(is_convex_hull(pts.begin(), cg::quick_hull(pts.begin(), pts.end()), pts.end()));
}

TEST(quick_hull, simple3)
{
    using cg::point_2;

    std::vector<point_2> pts = boost::assign::list_of(point_2(95.0938, -19.7291))
                                                     (point_2(-24.0541, -28.2266))
                                                     (point_2(-51.1062, 19.8668))
                                                     (point_2(74.1097, -66.2974))
                                                     (point_2(94.036, 97.6347))
                                                     (point_2(-67.9643, -30.467))
                                                     (point_2(79.7704, -61.9609))
                                                     (point_2(-1.94966, 50.4662))
                                                     (point_2(45.4063, -35.1337))
                                                     (point_2(-23.3787, 91.2357))
                                                     (point_2(-88.4949, 81.116))
                                                     (point_2(6.02942, -54.5359));

    EXPECT_TRUE(is_convex_hull(pts.begin(), cg::quick_hull(pts.begin(), pts.end()), pts.end()));

}

TEST(quick_hull, line)
{
    using cg::point_2;

    std::vector<point_2> pts = boost::assign::list_of(point_2(0, 0))
                                                     (point_2(1, 1))
                                                     (point_2(3, 3))
                                                     (point_2(-1, -1))
                                                     (point_2(4, 4));

    EXPECT_TRUE(is_convex_hull(pts.begin(), cg::quick_hull(pts.begin(), pts.end()), pts.end()));
}

TEST(quick_hull, one_point)
{
    using cg::point_2;

    std::vector<point_2> pts = boost::assign::list_of(point_2(1, 1));

    EXPECT_TRUE(is_convex_hull(pts.begin(), cg::quick_hull(pts.begin(), pts.end()), pts.end()));
}

TEST(quick_hull, same_points)
{
    using cg::point_2;

    std::vector<point_2> pts = boost::assign::list_of(point_2(1, 1))
                                                     (point_2(1, 1))
                                                     (point_2(1, 1))
                                                     (point_2(1, 1))
                                                     (point_2(1, 1));

    EXPECT_TRUE(is_convex_hull(pts.begin(), cg::quick_hull(pts.begin(), pts.end()), pts.end()));
}

TEST(quick_hull, uniform)
{
   using cg::point_2;

   std::vector<point_2> pts = uniform_points(10000000);
   EXPECT_TRUE(is_convex_hull(pts.begin(), cg::quick_hull(pts.begin(), pts.end()), pts.end()));
}

