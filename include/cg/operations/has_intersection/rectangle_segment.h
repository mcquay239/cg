#pragma once

#include <cg/primitives/rectangle.h>
#include <cg/primitives/segment.h>

#include <cg/operations/has_intersection/segment_segment.h>

namespace cg
{
	template<class Scalar>
	bool has_intersection(rectangle_2t<Scalar> const& r, segment_2t<Scalar> const& s)
	{
		if (r.contains(s[0]) || r.contains(s[1]))
			return true;
		point_2t<Scalar> max_point = s[0];
		point_2t<Scalar> min_point = s[1];
		if (max_point.y < min_point.y)
			std::swap(min_point, max_point);
		if (min_point.x > max_point.x)
			return has_intersection(segment_2t<Scalar>(r.corner(0, 0), r.corner(1, 1)), s);
		return has_intersection(segment_2t<Scalar>(r.corner(1, 0), r.corner(0, 1)), s);
	}
}
