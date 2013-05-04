#include <QColor>
#include <QApplication>

#include <boost/optional.hpp>

#include "cg/visualization/viewer_adapter.h"
#include "cg/visualization/draw_util.h"

#include <cg/primitives/segment.h>
#include <cg/primitives/point.h>

#include <cg/operations/has_intersection/segment_segment.h>

using cg::point_2;
using cg::point_2f;

struct segments_intersect_viewer : cg::visualization::viewer_adapter
{
   segments_intersect_viewer()
      : s_ { cg::segment_2(point_2(-100, -100), point_2(100, 100)), 
             cg::segment_2(point_2(100, -100), point_2(-100, 100)) },
	     rbutton_pressed_(false)
   {}

   void draw(cg::visualization::drawer_type & drawer) const
   {
      drawer.set_color(Qt::white);
      if (cg::has_intersection(s_[0], s_[1]))
         drawer.set_color(Qt::green);

      for (size_t l = 0; l != 2; l++)
         drawer.draw_line(s_[l][0], s_[l][1]);
	  
	  if (idx_)
	  {
		  drawer.set_color(rbutton_pressed_ ? Qt::red : Qt::yellow);
		  drawer.draw_point(s_[(*idx_).first][(*idx_).second], 5);
	  }
   }

   void print(cg::visualization::printer_type & p) const
   {
      p.corner_stream() << "press mouse rbutton near segment vertex to move it"
                        << cg::visualization::endl
                        << "if segments are green they intersect"
                        << cg::visualization::endl;
   }

   bool on_press(const point_2f & p)
   {
	   rbutton_pressed_ = true;
      return set_idx(p);
   }

   bool on_release(const point_2f & p)
   {
      rbutton_pressed_ = false;
      return false;
   }

   bool on_move(const point_2f & p)
   {
      if (!rbutton_pressed_)
         set_idx(p);
      if (!idx_)
         return true;

      if (rbutton_pressed_) s_[(*idx_).first][(*idx_).second] = p;
      return true;
   }

private:
   bool set_idx (const point_2f & p)
   {
      idx_.reset();
      float max_r;
      for (size_t l = 0; l != 2; ++l)
      {
         for (size_t k = 0; k != 2; ++k) {
            float current_r = (p.x - s_[l][k].x) * (p.x - s_[l][k].x) + (p.y - s_[l][k].y) * (p.y - s_[l][k].y);
            if ((idx_ && current_r < max_r) || (!idx_ && current_r < 100))
            {
               idx_ = std::make_pair(l, k);
               max_r = current_r;
            }
         }
      }
      return idx_;
   }
	
   cg::segment_2 s_[2];
   boost::optional< std::pair<size_t, size_t> > idx_;
   bool rbutton_pressed_;
};

int main(int argc, char ** argv)
{
   QApplication app(argc, argv);
   segments_intersect_viewer viewer;
   cg::visualization::run_viewer(&viewer, "segments intersect");
}
