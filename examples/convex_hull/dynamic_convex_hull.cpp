#include <QColor>
#include <QApplication>

#include <boost/optional.hpp>

#include <cg/visualization/viewer_adapter.h>
#include <cg/visualization/draw_util.h>
#include <cg/convex_hull/naive_dynamic.h>

#include <cg/io/point.h>

#include <cg/primitives/point.h>
#include <cg/convex_hull/graham.h>

using cg::point_2f;
using cg::point_2;

template <class Algo>
struct dynamic_hull_viewer : cg::visualization::viewer_adapter
{
   dynamic_hull_viewer()
      : mode(inserting), ch_size(0), all_pts(0)
   {}

   void draw(cg::visualization::drawer_type& drawer) const
   {
      drawer.set_color(Qt::white);

      for (vect_it it = all_pts_pair.first; it != all_pts_pair.second; it++)
      {
         drawer.draw_point(*it);
      }

      if (ch_size == 0)
      {
         return;
      }

      drawer.set_color(Qt::green);
      drawer.draw_point(*(hull_pair.first), 3);
      drawer.draw_line(*(hull_pair.first), *(hull_pair.second - 1));

      for (vect_it it = hull_pair.first + 1; it != hull_pair.second; it++)
      {
         drawer.draw_point(*it, 3);
         drawer.draw_line(*it, *(it - 1));
      }

      if (cur_it)
      {
         drawer.set_color(Qt::yellow);
         drawer.draw_point(**cur_it, 5);
      }
   }

   void print(cg::visualization::printer_type& p) const
   {
      p.corner_stream() << "______" << cg::visualization::endl
                        << "ACTIVE BUTTONS : "<< cg::visualization::endl
                        << "press mouse rbutton to manipulate with points" << cg::visualization::endl
                        << "press 'i' or 'd' to change mode (insert or delete)" << cg::visualization::endl
                        << "______" << cg::visualization::endl
                        << "INFO" << cg::visualization::endl
                        << "points: " << all_pts << " convex_hull: " << ch_size << cg::visualization::endl
                        << "current mode : " << (mode == inserting ? "INSERT" : "DELETE") << cg::visualization::endl;
   }

   bool on_release(const point_2f& p)
   {
      switch (mode)
      {
      case inserting :
         dh.add_point(p);
         update_all_pts();
         break;

      case deleting :
         if (cur_it)
         {
            dh.remove_point(**cur_it);
            update_all_pts();
         }

         break;
      }

      make_hull();
      set_cur_it(p);
      return true;
   }

   bool on_key(int key)
   {
      switch (key)
      {
      case Qt::Key_I :
         mode = inserting;
         break;

      case Qt::Key_D :
         mode = deleting;
         break;

      default :
         return false;
      }

      make_hull();
      return true;
   }

   bool on_move(const point_2f& p)
   {
      set_cur_it(p);
      current_p = &p;
      return true;
   }

private:
   void make_hull()
   {
      hull_pair = dh.get_hull();
      ch_size = std::distance(hull_pair.first, hull_pair.second);
   }

   bool set_cur_it (const point_2f& p)
   {
      cur_it.reset();
      float max_r;

      for (vect_it it = all_pts_pair.first; it != all_pts_pair.second; it++)
      {
         float current_r = (p.x - (*it).x) * (p.x - (*it).x) + (p.y - (*it).y) * (p.y - (*it).y);

         if ((cur_it && current_r < max_r) || (!cur_it && current_r < 100))
         {
            cur_it = it;
            max_r = current_r;
         }

      }

      return cur_it;
   }

   void update_all_pts()
   {
      all_pts_pair = dh.get_all_points();
      all_pts = std::distance(all_pts_pair.first, all_pts_pair.second);
   }

   enum modes
   {
      inserting = 0,
      deleting = 1
   } mode;

   Algo dh;

   boost::optional<vect_it> cur_it;
   const point_2f * current_p;
   size_t ch_size, all_pts;
   std::pair<vect_it, vect_it> hull_pair, all_pts_pair;
};

int main(int argc, char ** argv)
{
   QApplication app(argc, argv);
   dynamic_hull_viewer<cg::naive_dynamic_hull> viewer;
   cg::visualization::run_viewer(&viewer, "dynamic convex hull");
}
