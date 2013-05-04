#pragma once

#include <boost/random.hpp>
#include <cg/primitives/point.h>
#include <misc/random_utils.h>

inline std::vector<cg::point_2> uniform_points(size_t count)
{
    util::uniform_random_real<double> rand(-100., 100.);

    std::vector<cg::point_2> res(count);

    for (size_t l = 0; l != count; ++l)
    {
        rand >> res[l].x;
        rand >> res[l].y;
    }

    return res;
}
