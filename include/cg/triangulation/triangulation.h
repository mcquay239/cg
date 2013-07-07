#pragma once

#include <algorithm>
#include <vector>
#include <cg/primitives/point.h>
#include <cg/primitives/contour.h>
#include <cg/primitives/triangle.h>

#include <iostream> // for testing

namespace cg {
    std::vector<triangle_2> triangulate(std::vector<contour_2> polygon) {
        typedef contour_2::circulator_t circulator;

        std::vector<circulator> p;
        for (contour_2 c : polygon) {
            auto start = c.circulator();
            auto cur = start;
            do {
                p.push_back(cur++);
            } while (cur != start);
        }
        std::sort(p.begin(), p.end(), [](const circulator &c1, const circulator &c2) {
                    if (c1->y != c2->y) return c1->y > c2->y;
                    return c1->x < c2->x;
                });
        std::cout << "sorted" << std::endl;
        return std::vector<triangle_2>(1);
    }
}
