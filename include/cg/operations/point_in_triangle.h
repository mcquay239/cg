#pragma once

#include <cg/primitives/segment.h>
#include <cg/primitives/triangle.h>
#include <cg/operations/orientation.h>

namespace cg {

    template <typename T>
    bool point_in_triangle(point_2t<T> const & p, triangle_2t<T> const & t) {
        return orientation(t[0], t[1], p) != CG_RIGHT && orientation(t[1], t[2], p) != CG_RIGHT && orientation(t[2], t[0], p) != CG_RIGHT;
    }
}
