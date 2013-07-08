#pragma once

#include <algorithm>
#include <vector>
#include <map>
#include <cg/primitives/point.h>
#include <cg/primitives/contour.h>
#include <cg/primitives/triangle.h>
#include <cg/primitives/segment.h>
#include <cg/operations/orientation.h>

#include <iostream> // for testing

namespace cg {
    enum v_type {SPLIT, MERGE, LEFT_REGULAR, RIGHT_REGULAR, START, END};

    v_type vertex_type(contour_2::circulator_t c) {
        auto cur = *c;
        c--;
        auto prev = *c;
        c++;
        c++;
        auto next = *c;

        bool right = orientation(prev, cur, next) == CG_RIGHT;
        if (cur > prev && cur > next) return right ? SPLIT : START;
        if (cur < prev && cur < next) return right ? MERGE : END;
        return next > cur ? RIGHT_REGULAR : LEFT_REGULAR;
    }

    std::vector<triangle_2> triangulate(std::vector<contour_2> polygon) {
        typedef contour_2::circulator_t circulator;

        std::vector<circulator> p;
        for (contour_2 &c : polygon) {
            auto start = c.circulator();
            auto cur = start;
            do {
                p.push_back(cur++);
            } while (cur != start);
        }
        std::sort(p.begin(), p.end(), [](const circulator &c1, const circulator &c2) {
                    if (c1->x != c2->x) return c1->x > c2->x;
                    return c1->y > c2->y;
                });
        std::cout << "sorted" << std::endl;
        for (auto c : p) {
            std::cout << c->x << " " << c->y << " "  << " type: " << vertex_type(c) << std::endl;
        }

        auto segment_comp = [](const segment_2 &s1, const segment_2 &s2) {
                    auto slice = std::min(s1[0].x, s2[0].x);
                    auto y1 = s1[0].y + (s1[0].x - slice) / (s1[0].x - s1[1].x) * (s1[1].y - s1[0].y);
                    auto y2 = s2[0].y + (s2[0].x - slice) / (s2[0].x - s2[1].x) * (s2[1].y - s2[0].y); // what if /0 ??
                    if (std::abs(y1 - y2) > 1e-8) return y1 < y2;
                    if (s1[0] != s2[0]) return s1[0] < s2[0];
                    return s1[1] < s2[1];
                };
        std::map<segment_2, point_2, decltype(segment_comp)> helper(segment_comp);
        for (auto c : p) {
            switch (vertex_type(c)) {
                case SPLIT:
                    helper[segment_2(*c, *(c - 1))] = *c;
                    break;
                case MERGE:
                    helper.erase(segment_2(*(c + 1), *c));
                    break;
                case LEFT_REGULAR:
                    break;
                case RIGHT_REGULAR:
                    helper.erase(segment_2(*(c + 1), *c));
                    helper[segment_2(*c, *(c - 1))] = *c;
                    break;
                case START:
                    helper[segment_2(*c, *(c - 1))] = *c;
                    break;
                case END:
                    helper.erase(segment_2(*(c + 1), *c));
                    break;
            }
        }
        return std::vector<triangle_2>(1);
    }

}
