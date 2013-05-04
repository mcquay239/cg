#pragma once

#include "cg/primitives/triangle.h"
#include "point.h"

namespace cg
{
   template <class Scalar>
   std::istream & operator >> (std::istream & in, triangle_2t<Scalar> & tr)
   {
      using io::skip_char;

      return skip_char(skip_char(skip_char(skip_char(in, '(') >> tr[0], ',') >> tr[1], ',') >> tr[2], ')');
   }

   template <class Scalar>
   std::ostream & operator << (std::ostream & out, rectangle_2t<Scalar> const & tr)
   {
      out << "(" << tr[0] << ", " << tr[1] << ", " << tr[2] << ")";
      return out;
   }
}
