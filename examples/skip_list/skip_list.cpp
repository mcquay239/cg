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
#include <boost/range/algorithm.hpp>
#include <boost/tuple/tuple.hpp>

#include <cg/visualization/viewer_adapter.h>
#include <cg/visualization/draw_util.h>

#include <cg/io/point.h>

#include <cg/primitives/point.h>

using cg::point_2f;
using cg::point_2;

float infinite(float *)
{
   return std::numeric_limits<float>::infinity();
}

size_t infinite(size_t *)
{
   return std::numeric_limits<size_t>::max();
}

template <class Data, class Time>
struct persistent_set_t
{
   typedef Time time_t;
   typedef Data data_t;

   persistent_set_t()
      : current_time_(0)
   {}

   data_t const * lower_bound(data_t const & d, time_t t) const
   {
      node_ptr r = root(t);
      if (!r)
         return NULL;

      data_t const * res = NULL;
      while (r && r->data() <= d)
      {
         res = &r->data();
         r = r->next(t);
      }

      return res;
   }

   data_t const * lower_bound(data_t const & d) const
   {
      return lower_bound(d, current_time_);
   }

   time_t current_time() const { return current_time_; }

   std::list<data_t> slice(time_t t) const
   {
      std::list<data_t> res;
      node_ptr r = root(t);
      while (r)
      {
         res.push_back(r->data());
         r = r->next(t);
      }

      return res;
   }

   void insert(data_t const & d, time_t t1, time_t t2)
   {
      if (roots_.empty())
      {
         node_ptr new_node(new node_t(d, t1, meta_));
         roots_[t2] = node_ptr();
         roots_[t1] = new_node;
         new_node->terminate(t2);
      }
      else
      {
         node_ptr r, rn;
         boost::tie(r, rn) = lower_bound_impl(d, t1);
         node_ptr new_node(new node_t(d, t1, meta_, rn));

         do
         {
            new_node->set_next(t1, rn);
            new_node = new_node->instance(t1);

            set_next(r, new_node, t1);

            if (r)
            {
               boost::tie(r, t1) = r->next_event(t1);
            }
            else
            {
               typename std::map<time_t, node_ptr>::iterator it = roots_.upper_bound(t1);
               if (it == roots_.end())
               {
                  t1 = t2;
                  r = node_ptr();
                  rn = node_ptr();
               }
               else
               {
                  assert(t1 != it->first);
                  t1 = it->first;
                  r = it->second;
               }
            }

            if (r && r->data() > d)
            {
               rn = r;
               r = node_ptr();
            }
            else
            {
               assert(r);
               rn = r->next(t1);
               assert(rn->data() > d);
            }

         }
         while (t1 < t2);

         new_node->terminate(t2, r ? r : rn);
         set_next(r, rn, t2);
      }
   }

private:
   struct node_t;
   typedef boost::intrusive_ptr<node_t> node_ptr;

private:
   void set_next(node_ptr r, node_ptr rn, time_t t)
   {
      if (r)
         r->set_next(t, rn);
      else
         roots_[t] = rn;
   }

private:
   void insert(data_t const & d)
   {
      if (roots_.empty())
      {
         node_ptr new_node(new node_t(d, current_time_, meta_));
         roots_[current_time_] = new_node;
      }
      else
      {
         node_ptr r, rn;
         boost::tie(r, rn) = lower_bound_impl(d);

         assert(!(rn && rn->data() == d));

         node_ptr new_node(new node_t(d, current_time_, meta_, rn));
         if (r)
            r->set_next(current_time_, new_node);
         else
            roots_[current_time_] = new_node;
      }

      current_time_ += 1;
   }

   bool erase(data_t const & d)
   {
      if (roots_.empty())
         return false;

      node_ptr r, rn;
      boost::tie(r, rn) = lower_bound_impl(d);

      if (!rn || rn->data() != d)
         return false;

      node_ptr next = rn->next(current_time_);
      if (r)
         r->set_next(current_time_, next);
      else
         roots_[current_time_] = next;

      current_time_ += 1;

      return true;
   }

public:
   typedef boost::function<bool (data_t, time_t, data_t, time_t)> visitor_f;
   void visit(visitor_f const & v) const
   {
      BOOST_FOREACH(node_ptr node, roots_ | boost::adaptors::map_values)
         if (node)
            node->visit(v);
   }

   struct meta_info_t
   {
      meta_info_t()
         : nodes(0)
         , overhead(0)
      {}

      size_t nodes;
      size_t overhead;
   };

   meta_info_t const & meta() const { return meta_; }

protected:
   static time_t infinite_time()
   {
      return infinite((time_t *)0);
   }

private:
   struct node_t : boost::intrusive_ref_counter<node_t, boost::thread_unsafe_counter>
   {
      node_t(data_t const & data, time_t init_time, meta_info_t & meta, node_ptr next = node_ptr())
         : data_(data)
         , init_time_(init_time)
         , t_(infinite_time())
         , next_event_time_(infinite_time())
         , meta_(meta)
      {
         ++meta.nodes;
         next_[0] = next;
      }

