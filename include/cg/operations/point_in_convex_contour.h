#pragma once

#include <cg/primitives/segment.h>
#include <cg/primitives/contour.h>
#include <cg/operations/orientation.h>

namespace cg {
    template <typename T>
    bool point_in_convex_contour(point_2t<T> p, contour_2t<T> cont) {
        if (orientation(cont[0], cont[1], p) == CG_RIGHT || orientation(cont[0], cont[cont.vertices_num() - 1], p) == CG_LEFT) {
            return false;
        }
        typedef typename std::vector<point_2t<T> >::const_iterator const_iterator;

        const_iterator it = std::lower_bound(cont.begin(), cont.end(), p, [&cont](const point_2t<T> &lhs, const point_2t<T> &rhs) {
                         return orientation(cont[0], lhs, rhs) != CG_RIGHT;
        });

        if (it == cont.end()) {
            it--;
        }
        point_2t<T> second = (*it);
        point_2t<T> first = (*(--it));

        return orientation(first, second, p) != CG_RIGHT;
     }
}
