#include <QColor>
#include <QApplication>

#include <boost/optional.hpp>
#include <boost/assign/list_of.hpp>

#include <cg/visualization/viewer_adapter.h>
#include <cg/visualization/draw_util.h>

#include <cg/visibility/visibility_fast.h>
#include <cg/io/point.h>

#include <cg/primitives/point.h>
#include <cg/primitives/contour.h>
#include <cg/primitives/segment.h>

#include <vector>
#include <iostream>

using cg::point_2f;
using cg::point_2;
using cg::contour_2;
using cg::segment_2;

struct visibility_fast_viewer : cg::visualization::viewer_adapter
{

   visibility_fast_viewer()
   {
      std::vector<contour_2> vec;
      pts_ = vec;
      res = cg::get_visibility_graph(pts_, remove_redundant_edges);
   }

   void draw(cg::visualization::drawer_type &drawer) const
   {
      drawer.set_color(Qt::red);
      for (auto sgt = res.begin(); sgt != res.end(); ++sgt)
      {
         segment_2 seg= *sgt;
         drawer.draw_point(seg[0], 7);
         drawer.draw_point(seg[1], 7);
         drawer.draw_line(seg[0], seg[1]);
      }

      for (auto cnt = pts_.begin(); cnt != pts_.end(); ++cnt)
      {
         drawer.set_color(Qt::green);
         drawer.draw_point(*(cnt->begin()), 7);
         for(auto i = cnt->begin() + 1; i != cnt->end(); i++)
         {
            drawer.draw_point(*i, 7);
            drawer.draw_line(*i, *(i-1));
         }
         drawer.draw_line(*(cnt->begin()), *((cnt->end() - 1)));
      }
   }

   void print(cg::visualization::printer_type &p) const
   {
      p.corner_stream() << "Click to add vertex to current contour." << cg::visualization::endl
                        << "Press <C> to <C>lear the field" << cg::visualization::endl
                        << "Press <N> to start <N>ew contour" << cg::visualization::endl
                        << "Press <M> to change removing redundant edges <M>ode" << cg::visualization::endl
                        << cg::visualization::endl
                        << "Contour: " << (adding ? " new" : " old") << cg::visualization::endl
                        << "Remove edges mode: " << (remove_redundant_edges ? " yes" : " no") << cg::visualization::endl;
   }

   bool on_press(const cg::point_2f &p) override
   {
      if (adding)
      {
         std::vector<point_2> v;
         v.push_back(p);
         pts_.push_back(contour_2(v));
         adding = false;
      }
      else
      {
         pts_[pts_.size() - 1].add_point(p);
      }
      res = cg::get_visibility_graph(pts_, remove_redundant_edges);
   }

   bool on_key(int key)
   {
      switch (key)
      {
      case Qt::Key_N :
         adding = true;
         break;
      case Qt::Key_C :
         adding = true;
         pts_.clear();
         res.clear();
         break;
      case Qt::Key_M :
         remove_redundant_edges = !remove_redundant_edges;
         res = cg::get_visibility_graph(pts_, remove_redundant_edges);
         break;
      default :
         return false;
      }

      return true;
   }

private:

   std::vector<cg::contour_2> pts_;
   std::vector<cg::segment_2> res;
   bool adding = true;
   bool remove_redundant_edges = true;
};

int main(int argc, char **argv)
{
   QApplication app(argc, argv);
   visibility_fast_viewer viewer;
   cg::visualization::run_viewer(&viewer, "visibility_fast");
}
