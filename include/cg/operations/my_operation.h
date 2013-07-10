#pragma once

#include "cg/primitives/point.h"
#include <boost/numeric/interval.hpp>
#include <gmpxx.h>

#include <boost/optional.hpp>

namespace cg
{
    point_2 my_operation(point_2 &a, point_2 &b) {
        return point_2(a.x + b.x, a.y + b.y);
    }
}
