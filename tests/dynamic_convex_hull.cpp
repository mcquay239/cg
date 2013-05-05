#include <gtest/gtest.h>
#include <boost/assign/list_of.hpp>
#include <cg/convex_hull/naive_dynamic.h>
#include <cg/primitives/point.h>
#include <iostream>


#include "random_utils.h"

template <class FwdIter>
bool is_convex_hull(FwdIter ab, FwdIter ae, FwdIter hb, FwdIter he)
{
   for (FwdIter b = ab; b != ae; b++)
   {
      for (FwdIter t = boost::prior(he), s = hb; s != he; t = s++)
      {
         switch (orientation(*t, *s, *b))
         {
         case cg::CG_RIGHT:
            return false;

         case cg::CG_COLLINEAR:
            if(!collinear_are_ordered_along_line(*t, *b, *s))
            {
               return false;
            }

         case cg::CG_LEFT:
            continue;
         }
      }
   }

   return true;
}

TEST(dynamic_convex_hull, without_deleting1)
{
   using cg::point_2;

   std::vector<point_2> pts = boost::assign::list_of(point_2(0, 0))
                              (point_2(1, 0))
                              (point_2(0, 1))
                              (point_2(2, 0))
                              (point_2(0, 2))
                              (point_2(3, 0));
   cg::naive_dynamic_hull dh;

for (point_2 p : pts)
   {
      dh.add_point(p);
   }

   EXPECT_TRUE(is_convex_hull(pts.begin(), pts.end(), dh.get_hull().first, dh.get_hull().second));
}

TEST(dynamic_convex_hull, without_deleting2)
{
   using cg::point_2;

   std::vector<point_2> pts = boost::assign::list_of(point_2(0, 0))
                              (point_2(1, 1))
                              (point_2(2, 2))
                              (point_2(3, 3))
                              (point_2(4, 4))
                              (point_2(5, 5));

   cg::naive_dynamic_hull dh;

	for (point_2 p : pts)
   {
      dh.add_point(p);
   }

   EXPECT_TRUE(is_convex_hull(pts.begin(), pts.end(), dh.get_hull().first, dh.get_hull().second));

}

TEST(dynamic_convex_hull, unifrom_without_deleting)
{
   using cg::point_2;

   std::vector<point_2> pts = uniform_points(100000);
   cg::naive_dynamic_hull dh;

	for (point_2 p : pts)
   {
      dh.add_point(p);
   }

   EXPECT_TRUE(is_convex_hull(pts.begin(), pts.end(), dh.get_hull().first, dh.get_hull().second));

}

TEST(dynamic_convex_hull, with_deleting)
{
   using cg::point_2;

   std::vector<point_2> pts = boost::assign::list_of(point_2(0, 0))
                              (point_2(3, 0))
                              (point_2(4, 2))
                              (point_2(2, 2))
                              (point_2(2, 4))
                              (point_2(-1, 2))
                              (point_2(1, 1))
                              (point_2(0, 1));

   std::vector<point_2> not_removed = boost::assign::list_of(point_2(0, 0))
                                      (point_2(3, 0))
                                      (point_2(2, 2))
                                      (point_2(1, 1))
                                      (point_2(0, 1));
   cg::naive_dynamic_hull dh;

	for (point_2 p : pts)
   {
      dh.add_point(p);
   }

   dh.remove_point(point_2(4, 2));
   dh.remove_point(point_2(-1, 2));
   dh.remove_point(point_2(2, 4));
   EXPECT_TRUE(is_convex_hull(not_removed.begin(), not_removed.end(), dh.get_hull().first, dh.get_hull().second));


}

TEST(dynamic_convex_hull, unifrom_with_deleting)
{
   using cg::point_2;

   std::vector<point_2> pts = uniform_points(10000), after_deleting;
   cg::naive_dynamic_hull dh;
   std::set<point_2> added;

	for (point_2 p : pts)
   {
      if (rand() % 2 || added.empty())
      {
         dh.add_point(p);
         added.insert(p);
         after_deleting.push_back(p);
      }
      else
      {
         dh.remove_point(*added.begin());
         after_deleting.erase(std::find(after_deleting.begin(), after_deleting.end(), *added.begin()));
         added.erase(added.begin());
      }
   }

   EXPECT_TRUE(is_convex_hull(after_deleting.begin(), after_deleting.end(), dh.get_hull().first, dh.get_hull().second));

}


