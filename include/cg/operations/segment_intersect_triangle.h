#pragma once

#include <cg/primitives/segment.h>
#include <cg/primitives/triangle.h>
#include <cg/operations/orientation.h>
#include <cg/operations/point_in_triangle.h>
#include <cg/operations/segment_intersect.h>

namespace cg {

    template <typename T>
    bool segment_intersect_triangle(segment_2t<T> seg1, triangle_2t<T> tr) {
        if (point_in_triangle(seg1[0], tr) || point_in_triangle(seg1[1], tr)) return true;
        for (int i = 0; i < 3; i++) {
            segment_2t<T> seg2 = segment_2t<T>(tr[i], tr[(i + 1) % 3]);
            if (segments_intersected(seg1, seg2)) {
                return true;
            }
        }
        return false;
    }
}
