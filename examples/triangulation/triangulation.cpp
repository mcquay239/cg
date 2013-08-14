#include <vector>
#include <fstream>
#include <string>

#include <QColor>
#include <QApplication>

#include "cg/visualization/viewer_adapter.h"
#include "cg/visualization/draw_util.h"

#include <cg/primitives/contour.h>
#include "cg/triangulation/triangulation.h"

#include "cg/io/point.h"

using cg::point_2;
using cg::point_2f;
using cg::vector_2f;

struct triangulation_viewer : cg::visualization::viewer_adapter
{
   triangulation_viewer()
   {
      in_building_ = true;
      poly.push_back(cg::contour_2());
   }

   void draw(cg::visualization::drawer_type & drawer) const
   {
      if (in_building_) {
         drawer.set_color(Qt::white);

         for (size_t i = 1; i < poly.back().size(); ++i) {
            drawer.draw_line(poly.back()[i - 1], poly.back()[i], 3);
         }
         drawer.set_color(Qt::red);
         for (size_t i = 0; i < poly.size() - 1; i++) {
            auto &cont = poly[i];
            for (size_t i = 0; i < cont.size(); ++i) {
               drawer.draw_line(cont[i], cont[(i + 1) % cont.vertices_num()], 3);
            }
         }

         return;
      }

      auto res = cg::triangulate(poly);

      drawer.set_color(Qt::green);

      for (auto triangle : res) {
         for (int i = 0; i < 3; i++) {
            drawer.draw_line(triangle[i], triangle[(i + 1) % 3]);
         }
      }
      
      drawer.set_color(Qt::red);

      for (auto &cont : poly) {
         for (size_t i = 0; i < cont.size(); ++i) {
            drawer.draw_line(cont[i], cont[(i + 1) % cont.vertices_num()], 3);
         }
      }
   }

   void print(cg::visualization::printer_type & p) const
   {
      p.corner_stream() << "double-click to clear." << cg::visualization::endl
                        << "press mouse rbutton for add vertex (click to first point to complete contour)" << cg::visualization::endl
                        << "move vertex with rbutton" << cg::visualization::endl
                        << "press h to start setting hole / press t to write current test in file" << cg::visualization::endl
                        << "YOU SHOULD INPUT ONLY SIMPLE POLYGONES(without self-intersections). OUTTER SHOULD BE CCW AND HOLES SHOULD BE CW" << cg::visualization::endl;
      for (auto &cont : poly) {
         for (size_t i = 0; i < cont.size(); ++i) {
            p.global_stream((point_2f)cont[i] + vector_2f(5, 0)) << i;
         }
      }
   }

   bool on_double_click(const point_2f & p)
   {
      poly.clear();
      in_building_ = true;
      current_vertex_.release();
      poly.push_back(cg::contour_2());
      return true;
   }

   bool on_press(const point_2f & p)
   {
      if (in_building_)
      {
         if (poly.back().size() > 1)
         {
            if (fabs(poly.back()[0].x - p.x) < 15 && fabs(poly.back()[0].y - p.y) < 15)
            {
               in_building_ = false;
               return true;
            }
         }

         poly.back().add_point(p);
         return true;
      }

      for (auto &cont : poly) { 
         for (size_t i = 0; i < cont.size(); ++i) {
            if (fabs(cont[i].x - p.x) < 15 && fabs(cont[i].y - p.y) < 15) {
               current_vertex_.reset(&cont[i]);
               return true;
            }
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

      current_vertex_.release();

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
         *current_vertex_ = p;
      }

      return true;
   }

   bool on_key(int key)
   {
      if (key == Qt::Key_H) { in_building_ = true; poly.push_back(cg::contour_2()); }
      else if (key == Qt::Key_T) {
         if (in_building_) return false;
         std::ofstream out("../../tests/tests/" + std::to_string(current_test++));
         out << poly.size() << std::endl;
         for (auto cont : poly) {
            out << cont.size() << std::endl;
            for (size_t i = 0; i < cont.size(); i++) {
               out << cont[i].x << " " << cont[i].y << std::endl;
            }
         }
      } else return false;

      return true;
   }


private:
   bool in_building_;
   std::vector<cg::contour_2> poly;
   std::unique_ptr<point_2> current_vertex_;
   int current_test = 0;
};

int main(int argc, char ** argv)
{
   QApplication app(argc, argv);
   triangulation_viewer viewer;
   cg::visualization::run_viewer(&viewer, "triangulation viewer");
}
