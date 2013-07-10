#pragma once

#include <cg/primitives/contour.h>
#include <cg/operations/orientation.h>
#include <cg/operations/point_in_triangle.h>
#include <cg/operations/segment_intersect.h>

namespace cg {

    template <typename T>
    bool point_in_contour(point_2t<T> p, contour_2t<T> cont) {
        int count_intersection = 0;
        T max_x = (std::max_element(cont.begin(), cont.end(), [&cont](const point_2t<T> &lhs, const point_2t<T> &rhs) {
                         return lhs.x < rhs.x; }
        ))->x;
        segment_2t<T> seg(p, point_2t<T>(max_x * 2, p.y));
        for (int i = 0; i < cont.vertices_num(); i++) {
            point_2t<T> st(cont[i]), en(cont[(i + 1) % cont.vertices_num()]);
            segment_2t<T> cur_seg(st, en);
            if (segments_intersected(cur_seg, segment_2t<T>(p, p))) {
                return true;
            }
            if (st.y != en.y) {
                if (p.y > std::min(st.y, en.y) && p.y <= std::max(st.y, en.y) && segments_intersected(seg, cur_seg)) {
                    count_intersection++;
                }
            }
        }
        return count_intersection % 2;
    }
}
