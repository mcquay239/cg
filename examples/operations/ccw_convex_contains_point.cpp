#include <vector>
#include <stack>

#include <QColor>
#include <QApplication>

#include <boost/optional.hpp>

#include "cg/visualization/viewer_adapter.h"
#include "cg/visualization/draw_util.h"

#include <cg/operations/orientation.h>
#include <cg/operations/convex.h>
#include <cg/operations/contains/contour_point.h>
#include <cg/primitives/contour.h>

#include "cg/io/point.h"

using cg::point_2;
using cg::point_2f;
using cg::vector_2f;

struct ccw_convex_contour_contains_point_viewer : cg::visualization::viewer_adapter
{
   ccw_convex_contour_contains_point_viewer()
   {
      in_building = true;
   }

   void draw(cg::visualization::drawer_type & drawer) const
   {
      if (in_building)
      {
         drawer.set_color(Qt::white);

         for (size_t i = 1; i < points_.size(); ++i)
         {
            drawer.draw_line(points_[i - 1], points_[i]);
         }

         return;
      }

      cg::contour_2 cont(points_);

      if (!cg::counterclockwise(cont) || !cg::convex(cont))
      {
         drawer.set_color(Qt::yellow);
      } else
      {
         drawer.set_color(cg::convex_contains(cont, *current_point_) ? Qt::green : Qt::red);
      }

      for (size_t i = 1; i < points_.size(); ++i)
      {
         drawer.draw_line(points_[i - 1], points_[i]);
      }

      drawer.draw_line(points_.front(), points_.back());


   }

   void print(cg::visualization::printer_type & p) const
   {
      p.corner_stream() << "double-click to clear." << cg::visualization::endl
                        << "press mouse rbutton for add vertex (click to first point to complete contour)" << cg::visualization::endl
                        << "move vertex with rbutton" << cg::visualization::endl
                        << "yellow contour -- not ccw or convex" << cg::visualization::endl
                        << "green contour -- contains cursor" << cg::visualization::endl
                        << "red contour -- don't contains cursor" << cg::visualization::endl;

      for (size_t i = 0; i < points_.size(); ++i)
      {
         p.global_stream((point_2f)points_[i] + vector_2f(5, 0)) << i;
      }
   }

   bool on_double_click(const point_2f & p)
   {
      points_.clear();
      in_building = true;
      return true;
   }

   bool on_press(const point_2f & p)
   {
      if (in_building)
      {
         if (points_.size() > 1)
         {
            if (fabs(points_[0].x - p.x) < 15 && fabs(points_[0].y - p.y) < 15)
            {
               in_building = false;
               return true;
            }
         }

         points_.push_back(p);
         return true;
      }

      for (size_t i = 0; i < points_.size(); ++i)
      {
         if (fabs(points_[i].x - p.x) < 15 && fabs(points_[i].y - p.y) < 15)
         {
            current_vertex_ = i;
            return true;
         }
      }

      return true;
   }

   bool on_release(const point_2f & p)
   {
      if (in_building)
      {
         return true;
      }

      current_vertex_.reset();

      return true;
   }

   bool on_move(const point_2f & p)
   {
      current_point_ = p;

      if (in_building)
      {
         return true;
      }

      if (current_vertex_)
      {
         points_[*current_vertex_] = p;
      }

      return true;
   }

private:
   bool in_building;
   std::vector<point_2> points_;
   boost::optional<int> current_vertex_;
   boost::optional<point_2> current_point_;
};

int main(int argc, char ** argv)
{
   QApplication app(argc, argv);
   ccw_convex_contour_contains_point_viewer viewer;
   cg::visualization::run_viewer(&viewer, "contour ccw convex contains point viewer");
}
