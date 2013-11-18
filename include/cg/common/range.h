#pragma once

#include <iterator>
#include <boost/range/iterator.hpp>

namespace cg {
namespace common
{
   template<typename BidirectionalRange>
   struct range_circulator
   {
      typedef typename boost::range_const_iterator<BidirectionalRange>::type  iterator_type;
      typedef typename iterator_type::value_type                              value_type;

      explicit range_circulator(BidirectionalRange const & range)
         : beg_(range.begin())
         , end_(range.end())
         , it_(range.begin())
      {}

      range_circulator(BidirectionalRange const & range, iterator_type it)
         : beg_(range.begin())
         , end_(range.end())
         , it_(it)
      {}

      value_type const & operator *  () const { return *it_;  }
      value_type const * operator -> () const { return &*it_; }

      iterator_type iter() const { return it_; }

      range_circulator & operator ++ ()
      {
         ++it_;
         if (it_ == end_)
            it_ = beg_;
         return *this;
      }

      const range_circulator operator ++ (int)
      {
         range_circulator tmp = *this;
         ++(*this);
         return tmp;
      }

      range_circulator & operator -- ()
      {
         if (it_ == beg_)
            it_ = end_;
         --it_;
         return *this;
      }

      const range_circulator operator -- (int)
      {
         range_circulator tmp = *this;
         --(*this);
         return tmp;
      }

      range_circulator operator+ (int x) const {
         range_circulator res(*this);
         for (int i = 0; i < x; i++) ++res;
         return res;
      }

      range_circulator operator- (int x) const {
         range_circulator res(*this);
         for (int i = 0; i < x; i++) --res;
         return res;
      }

      template<typename Range>
      friend bool operator == (   range_circulator<Range> const & a,
                                  range_circulator<Range> const & b  );

   private:
      iterator_type beg_, end_, it_;
   };

   template<typename Range>
   bool operator == (  range_circulator<Range> const & a,
                       range_circulator<Range> const & b  )
   {
      assert(a.beg_ == b.beg_);
      assert(a.end_ == b.end_);

      return a.it_ == b.it_;
   }

   template<typename Range>
   bool operator != (  range_circulator<Range> const & a,
                       range_circulator<Range> const & b  )
   {
      return !(a == b);
   }
}}
