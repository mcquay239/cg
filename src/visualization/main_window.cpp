#include "stdafx.h"

#include <boost/lexical_cast.hpp>

#include "main_window.h"

#include "printer_impl.h"

#include "cg/io/point.h"
#include "cg/primitives/rectangle.h"

main_window_t::main_window_t(viewer_type * viewer)
   : viewer_(viewer)
   , size_(100000, 100000)
   , current_pos_(center_)
   , zoom_(1.0)
{
   setMouseTracking(true);
   viewer_->draw(drawer_);
}

void main_window_t::initializeGL()
{
   assert(doubleBuffer());
   setAutoBufferSwap(true);
   qglClearColor(Qt::black);
   //glEnable(GL_PROGRAM_POINT_SIZE);
}

using cg::vector_2f;

namespace
{
   float limit(cg::range_f const & range, double v)
   {
      if (v < double(range.inf))
         return range.inf;
      else if (v > double(range.sup))
         return range.sup;
      else
         return float(v);
   }

   point_2f limit(point_2f const & pt)
   {
      using cg::rectangle_2f;
      rectangle_2f max_rect = rectangle_2f::maximal();

      return point_2f(
               limit(max_rect.x, pt.x),
               limit(max_rect.y, pt.y)
               );
   }

   // в этом месте возможно переполнение!
   vector_2f const operator * (double alpha, vector_2f const & v)
   {
      using cg::rectangle_2f;
      rectangle_2f max_rect = rectangle_2f::maximal();

      return vector_2f(
               limit(max_rect.x, v.x * alpha),
               limit(max_rect.y, v.y * alpha)
               );
   }
}

void main_window_t::resize_impl(int screen_w, int screen_h)
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   vector_2f size = (zoom_ / 2) * vector_2f(screen_w, screen_h);

   point_2f left_bottom = center_ + (-size);
   point_2f right_top   = center_ + size;

   glOrtho(left_bottom.x, right_top.x, left_bottom.y, right_top.y, -1.0, 1.0);
   glViewport(0, 0, screen_w, screen_h);
}

void main_window_t::resizeGL(int screen_w, int screen_h)
{
   resize_impl(screen_w, screen_h);
}

void main_window_t::paintGL()
{
   glClear(GL_COLOR_BUFFER_BIT);

   for (auto const & pair : drawer_.point_buffers)
   {
      float radius = pair.first;
      drawer_impl::point_buffer_t const & buffer = pair.second;
      
      glPointSize(radius);

      glEnableClientState(GL_VERTEX_ARRAY);
      glEnableClientState(GL_COLOR_ARRAY);

      glVertexPointer (2, GL_FLOAT, 0, buffer.points.data());
      glColorPointer  (3, GL_FLOAT, 0, buffer.colors.data());

      glDrawArrays(GL_POINTS, 0, buffer.points.size() / 2);

      glDisableClientState(GL_VERTEX_ARRAY);
      glDisableClientState(GL_COLOR_ARRAY);
   }

   for (auto const & pair : drawer_.segment_buffers)
   {
      float width = pair.first;
      drawer_impl::segment_buffer_t const & buffer = pair.second;
      
      glLineWidth(width);

      glEnableClientState(GL_VERTEX_ARRAY);
      glEnableClientState(GL_COLOR_ARRAY);

      glVertexPointer (2, GL_FLOAT, 0, buffer.segments.data());
      glColorPointer  (3, GL_FLOAT, 0, buffer.colors.data());

      glDrawArrays(GL_LINES, 0, buffer.segments.size() / 2);

      glDisableClientState(GL_VERTEX_ARRAY);
      glDisableClientState(GL_COLOR_ARRAY);
   }

   printer_impl printer(   boost::bind(&main_window_t::draw_string,        this, _1, _2),
                           boost::bind(&main_window_t::draw_string_global, this, _1, _2)   );

   printer.corner_stream() << "Mouse pos: " << current_pos_ << endl;

   viewer_->print(printer);
}

