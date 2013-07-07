#pragma once

#include <algorithm>
#include <vector>
#include <cg/primitives/point.h>
#include <cg/primitives/contour.h>
#include <cg/primitives/triangle.h>
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
        return std::vector<triangle_2>(1);
    }

}
