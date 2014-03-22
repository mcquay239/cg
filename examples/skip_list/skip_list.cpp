#include <QColor>
#include <QApplication>

#include <map>
#include <set>

#include <boost/optional.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <boost/function.hpp>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/random.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/tuple/tuple.hpp>

#include <cg/visualization/viewer_adapter.h>
#include <cg/visualization/draw_util.h>

#include <cg/io/point.h>

#include <cg/primitives/point.h>

using cg::point_2f;
using cg::point_2;

struct VerifyException : std::runtime_error
{
   VerifyException(const char * msg) : std::runtime_error(msg) {}
};

#define Verify(x) do { if (!(x)) { std::cerr << "Assertion " << #x << " failed at function " << __FUNCTION__ << " at file " << __FILE__ << ":" << __LINE__ << std::endl << std::flush; throw VerifyException(#x); } } while ( 0 )

float infinite(float *)
{
   return std::numeric_limits<float>::infinity();
}

size_t infinite(size_t *)
{
   return std::numeric_limits<size_t>::max();
}

struct meta_info_t
{
   meta_info_t()
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
      : roots_(1, node_ptr(new node_t(boost::none, 0, meta_)))
      , roots_size_(1)
      , coin_(0.9)
   {}

   ~persistent_set_t()
   {
      roots_.back()->clean(); // shit

      roots_.clear();
      Verify(meta_.nodes == 0);
   }

//   data_t const * lower_bound(data_t const & d, time_t t) const
//   {
//      node_ptr res = boost::get<0>(lower_bound_impl(d, t));
//      if (re)
//   }

//   data_t const * lower_bound(data_t const & d) const
//   {
//      return lower_bound(d, current_time_);
//   }

//   time_t current_time() const { return current_time_; }

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
         ++meta_.localization;

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
         ++meta_.insertion;

         boost::tie(r, rn) = rrn;

         node_ptr new_node(new node_t(d, t, meta_, rn));
         new_node->set_down(down);
         down = new_node;

