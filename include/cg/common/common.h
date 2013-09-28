#pragma once

namespace cg {
    template <typename T>
    bool make_min(T & to_min, T min)
    {
        if (min < to_min)
        {
            to_min = min;
            return true;
        }
        return false;
    }

    template <typename T>
    bool make_max(T & to_max, T max)
    {
        if (max > to_max)
        {
            to_max = max;
            return true;
        }
        return false;
    }
}
