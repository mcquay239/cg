#pragma once

#include <algorithm>

namespace util
{
   template <class T>
   void sort2(T & a, T & b)
   {
      if (a > b) std::swap(a, b);
   }
}
