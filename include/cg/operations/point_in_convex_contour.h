#pragma once

#include <cg/primitives/segment.h>
#include <cg/primitives/contour.h>
#include <cg/operations/orientation.h>

namespace cg {
    template <typename T>
    bool point_in_convex_contour(point_2t<T> p, contour_2t<T> cont) {
       if (orientation(cont[0], cont[1], p) == CG_RIGHT) return false;
       auto it = std::lower_bound(cont.begin() + 2, cont.end(), p, [&cont](const point_2t<T> &lhs, const point_2t<T> &rhs) {
         return orientation(cont[0], lhs, rhs) == CG_LEFT;
       });
    if (it == cont.end()) {
        return false;
    }
    return orientation(*(it - 1), *it, p) != CG_RIGHT;
     }
}