void main_window_t::wheelEvent(QWheelEvent * e)
{
   double old_zoom = zoom_;

   int delta = e->delta() / 8 / 15;
   if (delta > 0)
   {
      for (int i = 0; i != delta; ++i)
         zoom_ *= 1.1;
   }
   else if (delta < 0)
   {
      for (int i = 0; i != delta; --i)
         zoom_ /= 1.1;
   }

   point_2f pos(e->pos().x(), e->pos().y());
   point_2f sz(size().width() / 2, size().height() / 2);

   vector_2f diff = pos - sz;

   center_ += (old_zoom - zoom_) * vector_2f(diff.x, -diff.y);
   center_ = limit(center_);

   e->accept();

   viewer_->on_move(limit(screen_to_global(e->pos())));

   resize_impl(size().width(), size().height());
   updateGL();
}

void main_window_t::mousePressEvent(QMouseEvent * e)
{
   if (e->button() == Qt::LeftButton && e->modifiers() == Qt::NoModifier)
      start_point_ = limit(screen_to_global(e->pos()));
   else if (e->button() == Qt::RightButton)
   {
      if (viewer_->on_press(current_pos_))
      {
         drawer_.clear();
         viewer_->draw(drawer_);
         updateGL();
      }
   }
   e->accept();
}

void main_window_t::mouseMoveEvent(QMouseEvent * e)
{
   current_pos_ = limit(screen_to_global(e->pos()));

   if (start_point_ )
   {
      const int w = size().width();
      const int h = size().height();

      point_2f pos(e->pos().x(), e->pos().y());
      point_2f sz(w / 2, h / 2);

      using cg::vector_2f;

      vector_2f diff = pos - sz;
      diff.x = -diff.x;

      center_ = *start_point_ + zoom_ * diff;
      center_ = limit(center_);

      resize_impl(w, h);
   }
   else if (viewer_->on_move(current_pos_))
   {
      drawer_.clear();
      viewer_->draw(drawer_);
   }
   e->accept();
   updateGL();
}

void main_window_t::mouseReleaseEvent(QMouseEvent * e)
{
   if (e->button() == Qt::LeftButton)
      start_point_ = boost::none;
   else if (viewer_->on_release(limit(screen_to_global(e->pos()))))
   {
      drawer_.clear();
      viewer_->draw(drawer_);
      updateGL();
   }
   e->accept();
}

void main_window_t::mouseDoubleClickEvent(QMouseEvent * event)
{
   if (viewer_->on_double_click(limit(screen_to_global(event->pos()))))
   {
      drawer_.clear();
      viewer_->draw(drawer_);
      updateGL();
   }
}

void main_window_t::keyReleaseEvent(QKeyEvent * event)
{
   if ((event->key() == Qt::Key_C) && (event->modifiers() == Qt::ControlModifier))
   {
      std::stringstream ss;
      ss << QInputDialog::getText(this, "center selection", "type point: ").toStdString();
      point_2f old_pos = current_pos_;
      ss >> current_pos_;

      center_ += (current_pos_ - old_pos);

      resize_impl(size().width(), size().height());
      updateGL();
   }
   else if ((event->key() == Qt::Key_I) && (event->modifiers() == Qt::ControlModifier))
   {
      auto txt = boost::lexical_cast<std::string>(current_pos_);
      QApplication::clipboard()->setText(txt.c_str());
   }
   else if (viewer_->on_key(event->key()))
   {
      drawer_.clear();
      viewer_->draw(drawer_);
      updateGL();
   }
   event->accept();
}

point_2f main_window_t::screen_to_global(QPoint const & screen_pos) const
{
   point_2f pos(screen_pos.x(), screen_pos.y());
   point_2f sz(size().width() / 2, size().height() / 2);

   vector_2f diff = pos - sz;
   diff.y = -diff.y;

   return center_ + zoom_ * diff;
}

void main_window_t::draw_string(const point_2i &pos, const char * s)
{
   qglColor(Qt::white);
   renderText(pos.x, pos.y, s);
}

void main_window_t::draw_string_global(const point_2f &pos, const char * s)
{
   qglColor(Qt::white);
   renderText(pos.x, pos.y, 0, s);
}
