#include <QColor>
#include <QApplication>

#include <boost/optional.hpp>

#include "cg/visualization/viewer_adapter.h"
#include "cg/visualization/draw_util.h"

#include <cg/primitives/segment.h>
#include <cg/primitives/point.h>
#include <cg/algorithms/diameter.h>
#include <cg/operations/distance.h>


using cg::point_2;
using cg::point_2f;
using cg::segment_2;
using cg::segment_2f;

struct diameter_viewer : cg::visualization::viewer_adapter
{
   diameter_viewer()
      : diameter(boost::none)
   {}

   void draw(cg::visualization::drawer_type & drawer) const
   {
      drawer.set_color(Qt::white);
      for (auto const& p : points)
      {
         drawer.draw_point(p, 2.);
      }
      if (diameter)
      {
         drawer.set_color(Qt::yellow);
         drawer.draw_line(*diameter);
      }
   }

   void print(cg::visualization::printer_type & p) const
   {
      p.corner_stream() << "Set diameter." << cg::visualization::endl
                        << "Double click to clear, right-click to add point." << cg::visualization::endl;
      if (diameter)
         p.corner_stream() << "Value: " << cg::length(*diameter) << cg::visualization::endl;
   }

   bool on_press(const point_2f & p)
   {
      for (size_t i = 0; i != points.size(); i++)
      {
         auto const& pt = points[i];
         if (fabs(pt.x - p.x) < 4 && fabs(pt.y - p.y) < 4)
         {
            idx = i;
            return true;
         }
      }

      points.push_back({p.x, p.y});
      auto res = cg::diameter(points.begin(), points.end());
      if (res.first == res.second || res.first == points.end())
         diameter = boost::none;
      else
         diameter = {*res.first, *res.second};
      return true;
   }

   bool on_double_click(point_2f const&)
   {
      points.clear();
      diameter = boost::none;
      return true;
   }

   bool on_release(const point_2f & p)
   {
      idx.reset();
      return true;
   }

   bool on_move(const point_2f & p)
   {
      if (idx)
      {
         points[*idx] = p;
         std::vector<point_2> cp (points);
         auto res = cg::diameter(cp.begin(), cp.end());
         if (res.first == res.second || res.first == points.end())
            diameter = boost::none;
         else
            diameter = {*res.first, *res.second};
      }

      return true;
   }

private:
   std::vector<point_2> points;
   boost::optional<segment_2f> diameter;
   boost::optional<size_t> idx;
};

int main(int argc, char ** argv)
{
   QApplication app(argc, argv);
   diameter_viewer viewer;
   cg::visualization::run_viewer(&viewer, "Simplify");
}
