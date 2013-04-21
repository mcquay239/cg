#include "drawer_impl.h"

namespace cg {
namespace visualization
{
   void drawer_impl::clear()
   {
      point_buffers.clear();
      segment_buffers.clear();
   }

   void drawer_impl::set_color(QColor const & c)
   {
      current_color_ = c;
   }

   void drawer_impl::draw_line(point_2f const & a, point_2f const & b, float width)
   {
      std::vector<GLfloat> & points_buffer = segment_buffers[width].segments;
      points_buffer.push_back(a.x);
      points_buffer.push_back(a.y);
      points_buffer.push_back(b.x);
      points_buffer.push_back(b.y);

      std::vector<GLfloat> & colors_buffer = segment_buffers[width].colors;
      colors_buffer.push_back(current_color_.redF());
      colors_buffer.push_back(current_color_.greenF());
      colors_buffer.push_back(current_color_.blueF());
      colors_buffer.push_back(current_color_.redF());
      colors_buffer.push_back(current_color_.greenF());
      colors_buffer.push_back(current_color_.blueF());
   }

   void drawer_impl::draw_line(segment_2f const & seg, float width)
   {
      draw_line(seg[0], seg[1], width);
   }

   void drawer_impl::draw_point(point_2f const & pt, float radius)
   {
      std::vector<GLfloat> & points_buffer = point_buffers[radius].points;
      points_buffer.push_back(pt.x);
      points_buffer.push_back(pt.y);

      std::vector<GLfloat> & colors_buffer = point_buffers[radius].colors;
      colors_buffer.push_back(current_color_.redF());
      colors_buffer.push_back(current_color_.greenF());
      colors_buffer.push_back(current_color_.blueF());
   }
}}
