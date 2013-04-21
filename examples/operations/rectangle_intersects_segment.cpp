#include <QColor>
#include <QApplication>

#include <boost/optional.hpp>

#include "cg/visualization/viewer_adapter.h"
#include "cg/visualization/draw_util.h"

#include <cg/primitives/segment.h>
#include <cg/primitives/rectangle.h>
#include <cg/primitives/range.h>
#include <cg/primitives/point.h>

#include <cg/operations/has_intersection/rectangle_segment.h>

using cg::point_2;
using cg::point_2f;

struct rectangle_intersects_segment_viewer : cg::visualization::viewer_adapter
{
   rectangle_intersects_segment_viewer()
      : s_(point_2(-100, -100), point_2(100, 100)),
        r_(cg::range(-50, 50), cg::range(-50, 50)),
	     rbutton_pressed_(false)
   {}

   void draw(cg::visualization::drawer_type & drawer) const
   {
      drawer.set_color(Qt::white);
      if (cg::has_intersection(r_, s_))
         drawer.set_color(Qt::green);

      drawer.draw_line(s_[0], s_[1]);
      for(size_t l = 0; l != 2; ++l) {
         drawer.draw_line(r_.corner(l, l), r_.corner(l^1, l));
         drawer.draw_line(r_.corner(l, l), r_.corner(l, l^1));
      }
	  
	   if (idx_) {
         drawer.set_color(rbutton_pressed_ ? Qt::red : Qt::yellow);
         if(on_rect)
            drawer.draw_point(r_.corner((*idx_).first, (*idx_).second), 5);
         else
            drawer.draw_point(s_[(*idx_).first], 5);
	   }
   }

   void print(cg::visualization::printer_type & p) const
   {
      p.corner_stream() << "press mouse rbutton near segment vertex to move it"
                        << cg::visualization::endl
                        << "if lines are green there is intersection"
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

      if (rbutton_pressed_) {
         if(on_rect) {
            (*idx_).first ? r_.x.sup = p.x : r_.x.inf = p.x;
            (*idx_).second ? r_.y.sup = p.y : r_.y.inf = p.y;
         }
         else
            s_[(*idx_).first] = p;
      }
      return true;
   }

private:
   bool set_idx (const point_2f & p)
   {
      idx_.reset();
      float max_r;
      for (size_t l = 0; l != 2; ++l)
      {
         for (size_t lp = 0; lp != 2; ++lp) {
            float current_r = (p.x - r_.corner(l, lp).x) * (p.x - r_.corner(l, lp).x) + (p.y - r_.corner(l, lp).y) * (p.y - r_.corner(l ,lp).y);
            if ((idx_ && current_r < max_r) || (!idx_ && current_r < 100))
            {
               idx_ = std::make_pair(l, lp);
               max_r = current_r;
               on_rect = true;
            }
         }
      }
      for (size_t l = 0; l != 2; ++l)
      {
         float current_r = (p.x - s_[l].x) * (p.x - s_[l].x) + (p.y - s_[l].y) * (p.y - s_[l].y);
         if ((idx_ && current_r < max_r) || (!idx_ && current_r < 100))
         {
            idx_ = std::make_pair(l, 0);
            max_r = current_r;
            on_rect = false;
         }
      }

      return idx_;
   }
	
   cg::segment_2 s_;
   bool on_rect;
   cg::rectangle_2 r_;
   boost::optional< std::pair<size_t, size_t> > idx_;
   bool rbutton_pressed_;
};

int main(int argc, char ** argv)
{
   QApplication app(argc, argv);
   rectangle_intersects_segment_viewer viewer;
   cg::visualization::run_viewer(&viewer, "rectangle intersects segment");
}
