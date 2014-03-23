#pragma once

#include "misc/Assert.h"

#include <list>

#include <boost/optional.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <boost/foreach.hpp>
#include <boost/random.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/chrono.hpp>

namespace cg
{
   namespace details
   {
      inline float infinite(float *)
      {
         return std::numeric_limits<float>::infinity();
      }

      inline size_t infinite(size_t *)
      {
         return std::numeric_limits<size_t>::max();
      }

   }

   struct profiling_t
   {
      profiling_t()
         : nodes(0)
         , overhead(0)
         , localization(0)
         , insertion(0)
      {}

      size_t nodes;
      size_t overhead;
      size_t localization;
      size_t insertion;
   };


   template <class Data, class Time>
   struct persistent_set_t
   {
      typedef Time time_t;
      typedef Data data_t;

      persistent_set_t()
         : roots_(1, node_ptr(new node_t(boost::none, 0, profiling_)))
         , roots_size_(1)
         , coin_(0.9)
      {}

      ~persistent_set_t()
      {
         roots_.back()->clean();

         roots_.clear();
         Verify(profiling_.nodes == 0);
      }

      std::list<data_t> slice(time_t t) const
      {
         std::list<data_t> res;
         node_ptr r = roots_.front()->instance(t)->next(t);
         while (r)
         {
            res.push_back(*(r->data()));
            r = r->next(t);
         }

         return res;
      }

      size_t levels() const { return roots_.size(); }

      void insert(data_t const & d, time_t const t, time_t const t2)
      {
         std::list<boost::tuple<node_ptr, node_ptr> > loc;
         BOOST_FOREACH(node_ptr root, roots_ | boost::adaptors::reversed)
         {
            ++profiling_.localization;

            node_ptr r, rn;
            node_ptr rt = (loc.empty() ? root : boost::get<0>(loc.front())->down());

            Verify(rt);

            boost::tie(r, rn) = lower_bound_impl(rt, d, t);

            loc.push_front(boost::make_tuple(r, rn));

            Verify(r);
         }

         node_ptr r, rn;
         node_ptr down;
         typedef boost::tuple<node_ptr, node_ptr> rrn_t;
         BOOST_FOREACH(rrn_t rrn, loc)
         {
            ++profiling_.insertion;

            boost::tie(r, rn) = rrn;

            node_ptr new_node(new node_t(d, t, profiling_, rn));
            new_node->set_down(down);
            down = new_node;

            time_t t1 = t;
            do
            {
               ++profiling_.insertion;

               new_node->set_next(t1, rn ? rn->instance(t1) : node_ptr());
               new_node = new_node->instance(t1);

               r->set_next(t1, new_node);

               t1 = r->next_event(t1);

               if (t1 > t2)
                  break;

               r = r->instance(t1);

               if (rn)
               {
                  rn = rn->last(t1);
                  if (rn->ends(t1))
                     rn->update_end(new_node, t1);
               }

               Verify(r);
               Verify(!r->data() || *(r->data()) < d);

               rn = next(r, t1);
               if (rn && *(rn->data()) < d)
               {
                  r = rn;
                  rn = next(r, t1);
               }

               Verify(!rn || *(rn->data()) > d);
            }
            while (t1 < t2);

            Verify(r);

            new_node->update_end(r, t2);

            r->set_next(t2, rn);

            if (coin_(eng_))
            {
               down = node_ptr();
               break;
            }
         }

         if (down && !coin_(eng_))
         {
            ++profiling_.insertion;

            node_ptr new_node(new node_t(d, t, profiling_));
            new_node->set_down(down);

            Verify(!roots_.empty());

            node_ptr proot = roots_.back();
            roots_.push_back(new node_t(boost::none, 0, profiling_));
            roots_.back()->set_down(proot);
            roots_.back()->set_next(t, new_node);
            roots_.back()->set_next(t2, node_ptr());

            ++roots_size_;
         }

      }

   private:
      struct node_t;
      typedef boost::intrusive_ptr<node_t> node_ptr;

   private:
      static node_ptr next(node_ptr r, time_t t)
      {
         r = r->instance(t);
         if (node_ptr res = r->next(t))
         {
            res = res->instance(t);
            r->set_next(t, res);

            return res;
         }

         return node_ptr();
      }

   public:
      profiling_t const & profiling() const { return profiling_; }

   protected:
      static time_t infinite_time()
      {
         return details::infinite((time_t *)0);
      }

