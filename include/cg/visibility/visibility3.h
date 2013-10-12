#pragma once

#include <cg/operations/orientation.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <cg/primitives/segment.h>
#include <cg/primitives/point.h>
#include <cg/primitives/contour.h>
#include <cg/operations/has_intersection/segment_segment.h>

namespace cg
{

	template <class Scalar>
	bool more_than_pi(point_2t<Scalar> const & a, point_2t<Scalar> const & b, point_2t<Scalar> const & c) {
		return orientation(a, b, c) != CG_LEFT;
	}

	template <class Scalar>
	bool not_intersect(point_2t<Scalar> const & a, point_2t<Scalar> const & b, std::vector<contour_2t<Scalar> > const & polygons) {
		typedef contour_2t<Scalar> Countour;
		typedef segment_2t<Scalar> Segment;

		for (Countour countor : polygons) {
			for (auto it_p = countor.begin(); it_p != countor.end(); it_p++) {
				auto it_next = (it_p + 1) == countor.end() ? (countor.begin()) : (it_p + 1);
				if (a == *it_next || b == *it_next || a == *it_p || b == *it_p) continue;
				if (has_intersection(Segment(a, b), Segment(*it_p, *it_next))) {
					return false;
				}
			}
		}
		return true;
	}

	template <class Scalar>
	void append_visible_points(point_2t<Scalar> const & finish_point, typename std::vector<contour_2t<Scalar> >::iterator contour_of_finish_point,
									std::vector<contour_2t<Scalar> > const & polygons, std::back_insert_iterator<std::vector<segment_2t<Scalar> > > out) {
		typedef contour_2t<Scalar> Countour;

		for (auto cur_polygon_it = polygons.begin(); cur_polygon_it != polygons.end(); cur_polygon_it++) {
			Countour cur_polygon = *cur_polygon_it;
			for (auto candidate_point_it = cur_polygon.begin(); candidate_point_it != cur_polygon.end(); candidate_point_it++) {
				if (*candidate_point_it == finish_point) {
					continue;
				}
				bool ok = not_intersect(finish_point, *candidate_point_it, polygons);

				if (contour_of_finish_point == cur_polygon_it) {
					auto left = candidate_point_it == cur_polygon.begin() ? (cur_polygon.end() - 1) : (candidate_point_it - 1);
					auto right = candidate_point_it == cur_polygon.end() - 1 ? (cur_polygon.begin()) : (candidate_point_it + 1);

					auto first_rotate = orientation(*left, *candidate_point_it, finish_point), second_rotate = orientation(*candidate_point_it, *right, finish_point);

					if (!more_than_pi(*left, *candidate_point_it, *right)) {
						ok &= first_rotate != CG_LEFT || second_rotate != CG_LEFT;
					} else {
						ok &= first_rotate != CG_LEFT && second_rotate != CG_LEFT;
					}
				}

				if (ok && (contour_of_finish_point == polygons.end() || finish_point < *candidate_point_it)) {
					(*out) = segment_2t<Scalar>(finish_point, *candidate_point_it);
					out++;
				}
			}

		}
	}

	template <class Scalar>
	std::vector<segment_2t<Scalar> > get_visibility_graph(point_2t<Scalar> const & start, point_2t<Scalar> const & end, std::vector<contour_2t<Scalar> > & polygons) {
		typedef segment_2t<Scalar> Segment;
		std::vector<Segment> ans;

		append_visible_points(start, polygons.end(), polygons, std::back_inserter(ans));
		for (auto it_poly = polygons.begin(); it_poly != polygons.end(); it_poly++) {
			auto cur_polygon = *it_poly;
			for (auto it = cur_polygon.begin(); it != cur_polygon.end(); it++) {
				append_visible_points(*it, it_poly, polygons, std::back_inserter(ans));
			}
		}
		append_visible_points(end, polygons.end(), polygons, std::back_inserter(ans));
		if (not_intersect(start, end, polygons)) {
			ans.push_back(Segment(start, end));
		}
		return rebuild_ans(ans, start, end, polygons);
	}

	template <class Scalar>
	std::vector<segment_2t<Scalar> > rebuild_ans(std::vector<segment_2t<Scalar> > const & prev_ans, point_2t<Scalar> const & start,
																point_2t<Scalar> const & end, std::vector<contour_2t<Scalar> > const & polygons)  {
		typedef contour_2t<Scalar> Countour;
		typedef segment_2t<Scalar> Segment;
		std::vector<Segment> ans;

		for (Segment cur_segment : prev_ans) {
			bool ok = true;
			for (Countour countor : polygons) {
				for (auto it_p = countor.begin(); it_p != countor.end(); it_p++) {
					for (int i = 0; i < 2; i++) {
						if (*it_p != cur_segment[i]) continue;

						auto it_prev = it_p == countor.begin() ? (countor.end() - 1) : (it_p - 1);
						auto it_next = it_p == countor.end() - 1 ? (countor.begin()) : (it_p + 1);
						if (cur_segment[1 - i] == *it_prev || cur_segment[1 - i] == *it_next) continue;

						auto first_rotate = orientation(*it_prev, *it_p, cur_segment[1 - i]), second_rotate = orientation(cur_segment[1 - i], *it_p, *it_next);
						if (first_rotate == CG_RIGHT && second_rotate == CG_RIGHT) {
							ok = false;
						}
					}

				}
			}
			if (ok) {
				ans.push_back(cur_segment);
			}
		}
		return ans;
	}
}
