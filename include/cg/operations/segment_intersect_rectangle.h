#pragma once

#include <cg/primitives/segment.h>
#include <cg/primitives/rectangle.h>
#include <cg/operations/orientation.h>
#include <cg/operations/point_in_triangle.h>
#include <cg/operations/segment_intersect.h>

namespace cg {

    template <typename T>
    bool segment_intersect_rectangle(segment_2t<T> seg1, rectangle_2t<T> rect) {
        if (rect.contains(seg1[0]) || rect.contains(seg1[1])) return true;
        for (int i = 0; i < 4; i++) {
            segment_2t<T> seg2(rect.p[i], rect.p[(i + 1) % 4]);
            if (segments_intersected(seg1, seg2)) return true;
        }
        return false;
    }
}