   private:
      struct node_t : boost::intrusive_ref_counter<node_t, boost::thread_unsafe_counter>
      {
         node_t(boost::optional<data_t> const & data, time_t init_time, profiling_t & profiling, node_ptr next = node_ptr())
            : data_initialized_(data)
            , init_time_(init_time)
            , t_(infinite_time())
            , next_event_time_(infinite_time())
            , profiling_(profiling)
         {
            if (data_initialized_)
               data_ = *data;

            ++profiling.nodes;
            next_[0] = next;
         }

         ~node_t()
         {
            --profiling_.nodes;
         }

         void clean()
         {
            std::list<node_ptr> s(1, node_ptr(this));
            while (!s.empty())
            {
               node_ptr t = s.back();
               s.pop_back();

               for (size_t l = 0; l != 2; ++l)
               {
                  if (t->next_[l])
                  {
                     s.push_back(t->next_[l]);
                     t->next_[l] = node_ptr();
                  }
               }

               if (t->next_event_)
               {
                  s.push_back(t->next_event_);
                  t->next_event_ = node_ptr();
               }

               if (t->down_)
               {
                  s.push_back(t->down_);
                  t->down_ = node_ptr();
               }
            }
         }

         node_ptr down() const
         {
            return down_;
         }

         node_ptr last(time_t t)
         {
            ++profiling_.overhead;

            node_ptr res(this);
            while (res->next_event_time_ != infinite_time() && res->next_event_time_ < t
                   && res->data() == res->next_event_->data())
            {
               Verify(res->t_ == infinite_time() || res->next_[0] != res->next_[1]);

               update_next(res->init_time_, res->next_[0]);
               update_next(res->t_, res->next_[1]);
               update_next(res->init_time_, res->down_);

               if (res->t_ != infinite_time() && res->next_[0] == res->next_[1])
               {
                  res->t_ = infinite_time();
                  res->next_[1] = node_ptr();
               }

               ++profiling_.overhead;

               Verify(res->t_ == infinite_time() || res->next_[0] != res->next_[1]);

               res = res->next_event_;

               Verify(res);
            }

            return res;
         }

         bool ends(time_t t) const
         {
            return next_event_time_ == t && next_event_->data() != data();
         }

         void update_end(node_ptr next_event, time_t t)
         {
            Verify(next_event_time_ == infinite_time()
                   || next_event_time_ == t);
            Verify(t_ == infinite_time() || t_ < t);
            Verify(next_event);
            Verify(!next_event_ || !next_event_->data()
                   || (*(next_event_->data()) < *(next_event->data())
                       && *data() >= *(next_event_->data())));
            next_event_time_ = t;
            next_event_ = next_event;
         }

         node_ptr instance(time_t t)
         {
            ++profiling_.overhead;

            node_ptr res(this);
            while (res->next_event_time_ != infinite_time() && res->next_event_time_ <= t)
            {
               Verify(res->t_ == infinite_time() || res->next_[0] != res->next_[1]);

               update_next(res->init_time_, res->next_[0]);
               update_next(res->t_, res->next_[1]);
               update_next(res->init_time_, res->down_);

               if (res->t_ != infinite_time() && res->next_[0] == res->next_[1])
               {
                  res->t_ = infinite_time();
                  res->next_[1] = node_ptr();
               }

               ++profiling_.overhead;

               Verify(res->t_ == infinite_time() || res->next_[0] != res->next_[1]);

               if (res->next_event_->init_time_ == res->next_event_->next_event_time_)
               {
                  Verify(res->next_event_->t_ == infinite_time());
                  Verify(res->next_event_->down_ == node_ptr());

                  for (size_t l = 0; l != 2; ++l)
                     Verify(res->next_event_->next_[l] == node_ptr());

                  res->next_event_ = res->next_event_->next_event_;
               }
               else
               {
                  if (res->init_time_ != res->next_event_time_ && res->next_event_->data() == res->data())
                  {
                     if (res->t_ != infinite_time() && res->next_event_->next_[0] == res->next_[1])
                     {
                        Verify(res->t_ != res->init_time_);

                        res->next_event_->init_time_ = res->t_;
                        res->next_event_time_ = res->t_;
                        res->next_event_->set_down(res->down_);
                        res->t_ = infinite_time();
                        res->next_[1] = node_ptr();
                     }
                     else if (res->t_ == infinite_time() && res->next_event_->next_[0] == res->next_[0])
                     {
                        res->next_[0] = node_ptr();
                        res->next_event_time_ = res->init_time_;
                        res->next_event_->init_time_ = res->init_time_;
                        res->next_event_->set_down(res->down_);
                        res->down_ = node_ptr();
                     }
                  }

                  res = res->next_event_;
               }

               Verify(res);
            }

            return res;
         }

