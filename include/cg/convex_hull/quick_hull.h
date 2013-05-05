#pragma once

#include <cg/primitives/point.h>
#include <cg/primitives/vector.h>
#include <cg/operations/orientation.h>
#include <algorithm>
#include <utility>
#include <functional>

namespace cg
{
    template <class BidIter>
    BidIter swap_ranges(BidIter first1, BidIter second1, BidIter first2)
    {
        while (first1 != second1)
        {
            std::iter_swap(first1++, first2++);
        }
        return first2;
    }

    struct pred_d
    {
       boost::optional<orientation_t> operator() (point_2 const & a, point_2 const & b, point_2 const & c, point_2 const & d) const
       {
          double l = (d.x - c.x) * (b.y - a.y);
          double r = (d.y - c.y) * (b.x - a.x);
          double res = l - r;
          double eps = (fabs(l) + fabs(r)) * 8 * std::numeric_limits<double>::epsilon();

          if (res > eps)
             return CG_LEFT;

          if (res < -eps)
             return CG_RIGHT;

          return boost::none;
       }
    };

    struct pred_i
    {
       boost::optional<orientation_t> operator() (point_2 const & a, point_2 const & b, point_2 const & c, point_2 const & d) const
       {
          typedef boost::numeric::interval_lib::unprotect<boost::numeric::interval<double> >::type interval;

          boost::numeric::interval<double>::traits_type::rounding _;
          interval res =   (interval(d.x) - c.x) * (interval(b.y) - a.y)
                         - (interval(d.y) - c.y) * (interval(b.x) - a.x);

          if (res.lower() > 0)
             return CG_LEFT;

          if (res.upper() < 0)
             return CG_RIGHT;

          if (res.upper() == res.lower())
             return CG_COLLINEAR;

          return boost::none;
       }
    };

    struct pred_r
    {
       boost::optional<orientation_t> operator() (point_2 const & a, point_2 const & b, point_2 const & c, point_2 const & d) const
       {
          mpq_class res =   (mpq_class(d.x) - c.x) * (mpq_class(b.y) - a.y)
                          - (mpq_class(d.y) - c.y) * (mpq_class(b.x) - a.x);

          int cres = cmp(res, 0);

          if (cres > 0)
             return CG_LEFT;

          if (cres < 0)
             return CG_RIGHT;

          return CG_COLLINEAR;
       }
    };

    inline orientation_t pred(point_2 const & a, point_2 const & b, point_2 const & c, point_2 const & d)
    {
       if (boost::optional<orientation_t> v = pred_d()(a, b, c, d))
          return *v;

       if (boost::optional<orientation_t> v = pred_i()(a, b, c, d))
          return *v;

       return *pred_r()(a, b, c, d);
    }

    template <class RanIter>
    RanIter build_part(RanIter begin, RanIter end, point_2 const &last_point)
    {
        if (begin + 1 == end)
        {
            return end;
        }

        RanIter highest_point_iter = std::max_element(begin, end, [begin, &last_point](point_2 const &largest, point_2 const &first)
        {
                return pred(largest, first, *begin, last_point) == CG_RIGHT;
        });

        point_2 highest_point = *highest_point_iter;

        if (orientation(*begin, last_point, highest_point) == CG_COLLINEAR)
        {
            return begin + 1;
        }
        std::iter_swap(begin + 1, highest_point_iter);

        RanIter first = std::partition(begin + 2, end, [begin, &highest_point](point_2 const &point)
        {
            return orientation(*begin, highest_point, point) == CG_RIGHT;
        });

        RanIter second = std::partition(first, end, [&highest_point, &last_point](point_2 const &point)
        {
            return orientation(highest_point, last_point, point) == CG_RIGHT;
        });

        std::iter_swap(begin + 1, first - 1);

        RanIter first_end = build_part(begin, first - 1, highest_point);
        RanIter second_end = build_part(first - 1, second, last_point);
        return swap_ranges(first - 1, second_end, first_end);
    }

    template <class RanIter>
    RanIter quick_hull(RanIter begin, RanIter end)
    {
        if (begin == end)
        {
            return end;
        }

        std::iter_swap(begin, std::min_element(begin, end));
        std::iter_swap(end - 1, std::max_element(begin, end));

        if (*begin == *(end - 1))
        {
            return ++begin;
        }

        RanIter bound = std::partition(begin + 1, end - 1, [begin, end](point_2 const &a)
        {
            return orientation(*begin, *(end - 1), a) == CG_RIGHT;
        });

        std::iter_swap(end - 1, bound);
        RanIter first = build_part(begin, bound, *bound);
        RanIter second = build_part(bound, end, *begin);
        return swap_ranges(bound, second, first);
    }
}