         time_t t1 = t;
         do
         {
            ++meta_.insertion;

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
         ++meta_.insertion;

         node_ptr new_node(new node_t(d, t, meta_));
         new_node->set_down(down);

         Verify(!roots_.empty());

         node_ptr proot = roots_.back();
         roots_.push_back(new node_t(boost::none, 0, meta_));
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

////public:
//   void insert(data_t const & d)
//   {
//      node_ptr r, rn;
//      boost::tie(r, rn) = lower_bound_impl(d);

//      Verify(r);
//      Verify(!(rn && rn->data() == d));

//      node_ptr new_node(new node_t(d, current_time_, meta_, rn));
//      r->set_next(current_time_, new_node);

//      current_time_ += 1;
//   }

//   bool erase(data_t const & d)
//   {
//      node_ptr r, rn;
//      boost::tie(r, rn) = lower_bound_impl(d);

//      Verify(r);

//      if (!rn || rn->data() != d)
//         return false;

//      node_ptr next = rn->next(current_time_);
//      r->set_next(current_time_, next);

//      current_time_ += 1;

//      return true;
//   }

public:
   typedef boost::function<bool (data_t, time_t, data_t, time_t)> visitor_f;
   void visit(visitor_f const & v) const
   {
      roots_->front()->visit(v);
   }

   meta_info_t const & meta() const { return meta_; }

protected:
   static time_t infinite_time()
   {
      return infinite((time_t *)0);
   }

private:
   struct node_t : boost::intrusive_ref_counter<node_t, boost::thread_unsafe_counter>
   {
      node_t(boost::optional<data_t> const & data, time_t init_time, meta_info_t & meta, node_ptr next = node_ptr())
         : data_initialized_(data)
         , init_time_(init_time)
         , t_(infinite_time())
         , next_event_time_(infinite_time())
         , meta_(meta)
      {
         if (data_initialized_)
            data_ = *data;

         ++meta.nodes;
         next_[0] = next;
      }

      ~node_t()
      {
         --meta_.nodes;
      }

      void clean()
      {
         for (size_t l = 0; l != 2; ++l)
         {
            if (next_[l])
            {
               node_ptr n = next_[l];
               next_[l] = node_ptr();
               n->clean();
            }
         }

         if (next_event_)
         {
            node_ptr n = next_event_;
            next_event_ = node_ptr();
            n->clean();
         }

         if (down_)
         {
            node_ptr n = down_;
            down_ = node_ptr();
            n->clean();
         }
      }

      void visit(const visitor_f &v) const
      {
         for (size_t l = 0; l != 2; ++l)
         {
            if (!next_[l])
               continue;

            if (!data() || v(*data(), init_time_, *(next_[l]->data()), next_[l]->init_time_))
               next_[l]->visit(v);
         }

         if (next_event_)
            next_event_->visit(v);
      }

      node_ptr down() const
      {
         return down_;
      }

      node_ptr last(time_t t)
      {
         ++meta_.overhead;

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

            ++meta_.overhead;

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
         ++meta_.overhead;

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

            ++meta_.overhead;

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
//         while (next && next->next_event_time_ <= init_time)
//         {
//            meta_.overhead++;
//            next = next->next_event_;
//         }
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
            node_ptr next_event = new node_t(data(), t, meta_, next);
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

      meta_info_t & meta_;

      node_ptr down_;
   };

private:
   boost::tuple<node_ptr, node_ptr> lower_bound_impl(node_ptr root, const data_t & d, time_t time) const
   {
      ++meta_.localization;

      node_ptr r = root->instance(time), rn = next(r, time);
      Verify(!rn || rn->data());

      while (rn && *(rn->data()) < d)
      {
         ++meta_.localization;

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

   boost::tuple<node_ptr, node_ptr> lower_bound_impl(const data_t &d) const
   {
      return lower_bound_impl(d, current_time_);
   }

private:
   time_t current_time_;
   mutable meta_info_t meta_;
   std::list<node_ptr> roots_;
   size_t roots_size_;
   boost::random::bernoulli_distribution<> coin_;
   boost::random::mt19937 eng_;
};

//struct skip_list_viewer : cg::visualization::viewer_adapter
//{
//   skip_list_viewer(persistent_set_t<float, size_t> & set)
//      : set_(set)
//   {}

//   void draw(cg::visualization::drawer_type & drawer) const
//   {
//      drawer.set_color(Qt::green);
//      set_.visit(boost::bind(&skip_list_viewer::draw, this, boost::ref(drawer), _1, _2, _3, _4));

//      drawer.set_color(Qt::white);
//      for (float y : set_.slice(mouse_time_))
//         drawer.draw_point(point_2f(mouse_time_, y), 5);

//      if (float const * l = set_.lower_bound(mouse_data_, mouse_time_))
//      {
//         drawer.set_color(*l == mouse_data_ ? Qt::blue : Qt::red);
//         drawer.draw_point(point_2f(mouse_time_, *l), 5);
//      }

//      if (float const * l = set_.lower_bound(mouse_data_))
//      {
//         drawer.set_color(*l == mouse_data_ ? Qt::red : Qt::blue);
//         drawer.draw_point(point_2f(set_.current_time(), mouse_data_), 5);
//      }
//   }

//   void print(cg::visualization::printer_type & p) const
//   {
//      p.corner_stream() << "insertions: " << insertions_ << cg::visualization::endl;
//      p.corner_stream() << "erasures: " << erasures_ << cg::visualization::endl;
//      p.corner_stream() << "nodes: " << set_.meta().nodes << cg::visualization::endl;
//      p.corner_stream() << "overheads: " << set_.meta().overhead << cg::visualization::endl;
//   }

//   bool on_move(const cg::point_2f & pt)
//   {
//      mouse_time_ = pt.x;
//      mouse_data_ = 20 * (int)(pt.y / 20);
//      return true;
//   }

//   bool on_release(const point_2f & p)
//   {
//      if (float const * l = set_.lower_bound(mouse_data_))
//      {
//         if (*l == mouse_data_)
//         {
//            if (set_.erase(*l))
//               ++erasures_;
//            return true;
//         }
//      }

//      set_.insert(mouse_data_);
//      ++insertions_;

//      return true;
//   }

//private:
//   bool draw(cg::visualization::drawer_type & drawer, float start_data, float start_time,
//             float finish_data, float finish_time) const
//   {
//      point_2f p[] = {
//         {start_time, start_data},
//         {finish_time, finish_data}
//      };

//      drawer.draw_line(p[0], p[1]);
//      for (point_2f const & pt : p)
//         drawer.draw_point(pt, 3);

//      return start_time <= finish_time;
//   }

//private:
//   float mouse_data_;
//   float mouse_time_;
//   persistent_set_t<float, size_t> & set_;

//   size_t insertions_;
//   size_t erasures_;
//};

struct segment_t
{
   float val;
   size_t t1;
   size_t t2;
};

typedef std::vector<segment_t> segments_t;

bool fill(segments_t const & segments)
{
   try
   {
      persistent_set_t<float, size_t> test_set;
      for (segment_t const & s: segments)
         test_set.insert(s.val, s.t1, s.t2);
   }
   catch (...)
   {
      return false;
   }

   return true;
}

boost::tuple<segments_t, segments_t> segm_partition(segments_t const & in, size_t n)
{
   Verify(n < in.size());

   segments_t pos, neg;
   for (size_t l = 0; l != in.size(); ++l)
   {
      double v1 = std::rand() * 1. / RAND_MAX;
      double v2 = (n - pos.size()) * 1. / (in.size() - l);

      if (v1 < v2)
         pos.push_back(in[l]);
      else
         neg.push_back(in[l]);
   }

   return boost::make_tuple(pos, neg);
}

bool iterate(segments_t & segments, size_t & cn)
{
   size_t tries = 10;
   for (size_t l = 0; l != tries; ++l)
   {
      std::cout << "try " << l << " cn " << cn << " segments size " << segments.size() << std::endl;

      segments_t p, n;
      boost::tie(p, n) = segm_partition(segments, cn);

      if (!fill(p))
      {
         std::cout << "pos" << std::endl;

         segments = p;
         cn = segments.size() / 2;

         return true;
      }

      if (!fill(n))
      {
         std::cout << "neg" << std::endl;

         segments = n;
         cn = segments.size() / 2;

         return true;
      }
   }

   return false;
}

void shrink(segments_t & segments)
{
   Verify(!fill(segments));

   size_t cn = segments.size() / 2;
   while (cn)
   {
      if (!iterate(segments, cn))
         cn /= 2;
   }

   std::cout << "end, segments.size() == " << segments.size() << std::endl;
}

meta_info_t test(size_t const operations, bool test_result)
{
   boost::random::bernoulli_distribution<> coin(0.75);
   boost::random::uniform_01<float> uf;
   boost::random::uniform_int_distribution<> ufi;
   boost::random::mt19937 eng;

   std::vector<segment_t> segments;

   std::map<float, size_t> status;
   std::map<size_t, std::set<float> > data;
   persistent_set_t<float, size_t> test_set;

   for (size_t l = 0; l != operations; ++l)
   {
      if (!data.empty())
         data[l] = data[l - 1];

      if (data.empty() || data[l - 1].empty() || coin(eng))
      {
         do
         {
            float e = uf(eng);
            if (data[l].count(e))
               continue;

            status[e] = l;
            data[l].insert(e);
            //test_set.insert(e);
            break;
         }
         while (true);
      }
      else
      {
         size_t max_idx = data[l].size() - 1;
         size_t idx = ufi(eng, boost::random::uniform_int_distribution<>::param_type(0, max_idx));
         float val = *boost::next(data[l].begin(), idx);

         segments.push_back({val, status[val], l});

         data[l].erase(val);
         //test_set.erase(val);
      }
   }

   for (size_t l = operations; !data.rbegin()->second.empty(); ++l)
   {
      data[l] = data[l - 1];

      size_t max_idx = data[l].size() - 1;
      size_t idx = ufi(eng, boost::random::uniform_int_distribution<>::param_type(0, max_idx));
      float val = *boost::next(data[l].begin(), idx);

      segments.push_back({val, status[val], l});

      data[l].erase(val);
   }

   boost::random_shuffle(segments);
   if (test_result)
      std::cout << "start fill" << std::endl;
   size_t l = 0;
   try
   {
      for (; l != segments.size(); ++l)
      {
         segment_t const & s = segments[l];
         //std::cout << s.val << " " << s.t1 << " " << s.t2 << std::endl;
         test_set.insert(s.val, s.t1, s.t2);
      }
   }
   catch (...)
   {
      if (l + 1 != segments.size())
         segments.erase(segments.begin() + l + 1, segments.end());

      shrink(segments);

      persistent_set_t<float, size_t> ps;
      for (segment_t const & s: segments)
      {
         std::cout << s.val << " " << s.t1 << " " << s.t2 << std::endl;
         ps.insert(s.val, s.t1, s.t2);
      }

      std::cout << "test end" << std::endl;

      return meta_info_t();
   }

   meta_info_t res = test_set.meta();

   if (test_result)
   {
      std::cout << "levels: " << test_set.levels() << std::endl;
      for (size_t l = 0; l != data.size(); ++l)
      {
         if (!boost::equal(test_set.slice(l), data[l]))
         {
            std::list<float> r1 = test_set.slice(l);
            std::cout << "test_set: " << r1.size() << std::endl;
            for (float f: test_set.slice(l))
               std::cout << " " << f;
            std::cout << std::endl;

            std::set<float> r2 = data[l];
            std::cout << "data[l]: " << r2.size() << std::endl;
            for (float f: data[l])
               std::cout << " " << f;
            std::cout << std::endl;

            std::set<float> res;
            boost::set_difference(r1, r2, std::inserter(res, res.end()));

            std::cout << "test_set - data:";
            for (float f: res)
               std::cout << " " << f;
            std::cout << std::endl;

            res.clear();
            boost::set_difference(r2, r1, std::inserter(res, res.end()));

            std::cout << "data - test_set:";
            for (float f: res)
               std::cout << " " << f;
            std::cout << std::endl;


   //         skip_list_viewer viewer(test_set);
   //         cg::visualization::run_viewer(&viewer, "skip list");

            Verify(0);
         }
      }
   }

   return res;
}

meta_info_t test_speed(size_t operations, bool dmp = false)
{
   std::vector<size_t> times(2 * operations);
   for (size_t l = 0; l != operations * 2; ++l)
      times[l] = l;

   std::vector<size_t> values(operations);
   for (size_t l = 0; l != operations; ++l)
      values[l] = 1 + l;

   boost::random_shuffle(values);
   boost::random_shuffle(times);

   if (dmp)
      std::cout << "data prepared" << std::endl;

   persistent_set_t<float, size_t> ps;
   for (size_t l = 0; l != operations; ++l)
   {
      if (times[2 * l] > times[2 * l + 1])
         std::swap(times[2 * l], times[2 * l + 1]);

      ps.insert(values[l], times[2 * l], times[2 * l + 1]);
   }

   if (dmp)
      std::cout << "done" << std::endl;

   return ps.meta();
}

int main(int argc, char ** argv)
{
   //QApplication app(argc, argv);

//   test(1, true);

//   for (size_t i = 1; i <= 30; ++i)
//   {
//      int l = 1 << i;
//      double r[4];
//      for (size_t k = 0; k != 4; ++k)
//         r[k] = 0;

//      for (size_t k = 0; k != 10; ++k)
//      {
//         meta_info_t mi = test_speed(l);
//         r[0] += mi.nodes;
//         r[1] += mi.overhead;
//         r[2] += mi.insertion;
//         r[3] += mi.localization;
//      }

//      for (size_t k = 0; k != 4; ++k)
//         r[k] /= 10 * l;

//      std::cout << l << ":\t ";
//      for (size_t k = 0; k != 4; ++k)
//         std::cout << r[k] << " " << r[k] / log(l) << " ";

//      std::cout << std::endl;
//   }

//   skip_list_viewer viewer;
//   cg::visualization::run_viewer(&viewer, "skip list");

   test_speed(1000000, true);
}

