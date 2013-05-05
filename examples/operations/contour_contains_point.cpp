#include <QColor>
#include <QApplication>

#include <boost/optional.hpp>

#include "cg/visualization/viewer_adapter.h"
#include "cg/visualization/draw_util.h"

#include "cg/io/point.h"

#include <cg/primitives/triangle.h>
#include <cg/primitives/point.h>

#include <cg/operations/contains/contour_point.h>
#include <vector>

using cg::point_2f;
using cg::point_2;


struct triangle_contains_point_viewer : cg::visualization::viewer_adapter
{
   triangle_contains_point_viewer()
      : contour(std::vector<point_2>({point_2(0, 0), point_2(100, 100), point_2(200, 0)}))
   {}

   void draw(cg::visualization::drawer_type & drawer) const override
   {
      drawer.set_color(Qt::white);
      if (current_point_ && cg::contains(contour, *current_point_))
         drawer.set_color(Qt::green);

      for (size_t l = 0, lp = contour.size() - 1; l != contour.size(); lp = l++)
         drawer.draw_line(contour[lp], contour[l]);

      if (idx_)
         drawer.draw_point(contour[*idx_], 5);
   }

   void print(cg::visualization::printer_type & p) const override
   {
      p.corner_stream() << "press mouse lbutton with CTRL key near contour vertex to move it"
                        << cg::visualization::endl
                        << "if triangle is green contour contains cursor point"
                        << cg::visualization::endl;
   }

   bool on_press(const point_2f & p) override
   {
      for (size_t l = 0; l != contour.size(); ++l)
      {
         if (fabs(p.x - contour[l].x) < 4 && fabs(p.y - contour[l].y) < 4)
         {
            idx_ = l;
            return true;
         }
      }

      contour.add_point(p);

      return false;
   }

   bool on_release(const point_2f & p) override
   {
      idx_.reset();
      return false;
   }

   bool on_move(const point_2f & p) override
   {
      current_point_ = p;

      if (!idx_)
         return true;

      contour[*idx_] = p;
      return true;
   }

private:
   cg::contour_2 contour;
   boost::optional<size_t> idx_;
   boost::optional<cg::point_2> current_point_;
};

int main(int argc, char ** argv)
{
   QApplication app(argc, argv);
   triangle_contains_point_viewer viewer;
   cg::visualization::run_viewer(&viewer, "Contour contains point");
}
