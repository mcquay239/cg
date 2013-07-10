#pragma once

#include <cg/primitives/segment.h>
#include <cg/primitives/contour.h>
#include <cg/operations/orientation.h>

namespace cg {
    enum poly_orientation {
        CLOCKWISE = -1, CONTERCLOCKWISE = 1
    };

    template <typename T>
    poly_orientation polygon_orientation(contour_2t<T> p) {
        typedef typename std::vector<point_2t<T> >::const_iterator const_iterator;
        const_iterator min = std::min_element(p.begin(), p.end());
        const_iterator prev;
        if (min == p.begin()) {
            prev = p.end() - 1;
        } else {
            prev = min - 1;
        }
    }
}
