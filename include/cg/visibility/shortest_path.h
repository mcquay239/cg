#pragma once
#include <cg/visibility/visibility_naive.h>
#include <cg/common/values.h>
#include <map>

namespace cg
{
   template<class Scalar>
   void add_point(int * num, point_2t<Scalar> const & p, std::map<point_2t<Scalar>, int> & number_by_point,
                  std::map<int, point_2t<Scalar> > & point_by_number)
   {
      number_by_point[p] = *num;
      point_by_number[*num] = p;
      (*num)++;
   }

   std::vector<int> get_path(std::vector<std::vector<std::pair<int, double> > > const& a, int start, int end, int size)
   {
      std::vector<double> d(size, DOUBLE_INF);
      std::vector<int> par(size, ILLEGAL_VERTEX);
      std::vector<bool> used(size, false);
      d[start] = 0;
      for (int i = 0; i < size; i++)
      {
         double dmin = DOUBLE_INF;
         int index = ILLEGAL_VERTEX;
         for (int j = 0; j < size; j++)
         {
            if (!used[j] && dmin >= d[j])
            {
               dmin = d[j];
               index = j;
            }
         }
         used[index] = true;
         for (auto ver_pair : a[index])
         {
            if (!used[ver_pair.first])
            {
               if (d[ver_pair.first] > d[index] + ver_pair.second)
               {
                  d[ver_pair.first] = d[index] + ver_pair.second;
                  par[ver_pair.first] = index;
               }
            }
         }
      }
      int cur = end;
      std::vector<int> ans;
      while (cur != start)
      {
         ans.push_back(cur);
         cur = par[cur];
      }
      ans.push_back(start);
      std::reverse(ans.begin(), ans.end());
      return ans;
   }

   // TODO: After accepting pull request #30 get rid of it
   template <class Scalar>
   inline double distance(point_2t<Scalar> const & a, point_2t<Scalar> const & b)
   {
      auto vect(a - b);
      return sqrt(vect * vect);
   }

   template <class Scalar>
   std::vector<point_2t<Scalar> > get_shortest_path(point_2t<Scalar> const & start, point_2t<Scalar> const & end, std::vector<contour_2t<Scalar> > & polygons)
   {
      typedef contour_2t<Scalar> Contour;
      typedef segment_2t<Scalar> Segment;
      typedef point_2t<Scalar> Point;

      std::vector<Segment> vis_graph = get_visibility_graph(polygons);
      std::map<Point, int> number_by_point;
      std::map<int, Point> point_by_number;
      int cur_point = 0;
      add_point(&cur_point, start, number_by_point, point_by_number);
      for (Contour contour : polygons)
      {
         for (auto it = contour.begin(); it != contour.end(); it++)
         {
            point_2t<Scalar> p = *it;
            if (number_by_point.count(p) == 0) add_point(&cur_point, p, number_by_point, point_by_number);
         }
      }
      add_point(&cur_point, end, number_by_point, point_by_number);
      std::vector<Point> ans;
      int size = cur_point;
      std::vector<std::vector<std::pair<int, double> > > a(size);
      for (Segment segment : vis_graph)
      {
         double dist = distance(segment[0], segment[1]);
         int num1 = number_by_point[segment[0]], num2 = number_by_point[segment[1]];
         a[num1].push_back(std::make_pair(num2, dist));
         a[num2].push_back(std::make_pair(num1, dist));
      }
      auto ans_points = get_path(a, 0, size - 1, size);
      for (int i : ans_points)
         ans.push_back(point_by_number[i]);

      return ans;
   }
}
