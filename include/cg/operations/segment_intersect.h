#pragma once

#include <cg/primitives/segment.h>
#include <cg/operations/orientation.h>

namespace cg {

    template <typename T>
    bool segments_intersected(segment_2t<T> seg1, segment_2t<T> seg2) {
        orientation_t resor1 = orientation(seg1[0], seg1[1], seg2[0]);
        orientation_t resor2 = orientation(seg1[0], seg1[1], seg2[1]);
        if (resor1 == CG_COLLINEAR && resor2 == CG_COLLINEAR) {
            return (min(seg2) <= max(seg1) && max(seg2) >= min(seg1));
        } else if (resor1 == resor2) {
            return false;
        } else {
            return orientation(seg2[0], seg2[1], seg1[0]) != orientation(seg2[0], seg2[1], seg1[1]);
        }
    }
}