      ~node_t()
      {
         --meta_.nodes;
      }

      void visit(const visitor_f &v) const
      {
         for (size_t l = 0; l != 2; ++l)
         {
            if (!next_[l])
               continue;

            if (v(data_, init_time_, next_[l]->data_, next_[l]->init_time_))
               next_[l]->visit(v);
         }

         if (next_event_)
            next_event_->visit(v);
      }

      node_ptr instance(time_t t)
      {
         node_ptr res(this);
         while (res->next_event_time_ <= t)
         {
            update_next(res->init_time_, res->next_[0]);
            update_next(res->t_, res->next_[1]);

            ++meta_.overhead;
            res = res->next_event_;

            assert(res);
         }

         return res;
      }

      node_ptr next(time_t t)
      {
         node_ptr n = instance(t);
         return t < n->t_ ? n->next_[0] : n->next_[1];
      }

      void terminate(time_t t)
      {
         node_ptr n = instance(t);

         assert(!n->next_event_);
         assert(n->next_event_time_ == infinite_time());

         n->next_event_time_ = t;
      }

      void set_next(time_t t, node_ptr next)
      {
         node_ptr n = instance(t);
         if (n->t_ == infinite_time())
         {
            if (t == n->init_time_)
            {
               n->next_[0] = next;
            }
            else if (n->next_[0] != next)
            {
               n->t_ = t;
               n->next_[1] = next;
            }
         }
         else
         {
            if (n->t_ == t)
            {
               if (n->next_[0] == next)
               {
                  n->t_ = infinite_time();
                  n->next_[1] = node_ptr();
               }
               else
               {
                  n->next_[1] = next;
               }
            }
            else if (n->next_[1] != next)
            {
               n->next_event_time_ = t;
               node_ptr next_event = new node_t(n->data_, t, n->meta_, next);
               next_event->next_event_ = n->next_event_;
               n->next_event_ = next_event;
            }
         }
      }

      boost::tuple<node_ptr, time_t> next_event(time_t t)
      {
         node_ptr n = instance(t);
         if (n->t_ != infinite_time() && n->t_ > t)
            return boost::make_tuple(n, n->t_);

         return boost::make_tuple(n->next_event_, n->next_event_time_);
      }

      data_t const & data() const { return data_; }

   private:
      void update_next(time_t init_time, node_ptr & next) const
      {
         while (next && next->next_event_time_ <= init_time)
         {
            meta_.overhead++;
            next = next->next_event_;
         }
      }

   private:
      data_t data_;
      time_t init_time_;

      mutable node_ptr next_[2];
      time_t t_;

      node_ptr next_event_;
      time_t next_event_time_;

      meta_info_t & meta_;
   };

private:
   node_ptr root(time_t t) const
   {
      typename std::map<time_t, node_ptr>::const_iterator it = roots_.upper_bound(t);
      if (it == roots_.begin())
         return node_ptr();

      return boost::prior(it)->second;
   }

private:
   boost::tuple<node_ptr, node_ptr> lower_bound_impl(const data_t &d, time_t time) const
   {
      node_ptr r, rn = root(time);
      while (rn && rn->data() < d)
      {
         rn = rn->instance(current_time_);
         if (r)
            r->set_next(current_time_, rn);

         r = rn;
         rn = rn->next(current_time_);
      }

      return boost::make_tuple(r, rn);
   }

   boost::tuple<node_ptr, node_ptr> lower_bound_impl(const data_t &d) const
   {
      return lower_bound_impl(d, current_time_);
   }

private:
   time_t current_time_;
   meta_info_t meta_;
   std::map<time_t, node_ptr> roots_;
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

double test()
{
   const size_t operations = 4;

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
   for (segment_t const & s: segments)
      test_set.insert(s.val, s.t1, s.t2);

   double res = test_set.meta().nodes * 1. / operations;
   std::cout << "nodes / operations == " << res << std::endl;
   res = test_set.meta().overhead * 1. / operations;
   std::cout << "overhead / operations == " << res << std::endl;

   for (size_t l = 0; l != operations; ++l)
   {
      if (!boost::equal(test_set.slice(l), data[l]))
      {
         std::list<float> r1 = test_set.slice(l);
         std::cout << "test_set: " << r1.size() << std::endl;
//         for (float f: test_set.slice(l))
//            std::cout << " " << f;
//         std::cout << std::endl;

         std::set<float> r2 = data[l];
         std::cout << "data[l]: " << r2.size() << std::endl;

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

//         for (float f: data[l])
//            std::cout << " " << f;
//         std::cout << std::endl;

//         skip_list_viewer viewer(test_set);
//         cg::visualization::run_viewer(&viewer, "skip list");

         assert(0);
      }
   }

   return res;
}

int main(int argc, char ** argv)
{
   //QApplication app(argc, argv);

   test();

//   skip_list_viewer viewer;
//   cg::visualization::run_viewer(&viewer, "skip list");
}