         node_ptr next(time_t t)
         {
            node_ptr n = instance(t);
            return t < n->t_ ? n->next_[0] : n->next_[1];
         }

         void set_down(node_ptr down)
         {
            down_ = down;
            update_next(init_time_, down_);
         }

         void set_next(time_t t, node_ptr next)
         {
            node_ptr n = instance(t);

            Verify(n->t_ == infinite_time() || n->next_[0] != n->next_[1]);
            Verify(n->t_ != n->init_time_);
            Verify(n->next_[0] == node_ptr() || n->next_[0] != n->next_[1]);
            Verify(n->t_ == infinite_time() || n->t_ < n->next_event_time_);

            if (n->init_time_ == t)
            {
               n->set_next_impl(0, next);
            }
            else if (n->t_ == t)
            {
               n->set_next_impl(1, next);
            }
            else if (n->t_ == infinite_time())
            {
               n->t_ = t;
               n->set_next_impl(1, next);
            }
            else if (n->t_ > t)
            {
               if (n->next_[1] == next)
               {
                  n->t_ = t;
               }
               else if (n->next_[0] != next)
               {
                  n->insert_next_impl(n->t_, n->next_[1]);
                  n->t_ = t;
                  n->next_[1] = next;
               }
            }
            else if (n->next_[1] != next)
            {
               n->insert_next_impl(t, next);
            }

            Verify(n->t_ == infinite_time() || n->next_[0] != n->next_[1]);
            Verify(n->t_ != n->init_time_);
            Verify(n->next_[0] == node_ptr() || n->next_[0] != n->next_[1]);
            Verify(n->t_ == infinite_time() || n->t_ < n->next_event_time_);
         }

         time_t next_event(time_t t)
         {
            node_ptr n = instance(t);
            if (n->t_ != infinite_time() && n->t_ > t)
               return n->t_;

            return n->next_event_time_;
         }

         boost::optional<data_t> data() const
         {
            if (data_initialized_)
               return data_;

            return boost::none;
         }

      private:
         static void update_next(time_t init_time, node_ptr & next)
         {
            next = (next ? next->instance(init_time) : node_ptr());
         }

         void set_next_impl(size_t i, node_ptr node)
         {
            next_[i] = node;
            if (next_[0] == next_[1])
            {
               next_[1] = node_ptr();
               t_ = infinite_time();
            }
         }

         void insert_next_impl(time_t t, node_ptr next)
         {
            if (!next_event_ || data() != next_event_->data() || next_event_->t_ != infinite_time())
            {
               node_ptr next_event = new node_t(data(), t, profiling_, next);
               next_event->set_down(down_);
               next_event->next_event_ = next_event_;
               next_event->next_event_time_ = next_event_time_;
               next_event_ = next_event;
               next_event_time_ = t;
            }
            else
            {
               Verify(next_event_->init_time_ != t);

               next_event_->next_[1] = next_event_->next_[0];
               next_event_->t_ = next_event_->init_time_;
               next_event_->init_time_ = t;
               next_event_->set_down(down_);
               next_event_->set_next_impl(0, next);
               next_event_time_ = t;

               if (next_event_->t_ == next_event_->next_event_time_)
               {
                  Verify(next_event_->next_[1] == node_ptr());
                  next_event_->t_ = infinite_time();
               }
            }
         }

      private:
         // debugger shit
         bool data_initialized_;
         data_t data_;

         time_t init_time_;

         mutable node_ptr next_[2];
         time_t t_;

         node_ptr next_event_;
         time_t next_event_time_;

         profiling_t & profiling_;

         node_ptr down_;
      };

   private:
      boost::tuple<node_ptr, node_ptr> lower_bound_impl(node_ptr root, const data_t & d, time_t time) const
      {
         ++profiling_.localization;

         node_ptr r = root->instance(time), rn = next(r, time);
         Verify(!rn || rn->data());

         while (rn && *(rn->data()) < d)
         {
            ++profiling_.localization;

            r = rn;
            rn = next(r, time);

            Verify(!rn || rn->data());
         }

         return boost::make_tuple(r, rn);
      }

      boost::tuple<node_ptr, node_ptr> lower_bound_impl(const data_t &d, time_t time) const
      {
         return lower_bound_impl(roots_.front(), d, time);
      }

   private:
      mutable profiling_t profiling_;
      std::list<node_ptr> roots_;
      size_t roots_size_;
      boost::random::bernoulli_distribution<> coin_;
      boost::random::mt19937 eng_;
   };
}
