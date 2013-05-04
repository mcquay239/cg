#pragma once

#include "cg/primitives/rectangle.h"
#include "range.h"

namespace cg
{
   template <class Scalar>
   std::istream & operator >> (std::istream & in, rectangle_2t<Scalar> & rect)
   {
      using io::skip_char;

      return skip_char(skip_char(skip_char(in, '(') >> rect.x, ',') >> rect.y, ')');
   }

   template <class Scalar>
   std::ostream & operator << (std::ostream & out, rectangle_2t<Scalar> const & rect)
   {
      out << "(" << rect.x << ", " << rect.y << ")";
      return out;
   }
}
