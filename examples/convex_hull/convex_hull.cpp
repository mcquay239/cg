#include <QColor>
#include <QApplication>

#include <boost/optional.hpp>

#include <cg/visualization/viewer_adapter.h>
#include <cg/visualization/draw_util.h>

#include <cg/io/point.h>

#include <cg/primitives/point.h>
#include <cg/convex_hull/graham.h>
#include <cg/convex_hull/andrew.h>
#include <cg/convex_hull/jarvis.h>

using cg::point_2f;
using cg::point_2;


struct graham_viewer : cg::visualization::viewer_adapter
{
   graham_viewer()
      : algo(jarvis), ch_size_(0)
   {}

   void draw(cg::visualization::drawer_type & drawer) const
   {
      drawer.set_color(Qt::white);
      for (point_2 const & p : pts_)
         drawer.draw_point(p);

      drawer.set_color(Qt::green);
      for (size_t lp = ch_size_ - 1, l = 0; l != ch_size_; lp = l++)
      {
         drawer.draw_point(pts_[l], 3);
         drawer.draw_line(pts_[lp], pts_[l]);
      }
   }

   void print(cg::visualization::printer_type & p) const
   {
      p.corner_stream() << "press mouse rbutton to add point" << cg::visualization::endl
                        << "points: " << pts_.size() << " convex_hull: " << ch_size_ << cg::visualization::endl
                        << "press 'g' or 'a' to change algorithm" << cg::visualization::endl;
      switch (algo)
      {
         case graham : p.corner_stream() << "algorithm: Graham" << cg::visualization::endl; break;
         case andrew : p.corner_stream() << "algorithm: Andrew" << cg::visualization::endl; break;
         case jarvis : p.corner_stream() << "algorithm: Jarvis" << cg::visualization::endl; break;
      }
   }

   void make_hull() {
      std::vector<point_2>::iterator it;
      switch (algo)
      {
         case graham : it = cg::graham_hull(pts_.begin(), pts_.end()); break;
         case andrew : it = cg::andrew_hull(pts_.begin(), pts_.end()); break;
         case jarvis : it = cg::jarvis_hull(pts_.begin(), pts_.end()); break;
      }
      ch_size_ = std::distance(pts_.begin(), it);
   }

   bool on_release(const point_2f & p)
   {
      pts_.push_back(p);
      make_hull();
      return true;
   }

   bool on_key(int key) {
      switch (key)
      {
         case Qt::Key_G : algo = graham; break;
         case Qt::Key_A : algo = andrew; break;
         default : return false;
      }
      make_hull();
      return true;
   }

private:
   enum algorithms {
      graham =       0,
      andrew =       1,
      jarvis =       2
   } algo;
   std::vector<point_2> pts_;
   size_t ch_size_;
};

int main(int argc, char ** argv)
{
   QApplication app(argc, argv);
   graham_viewer viewer;
   cg::visualization::run_viewer(&viewer, "convex hull");
}
