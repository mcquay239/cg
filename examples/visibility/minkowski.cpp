#include <vector>
#include <stack>
#include <algorithm>

#include <QColor>
#include <QApplication>

#include <boost/optional.hpp>

#include "cg/visualization/viewer_adapter.h"
#include "cg/visualization/draw_util.h"

#include <cg/operations/orientation.h>

#include <cg/primitives/contour.h>

#include "cg/io/point.h"
#include <cg/visibility/minkovsky_sum.h>

using cg::point_2;
using cg::contour_2;

using cg::point_2f;
using cg::vector_2f;
using cg::contour_2f;

typedef std::vector<cg::point_2> contour;

struct minkowski_viewer : cg::visualization::viewer_adapter
{
	minkowski_viewer()
	{
		in_building_ = true;
		center_moving = false;
	}

	void draw(cg::visualization::drawer_type & drawer) const
	{
		 for (size_t idx = 0; idx < cur_contour; idx++)
		 {
			  contour const & cont = conts[idx];
			  drawer.set_color(Qt::red);
			  if (cg::counterclockwise(cont))
				  drawer.set_color(Qt::green);

			  for (size_t i = 0; i < cont.size(); i++)
			  {
				  size_t j = (i + 1) % cont.size();
				  cg::point_2 p1 = cont[i], p2 = cont[j];
				  drawer.draw_line(p1, p2);
			  }
		 }
		if (in_building_)
		{
			drawer.set_color(Qt::white);

			for (size_t i = 1; i < conts[cur_contour].size(); ++i)
			{
				drawer.draw_line(conts[cur_contour][i - 1], conts[cur_contour][i]);
			}
		} else
		{
			 drawer.set_color(Qt::blue);
			 drawer.draw_point(center, 3);
			 auto result_cont = cg::get_minkovsky_sum(contour_2(conts[0]), contour_2(reversed));
			 contour result;
			 for (auto it = result_cont.begin(); it != result_cont.end(); it++) {
				 result.push_back(*it);
			 }

			 for (size_t i = 0; i < result.size(); ++i)
			 {
				  size_t j = (i + 1) % result.size();
				 drawer.draw_line(result[i], result[j]);
			 }
		}
	}

	void print(cg::visualization::printer_type & p) const
	{
		p.corner_stream() << "double-click to clear." << cg::visualization::endl
								<< "press mouse rbutton for add vertex (click to first point to complete contour)" << cg::visualization::endl
								<< "move vertex with rbutton" << cg::visualization::endl;
	}

	bool on_double_click(const point_2f & p)
	{
	  conts[0].clear();
	  conts[1].clear();
	  cur_contour = 0;
	  in_building_ = true;
	  current_vertex_.reset();
	  return true;
	}

	bool on_press(const point_2f & p)
	{
		if (in_building_)
		{
			contour & cont = conts[cur_contour];
			if (cont.size() > 1)
			{
				if (fabs(cont[0].x - p.x) < 15 && fabs(cont[0].y - p.y) < 15)
				{
					cur_contour++;
					if (cur_contour == 2)
					{
						 center = cg::point_2(0, 0);
						 for (cg::point_2 & p : conts[1]) center += p;
						 center = cg::point_2(center.x / conts[1].size(), center.y / conts[1].size());

						 reversed.clear();
						 for (cg::point_2 & p : conts[1]) reversed.push_back(cg::point_2(-(p.x - center.x), -(p.y - center.y)));
						 if (!cg::counterclockwise(reversed)) std::reverse(reversed.begin(), reversed.end());
						 in_building_ = false;
					}
					return true;
				}
			}

			cont.push_back(p);
			return true;
		}

		if (fabs(center.x - p.x) < 35 && fabs(center.y - p.y) < 35)
		{
			 center_moving = true;
			 return true;
		}

		for (contour & cont : conts)
		{
			 for (contour::iterator it = cont.begin(); it != cont.end(); ++it)
			 {
				  if (fabs(it->x - p.x) < 15 && fabs(it->y - p.y) < 15)
				  {
					  current_vertex_ = it;
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

		if (center_moving)
		{
			 for (cg::point_2 & q : conts[1]) q = cg::point_2(q.x + p.x - center.x, q.y + p.y - center.y);
			 center = p;
			 center_moving = false;
			 return true;
		}

		current_vertex_.reset();

		return true;
	}

	bool on_move(const point_2f & p)
	{
		if (in_building_)
		{
			return true;
		}


		if (center_moving)
		{
			 for (cg::point_2 & q : conts[1]) q = cg::point_2(q.x + p.x - center.x, q.y + p.y - center.y);
			 center = p;
			 return true;
		}

		if (current_vertex_)
		{
			 *(*current_vertex_) = p;
		}

		return true;
	}

private:
	bool in_building_;
	bool center_moving;
	static size_t const count_contours = 2;
	size_t cur_contour = 0;
	contour conts[count_contours];
	contour reversed;
	cg::point_2 center;
	boost::optional<contour::iterator> current_vertex_;
};

int main(int argc, char ** argv)
{
	QApplication app(argc, argv);
	minkowski_viewer viewer;
	cg::visualization::run_viewer(&viewer, "minkowski viewer");
}
