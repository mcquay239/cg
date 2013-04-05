#pragma once

#include "io_utils.h"
#include "cg/primitives/range.h"

namespace cg
{
   template <class Scalar>
   std::istream & operator >> (std::istream & in, range_t<Scalar> & r)
   {
      using io::skip_char;

      return skip_char(skip_char(skip_char(in, '[') >> r.inf, ',') >> r.sup, ']');
   }

   template <class Scalar>
   std::ostream & operator << (std::ostream & out, range_t<Scalar> const & r)
   {
      out << "[" << r.inf << ", " << r.sup << "]";
      return out;
   }
}
