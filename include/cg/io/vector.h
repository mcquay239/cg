#pragma once

#include "io_utils.h"
#include "cg/primitives/vector.h"

namespace cg
{
   template <class Scalar>
   std::istream & operator >> (std::istream & in, vector_2t<Scalar> & v)
   {
      using io::skip_char;

      return skip_char(skip_char(skip_char(in, '(') >> v.x, ',') >> v.y, ')');
   }

   template <class Scalar>
   std::ostream & operator << (std::ostream & out, vector_2t<Scalar> const & v)
   {
      out << "(" << v.x << ", " << v.y << ")";
      return out;
   }
}
