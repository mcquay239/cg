#include <QColor>
#include <QApplication>

#include <boost/optional.hpp>

#include "cg/visualization/viewer_adapter.h"
#include "cg/visualization/draw_util.h"

#include "cg/io/point.h"

#include <cg/primitives/triangle.h>
#include <cg/primitives/point.h>

#include <cg/operations/contains/triangle_point.h>

using cg::point_2f;
using cg::point_2;


struct triangle_contains_point_viewer : cg::visualization::viewer_adapter
{
   triangle_contains_point_viewer()
      : t_(point_2(0, 0), point_2(100, 100), point_2(200, 0))
	  , lbutton_pressed_(false)
   {}

   void draw(cg::visualization::drawer_type & drawer) const
   {
      drawer.set_color(Qt::white);
      if (current_point_ && cg::contains(t_, *current_point_))
         drawer.set_color(Qt::green);

      for (size_t l = 0, lp = 2; l != 3; lp = l++)
         drawer.draw_line(t_[lp], t_[l]);
	  
	  if (idx_)
	  {
		  drawer.set_color((lbutton_pressed_)? Qt::red : Qt::yellow);
		  drawer.draw_point(t_[*idx_], 5);
	  }
   }

   void print(cg::visualization::printer_type & p) const
   {
      p.corner_stream() << "press mouse rbutton near triangle vertex to move it"
                        << cg::visualization::endl
                        << "if triangle is green triangle contains cursor point"
                        << cg::visualization::endl;
   }

   bool on_press(const point_2f & p)
   {
	  lbutton_pressed_ = true;
      return set_idx(p);
   }

   bool on_release(const point_2f & p)
   {
      lbutton_pressed_ = false;
      return false;
   }

   bool on_move(const point_2f & p)
   {
      current_point_ = p;
      set_idx(p);
      if (!idx_)
         return true;

      if (lbutton_pressed_) t_[*idx_] = p;
      return true;
   }

private:
   bool set_idx (const point_2f & p)
   {
	   idx_.reset();
       for (size_t l = 0; l != 3; ++l)
       if (fabs(p.x - t_[l].x) < 25 && fabs(p.y - t_[l].y) < 25)
       {
          idx_ = l;
          return true;
       }
       return false;
   }
	
   cg::triangle_2 t_;
   boost::optional<size_t> idx_;
   boost::optional<cg::point_2> current_point_;
   bool lbutton_pressed_;
};

int main(int argc, char ** argv)
{
   QApplication app(argc, argv);
   triangle_contains_point_viewer viewer;
   cg::visualization::run_viewer(&viewer, "triangle contains point");
}
