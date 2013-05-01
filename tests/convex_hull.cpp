#include <gtest/gtest.h>

#include <boost/assign/list_of.hpp>

#include <cg/convex_hull/graham.h>
#include <cg/convex_hull/andrew.h>
#include <cg/convex_hull/jarvis.h>
#include <cg/operations/contains/segment_point.h>

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

TEST(jarvis_hull, simple)
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

TEST(jarvis_hull, uniform)
{
   using cg::point_2;

   for (int cnt = 2; cnt <= 100; ++cnt)
   {
      for (int i = 0; i < 100; ++i)
      {
         std::vector<point_2> pts = uniform_points(cnt);
         EXPECT_TRUE(is_convex_hull(pts.begin(), cg::jarvis_hull(pts.begin(), pts.end()), pts.end()));
      }
   }
}

TEST(jarvis_hull, same_line)
{
   using cg::point_2;

   std::vector<point_2> pts;
   int sz = 10;
   for (int i = 0; i < sz; i++) {
      pts.push_back(point_2(i, 0));
      pts.push_back(point_2(sz, i));
      pts.push_back(point_2(0, i + 1));
      pts.push_back(point_2(i + 1, sz));
   }

   for (int it = 0; it < 10; ++it)
   {
      EXPECT_TRUE(is_convex_hull(pts.begin(), cg::jarvis_hull(pts.begin(), pts.end()), pts.end()));
      std::random_shuffle(pts.begin(), pts.end());
   }
}
