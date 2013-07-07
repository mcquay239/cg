#include <vector>
#include <stack>

#include <QColor>
#include <QApplication>

#include <boost/optional.hpp>

#include "cg/visualization/viewer_adapter.h"
#include "cg/visualization/draw_util.h"

#include <cg/operations/orientation.h>

#include <cg/primitives/contour.h>

#include "cg/io/point.h"

using cg::point_2;
using cg::point_2f;
using cg::vector_2f;

struct contour_ccw_viewer : cg::visualization::viewer_adapter
{
   contour_ccw_viewer()
   {
      in_building_ = true;
   }

   void draw(cg::visualization::drawer_type & drawer) const
   {
      if (in_building_)
      {
         drawer.set_color(Qt::white);

         for (size_t i = 1; i < points_.size(); ++i)
         {
            drawer.draw_line(points_[i - 1], points_[i]);
         }

         return;
      }


      cg::contour_2 cont(points_);

      drawer.set_color(Qt::red);
      if (cg::counterclockwise(cont))
      {
         drawer.set_color(Qt::green);
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
                        << "green contour -- ccw" << cg::visualization::endl
                        << "red contour -- cw" << cg::visualization::endl;

      for (size_t i = 0; i < points_.size(); ++i)
      {
         p.global_stream((point_2f)points_[i] + vector_2f(5, 0)) << i;
      }
   }

   bool on_double_click(const point_2f & p)
   {
      points_.clear();
      in_building_ = true;
      current_vertex_.reset();
      return true;
   }

   bool on_press(const point_2f & p)
   {
      if (in_building_)
      {
         if (points_.size() > 1)
         {
            if (fabs(points_[0].x - p.x) < 15 && fabs(points_[0].y - p.y) < 15)
            {
               in_building_ = false;
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
      if (in_building_)
      {
         return true;
      }

      current_vertex_.reset();

      return true;
   }

   bool on_move(const point_2f & p)
   {
      if (in_building_)
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
   bool in_building_;
   std::vector<point_2> points_;
   boost::optional<int> current_vertex_;
};

int main(int argc, char ** argv)
{
   QApplication app(argc, argv);
   contour_ccw_viewer viewer;
   cg::visualization::run_viewer(&viewer, "contour ccw viewer");
}
