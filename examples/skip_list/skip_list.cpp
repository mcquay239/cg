#include <QColor>
#include <QApplication>

#include <map>
#include <set>

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <cg/visualization/viewer_adapter.h>
#include <cg/visualization/draw_util.h>

#include <cg/io/point.h>

#include <cg/primitives/point.h>

//#include <cg/psl/psl.h>

//using cg::meta_info_t;
//using cg::persistent_set_t;

using cg::point_2f;
using cg::point_2;

//struct skip_list_viewer : cg::visualization::viewer_adapter
//{
//   skip_list_viewer(persistent_set_t<float, size_t> & set)
//      : set_(set)
//   {}

//   void draw(cg::visualization::drawer_type & drawer) const
//   {
//      drawer.set_color(Qt::green);
//      set_.visit(boost::bind(&skip_list_viewer::draw, this, boost::ref(drawer), _1, _2, _3, _4));

//      drawer.set_color(Qt::white);
//      for (float y : set_.slice(mouse_time_))
//         drawer.draw_point(point_2f(mouse_time_, y), 5);

//      if (float const * l = set_.lower_bound(mouse_data_, mouse_time_))
//      {
//         drawer.set_color(*l == mouse_data_ ? Qt::blue : Qt::red);
//         drawer.draw_point(point_2f(mouse_time_, *l), 5);
//      }

//      if (float const * l = set_.lower_bound(mouse_data_))
//      {
//         drawer.set_color(*l == mouse_data_ ? Qt::red : Qt::blue);
//         drawer.draw_point(point_2f(set_.current_time(), mouse_data_), 5);
//      }
//   }

//   void print(cg::visualization::printer_type & p) const
//   {
//      p.corner_stream() << "insertions: " << insertions_ << cg::visualization::endl;
//      p.corner_stream() << "erasures: " << erasures_ << cg::visualization::endl;
//      p.corner_stream() << "nodes: " << set_.meta().nodes << cg::visualization::endl;
//      p.corner_stream() << "overheads: " << set_.meta().overhead << cg::visualization::endl;
//   }

//   bool on_move(const cg::point_2f & pt)
//   {
//      mouse_time_ = pt.x;
//      mouse_data_ = 20 * (int)(pt.y / 20);
//      return true;
//   }

//   bool on_release(const point_2f & p)
//   {
//      if (float const * l = set_.lower_bound(mouse_data_))
//      {
//         if (*l == mouse_data_)
//         {
//            if (set_.erase(*l))
//               ++erasures_;
//            return true;
//         }
//      }

//      set_.insert(mouse_data_);
//      ++insertions_;

//      return true;
//   }

//private:
//   bool draw(cg::visualization::drawer_type & drawer, float start_data, float start_time,
//             float finish_data, float finish_time) const
//   {
//      point_2f p[] = {
//         {start_time, start_data},
//         {finish_time, finish_data}
//      };

//      drawer.draw_line(p[0], p[1]);
//      for (point_2f const & pt : p)
//         drawer.draw_point(pt, 3);

//      return start_time <= finish_time;
//   }

//private:
//   float mouse_data_;
//   float mouse_time_;
//   persistent_set_t<float, size_t> & set_;

//   size_t insertions_;
//   size_t erasures_;
//};




int main(int argc, char ** argv)
{
   //QApplication app(argc, argv);

//   test(1, true);


//   skip_list_viewer viewer;
//   cg::visualization::run_viewer(&viewer, "skip list");
}

